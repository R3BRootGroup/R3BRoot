# NeuLAND Command Line Interface {#neuland_exe}

The motivation of using a cli executable with a JSON file as the configuration is to provide users a much easier and robust way to run the R3BRoot tasks, such as for simulation and analysis. Compared to a ROOT Macro file, a JSON file only contains two things: strings and numbers. To run a task, users only need to look for some names and change the following values, without being burdened with unnecessary overhead in the ROOT Macro file, such as setting up `FairRun` or `EventHeader` in the correct order. It also provides opportunities for the users who just want to use the software without going deep to the ROOT/C++ programming language and code practices. The usage of a cli executable is also much safer and more robust than a ROOT Macro because the programming logic is fixed and users are only allowed to change the values according to their own needs.

[TOC]

## General usage

Before running any application, please make sure the `config.sh` script in the `R3BRoot/build` folder is correctly sourced. The NeuLAND cli executable, `neuland`, includes the interfaces to multiple applications (modes), like simulation, offline and online (not yet implemented) analysis. The interface of each application/mode also take in multiple program options for further configurations. Configuration can be specified by either through the program options or a JSON file. When they are both used for the configuration (see below), the final configuration value should be the one specified by the program options.

> [!note]
> It's highly recommended to use the JSON file as it provides much more detailed options available for each application.

### Synopsis

```bash
neuland mode [options]
```

The available `mode` values from the cli executable are

- `sim`: interface to simulation.
- `ana`: interface to offline analysis.
- `online`: interface to online analysis. (not yet implemented)

For example,

```bash
neuland sim -h # also for 'ana' or 'online'
```

prints out the help message for the chosen application.

There are some program options that are shared by all modes/applications, such as input data and parameter file names, output data and parameter file names, run id, etc. The most important ones are specifying a JSON configuration file and dumping the default configuration to a JSON file.

### Configuration with a JSON file

If the JSON file is not available, using the option `--dump-config`, every mode/application generates a JSON file containing the corresponding default configuration values. This generated file can be used as a template and users should open the file and change the values according to their own needs. For example,

```bash
neuland sim --dump-config config.json # also for 'ana' or 'online'
```

saves all default configurations to a new file called "config.json". If the file name is left empty, the default value will be used, which is different for each application.

> [!warning]
> Please make sure the JSON file with the specified file name is not already existed. Otherwise, the old file would be overwritten with the default values.

To use the JSON file as the configuration:

```bash
neuland sim -c config.json # also for 'ana' and 'online'
```

If the configurations specified in the JSON file are the same as the default, they could be removed and the existing configurations will be imposed on the default values. Multiple JSON files can also be given, in which the configurations from the JSON file will overwrite the ones from the previous JSON file.

### Configuration with program options and a JSON file

The program options from `neuland mode -h` can be used to overwrite certain values from the JSON file. For example,

```bash
neuland sim -c config.json -n 5000
```

simulates 5000 events regardless to the event value specified in JSON file.

> [!important]
> To make this work, `-c file.json` should always come before the other program options.

### General JSON configuration {#GeneralJsonConfig}

The general JSON configuration exists for every application/mode.

- `general`: General options.
  - `run-id`: Run ID for the current run. (default: 999)
  - `number-of-events`: Number of events to simulate/analyze. (default: 100)
  - `enable-mpi`: Enable MPI execution. **This should be true** when running in HPC. (default: `false`)
  - `log-level`: Log (severity) level. Available options: `fatal`, `error`, `warn`, etc. See section @ref log below. (default: `error`)
  - `verbosity-level`: Verbosity level. Available options: `low`, `high`, `user1`, etc. See section @ref log below. (default: `user1`)
  - `input`: Options for inputs.
    - `working-dir`: The working directory containing all input data and parameters. If it's empty and the following input file path is a relative path, the working directory is the current directory.
    - `data`: File name (regex) for input data file, containing the data tree and the FairRoot folder structure. (default: see `-h` for each application)
    - `tree-data`: File name (regex) for input data file, containing only the data tree. (default: "")
    - `first-par`: File name of the first parameter file. (default: "")
    - `second-par`: File name of the second parameter file. (default: "")
  - `output`: Options for outputs.
    - `working-dir`: The working directory to save output data and parameters. If it's empty and the following output file path is a relative path, the working directory is the current directory
    - `data`: File name of the output data file. (default: see `-h` for each application)
    - `par`: File name of the output parameter file. (default: see `-h` for each application)

Further information:

