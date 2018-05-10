#ifndef MANTIDQT_CUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFCALIBRATIONMODEL_H_
#define MANTIDQT_CUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFCALIBRATIONMODEL_H_

#include "IEnggDiffractionCalibration.h"

#include "MantidAPI/ITableWorkspace_fwd.h"
#include "MantidAPI/MatrixWorkspace_fwd.h"

#include <boost/optional.hpp>

#include <string>

namespace MantidQt {
namespace CustomInterfaces {

class IEnggDiffCalibrationModel {
public:
  struct DIFC_TZERO {
    DIFC_TZERO(const double _difc, const double _tzero)
        : difc(_difc), tzero(_tzero) {}
    const double difc;
    const double tzero;
  };

  virtual Mantid::API::MatrixWorkspace_sptr
  loadCalibrationSampleWS(const std::string &runNumber) const = 0;

  virtual DIFC_TZERO runEnggCalibrate(
      const size_t bankID,
      const Mantid::API::MatrixWorkspace_sptr calibrationSampleWS,
      const Mantid::API::MatrixWorkspace_sptr vanCurvesWS,
      const Mantid::API::ITableWorkspace_sptr vanIntegrationWS) const = 0;

  virtual DIFC_TZERO runEnggCalibrate(
      const std::string &spectrumNumbers,
      const boost::optional<std::string> &bankName,
      const Mantid::API::MatrixWorkspace_sptr calibrationSampleWS,
      const Mantid::API::MatrixWorkspace_sptr vanCurvesWS,
      const Mantid::API::ITableWorkspace_sptr vanIntegrationWS) const = 0;

  virtual void setInstrument(const std::string &instrumentName) = 0;

  virtual void writeOutCalibFileAllBanks(
      const std::string &filename,
      const std::vector<GSASCalibrationParms> &calibParams,
      const std::string &calibrationSampleRunNumber,
      const std::string &vanadiumRunNumber) const = 0;

  virtual void
  writeOutCalibFileSingleBank(const std::string &filename,
                              const GSASCalibrationParms &calibParams,
                              const std::string &bankName,
                              const std::string &calibrationSampleRunNumber,
                              const std::string &vanadiumRunNumber) const = 0;
};

} // CustomInterfaces
} // MantidQt

#endif // MANTIDQT_CUSTOMINTERFACES_ENGGDIFFRACTION_IENGGDIFFCALIBRATIONMODEL_H_
