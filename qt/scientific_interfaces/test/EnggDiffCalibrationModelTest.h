#ifndef MANTIDQT_CUSTOMINTERFACES_ENGGDIFFCALIBRATIONMODELTEST_H_
#define MANTIDQT_CUSTOMINTERFACES_ENGGDIFFCALIBRATIONMODELTEST_H_

#include "../EnggDiffraction/EnggDiffCalibrationModel.h"
#include "EnggDiffractionPythonRunnerMock.h"

#include "MantidAPI/AnalysisDataService.h"
#include "MantidAPI/FrameworkManager.h"
#include "MantidAPI/MatrixWorkspace.h"

#include <boost/make_shared.hpp>

#include <cxxtest/TestSuite.h>

using namespace MantidQt::CustomInterfaces;

class EnggDiffCalibrationModelTest : public CxxTest::TestSuite {

public:
  EnggDiffCalibrationModelTest() { Mantid::API::FrameworkManager::Instance(); }

  static EnggDiffCalibrationModelTest *createSuite() {
    return new EnggDiffCalibrationModelTest();
  }

  static void destroySuite(EnggDiffCalibrationModelTest *suite) {
    delete suite;
  }

  void test_loadCalibrationSampleWS() {
    auto model = setUpModel();
    Mantid::API::MatrixWorkspace_sptr loadedWS;

    TS_ASSERT_THROWS_NOTHING(loadedWS =
                                 model.loadCalibrationSampleWS("241391"));
    TS_ASSERT(Mantid::API::AnalysisDataService::Instance().doesExist(
        "ENGINX00241391"));
    TS_ASSERT_EQUALS(loadedWS->getRunNumber(), 241391);
  }

  void test_writeOutCalibFileAllBanks() {
    auto model = setUpModel();

    const std::string expectedPythonScript =
        "import EnggUtils\n"
        "import os\n"

        "GSAS_iparm_fname = os.path.normpath('output_file.prm')\n"
        "bank_names = []\n"
        "ceria_number = 123\n"
        "van_number = 456\n"
        "Difcs = []\n"
        "Zeros = []\n"
        "template_file = None\n"

        "bank_names.append('North')\n"
        "Difcs.append(1.000000)\n"
        "Zeros.append(2.000000)\n"

        "bank_names.append('South')\n"
        "Difcs.append(3.000000)\n"
        "Zeros.append(4.000000)\n"

        "EnggUtils.write_ENGINX_GSAS_iparam_file(output_file=GSAS_iparm_fname, "
        "bank_names=bank_names, difc=Difcs, tzero=Zeros, "
        "ceria_run=ceria_number, "
        "vanadium_run=van_number, template_file=template_file) \n";
    EXPECT_CALL(*m_mockPythonRunner, enggRunPythonCode(expectedPythonScript));
    model.writeOutCalibFileAllBanks(
        "output_file.prm", {GSASCalibrationParms(1, 1.0, 0.0, 2.0, ""),
                            GSASCalibrationParms(2, 3.0, 0.0, 4.0, "")},
        "123", "456");
  }

  void test_writeOutCalibFileSingleBank() {
    auto model = setUpModel();

    const std::string expectedPythonScript =
        "import EnggUtils\n"
        "import os\n"

        "GSAS_iparm_fname = os.path.normpath('output_file.prm')\n"
        "bank_names = []\n"
        "ceria_number = 123\n"
        "van_number = 456\n"
        "Difcs = []\n"
        "Zeros = []\n"
        "template_file = 'template_ENGINX_241391_236516_South_bank.prm'\n"

        "bank_names.append('CustomBankName')\n"
        "Difcs.append(1.000000)\n"
        "Zeros.append(2.000000)\n"

        "EnggUtils.write_ENGINX_GSAS_iparam_file(output_file=GSAS_iparm_fname, "
        "bank_names=bank_names, difc=Difcs, tzero=Zeros, "
        "ceria_run=ceria_number, "
        "vanadium_run=van_number, template_file=template_file) \n";

    EXPECT_CALL(*m_mockPythonRunner, enggRunPythonCode(expectedPythonScript));
    model.writeOutCalibFileSingleBank(
        "output_file.prm", GSASCalibrationParms(1, 1.0, 0.0, 2.0, ""),
        "CustomBankName", "123", "456");
  }

private:
  const static std::string INSTRUMENT_NAME;

  MockEnggDiffractionPythonRunner *m_mockPythonRunner;

  EnggDiffCalibrationModel setUpModel() {
    auto mockRunner = boost::make_shared<
        testing::NiceMock<MockEnggDiffractionPythonRunner>>();
    m_mockPythonRunner = mockRunner.get();

    return EnggDiffCalibrationModel(INSTRUMENT_NAME, std::move(mockRunner));
  }
};

const std::string EnggDiffCalibrationModelTest::INSTRUMENT_NAME = "ENGINX";

#endif // MANTIDQT_CUSTOMINTERFACES_ENGGDIFFCALIBRATIONMODELTEST_H_
