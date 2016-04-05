#pylint: disable=too-many-public-methods,invalid-name,no-init

"""
Unit test for Vesuvio corrections steps

Assumes that mantid can be imported and the data paths
are configured to find the Vesuvio data
"""

import stresstesting
import numpy as np
import platform


from mantid.api import *
import vesuvio.testing as testing
import mantid.simpleapi as ms
from mantid import *

#====================================Helper Functions=======================================
def setUp():
    test_ws = testing.create_test_ws()
    test_container_ws = testing.create_test_container_ws()
    return test_ws, test_container_ws

def tearDown():
    workspace_names =['__Correction','__Corrected','__Output','__LinearFit']
    for name in workspace_names:
        if mtd.doesExist(name):
            mtd.remove(name)


def _check_platform():
    # Check OS for RHEL7 or Ubuntu
    dist = platform.linux_distribution()
    is_linux, is_rhel6 = False, False
    if any(dist):
        is_linux = True
        if dist[0] == 'Red Hat Enterprise Linux Workstation' and dist[1] == '6.7':
            is_rhel6 = True
    return is_linux, is_rhel6

def _create_algorithm(**kwargs):
    alg = AlgorithmManager.createUnmanaged("VesuvioCorrections")
    alg.initialize()
    alg.setChild(True)
    alg.setProperty("OutputWorkspace", "__Output")
    alg.setProperty("CorrectionWorkspaces", "__Correction")
    alg.setProperty("CorrectedWorkspaces", "__Corrected")
    alg.setProperty("LinearFitResult", "__LinearFit")
    for key, value in kwargs.iteritems():
        alg.setProperty(key, value)
    return alg


def _create_dummy_fit_parameters():
    params = ms.CreateEmptyTableWorkspace(OutputWorkspace='__VesuvioCorrections_test_fit_params')

    params.addColumn('str', 'Name')
    params.addColumn('float', 'Value')
    params.addColumn('float', 'Error')

    params.addRow(['f0.Width', 4.72912, 0.41472])
    params.addRow(['f0.FSECoeff', 0.557332, 0])
    params.addRow(['f0.C_0', 11.8336, 1.11468])
    params.addRow(['f1.Width', 10, 0])
    params.addRow(['f1.Intensity', 2.21085, 0.481347])
    params.addRow(['f2.Width', 13, 0])
    params.addRow(['f2.Intensity', 1.42443, 0.583283])
    params.addRow(['f3.Width', 30, 0])
    params.addRow(['f3.Intensity', 0.499497, 0.28436])
    params.addRow(['f4.A0', -0.00278903, 0.00266163])
    params.addRow(['f4.A1', 14.5313, 22.2307])
    params.addRow(['f4.A2', -5475.01, 35984.4])
    params.addRow(['Cost function value', 2.34392, 0])

    return params


def _create_dummy_masses():
    return [1.0079, 16.0, 27.0, 133.0]


def _create_dummy_profiles():
    return "function=GramCharlier,hermite_coeffs=[1, 0, 0],k_free=0,sears_flag=1," \
           + "width=[2, 5, 7];function=Gaussian,width=10;function=Gaussian,width=13;" \
           + "function=Gaussian,width=30"


#===========================================================================================
#========================================Success cases======================================