- General introduction about JSON data format can be found in this [webpage](https://www.json.org/json-en.html).
- If `number-of-events` is less or equal to 0, the program will run through all events available in the data file.
- Input files and output files can be disabled by setting the file name empty.
- Each input file name can be a [regex](https://users.cs.cf.ac.uk/Dave.Marshall/Internet/NEWS/regexp.html). The program will use all file names that can be matched by the regex.
- Users can give multiple values for the field `input.data` and `input.tree-data`. Each value must be inside a square bracket and separated by comma.

### Logging specifications {#log}

The logging tool used in this program is [fairlogger](https://github.com/FairRootGroup/FairLogger). There are two different levels user can adjust: severity (`log-level`) and verbosity (`verbosity-level`). The severity specifies the level threshold whether a certain message should be printed out whereas the verbosity specifies the format with which the printed messages are displayed. Their available options are:

- severity: `nolog`, `trace`, `debug4`, `debug3`, `debug2`, `debug1`, `debug`, `detail`, `info`, `state`, `warn`, `important`, `alarm`, `error`, `critical`, `fatal`

- verbosity: `verylow`, `low`, `medium`, `high`, `veryhigh`, `user1`, `user2`

The format of each verbosity is shown in the following table:

| verbosity | format                                                            |
| :-------: | ----------------------------------------------------------------- |
|  verylow  | message                                                           |
|    low    | [severity] message                                                |
|  medium   | [HH:MM:SS][severity] message                                      |
|   high    | [process_name][HH:MM:SS][severity] message                        |
| veryhigh  | [process_name][HH:MM:SS:µS][severity][file:line:function] message |
|   user1   | [severity][file:line:function] message                            |
|   user2   | [severity][HH:MM:SS][file:line:function] message                  |

## Simulation application

Simulation application `neuland sim` is used for the simulation tasks using different particle generators and detector geometries.

### Synopsis

```bash
neuland sim [-h] [options]
```

### JSON configuration

- `simulation`: Options for simulation specifics.
  - `event-print-num`: The number of events for one print out. (default: 1)
  - `store-trajectory`: Whether to store the trajectory data or not. (default: `true`)
  - `material-filename`: The file name of material parameter. (default: "media_r3b.geo")
  - `engine`: The name of the simulation engine. (default: "TGeant4")
  - `generator`: Options for particle generators
    - `random-seed`: The seed value of the random generator. (default: `0`)
    - `type`: The type of generator. Available options: `muon` and `box`. (default: `box`)
    - `multiplicity`: Multiplicity of particles used in `box` generator. (default: 1)
    - `energy`: Energy value (GeV) of particles used in `box` generator. (default: 0.6)
- `detectors`: Options for detectors
  - `cave`: Options for Cave
    - `enable`: Whether the detector is enabled or not. (default: `true`)
    - `geo-file`: Geometry file name for Cave. (default: "r3b_cave.geo")
  - `neuland`: Options for NeuLAND simulation class. See `R3BNeuland`.
    - `enable`: Whether the detector is enabled or not. (default: `true`)
    - `num-of-dp`: Number of double planes. (default: 13)
    - `location`: 3d coordinate (center point from the first plane) of the Detector.
    - `enable-auto-geo-build`: Whether geometry is built automatically. If false, a geometry file must exist for the specified number of the double planes. (default: `true`)

## Offline analysis application

The offline analysis application `neuland ana` is used to run the analysis tasks, such as digitization of the simulation output and event reconstruction.

### Synopsis

```bash
neuland ana [-h] [options]
```

### JSON configuration

- `tasks`: Options for multiple tasks.
  - `NeulandDigitizer`: See `R3BNeulandDigitizer`.
    - `paddle`: The paddle class used by the digitization engine (`R3B::Digitizing::DigitizingEngine`). Available options: `neuland` and `mock`. (default: `neuland`)
    - `channel`: The channel class used by the digitization engine. Available options: `tamex`, `tacquila` and `mock`. (default: `tamex`)
    - `par`: Parameter values for `tamex` channel class. See `R3B::Digitizing::Neuland::Tamex::Channel` and `R3B::Digitizing::Neuland::Tamex::Params`. (default: see the JSON file)
    - `pileup-strategy`: The pileup-strategy used by `tamex` channel. Available values: `width`, `distance` and `time_window`. See `R3B::Digitizing::Neuland::Tamex::PeakPileUpStrategy` for more details. (default: `width`)
    - `enable-sim-cal`: Output simulated cal-level data in the type `R3B::Neuland::SimCalData` (default branch name "NeulandSimCal"). To convert this data to the experimental cal-level data, please enable `NeulandSimCal2Cal` task as well. This is useful for simulation of the calibration process. (default: false)
    - `enable-hit-par`: Use the parameter values in `par` from the parameter file containing a variable with the name "NeulandHitPar". (default: false)
  - `NeulandSimCal2Cal`: See `R3B::Neuland::SimCal2Cal`.
  - `NeulandHitMon`: See `R3BNeulandHitMon`.
  - `NeulandPrimaryInteractionFinder`: See `R3BNeulandPrimaryInteractionFinder`.
  - `NeulandClusterFinder`. See `R3BNeulandClusterFinder`.
  - `NeulandPrimaryClusterFinder`. See `R3BNeulandPrimaryClusterFinder`.
  - `NeulandMultiplicityCalorimetricTrain`. See `R3BNeulandMultiplicityCalorimetricTrain`.
  - `NeulandMultiplicityBayesTrain`. See `R3BNeulandMultiplicityBayesTrain`.
  - `NeulandMultiplicityBayes`. See `R3BNeulandMultiplicityBayes`.
  - `NeulandNeutronsRValue`. See `R3BNeulandNeutronsRValue`.
  - `NeulandCal2HitParTask`: See `R3B::Neuland::Cal2HitParTask`.
    - `min-stat`: Minimal number of hits from the events that are used for the calibration.
    - `method`: Method of the calibration. Available options: `LSQT`, `predecessor` and `millepede`. See `R3B::Neuland::Cal2HitParMethod`.

All tasks listed above have three common options: "enable", "read" and "write". The "enable" option specifies whether the task is added or not. If some tasks should be added, simply change its value to `true`.

Options "read" and "write" specify the names of input branch or parameter that the task requires and the names of output branch and parameter the task produces. If not explicitly specified and one of the inputs is absent or not provided by the outputs of other task, the program will fail. Different tasks whose output can be fed as the input of the other can be enabled at the same time in one execution. However, if a task requires an input parameter, which can be generated by another task, they **may not** be enabled in the same execution as, most of time, the output parameter is only calculated in the very end of the run.

> [!important]
> Names in the "read" and "write" options must be separated by a semicolon `;` and their order matters. They should be kept the same if not under very special circumstances.

## Online analysis application

To be implemented ...
