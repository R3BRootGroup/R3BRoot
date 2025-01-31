"""@package docstring
Add random cal_to_hit_par to the parameter file, which could be the output from the simulation.
"""

import math
import random
from enum import Enum

import ROOT


class Mode(Enum):
    """
    A enumerator for the random type of a parameter.
    """

    ## Generate value according to uniform distribution.
    UNIFORM = 1
    ## Generate value according to distribution of a cosine function.
    COSINE = 2


class ParValueSet:
    """
    Data structure containing all information to generate a random value
    """

    def __init__(self, **kwargs):
        """Costructor

        @param kwargs key-pairs passed to set() member function.
        """
        self.__is_set = True
        self.set(**kwargs)

    @classmethod
    def empty(cls):
        """Factory method to create an invalid empty object.
        @return An invalid empty object.
        """
        value = cls(mode=Mode.UNIFORM, offset=0.0, amp=0.0, period=0, err=0.0)
        value.unset()
        return value

    def is_set(self):
        """Query whether the object is valid or not"""
        return self.__is_set

    def set(
        self, mode: Mode, offset: float, amp: float, period: int = 100, err: float = 0.5
    ):
        """Set all parameters of random value generations with different mode and make the object valid.

        @param mode Set a mode value (enumerator from Mode).
        @param offset Set a offset value
        @param amp Set a amplitude value
        @param period Set period value if Mode.COSINE is chosen
        @param err Set an error value
        """
        self.__is_set = True
        self.mode = mode
        self.offset = offset
        self.amp = amp
        self.period = period
        self.err = err

    def set_cos(self, **kwargs):
        """Set all random generator parameter with the cosine distribution.

        @param kwargs Key-value pairs passed to set() member function
        """
        self.set(Mode.COSINE, **kwargs)

    def set_uniform(self, **kwargs):
        """Set all random generator parameter with the uniform distribution.

        @param kwargs Key-value pairs passed to set() member function
        """
        self.set(Mode.UNIFORM, **kwargs)

    def unset(self):
        """Make the object invalid"""
        self.__is_set = False


