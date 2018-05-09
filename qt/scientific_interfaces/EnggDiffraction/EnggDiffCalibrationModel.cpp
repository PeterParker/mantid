#include "EnggDiffCalibrationModel.h"

#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidAPI/MatrixWorkspace.h"

namespace {

std::string generateFitParamsTableName(const size_t bankID) {
  return "enggggui_calibration_bank_" + std::to_string(bankID);
}

std::string
generateFitParamsTableName(const boost::optional<std::string> &bankName) {
  if (!bankName) {
    return "enggggui_calibration_bank_cropped";
  }
  if (*bankName == "North") {
    return "engggui_calibration_bank_1";
  }
  if (*bankName == "South") {
    return "engggui_calibration_bank_2";
  }
  return "engggui_calibration_bank_" + *bankName;
}
}

namespace MantidQt {
namespace CustomInterfaces {

EnggDiffCalibrationModel::EnggDiffCalibrationModel(
    const std::string &instrumentName,
    boost::shared_ptr<IEnggDiffractionPythonRunner> pythonRunner)
    : m_currentInstrument(instrumentName), m_pythonRunner(pythonRunner) {}

const std::string EnggDiffCalibrationModel::NORTH_BANK_PRM_TEMPLATE =
    "template_ENGINX_241391_236516_North_bank.prm";

const std::string EnggDiffCalibrationModel::SOUTH_BANK_PRM_TEMPLATE =
    "template_ENGINX_241391_236516_South_bank.prm";

Mantid::API::MatrixWorkspace_sptr
EnggDiffCalibrationModel::loadCalibrationSampleWS(
    const std::string &runNumber) const {
  const static size_t normalisedRunNumberLength = 8;
  const auto runName =
      m_currentInstrument +
      std::string(normalisedRunNumberLength - runNumber.length(), '0') +
      runNumber;

  auto load = Mantid::API::AlgorithmManager::Instance().create("Load");
  load->initialize();
  load->setPropertyValue("Filename", runName);
  load->setPropertyValue("OutputWorkspace", runName);
  load->execute();

  return Mantid::API::AnalysisDataService::Instance()
      .retrieveWS<Mantid::API::MatrixWorkspace>(runName);
}

EnggDiffCalibrationModel::DIFC_TZERO EnggDiffCalibrationModel::runEnggCalibrate(
    const size_t bankID,
    const Mantid::API::MatrixWorkspace_sptr calibrationSampleWS,
    const Mantid::API::MatrixWorkspace_sptr vanCurvesWS,
    const Mantid::API::ITableWorkspace_sptr vanIntegrationWS) const {
  auto alg = Mantid::API::AlgorithmManager::Instance().create("EnggCalibrate");
  alg->initialize();
  alg->setProperty("InputWorkspace", calibrationSampleWS);
  alg->setProperty("VanIntegrationWorkspace", vanIntegrationWS);
  alg->setProperty("VanCurvesWorkspace", vanCurvesWS);
  alg->setPropertyValue("Bank", std::to_string(bankID));

  const auto fitParamsTableName = generateFitParamsTableName(bankID);
  alg->setPropertyValue("FittedPeaks", fitParamsTableName);
  alg->setPropertyValue("OutputParametersTableName", fitParamsTableName);

  alg->execute();

  DIFC_TZERO result(alg->getProperty("DIFC"), alg->getProperty("TZERO"));
  return result;
}

EnggDiffCalibrationModel::DIFC_TZERO EnggDiffCalibrationModel::runEnggCalibrate(
    const std::string &spectrumNumbers,
    const boost::optional<std::string> &bankName,
    const Mantid::API::MatrixWorkspace_sptr calibrationSampleWS,
    const Mantid::API::MatrixWorkspace_sptr vanCurvesWS,
    const Mantid::API::ITableWorkspace_sptr vanIntegrationWS) const {
  auto alg = Mantid::API::AlgorithmManager::Instance().create("EnggCalibrate");
  alg->initialize();
  alg->setProperty("InputWorkspace", calibrationSampleWS);
  alg->setProperty("VanIntegrationWorkspace", vanIntegrationWS);
  alg->setProperty("VanCurvesWorkspace", vanCurvesWS);
  alg->setProperty("SpectrumNumbers", spectrumNumbers);

  const auto fitParamsTableName = generateFitParamsTableName(bankName);
  alg->setPropertyValue("FittedPeaks", fitParamsTableName);
  alg->setPropertyValue("OutputParametersTableName", fitParamsTableName);

  alg->execute();

  DIFC_TZERO result(alg->getProperty("DIFC"), alg->getProperty("TZERO"));
  return result;
}

void EnggDiffCalibrationModel::setInstrument(
    const std::string &instrumentName) {
  m_currentInstrument = instrumentName;
}

void EnggDiffCalibrationModel::writeOutCalibFileAllBanks(
    const std::string &filename,
    const std::vector<GSASCalibrationParms> &calibParams,
    const std::string &calibrationSampleRunNumber,
    const std::string &vanadiumRunNumber) const {
  runEnggWritePRMScript(filename, calibParams, {"North", "South"},
                        calibrationSampleRunNumber, vanadiumRunNumber,
                        boost::none);
}

void EnggDiffCalibrationModel::writeOutCalibFileSingleBank(
    const std::string &filename, const GSASCalibrationParms &calibParams,
    const std::string &bankName, const std::string &calibrationSampleRunNumber,
    const std::string &vanadiumRunNumber) const {
  std::string templateFile;
  if (calibParams.bankid == 1) {
    templateFile = SOUTH_BANK_PRM_TEMPLATE;
  } else {
    templateFile = NORTH_BANK_PRM_TEMPLATE;
  }
  runEnggWritePRMScript(filename, {calibParams}, {bankName},
                        calibrationSampleRunNumber, vanadiumRunNumber,
                        templateFile);
}

void EnggDiffCalibrationModel::runEnggWritePRMScript(
    const std::string &outFilename,
    const std::vector<GSASCalibrationParms> &calibParams,
    const std::vector<std::string> &bankNames,
    const std::string &calibSampleRunNo, const std::string &vanadiumRunNo,
    const boost::optional<std::string> &templateFilename) const {
  // TODO: this is horrible and should be changed to avoid running
  // Python code. Update this as soon as we have a more stable way of
  // generating IPARM/PRM files.

  // Writes a file doing this:
  // write_ENGINX_GSAS_iparam_file(output_file, difc, zero, ceria_run=241391,
  // vanadium_run=236516, template_file=None):

  // this replace is to prevent issues with network drives on windows:
  const std::string normalisedOutFilename =
      boost::replace_all_copy(outFilename, "\\", "/");
  std::string pyCode = "import EnggUtils\n";
  pyCode += "import os\n";
  // normalize apparently not needed after the replace, but to be double-safe:
  pyCode +=
      "GSAS_iparm_fname = os.path.normpath('" + normalisedOutFilename + "')\n";
  pyCode += "bank_names = []\n";
  pyCode += "ceria_number = " + calibSampleRunNo + "\n";
  pyCode += "van_number = " + vanadiumRunNo + "\n";
  pyCode += "Difcs = []\n";
  pyCode += "Zeros = []\n";
  std::string templateFileVal = "None";
  if (templateFilename) {
    templateFileVal = "'" + *templateFilename + "'";
  }
  pyCode += "template_file = " + templateFileVal + "\n";
  for (size_t i = 0; i < calibParams.size(); ++i) {
    const auto &calibParam = calibParams[i];
    pyCode += "bank_names.append('" + bankNames[i] + "')\n";
    pyCode += "Difcs.append(" + std::to_string(calibParam.difc) + ")\n";
    pyCode += "Zeros.append(" + std::to_string(calibParam.tzero) + ")\n";
  }
  pyCode +=
      "EnggUtils.write_ENGINX_GSAS_iparam_file(output_file=GSAS_iparm_fname, "
      "bank_names=bank_names, difc=Difcs, tzero=Zeros, ceria_run=ceria_number, "
      "vanadium_run=van_number, template_file=template_file) \n";

  m_pythonRunner->enggRunPythonCode(pyCode);
}

} // CustomInterfaces
} // MantidQt
