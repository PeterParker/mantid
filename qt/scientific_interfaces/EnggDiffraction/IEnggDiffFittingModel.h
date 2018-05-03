#ifndef MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGMODEL_H_
#define MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGMODEL_H_

#include "RunLabel.h"

#include "DllConfig.h"
#include "IEnggDiffractionCalibration.h"
#include "MantidAPI/ITableWorkspace.h"
#include "MantidAPI/MatrixWorkspace_fwd.h"

#include <vector>

namespace MantidQt {
namespace CustomInterfaces {

class IEnggDiffFittingModel {

public:
  virtual ~IEnggDiffFittingModel() = default;

  virtual void addFilename(const RunLabel &runLabel,
                           const std::string &filename) = 0;

  virtual void addFocusedRun(const RunLabel &runLabel,
                             const Mantid::API::MatrixWorkspace_sptr ws) = 0;

  virtual Mantid::API::MatrixWorkspace_sptr
  alignDetectors(const Mantid::API::MatrixWorkspace_sptr inputWS,
                 const std::string &outputWSName) = 0;

  virtual Mantid::API::MatrixWorkspace_sptr
  getFocusedWorkspace(const RunLabel &runLabel) const = 0;

  virtual Mantid::API::MatrixWorkspace_sptr
  getFittedPeaksWS(const RunLabel &runLabel) const = 0;

  virtual Mantid::API::ITableWorkspace_sptr
  getFitResults(const RunLabel &runLabel) const = 0;

  virtual const std::string &
  getWorkspaceFilename(const RunLabel &runLabel) const = 0;

  virtual void removeRun(const RunLabel &runLabel) = 0;

  virtual Mantid::API::MatrixWorkspace_sptr
  loadWorkspace(const std::string &filename) = 0;

  virtual std::vector<RunLabel> getRunLabels() const = 0;

  virtual void
  setDifcTzero(Mantid::API::MatrixWorkspace_sptr ws, const RunLabel &runLabel,
               const std::vector<GSASCalibrationParms> &calibParams) = 0;

  virtual Mantid::API::MatrixWorkspace_sptr
  enggFitPeaks(const RunLabel &runLabel,
               const Mantid::API::MatrixWorkspace_sptr inputWS,
               const std::string &expectedPeaks) = 0;

  virtual void saveDiffFittingAscii(const RunLabel &runLabel,
                                    const std::string &filename) const = 0;

  virtual void addAllFitResultsToADS() const = 0;

  virtual void addAllFittedPeaksToADS() const = 0;

  virtual bool hasFittedPeaksForRun(const RunLabel &runLabel) const = 0;
};

} // namespace MantidQt
} // namespace CustomInterfaces

#endif // MANTIDQTCUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFFITTINGMODEL_H_