class TestGammaAndMsCorrectWorkspaceIndexOne(stresstesting.MantidStressTest):

    _algorithm = None
    _is_linux = None
    _is_rhel6 = None
    _input_bins = None

    def runTest(self):
        test_ws, _ = setUp()
        self._input_bins = test_ws.blocksize()
        self._is_linux, self._is_rhel6 = _check_platform()
        self._algorithm = _create_algorithm(InputWorkspace=test_ws,
                                            GammaBackground=True,
                                            FitParameters=_create_dummy_fit_parameters(),
                                            Masses=_create_dummy_masses(),
                                            MassProfiles=_create_dummy_profiles())

        self._algorithm.execute()

    def validate(self):
        self.assertTrue(self._algorithm.isExecuted())
        # Test Corrections Workspaces
        corrections_wsg = self._algorithm.getProperty("CorrectionWorkspaces").value
        _validate_group_structure(self, corrections_wsg, 3)
        corrections_gb_peak = 4.22729680e-07
        corrections_ts_peak = 0.083994253007
        corrections_ms_peak = 8.64757253e-05
        if self._is_linux and not self._is_rhel6:
            corrections_gb_peak = 6.170476e-07
            corrections_ts_peak = 8.545568e-02
            corrections_ms_peak = 9.109919e-05

        _validate_matrix_peak_height(self, corrections_wsg.getItem(0), corrections_gb_peak)
        _validate_matrix_peak_height(self, corrections_wsg.getItem(1), corrections_ts_peak)
        _validate_matrix_peak_height(self, corrections_wsg.getItem(2), corrections_ms_peak)

        # Test Corrected Workspaces
        corrected_wsg = self._algorithm.getProperty("CorrectedWorkspaces").value
        _validate_group_structure(self, corrected_wsg, 3)
        corrected_gb_peak = 0.46638088
        corrected_ts_peak = 0.46593182
        corrected_ms_peak = 0.46635277
        if self._is_linux and not self._is_rhel6:
            corrected_gb_peak = 4.663811e-01
            corrected_ts_peak = 4.659339e-01
            corrected_ms_peak = 4.663553e-01

        _validate_matrix_peak_height(self, corrected_wsg.getItem(0), corrected_gb_peak)
        _validate_matrix_peak_height(self, corrected_wsg.getItem(1), corrected_ts_peak)
        _validate_matrix_peak_height(self, corrected_wsg.getItem(2), corrected_ms_peak)

        # Test OutputWorkspace
        output_ws = self._algorithm.getProperty("OutputWorkspace").value
        _validate_matrix_structure(self, output_ws, 1, self._input_bins)
        output_expected_peak = 0.46635315
        if self._is_linux and not self._is_rhel6:
            output_expected_peak = 4.663559e-01
        _validate_matrix_peak_height(self, output_ws, output_expected_peak)

        # Test Linear fit Result Workspace
        linear_params = self._algorithm.getProperty("LinearFitResult").value
        _validate_table_workspace(self, linear_params, 7, 3)
        expected_values = [4.17063e-05, 0.0, 1.0, 2.026619013, 0.0, 1.0, 11.799966]
        if self._is_linux and not self._is_rhel6:
            expected_values = [6.087759e-05, 0.0, 1.0, 2.019595, 0.0, 1.0, 11.80356]
        _validate_table_values_top_to_bottom(self, linear_params, expected_values)
        tearDown()

