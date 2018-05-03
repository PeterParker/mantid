#ifndef MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_ENGGDIFFFITTINGMODEL_H_
#define MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_ENGGDIFFFITTINGMODEL_H_

#include "DllConfig.h"
#include "IEnggDiffFittingModel.h"
#include "IEnggDiffractionCalibration.h"
#include "RunMap.h"

#include <array>
#include <unordered_map>

namespace MantidQt {
namespace CustomInterfaces {

class MANTIDQT_ENGGDIFFRACTION_DLL EnggDiffFittingModel
    : public IEnggDiffFittingModel {

public:
  void addFilename(const RunLabel &runLabel,
                   const std::string &filename) override;

  void addFocusedRun(const RunLabel &runLabel,
                     const Mantid::API::MatrixWorkspace_sptr ws) override;

  Mantid::API::MatrixWorkspace_sptr
  alignDetectors(const Mantid::API::MatrixWorkspace_sptr inputWS,
                 const std::string &outputWSName) override;

  Mantid::API::MatrixWorkspace_sptr
  getFocusedWorkspace(const RunLabel &runLabel) const override;

  Mantid::API::MatrixWorkspace_sptr
  getFittedPeaksWS(const RunLabel &runLabel) const override;

  Mantid::API::ITableWorkspace_sptr
  getFitResults(const RunLabel &runLabel) const override;

  const std::string &
  getWorkspaceFilename(const RunLabel &runLabel) const override;

  void removeRun(const RunLabel &runLabel) override;

  Mantid::API::MatrixWorkspace_sptr
  loadWorkspace(const std::string &filename) override;

  std::vector<RunLabel> getRunLabels() const override;

  void
  setDifcTzero(Mantid::API::MatrixWorkspace_sptr ws, const RunLabel &runLabel,
               const std::vector<GSASCalibrationParms> &calibParams) override;

  Mantid::API::MatrixWorkspace_sptr
  enggFitPeaks(const RunLabel &runLabel,
               const Mantid::API::MatrixWorkspace_sptr inputWS,
               const std::string &expectedPeaks) override;

  void saveDiffFittingAscii(const RunLabel &runLabel,
                            const std::string &filename) const override;

  void addAllFitResultsToADS() const override;

  void addAllFittedPeaksToADS() const override;

  bool hasFittedPeaksForRun(const RunLabel &runLabel) const override;

protected:
  void addFitResults(const RunLabel &runLabel,
                     const Mantid::API::ITableWorkspace_sptr ws);

  void mergeTables(const Mantid::API::ITableWorkspace_sptr tableToCopy,
                   Mantid::API::ITableWorkspace_sptr targetTable) const;

private:
  static const size_t MAX_BANKS = 2;
  static const double DEFAULT_DIFC;
  static const double DEFAULT_DIFA;
  static const double DEFAULT_TZERO;
  static const std::string FOCUSED_WS_NAME;
  static const std::string FIT_RESULTS_TABLE_NAME;
  static const std::string FITTED_PEAKS_WS_NAME;

  RunMap<MAX_BANKS, Mantid::API::MatrixWorkspace_sptr> m_focusedWorkspaceMap;
  RunMap<MAX_BANKS, std::string> m_wsFilenameMap;
  RunMap<MAX_BANKS, Mantid::API::ITableWorkspace_sptr> m_fitParamsMap;
  RunMap<MAX_BANKS, Mantid::API::MatrixWorkspace_sptr> m_fittedPeaksMap;

  std::string createFunctionString(
      const Mantid::API::ITableWorkspace_sptr fitFunctionParams,
      const size_t row);

  std::pair<double, double> getStartAndEndXFromFitParams(
      const Mantid::API::ITableWorkspace_sptr fitFunctionParams,
      const size_t row);

  Mantid::API::MatrixWorkspace_sptr
  createFittedPeaksWS(const RunLabel &runLabel,
                      const Mantid::API::MatrixWorkspace_sptr focusedWS);

  void evaluateFunction(const std::string &function,
                        const Mantid::API::MatrixWorkspace_sptr inputWS,
                        const std::string &outputWSName, const double startX,
                        const double endX);

  void cropWorkspace(const std::string &inputWSName,
                     const std::string &outputWSName, const int startWSIndex,
                     const int endWSIndex);

  void
  rebinToFocusedWorkspace(const std::string &wsToRebinName,
                          const Mantid::API::MatrixWorkspace_sptr wsToMatch,
                          const std::string &outputWSName);

  void cloneWorkspace(const Mantid::API::MatrixWorkspace_sptr inputWorkspace,
                      const std::string &outputWSName) const;

  void setDataToClonedWS(const std::string &wsToCopyName,
                         const std::string &targetWSName);

  void appendSpectra(const std::string &ws1Name,
                     const std::string &ws2Name) const;

  std::tuple<double, double, double>
  getDifcDifaTzero(Mantid::API::MatrixWorkspace_const_sptr ws);

  Mantid::API::ITableWorkspace_sptr
  createCalibrationParamsTable(Mantid::API::MatrixWorkspace_const_sptr inputWS);

  void convertFromDistribution(Mantid::API::MatrixWorkspace_sptr inputWS);

  Mantid::API::MatrixWorkspace_sptr
  alignDetectors(const std::string &inputWSName,
                 const std::string &outputWSName);

  void renameWorkspace(Mantid::API::Workspace_sptr inputWS,
                       const std::string &newName) const;

  void groupWorkspaces(const std::vector<std::string> &workspaceNames,
                       const std::string &outputWSName);

  size_t guessBankID(Mantid::API::MatrixWorkspace_const_sptr) const;
};

} // namespace CustomInterfaces
} // namespace MantidQT

#endif // MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_ENGGDIFFFITTINGMODEL_H_
