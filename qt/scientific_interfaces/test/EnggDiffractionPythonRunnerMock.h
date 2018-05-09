#ifndef MANTIDQT_CUSTOMINTERFACES_ENGGDIFFPYTHONRUNNERMOCK_H_
#define MANTIDQT_CUSTOMINTERFACES_ENGGDIFFPYTHONRUNNERMOCK_H_

#include "../EnggDiffraction/IEnggDiffractionPythonRunner.h"
#include "MantidKernel/WarningSuppressions.h"

#include <gmock/gmock.h>

GCC_DIAG_OFF_SUGGEST_OVERRIDE

class MockEnggDiffractionPythonRunner
    : public MantidQt::CustomInterfaces::IEnggDiffractionPythonRunner {
public:
  MOCK_METHOD1(enggRunPythonCode, std::string(const std::string &pyCode));
};

GCC_DIAG_ON_SUGGEST_OVERRIDE

#endif // MANTIDQT_CUSTOMINTERFACES_ENGGDIFFPYTHONRUNNERMOCK_H_
