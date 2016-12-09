from __future__ import (absolute_import, division, print_function)

import os

import mantid.simpleapi as mantid

import isis_powder.routines.common as common
from isis_powder.routines import yaml_parser
from isis_powder.abstract_inst import AbstractInst
from isis_powder.polaris_routines import polaris_advanced_config, polaris_algs, polaris_output


class Polaris(AbstractInst):
    # Instrument specific properties

    def __init__(self, chopper_on, config_file=None, **kwargs):

        expected_keys = ["user_name", "calibration_directory", "output_directory",
                         "apply_solid_angle", "calibration_mapping_file"]
        yaml_parser.set_kwargs_from_config_file(config_path=config_file, kwargs=kwargs, keys_to_find=expected_keys)
        super(Polaris, self).__init__(user_name=kwargs["user_name"], calibration_dir=kwargs["calibration_directory"],
                                      output_dir=kwargs["output_directory"])

        self._calibration_mapping_path = kwargs["calibration_mapping_file"]
        self._chopper_on = chopper_on

        # Hold the last dictionary later to avoid us having to keep parsing the YAML
        self._run_details_last_run_number = None
        self._run_details_cached_obj = None

        self._ads_workaround = 0

    def focus(self, run_number, input_mode, do_van_normalisation=True):
        return self._focus(run_number=run_number, input_batching=input_mode, do_van_normalisation=do_van_normalisation)

    def create_calibration_vanadium(self, run_in_range, do_absorb_corrections=True, gen_absorb_correction=False):
        run_details = self.get_run_details(run_number_string=int(run_in_range))
        run_details.run_number = run_details.vanadium_run_numbers
        return self._create_calibration_vanadium(vanadium_runs=run_details.vanadium_run_numbers,
                                                 empty_runs=run_details.empty_runs,
                                                 do_absorb_corrections=do_absorb_corrections,
                                                 gen_absorb_correction=gen_absorb_correction)

    def get_default_group_names(self):
        return self._calibration_grouping_names

    # Abstract implementation

    def get_run_details(self, run_number_string):
        input_run_number_list = common.generate_run_numbers(run_number_string=run_number_string)
        first_run = input_run_number_list[0]
        if self._run_details_last_run_number == first_run:
            return self._run_details_cached_obj

        solid_angle_on = bool(polaris_advanced_config.standard_variables["apply_solid_angle_corrections"])
        run_details = polaris_algs.get_run_details(chopper_on=self._chopper_on, sac_on=solid_angle_on,
                                                   run_number=first_run, calibration_dir=self._calibration_dir,
                                                   mapping_path=self._calibration_mapping_path)

        # Hold obj in case same run range is requested
        self._run_details_last_run_number = first_run
        self._run_details_cached_obj = run_details

        return run_details

    @staticmethod
    def generate_inst_file_name(run_number):
        if isinstance(run_number, list):
            updated_list = ["POL" + str(val) for val in run_number]
            return updated_list
        else:
            return "POL" + str(run_number)

    def normalise_ws(self, ws_to_correct, run_details=None):
        normalised_ws = mantid.NormaliseByCurrent(InputWorkspace=ws_to_correct, OutputWorkspace=ws_to_correct)
        return normalised_ws

    def apply_solid_angle_efficiency_corr(self, ws_to_correct, run_details):
        solid_angle_on = bool(polaris_advanced_config.standard_variables["apply_solid_angle_corrections"])

        if not solid_angle_on:
            return ws_to_correct

        corrections = polaris_algs.generate_solid_angle_corrections(run_details=run_details, instrument=self)
        corrected_ws = mantid.Divide(LHSWorkspace=ws_to_correct, RHSWorkspace=corrections)
        common.remove_intermediate_workspace(corrections)
        common.remove_intermediate_workspace(ws_to_correct)
        ws_to_correct = corrected_ws
        return ws_to_correct

    def correct_sample_vanadium(self, focus_spectra, vanadium_spectra=None):
        spectra_name = "sample_ws-" + str(self._ads_workaround + 1)
        self._ads_workaround += 1

        if vanadium_spectra:
            van_rebinned = mantid.RebinToWorkspace(WorkspaceToRebin=vanadium_spectra, WorkspaceToMatch=focus_spectra)
            mantid.Divide(LHSWorkspace=focus_spectra, RHSWorkspace=van_rebinned, OutputWorkspace=spectra_name)
            common.remove_intermediate_workspace(van_rebinned)

        return spectra_name

    def spline_vanadium_ws(self, focused_vanadium_spectra, instrument_version=''):
        masking_file_name = polaris_advanced_config.file_names["bragg_peaks_masking"]
        spline_coeff = polaris_advanced_config.standard_variables["b_spline_coefficient"]
        masking_file_path = os.path.join(self.calibration_dir, masking_file_name)
        output = polaris_algs.process_vanadium_for_focusing(bank_spectra=focused_vanadium_spectra,
                                                            spline_number=spline_coeff,
                                                            mask_path=masking_file_path)

        return output

    def generate_vanadium_absorb_corrections(self, calibration_full_paths, ws_to_match):
        return polaris_algs.generate_absorb_corrections(ws_to_match=ws_to_match)

    def output_focused_ws(self, processed_spectra, run_details, output_mode=None):
        d_spacing_group, tof_group = polaris_algs.split_into_tof_d_spacing_groups(processed_spectra)
        output_paths = self._generate_out_file_paths(run_details=run_details)

        polaris_output.save_polaris_focused_data(d_spacing_group=d_spacing_group, tof_group=tof_group,
                                                 output_paths=output_paths, run_number=run_details.run_number)

        return d_spacing_group

    def crop_to_sane_tof(self, ws_to_crop):
        cropped_ws = common.crop_in_tof(ws_to_rebin=ws_to_crop, x_min=800, x_max=20000)
        return cropped_ws
