#ifndef MANTIDQT_CUSTOMINTERFACES_ENGGDIFFCALIBRATIONMODELMOCK_H_
#define MANTIDQT_CUSTOMINTERFACES_ENGGDIFFCALIBRATIONMODELMOCK_H_

#include "../EnggDiffraction/IEnggDiffCalibrationModel.h"
#include "MantidKernel/WarningSuppressions.h"

#include <gmock/gmock.h>

GCC_DIAG_OFF_SUGGEST_OVERRIDE

using namespace MantidQt::CustomInterfaces;
using namespace Mantid::API;

class MockEnggDiffCalibrationModel : public IEnggDiffCalibrationModel {
public:
  MOCK_CONST_METHOD1(loadCalibrationSampleWS,
                     MatrixWorkspace_sptr(const std::string &runNumber));

  MOCK_CONST_METHOD4(runEnggCalibrate,
                     DIFC_TZERO(const size_t bankID,
                                const MatrixWorkspace_sptr ceriaWS,
                                const MatrixWorkspace_sptr vanCurvesWS,
                                const ITableWorkspace_sptr vanIntegWS));

  MOCK_CONST_METHOD5(runEnggCalibrate,
                     DIFC_TZERO(const std::string &spectrumNumbers,
                                const std::string &bankName,
                                const MatrixWorkspace_sptr ceriaWS,
                                const MatrixWorkspace_sptr vanCurvesWS,
                                const ITableWorkspace_sptr vanIntegWS));

  MOCK_METHOD1(setInstrument, void(const std::string &instrumentName));

  MOCK_CONST_METHOD4(writeOutCalibFileAllBanks,
                     void(const std::string &filename,
                          const std::vector<GSASCalibrationParms> &calibParams,
                          const std::string &ceriaNo,
                          const std::string &vanNo));

  MOCK_CONST_METHOD5(writeOutCalibFileSingleBank,
                     void(const std::string &filename,
                          const GSASCalibrationParms &calibParams,
                          const std::string &bankName,
                          const std::string &ceriaNo,
                          const std::string &vanNo));
};

GCC_DIAG_ON_SUGGEST_OVERRIDE

#endif // MANTIDQT_CUSTOMINTERFACES_ENGGDIFFCALIBRATIONMODELMOCK_H_
