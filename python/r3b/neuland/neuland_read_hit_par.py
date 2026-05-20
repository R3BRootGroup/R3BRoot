"""@package docstring
Checking the values of cal_to_hit parameters in the root file. The corresponding pandas dataframe can be retrieved.
"""

import warnings

import os
import numpy as np
import pandas as pd
import ROOT


class HitParReader:
    def __init__(self, filename: str, is_verbose: bool = True):
        self._filename: str = filename
        self._base_filename: str = os.path.basename(filename)
        self._hit_par_name: str = "NeulandHitPar"
        self._par_name_list: list[str] = [
            "t_diff",
            "t_sync",
            "effective_speed",
            "light_attenuation_length",
            "light_attenuation_factor",
            "pedestal",
            "energy_gain",
            "pmt_saturation",
            "pmt_threshold",
        ]
        self._par_dict: dict[str, np.typing.NDArray] = {}
        self._dataframe = pd.DataFrame()
        self._dataframes: dict[int, pd.DataFrame] = {}
        self._verbose = is_verbose

    @property
    def par_dict(self):
        """The par_dict property."""
        return self._par_dict

    @par_dict.setter
    def par_dict(self, value):
        self._par_dict = value

    @property
    def filename(self):
        """The filename property."""
        return self._filename

    @filename.setter
    def filename(self, value):
        self._filename = value

    @property
    def hit_par_name(self):
        """The hit_par_name property."""
        return self._hit_par_name

    @hit_par_name.setter
    def hit_par_name(self, value):
        self._hit_par_name = value

    @property
    def par_name_list(self):
        """The par_name_list property."""
        return self._par_name_list

    @par_name_list.setter
    def par_name_list(self, value: list[str]):
        self._par_name_list = value

    @property
    def dataframe(self):
        """The dataframe property."""
        return self._dataframe

    @dataframe.setter
    def dataframe(self, value):
        self._dataframe = value

    def set_verbose(self, is_verbose: bool = False):
        self._verbose = is_verbose

    def header(self):
        return list(self._dataframe.columns.values)

    def read(self, all_cycles: bool = False):
        par_file = ROOT.TFile(self._filename, "read")
        cycles = (
            self._get_all_cycles(par_file, self._hit_par_name)
            if all_cycles
            else [9999]
        )
        if self._verbose:
            print(f"In file {self._base_filename} \t> Cycle numbers read: {cycles}")
        for cycle in cycles:
            hit_par = par_file.Get(f"{self._hit_par_name};{cycle}")
            self._construct_empty_dict(hit_par, cycle)
            self._fill_data(hit_par)
            self._dataframes[cycle] = pd.DataFrame(self._par_dict)
        self._dataframe = self._dataframes[max(self._dataframes)]
        return self._dataframes if all_cycles else self._dataframe

    def _get_all_cycles(self, root_file, par_name: str):
        keys = root_file.GetListOfKeys()
        return [key.GetCycle() for key in keys if key.GetName() == par_name]

    def _construct_empty_dict(self, hit_par, cycle):
        num_of_modules = hit_par.GetNumOfModules()
        if self._verbose:
            print(
                    f"In file {self._base_filename} \t> Number of modules read: {num_of_modules}. Cycle number: {cycle}"
            )
        self._par_dict["bar_id"] = np.zeros(num_of_modules)
        for par_name in self._par_name_list:
            dicts = dir(getattr(ROOT.R3B.Neuland.HitModulePar(), par_name))
            if "left" in dicts:
                for side in ["left", "right"]:
                    self._par_dict[f"{par_name}.{side}.value"] = np.zeros(
                        num_of_modules
                    )
                    self._par_dict[f"{par_name}.{side}.error"] = np.zeros(
                        num_of_modules
                    )
            else:
                self._par_dict[f"{par_name}.error"] = np.zeros(num_of_modules)
                self._par_dict[f"{par_name}.value"] = np.zeros(num_of_modules)

    def _fill_data(self, hit_par):
        num_of_modules = hit_par.GetNumOfModules()
        for bar_id in range(num_of_modules):
            self._par_dict["bar_id"][bar_id] = bar_id
            if not hit_par.HasModuleParAt(bar_id + 1):
                warnings.warn(
                    f"Parameter with module num {bar_id + 1} doesn't exist in the file!"
                )
                continue
            module_par = hit_par.GetModuleParAt(bar_id + 1)
            for par_name in self._par_name_list:
                dicts = dir(getattr(ROOT.R3B.Neuland.HitModulePar(), par_name))
                if "left" in dicts:
                    for side in ["left", "right"]:
                        self._par_dict[f"{par_name}.{side}.value"][bar_id] = (
                            getattr(
                                getattr(module_par, par_name), side
                            )().value
                        )
                        self._par_dict[f"{par_name}.{side}.error"][bar_id] = (
                            getattr(
                                getattr(module_par, par_name), side
                            )().error
                        )
                else:
                    self._par_dict[f"{par_name}.value"][bar_id] = getattr(
                        module_par, par_name
                    ).value
                    self._par_dict[f"{par_name}.error"][bar_id] = getattr(
                        module_par, par_name
                    ).error