'''
class TestGammaAndMsCorrectWorkspaceIndexTwo(stresstesting.MantidStressTest):

    _algorithm = None
    _is_linux = None
    _is_rhel6 = None
    _input_bins = None

    def runTest(self):
        test_ws, _ = setUp()
        self._input_bins = test_ws.blocksize()
        self._is_linux, self._is_rhel6 = _check_platform()
        self._algorithm = _create_algorithm(InputWorkspace=test_ws,
                                            GammaBackground=True,
                                            FitParameters=_create_dummy_fit_parameters(),
                                            Masses=_create_dummy_masses(),
                                            MassProfiles=_create_dummy_profiles(),
                                            WorkspaceIndex=1)

        self._algorithm.execute()

    def validate(self):
        self.assertTrue(self._algorithm.isExecuted())
        # Test Corrections Workspaces
        corrections_wsg = self._algorithm.getProperty("CorrectionWorkspaces").value
        _validate_group_structure(self, corrections_wsg, 3)
        corrections_gb_peak = 1.2250951e-06
        corrections_ts_peak = 0.09844847237
        corrections_ms_peak = 9.9243003e-05
        if self._is_linux and not self._is_rhel6:
            corrections_gb_peak = 1.605327e-06
            corrections_ts_peak = 9.994254e-02
            corrections_ms_peak = 1.089477e-04

        _validate_matrix_peak_height(self, corrections_wsg.getItem(0), corrections_gb_peak)
        _validate_matrix_peak_height(self, corrections_wsg.getItem(1), corrections_ts_peak)
        _validate_matrix_peak_height(self, corrections_wsg.getItem(2), corrections_ms_peak)

        # Test Corrected Workspaces
        corrected_wsg = self._algorithm.getProperty("CorrectedWorkspaces").value
        _validate_group_structure(self, corrected_wsg, 3)
        corrected_gb_peak = 0.52350495
        corrected_ts_peak = 0.52341546
        corrected_ms_peak = 0.52344493
        if self._is_linux and not self._is_rhel6:
            corrected_gb_peak = 5.235046e-01
            corrected_ts_peak = 5.234076e-01
            corrected_ms_peak = 5.234477e-01

        _validate_matrix_peak_height(self, corrected_wsg.getItem(0), corrected_gb_peak)
        _validate_matrix_peak_height(self, corrected_wsg.getItem(1), corrected_ts_peak)
        _validate_matrix_peak_height(self, corrected_wsg.getItem(2), corrected_ms_peak)

        # Test OutputWorkspace
        output_ws = self._algorithm.getProperty("OutputWorkspace").value
        _validate_matrix_structure(self, output_ws, 1, self._input_bins)
        output_expected_peak = 0.5234438813082305
        if self._is_linux and not self._is_rhel6:
            output_expected_peak = 0.5234463
        _validate_matrix_peak_height(self, output_ws, output_expected_peak)

        # Test Linear fit Result Workspace
        linear_params = self._algorithm.getProperty("LinearFitResult").value
        _validate_table_workspace(self, linear_params, 7, 3)
        expected_values = [0.0001183, 0.0, 1.0, 2.4028667, 0.0, 1.0, 10.5412496]
        if self._is_linux and not self._is_rhel6:
            expected_values = [1.550200e-04, 0.0, 1.0, 2.390063, 0.0, 1.0, 10.055330]
        _validate_table_values_top_to_bottom(self, linear_params, expected_values)
        logger.warning(str(linear_params.column(0)))
        tearDown()


class TestMsCorrectWithContainer(stresstesting.MantidStressTest):

    _algorithm = None
    _input_bins = None

    def runTest(self):
        test_ws, test_container_ws = setUp()
        self._input_bins = test_ws.blocksize()
        self._algorithm = _create_algorithm(InputWorkspace=test_ws,
                                            ContainerWorkspace=test_container_ws,
                                            GammaBackground=False,
                                            FitParameters=_create_dummy_fit_parameters(),
                                            Masses=_create_dummy_masses(),
                                            MassProfiles=_create_dummy_profiles())

        self._algorithm.execute()

    def validate(self):
        self.assertTrue(self._algorithm.isExecuted())

        corrections_wsg = self._algorithm.getProperty("CorrectionWorkspaces").value
        _validate_group_structure(self, corrections_wsg, 3)

        corrected_wsg = self._algorithm.getProperty("CorrectedWorkspaces").value
        _validate_group_structure(self, corrected_wsg, 3)

        output_ws = self._algorithm.getProperty("OutputWorkspace").value
        _validate_matrix_structure(self, output_ws, 1, self._input_bins)

        linear_params = self._algorithm.getProperty("LinearFitResult").value
        _validate_table_workspace(self, linear_params, 7, 3)
        tearDown()


class TestGammaAndMsCorrectWithContainer(stresstesting.MantidStressTest):

    _algorithm = None
    _input_bins = None

    def runTest(self):
        test_ws, test_container_ws = setUp()
        self._input_bins = test_ws.blocksize()
        self._algorithm = _create_algorithm(InputWorkspace=test_ws,
                                            ContainerWorkspace=test_container_ws,
                                            FitParameters=_create_dummy_fit_parameters(),
                                            Masses=_create_dummy_masses(),
                                            MassProfiles=_create_dummy_profiles())

    def validate(self):
        self._algorithm.execute()
        self.assertTrue(self._algorithm.isExecuted())

        corrections_wsg = self._algorithm.getProperty("CorrectionWorkspaces").value
        _validate_group_structure(self, corrections_wsg, 4)

        corrected_wsg = self._algorithm.getProperty("CorrectedWorkspaces").value
        _validate_group_structure(self, corrected_wsg, 4)

        output_ws = self._algorithm.getProperty("OutputWorkspace").value
        _validate_matrix_structure(self, output_ws, 1, self._input_bins)

        linear_params = self._algorithm.getProperty("LinearFitResult").value
        _validate_table_workspace(self, linear_params, 10, 3)
        tearDown()


class TestGammaAndMsCorrectWithContainerFixedScaling(stresstesting.MantidStressTest):

    _algorithm = None
    _input_bins = None

    def runTest(self):
        test_ws, test_container_ws = setUp()
        self._input_bins = test_ws.blocksize()
        self._algorithm = _create_algorithm(InputWorkspace=test_ws,
                                            ContainerWorkspace=test_container_ws,
                                            GammaBackground=True,
                                            FitParameters=_create_dummy_fit_parameters(),
                                            Masses=_create_dummy_masses(),
                                            MassProfiles=_create_dummy_profiles(),
                                            ContainerScale=0.1,
                                            GammaBackgroundScale=0.2)

        self._algorithm.execute()

    def validate(self):
        self.assertTrue(self._algorithm.isExecuted())

        corrections_wsg = self._algorithm.getProperty("CorrectionWorkspaces").value
        _validate_group_structure(self, corrections_wsg, 4)

        corrected_wsg = self._algorithm.getProperty("CorrectedWorkspaces").value
        _validate_group_structure(self, corrected_wsg, 4)

        output_ws = self._algorithm.getProperty("OutputWorkspace").value
        _validate_matrix_structure(self, output_ws, 1, self._input_bins)

        linear_params = self._algorithm.getProperty("LinearFitResult").value
        _validate_table_workspace(self, linear_params, 10, 3)

        expected_table_values = [0.1,0.0,1.0,0.2,0.0,1.0,'skip',0.0,1.0]
        _validate_table_values_top_to_bottom(self, linear_params, expected_table_values)
        tearDown()


#========================================Failure cases======================================

class TestRunningWithoutFitParamsRaisesError(stresstesting.MantidStressTest):

    _algorithm = None

    def runTest(self):
        test_ws, _ = setUp()
        self._algorithm = _create_algorithm(InputWorkspace=test_ws,
                                            Masses=_create_dummy_masses(),
                                            MassProfiles=_create_dummy_profiles())
    def validate(self):
        self.assertRaises(RuntimeError, self._algorithm.execute)

class TestRunningWithoutMassesRaisesError(stresstesting.MantidStressTest):

    _algorithm = None

    def runTest(self):
        test_ws, _ = setUp()
        self._algorithm = _create_algorithm(InputWorkspace=test_ws,
                                            FitParameters=_create_dummy_fit_parameters(),
                                            MassProfiles=_create_dummy_profiles())
    def validate(self):
        self.assertRaises(RuntimeError, self._algorithm.execute)


class TestRunningWithoutProfilesRaisesError(stresstesting.MantidStressTest):

    _algorithm = None

    def runTest(self):
        test_ws, _ = setUp()
        self._algorithm = _create_algorithm(InputWorkspace=test_ws,
                                            FitParameters=_create_dummy_fit_parameters(),
                                            Masses=_create_dummy_masses())
    def validate(self):
        self.assertRaises(RuntimeError, self._algorithm.execute)
'''