class NeulandRandHitParAdder:
    """
    Adding parameters whose values are randomly generated from the original file to the output file.

    Example:
        @code{.py}
        from neuland_add_random_hitpar import NeulandRandHitParAdder as ParAdder

        NUMBER_OF_BARS = 1300
        par_adder = ParAdder(NUMBER_OF_BARS)

        COS_PERIOD = 1000

        # Use normal set function
        par_adder.effective_speed.set(mode = Mode.COSINE, offset = 10., amp = 5., period = COS_PERIOD, err = 0.5)
        par_adder.t_sync.set(mode = Mode.UNIFORM, offset = 15., amp = 5, period = COS_PERIOD, err = 0.5)

        # Use alias set_uniform function. The range of the uniform distribution is [-3., 3.]
        par_adder.t_diff.set_uniform( offset = 0., amp = 3., err = 0.5)

        # Use alias set_cos function
        par_adder.light_attenuation_length.set_cos( offset = 0.008, amp = 0.004, period = COS_PERIOD, err = 0.0005)
        par_adder.energy_gain.set_cos( offset = 15., amp = 5., period = COS_PERIOD, err = 0.5)
        par_adder.pedestal.set_cos( offset = 25., amp = 5., period = COS_PERIOD, err = 0.5)
        par_adder.pmt_thresh.set_cos( offset = 0.75, amp = 0.25, period = COS_PERIOD, err = 0.5)
        par_adder.saturation_coefficient.set_cos( offset = 0.5, amp = 0.4, period = COS_PERIOD, err = 0.5)

        input_filename = "sim.par.root.0"
        output_filename = "added_sim.par.root.0"

        # Add a new parameter with random values with other paramters from the input file.
        # After the call, the output parameter file will be created.
        par_adder.insert_parameters(input_filename, output_filename);
        @endcode
    """

    def __init__(self, module_num: int):
        """
        Constructor.

        @param module_num The number of bars
        """
        ## Number of bars in the NeuLAND detector
        self.__module_num: int = module_num

        ## Run id specified in the input parameter file
        self.__run_id: int = 999

        ## Effective speed of light paramter in the scintillation material
        self.effective_speed = ParValueSet.empty()

        ## Time synchronization paramter
        self.t_sync = ParValueSet.empty()

        ## Time offset parameter
        self.t_diff = ParValueSet.empty()

        ## Light attenuation length parameter
        self.light_attenuation_length = ParValueSet.empty()

        ## Energy gain parameter of the PMT (Both sides)
        self.energy_gain = ParValueSet.empty()

        ## Pedestal parameter of the PMT (Both sides)
        self.pedestal = ParValueSet.empty()

        ## PMT threshold parameter (Both sides)
        self.pmt_thresh = ParValueSet.empty()

        ## Saturation coefficient of the PMT (Both sides)
        self.saturation_coefficient = ParValueSet.empty()

    @staticmethod
    def generate_random_value_with_cos(par: ParValueSet, module_id):
        """
        Generating a random value according to the module id and the given random generator parameters.

        @param module_id The module ID (0-based number) of the bar
        """
        noise = random.uniform(-par.err, par.err)  # Zufällige Abweichung hinzufügen
        value = 0.0
        if par.mode == Mode.COSINE:
            value = par.offset + par.amp * math.cos(
                module_id * 4 * math.pi / par.period
            )
        else:
            value = random.uniform(par.offset - par.amp, par.offset + par.amp)
        return round(value + noise, 3)

    def set_run_id(self, val: int):
        """Set the run id in the parameter file

        @param val Run id
        """
        self.__run_id = val

    def __assign_random_values(self, module_id: int):
        """
        Create a HitModulePar with random values.

        @param module_id The module ID (0-based number) of the bar
        """
        one_module_par = ROOT.R3B.Neuland.HitModulePar()
        one_module_par.module_num = module_id

        effective_speed = self.generate_random_value_with_cos(
            self.effective_speed, module_id
        )
        t_sync = self.generate_random_value_with_cos(self.t_sync, module_id)
        t_diff = self.generate_random_value_with_cos(self.t_diff, module_id)
        light_attenuation_length = self.generate_random_value_with_cos(
            self.light_attenuation_length, module_id
        )

        energy_gain = self.generate_random_value_with_cos(self.energy_gain, module_id)
        pedestal = self.generate_random_value_with_cos(self.pedestal, module_id)
        pmt_thresh = self.generate_random_value_with_cos(self.pmt_thresh, module_id)
        saturation_coefficient = self.generate_random_value_with_cos(
            self.saturation_coefficient, module_id
        )

        # Werte im ROOT-Objekt setzen
        one_module_par.effectiveSpeed.value = effective_speed
        one_module_par.effectiveSpeed.error = 0
        one_module_par.tSync.value = t_sync
        one_module_par.tSync.error = 0
        one_module_par.tDiff.value = t_diff
        one_module_par.tDiff.error = 0

        one_module_par.lightAttenuationLength.value = light_attenuation_length
        one_module_par.lightAttenuationLength.error = 0

        one_module_par.energyGain.setLeft(ROOT.R3B.ValueErrorD(energy_gain, 0))
        one_module_par.energyGain.setRight(ROOT.R3B.ValueErrorD(energy_gain, 0))
        one_module_par.pedestal.setLeft(int(pedestal))
        one_module_par.pedestal.setRight(int(pedestal))

        one_module_par.PMTThreshold.setLeft(ROOT.R3B.ValueErrorD(pmt_thresh, 0.0))
        one_module_par.PMTThreshold.setRight(ROOT.R3B.ValueErrorD(pmt_thresh, 0.0))
        one_module_par.PMTSaturation.setLeft(
            ROOT.R3B.ValueErrorD(saturation_coefficient, 0.0)
        )
        one_module_par.PMTSaturation.setRight(
            ROOT.R3B.ValueErrorD(saturation_coefficient, 0.0)
        )
        return one_module_par

    def __check_if_all_parameters_set(self):
        """
        Check if all random generation parameters are set for each calibration parameter.
        """
        for member_str in dir(self):
            if not member_str.startswith("_") and not callable(
                getattr(self, member_str)
            ):
                if not getattr(self, member_str).is_set():
                    raise RuntimeError(f"parameter {member_str} is not set!")

    def insert_parameters(self, original_filepath: str, output_filepath: str):
        """
        Adding the parameters with random values to the output file

        @param original_filepath The original input file to be added
        @param output_filepath The output file containing the new parameter
        """
        input_par_file = ROOT.TFile(original_filepath, "read")
        output_file = ROOT.TFile(output_filepath, "recreate")

        self.__check_if_all_parameters_set()

        for key in input_par_file.GetListOfKeys():
            if key.GetName() in [f"{self.__run_id}", "ProcessID0"]:
                continue
            var = key.ReadObj()
            output_file.WriteObject(var, var.GetName(), "update")

        # Neues Parameter-Objekt erstellen und Module hinzufügen
        par_run = input_par_file.Get(f"{self.__run_id}")
        cal_to_hit_par = ROOT.R3B.Neuland.Cal2HitPar("NeulandHitPar")
        for module_id in range(0, self.__module_num):
            one_module_par = self.__assign_random_values(module_id)
            cal_to_hit_par.AddModulePar(one_module_par)

        # Version des neuen Parameters hinzufügen
        cal_to_hit_par_version = ROOT.FairParVersion("NeulandHitPar")
        cal_to_hit_par_version.setRootVersion(1)
        par_run.addParVersion(cal_to_hit_par_version)

        # Parameter-Objekte schreiben
        output_file.WriteObject(par_run, par_run.GetName(), "update")
        output_file.WriteObject(cal_to_hit_par, cal_to_hit_par.GetName(), "override")
        output_file.Close()
