#ifndef MANTIDQT_CUSTOMINTERFACES_ENGGDIFFRACTION_ENGGDIFFCALIBMODEL_H_
#define MANTIDQT_CUSTOMINTERFACES_ENGGDIFFRACTION_ENGGDIFFCALIBMODEL_H_

#include "DllConfig.h"
#include "IEnggDiffCalibrationModel.h"
#include "IEnggDiffractionPythonRunner.h"

#include <boost/optional.hpp>

namespace MantidQt {
namespace CustomInterfaces {

class MANTIDQT_ENGGDIFFRACTION_DLL EnggDiffCalibrationModel
    : public IEnggDiffCalibrationModel {
public:
  EnggDiffCalibrationModel(
      const std::string &instrumentName,
      boost::shared_ptr<IEnggDiffractionPythonRunner> pythonRunner);

  Mantid::API::MatrixWorkspace_sptr
  loadCalibrationSampleWS(const std::string &runNumber) const override;

  DIFC_TZERO runEnggCalibrate(
      const size_t bankID,
      const Mantid::API::MatrixWorkspace_sptr calibrationSampleWS,
      const Mantid::API::MatrixWorkspace_sptr vanCurvesWS,
      const Mantid::API::ITableWorkspace_sptr vanIntegrationWS) const override;

  DIFC_TZERO runEnggCalibrate(
      const std::string &spectrumNumbers, const std::string &bankName,
      const Mantid::API::MatrixWorkspace_sptr calibrationSampleWS,
      const Mantid::API::MatrixWorkspace_sptr vanCurvesWS,
      const Mantid::API::ITableWorkspace_sptr vanIntegrationWS) const override;

  void setInstrument(const std::string &instrumentName) override;

  void writeOutCalibFileAllBanks(
      const std::string &filename,
      const std::vector<GSASCalibrationParms> &calibParams,
      const std::string &calibrationSampleRunNumber,
      const std::string &vanadiumRunNumber) const override;

  void writeOutCalibFileSingleBank(
      const std::string &filename, const GSASCalibrationParms &calibParams,
      const std::string &bankName,
      const std::string &calibrationSampleRunNumber,
      const std::string &vanadiumRunNumber) const override;

private:
  const static std::string NORTH_BANK_PRM_TEMPLATE;
  const static std::string SOUTH_BANK_PRM_TEMPLATE;

  void runEnggWritePRMScript(
      const std::string &outFilename,
      const std::vector<GSASCalibrationParms> &calibParams,
      const std::vector<std::string> &bankNames,
      const std::string &calibSampleRunNo, const std::string &vanadiumRunNo,
      const boost::optional<std::string> &templateFilename) const;

  std::string m_currentInstrument;

  boost::shared_ptr<IEnggDiffractionPythonRunner> m_pythonRunner;
};

} // CustomInterfaces
} // MantidQt

#endif // MANTIDQT_CUSTOMINTERFACES_ENGGDIFFRACTION_ENGGDIFFCALIBMODEL_H_