#=========================================Validation======================================
#=========================================Structure=======================================

def _validate_group_structure(self, ws_group, expected_entries):
    """
    Checks that a workspace is a group and has the correct number of entries
    ws_group            :: Workspace to be validated
    expected_entries    :: Expected number of entries
    """
    self.assertTrue(isinstance(ws_group, WorkspaceGroup))
    num_entries = ws_group.getNumberOfEntries()
    self.assertEqual(num_entries, expected_entries)

def _validate_matrix_structure(self, matrix_ws, expected_hist, expected_bins):
    """
    Checks that a workspace is a matrix workspace and has the correct number of histograms and bins
    matrix_ws           :: Workspace to be validated
    expected_hist       :: Expected number of histograms
    expected_bins       :: Expected number of bins
    """
    self.assertTrue(isinstance(matrix_ws, MatrixWorkspace))
    num_hists = matrix_ws.getNumberHistograms()
    num_bins = matrix_ws.blocksize()
    self.assertEqual(num_hists, expected_hist)
    self.assertEqual(num_bins, expected_bins)

def _validate_table_workspace(self, table_ws, expected_rows, expected_columns):
    """
    Checks that a workspace is a table workspace and has the correct number of rows and columns
    table_ws            :: Workspace to be validated
    expected_rows       :: Expected number of rows
    ecpected_columns    :: Expected number of columns
    """
    self.assertTrue(isinstance(table_ws, ITableWorkspace))
    num_rows = table_ws.rowCount()
    num_columns = table_ws.columnCount()
    self.assertEqual(num_rows, expected_rows)
    self.assertEqual(num_columns, expected_columns)

#=======================================Values===========================================

def _validate_table_values_top_to_bottom(self, table_ws, expected_values, tolerance=0.05):
    """
    Checks that a table workspace has the expected values from top to bottom
    table_ws            :: Workspace to validate
    expected_values     :: The expected values to be in the table workspace,
                           if any values contained in the list are skip then
                           that value will not be tested.
    """
    for i in range(0, len(expected_values)):
        if expected_values[i] != 'skip':
            tolerance_value = expected_values[i] * tolerance
            abs_difference = abs(expected_values[i] - table_ws.cell(i,1))
            self.assertTrue(abs_difference <= abs(tolerance_value))

def _validate_matrix_peak_height(self, matrix_ws, expected_height, ws_index=0, tolerance=0.05):
    """
    Checks that the heightest peak value is as expected
    matrix_ws       :: Workspace to validate
    expected_height :: Expected maximum y value (peak height)
    ws_index        :: The Index to read from the workspace
    """
    y_data = matrix_ws.readY(ws_index)
    peak_height = np.amax(y_data)
    tolerance_value = expected_height * tolerance
    abs_difference = abs(expected_height - peak_height)
    self.assertTrue(abs_difference <= abs(tolerance_value))


if __name__ == "__main__":
    unittest.main()
