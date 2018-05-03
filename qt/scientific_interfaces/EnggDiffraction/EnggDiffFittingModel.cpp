#include "EnggDiffFittingModel.h"

#include "MantidAPI/AlgorithmManager.h"
#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/ITableWorkspace_fwd.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/Run.h"
#include "MantidAPI/TableRow.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/WorkspaceGroup.h"
#include "MantidKernel/PropertyWithValue.h"

#include <algorithm>
#include <numeric>

using namespace Mantid;

namespace { // helpers

bool isDigit(const std::string &text) {
  return std::all_of(text.cbegin(), text.cend(), ::isdigit);
}

} // anonymous namespace

namespace MantidQt {
namespace CustomInterfaces {

void EnggDiffFittingModel::addFocusedRun(const RunLabel &runLabel,
                                         const API::MatrixWorkspace_sptr ws) {
  m_focusedWorkspaceMap.add(runLabel, ws);
}

void EnggDiffFittingModel::addFilename(const RunLabel &runLabel,
                                       const std::string &filename) {
  m_wsFilenameMap.add(runLabel, filename);
}

void EnggDiffFittingModel::addFitResults(
    const RunLabel &runLabel, const Mantid::API::ITableWorkspace_sptr ws) {
  m_fitParamsMap.add(runLabel, ws);
}

const std::string &
EnggDiffFittingModel::getWorkspaceFilename(const RunLabel &runLabel) const {
  return m_wsFilenameMap.get(runLabel);
}

Mantid::API::ITableWorkspace_sptr
EnggDiffFittingModel::getFitResults(const RunLabel &runLabel) const {
  return m_fitParamsMap.get(runLabel);
}

namespace {

template <size_t S, typename T>
void removeFromRunMapAndADS(const RunLabel &runLabel, RunMap<S, T> &map,
                            Mantid::API::AnalysisDataServiceImpl &ADS) {
  if (map.contains(runLabel)) {
    const auto &name = map.get(runLabel)->getName();
    map.remove(runLabel);
    if (ADS.doesExist(name)) {
      ADS.remove(name);
    }
  }
}

} // anonymous namespace

void EnggDiffFittingModel::removeRun(const RunLabel &runLabel) {
  m_wsFilenameMap.remove(runLabel);

  auto &ADS = Mantid::API::AnalysisDataService::Instance();
  removeFromRunMapAndADS(runLabel, m_fittedPeaksMap, ADS);
  removeFromRunMapAndADS(runLabel, m_fitParamsMap, ADS);
}

void EnggDiffFittingModel::setDifcTzero(
    Mantid::API::MatrixWorkspace_sptr ws, const RunLabel &runLabel,
    const std::vector<GSASCalibrationParms> &calibParams) {
  auto &run = ws->mutableRun();
  const std::string units = "none";

  if (calibParams.empty()) {
    run.addProperty<double>("difc", DEFAULT_DIFC, units, true);
    run.addProperty<double>("difa", DEFAULT_DIFA, units, true);
    run.addProperty<double>("tzero", DEFAULT_TZERO, units, true);
  } else {
    GSASCalibrationParms params(0, 0.0, 0.0, 0.0);
    for (const auto &paramSet : calibParams) {
      if (paramSet.bankid == runLabel.bank) {
        params = paramSet;
        break;
      }
    }
    if (params.difc == 0) {
      params = calibParams.front();
    }

    run.addProperty<double>("difc", params.difc, units, true);
    run.addProperty<double>("difa", params.difa, units, false);
    run.addProperty<double>("tzero", params.tzero, units, false);
  }
}

Mantid::API::MatrixWorkspace_sptr EnggDiffFittingModel::enggFitPeaks(
    const RunLabel &runLabel, const Mantid::API::MatrixWorkspace_sptr inputWS,
    const std::string &expectedPeaks) {
  auto enggFitPeaksAlg =
      Mantid::API::AlgorithmManager::Instance().create("EnggFitPeaks");

  enggFitPeaksAlg->initialize();
  enggFitPeaksAlg->setProperty("InputWorkspace", inputWS);
  if (!expectedPeaks.empty()) {
    enggFitPeaksAlg->setProperty("ExpectedPeaks", expectedPeaks);
  }
  enggFitPeaksAlg->setProperty("FittedPeaks", FIT_RESULTS_TABLE_NAME);
  enggFitPeaksAlg->execute();

  API::AnalysisDataServiceImpl &ADS = API::AnalysisDataService::Instance();
  const auto fitResultsTable =
      ADS.retrieveWS<API::ITableWorkspace>(FIT_RESULTS_TABLE_NAME);
  addFitResults(runLabel, fitResultsTable);

  return createFittedPeaksWS(runLabel, inputWS);
}

void EnggDiffFittingModel::saveDiffFittingAscii(
    const RunLabel &runLabel, const std::string &filename) const {
  const auto ws = getFitResults(runLabel);
  auto saveAlg =
      Mantid::API::AlgorithmManager::Instance().create("SaveDiffFittingAscii");
  saveAlg->initialize();
  saveAlg->setProperty("InputWorkspace", ws);
  saveAlg->setProperty("RunNumber", std::to_string(runLabel.runNumber));
  saveAlg->setProperty("Bank", std::to_string(runLabel.bank));
  saveAlg->setProperty("OutMode", "AppendToExistingFile");
  saveAlg->setProperty("Filename", filename);
  saveAlg->execute();
}

Mantid::API::MatrixWorkspace_sptr EnggDiffFittingModel::createFittedPeaksWS(
    const RunLabel &runLabel,
    const Mantid::API::MatrixWorkspace_sptr focusedWS) {
  const auto fitFunctionParams = getFitResults(runLabel);

  const size_t numberOfPeaks = fitFunctionParams->rowCount();

  for (size_t i = 0; i < numberOfPeaks; ++i) {
    const auto functionDescription = createFunctionString(fitFunctionParams, i);
    double startX, endX;
    std::tie(startX, endX) = getStartAndEndXFromFitParams(fitFunctionParams, i);

    const std::string singlePeakWSName =
        "__engggui_fitting_single_peak_" + std::to_string(i);

    evaluateFunction(functionDescription, focusedWS, singlePeakWSName, startX,
                     endX);

    cropWorkspace(singlePeakWSName, singlePeakWSName, 1, 1);

    rebinToFocusedWorkspace(singlePeakWSName, focusedWS, singlePeakWSName);

    if (i == 0) {
      cloneWorkspace(focusedWS, FITTED_PEAKS_WS_NAME);
      setDataToClonedWS(singlePeakWSName, FITTED_PEAKS_WS_NAME);
    } else {
      const std::string clonedWSName =
          "__engggui_cloned_peaks_" + std::to_string(i);
      cloneWorkspace(focusedWS, clonedWSName);
      setDataToClonedWS(singlePeakWSName, clonedWSName);

      appendSpectra(FITTED_PEAKS_WS_NAME, clonedWSName);
    }
  }

  const auto fittedPeaksWS =
      alignDetectors(FITTED_PEAKS_WS_NAME, FITTED_PEAKS_WS_NAME);
  m_fittedPeaksMap.add(runLabel, fittedPeaksWS);
  return fittedPeaksWS;
}

bool EnggDiffFittingModel::hasFittedPeaksForRun(
    const RunLabel &runLabel) const {
  return m_fittedPeaksMap.contains(runLabel);
}

Mantid::API::MatrixWorkspace_sptr
EnggDiffFittingModel::getFittedPeaksWS(const RunLabel &runLabel) const {
  return m_fittedPeaksMap.get(runLabel);
}

void EnggDiffFittingModel::evaluateFunction(
    const std::string &function,
    const Mantid::API::MatrixWorkspace_sptr inputWS,
    const std::string &outputWSName, const double startX, const double endX) {

  auto evalFunctionAlg =
      Mantid::API::AlgorithmManager::Instance().create("EvaluateFunction");
  evalFunctionAlg->initialize();
  evalFunctionAlg->setProperty("Function", function);
  evalFunctionAlg->setProperty("InputWorkspace", inputWS);
  evalFunctionAlg->setProperty("OutputWorkspace", outputWSName);
  evalFunctionAlg->setProperty("StartX", startX);
  evalFunctionAlg->setProperty("EndX", endX);
  evalFunctionAlg->execute();
}

void EnggDiffFittingModel::cropWorkspace(const std::string &inputWSName,
                                         const std::string &outputWSName,
                                         const int startWSIndex,
                                         const int endWSIndex) {
  auto cropWSAlg =
      Mantid::API::AlgorithmManager::Instance().create("CropWorkspace");
  cropWSAlg->initialize();
  cropWSAlg->setProperty("InputWorkspace", inputWSName);
  cropWSAlg->setProperty("OutputWorkspace", outputWSName);
  cropWSAlg->setProperty("StartWorkspaceIndex", startWSIndex);
  cropWSAlg->setProperty("EndWorkspaceIndex", endWSIndex);
  cropWSAlg->execute();
}

void EnggDiffFittingModel::rebinToFocusedWorkspace(
    const std::string &wsToRebinName,
    const Mantid::API::MatrixWorkspace_sptr wsToMatch,
    const std::string &outputWSName) {
  auto rebinToWSAlg =
      Mantid::API::AlgorithmManager::Instance().create("RebinToWorkspace");

  rebinToWSAlg->initialize();
  rebinToWSAlg->setProperty("WorkspaceToRebin", wsToRebinName);

  rebinToWSAlg->setProperty("WorkspaceToMatch", wsToMatch);
  rebinToWSAlg->setProperty("OutputWorkspace", outputWSName);
  rebinToWSAlg->execute();
}

void EnggDiffFittingModel::cloneWorkspace(
    const Mantid::API::MatrixWorkspace_sptr inputWorkspace,
    const std::string &outputWSName) const {
  auto cloneWSAlg =
      Mantid::API::AlgorithmManager::Instance().create("CloneWorkspace");
  cloneWSAlg->initialize();
  cloneWSAlg->setProperty("InputWorkspace", inputWorkspace);
  cloneWSAlg->setProperty("OutputWorkspace", outputWSName);
  cloneWSAlg->execute();
}

void EnggDiffFittingModel::setDataToClonedWS(const std::string &wsToCopyName,
                                             const std::string &targetWSName) {
  auto &ADS = Mantid::API::AnalysisDataService::Instance();
  auto wsToCopy = ADS.retrieveWS<Mantid::API::MatrixWorkspace>(wsToCopyName);
  auto currentClonedWS =
      ADS.retrieveWS<Mantid::API::MatrixWorkspace>(targetWSName);
  currentClonedWS->mutableY(0) = wsToCopy->y(0);
  currentClonedWS->mutableE(0) = wsToCopy->e(0);
}

void EnggDiffFittingModel::appendSpectra(const std::string &ws1Name,
                                         const std::string &ws2Name) const {
  auto appendSpectraAlg =
      Mantid::API::AlgorithmManager::Instance().create("AppendSpectra");

  appendSpectraAlg->initialize();
  appendSpectraAlg->setProperty("InputWorkspace1", ws1Name);
  appendSpectraAlg->setProperty("InputWorkspace2", ws2Name);
  appendSpectraAlg->setProperty("OutputWorkspace", ws1Name);
  appendSpectraAlg->execute();
}

std::tuple<double, double, double> EnggDiffFittingModel::getDifcDifaTzero(
    Mantid::API::MatrixWorkspace_const_sptr ws) {
  const auto run = ws->run();

  const auto difc = run.getPropertyValueAsType<double>("difc");
  const auto difa = run.getPropertyValueAsType<double>("difa");
  const auto tzero = run.getPropertyValueAsType<double>("tzero");

  return std::tuple<double, double, double>(difc, difa, tzero);
}

Mantid::API::ITableWorkspace_sptr
EnggDiffFittingModel::createCalibrationParamsTable(
    Mantid::API::MatrixWorkspace_const_sptr inputWS) {
  double difc, difa, tzero;
  std::tie(difc, difa, tzero) = getDifcDifaTzero(inputWS);

  auto calibrationParamsTable =
      Mantid::API::WorkspaceFactory::Instance().createTable();

  calibrationParamsTable->addColumn("int", "detid");
  calibrationParamsTable->addColumn("double", "difc");
  calibrationParamsTable->addColumn("double", "difa");
  calibrationParamsTable->addColumn("double", "tzero");

  Mantid::API::TableRow row = calibrationParamsTable->appendRow();
  const auto &spectrum = inputWS->getSpectrum(0);

  Mantid::detid_t detID = *(spectrum.getDetectorIDs().cbegin());
  row << detID << difc << difa << tzero;
  return calibrationParamsTable;
}

void EnggDiffFittingModel::convertFromDistribution(
    Mantid::API::MatrixWorkspace_sptr inputWS) {
  const auto name = inputWS->getName();
  auto convertFromDistAlg = Mantid::API::AlgorithmManager::Instance().create(
      "ConvertFromDistribution");
  convertFromDistAlg->initialize();
  convertFromDistAlg->setProperty("Workspace", inputWS);
  convertFromDistAlg->execute();
}

Mantid::API::MatrixWorkspace_sptr
EnggDiffFittingModel::alignDetectors(const std::string &inputWSName,
                                     const std::string &outputWSName) {
  const auto &ADS = Mantid::API::AnalysisDataService::Instance();
  const auto inputWS =
      ADS.retrieveWS<Mantid::API::MatrixWorkspace>(inputWSName);
  return alignDetectors(inputWS, outputWSName);
}

Mantid::API::MatrixWorkspace_sptr
EnggDiffFittingModel::alignDetectors(Mantid::API::MatrixWorkspace_sptr inputWS,
                                     const std::string &outputWSName) {
  const auto calibrationParamsTable = createCalibrationParamsTable(inputWS);

  if (inputWS->isDistribution()) {
    convertFromDistribution(inputWS);
  }

  auto alignDetAlg =
      Mantid::API::AlgorithmManager::Instance().create("AlignDetectors");
  alignDetAlg->initialize();
  alignDetAlg->setProperty("InputWorkspace", inputWS);
  alignDetAlg->setProperty("OutputWorkspace", outputWSName);
  alignDetAlg->setProperty("CalibrationWorkspace", calibrationParamsTable);
  alignDetAlg->execute();

  const auto &ADS = Mantid::API::AnalysisDataService::Instance();
  return ADS.retrieveWS<Mantid::API::MatrixWorkspace>(outputWSName);
}

void EnggDiffFittingModel::renameWorkspace(API::Workspace_sptr inputWS,
                                           const std::string &newName) const {
  auto renameAlg = API::AlgorithmManager::Instance().create("RenameWorkspace");
  renameAlg->setProperty("InputWorkspace", inputWS);
  renameAlg->setProperty("OutputWorkspace", newName);
  renameAlg->execute();
}

void EnggDiffFittingModel::groupWorkspaces(
    const std::vector<std::string> &workspaceNames,
    const std::string &outputWSName) {
  auto groupAlg = API::AlgorithmManager::Instance().create("GroupWorkspaces");
  groupAlg->setProperty("InputWorkspaces", workspaceNames);
  groupAlg->setProperty("OutputWorkspace", outputWSName);
  groupAlg->execute();
}

API::MatrixWorkspace_sptr
EnggDiffFittingModel::getFocusedWorkspace(const RunLabel &runLabel) const {
  return m_focusedWorkspaceMap.get(runLabel);
}

void EnggDiffFittingModel::mergeTables(
    const API::ITableWorkspace_sptr tableToCopy,
    API::ITableWorkspace_sptr targetTable) const {
  for (size_t i = 0; i < tableToCopy->rowCount(); ++i) {
    API::TableRow rowToCopy = tableToCopy->getRow(i);
    API::TableRow newRow = targetTable->appendRow();

    for (size_t j = 0; j < tableToCopy->columnCount(); ++j) {
      double valueToCopy;
      rowToCopy >> valueToCopy;
      newRow << valueToCopy;
    }
  }
}

void EnggDiffFittingModel::addAllFitResultsToADS() const {
  auto fitParamsTable = Mantid::API::WorkspaceFactory::Instance().createTable();
  renameWorkspace(fitParamsTable, FIT_RESULTS_TABLE_NAME);

  const auto runLabels = getRunLabels();

  for (const auto &runLabel : runLabels) {
    const auto singleWSFitResults = getFitResults(runLabel);

    if (runLabel == *runLabels.begin()) {
      // First element - copy column headings over
      const auto columnHeaders = singleWSFitResults->getColumnNames();
      for (const auto &header : columnHeaders) {
        fitParamsTable->addColumn("double", header);
      }
    }
    mergeTables(singleWSFitResults, fitParamsTable);
  }
}

void EnggDiffFittingModel::addAllFittedPeaksToADS() const {
  const auto runLabels = getRunLabels();
  if (runLabels.size() < 1) {
    return;
  }
  const auto firstWSLabel = runLabels[0];
  auto fittedPeaksWS = getFittedPeaksWS(firstWSLabel);
  cloneWorkspace(fittedPeaksWS, FITTED_PEAKS_WS_NAME);

  for (size_t i = 1; i < runLabels.size(); ++i) {
    auto wsToAppend = getFittedPeaksWS(runLabels[i]);
    appendSpectra(FITTED_PEAKS_WS_NAME, wsToAppend->getName());
  }
}

namespace {

std::string stripWSNameFromFilename(const std::string &fullyQualifiedFilename) {
  std::vector<std::string> directories;
  boost::split(directories, fullyQualifiedFilename, boost::is_any_of("\\/"));
  const std::string filename = directories.back();
  std::vector<std::string> filenameSegments;
  boost::split(filenameSegments, filename, boost::is_any_of("."));
  return filenameSegments[0];
}
}

Mantid::API::MatrixWorkspace_sptr
EnggDiffFittingModel::loadWorkspace(const std::string &filename) {
  // Set ws name to filename first, in case we need to guess bank ID from it
  const std::string temporaryWSName = stripWSNameFromFilename(filename);

  auto loadAlg = API::AlgorithmManager::Instance().create("Load");
  loadAlg->setProperty("Filename", filename);
  loadAlg->setProperty("OutputWorkspace", temporaryWSName);
  loadAlg->execute();

  API::AnalysisDataServiceImpl &ADS = API::AnalysisDataService::Instance();
  return ADS.retrieveWS<API::MatrixWorkspace>(temporaryWSName);
}

std::vector<RunLabel> EnggDiffFittingModel::getRunLabels() const {
  return m_fittedPeaksMap.getRunLabels();
}

size_t
EnggDiffFittingModel::guessBankID(API::MatrixWorkspace_const_sptr ws) const {
  const static std::string bankIDName = "bankid";
  if (ws->run().hasProperty(bankIDName)) {
    const auto log = dynamic_cast<Kernel::PropertyWithValue<int> *>(
        ws->run().getLogData(bankIDName));
    return boost::lexical_cast<size_t>(log->value());
  }

  // couldn't get it from sample logs - try using the old naming convention
  const std::string name = ws->getName();
  std::vector<std::string> chunks;
  boost::split(chunks, name, boost::is_any_of("_"));
  bool isNum = isDigit(chunks.back());
  if (!chunks.empty() && isNum) {
    try {
      return boost::lexical_cast<size_t>(chunks.back());
    } catch (boost::exception &) {
      // If we get a bad cast or something goes wrong then
      // the file is probably not what we were expecting
      // so throw a runtime error
      throw std::runtime_error(
          "Failed to fit file: The data was not what is expected. "
          "Does the file contain a focused workspace?");
    }
  }

  throw std::runtime_error("Could not guess run number from input workspace. "
                           "Are you sure it has been focused correctly?");
}

std::string EnggDiffFittingModel::createFunctionString(
    const Mantid::API::ITableWorkspace_sptr fitFunctionParams,
    const size_t row) {
  const auto A0 = fitFunctionParams->cell<double>(row, size_t(1));
  const auto A1 = fitFunctionParams->cell<double>(row, size_t(3));
  const auto I = fitFunctionParams->cell<double>(row, size_t(13));
  const auto A = fitFunctionParams->cell<double>(row, size_t(7));
  const auto B = fitFunctionParams->cell<double>(row, size_t(9));
  const auto X0 = fitFunctionParams->cell<double>(row, size_t(5));
  const auto S = fitFunctionParams->cell<double>(row, size_t(11));

  const std::string function =
      "name=LinearBackground,A0=" + boost::lexical_cast<std::string>(A0) +
      ",A1=" + boost::lexical_cast<std::string>(A1) +
      ";name=BackToBackExponential,I=" + boost::lexical_cast<std::string>(I) +
      ",A=" + boost::lexical_cast<std::string>(A) + ",B=" +
      boost::lexical_cast<std::string>(B) + ",X0=" +
      boost::lexical_cast<std::string>(X0) + ",S=" +
      boost::lexical_cast<std::string>(S);
  return function;
}

std::pair<double, double> EnggDiffFittingModel::getStartAndEndXFromFitParams(
    const Mantid::API::ITableWorkspace_sptr fitFunctionParams,
    const size_t row) {
  const auto X0 = fitFunctionParams->cell<double>(row, size_t(5));
  const auto S = fitFunctionParams->cell<double>(row, size_t(11));
  const double windowLeft = 9;
  const double windowRight = 12;

  const auto startX = X0 - (windowLeft * S);
  const auto endX = X0 + (windowRight * S);
  return std::pair<double, double>(startX, endX);
}

const std::string EnggDiffFittingModel::FOCUSED_WS_NAME =
    "engggui_fitting_focused_ws";
const std::string EnggDiffFittingModel::FIT_RESULTS_TABLE_NAME =
    "engggui_fitting_fitpeaks_params";
const std::string EnggDiffFittingModel::FITTED_PEAKS_WS_NAME =
    "engggui_fitting_single_peaks";

const double EnggDiffFittingModel::DEFAULT_DIFA = 0.0;
const double EnggDiffFittingModel::DEFAULT_DIFC = 18400.0;
const double EnggDiffFittingModel::DEFAULT_TZERO = 4.0;

} // namespace CustomInterfaces
} // namespace MantidQT
