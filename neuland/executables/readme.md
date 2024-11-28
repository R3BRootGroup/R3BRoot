# Neuland Command line Application

Before running any application, please make sure the `config.sh` script in the `R3BRoot/build` folder is correctly sourced.


## Simulation application

```bash
neulandSim -h
```

Output:

```text
options for neuland simulation:
  -h [ --help ]         help message [ = false ]
  --eventNum arg        set total event number [ = 10 ]
  --eventPrint arg      set event print number [ = 1 ]
  --runID arg           set runID [ = 999 ]
  --multiplicity arg    set particle multiplicity [ = 1 ]
  --energy arg          set energy value (GeV) of the particle [ = 1 ]
  --simuFile arg        set the base filename of simulation ouput [ =
                        "simu.root" ]
  --paraFile arg        set the base filename of parameter sink [ = "para.root"
                        ]
  -v [ --logLevel ] arg set log level of fairlog [ = "error" ]
```

## Analysis application

Analysis application `neuland_ana` is used to run the analysis tasks, such as digitization of the simulation output and event reconstruction.

### Run the application

It's **highly recommended** to run the application with a JSON configuration file, as it provides much more options compared to the command line options.

To the run the application with a JSON file:

```bash
neuland_ana -c [your-json-filename.json]
```

See below to how to obtain a JSON file.

### Configuration through JSON file

To obtain the default JSON configuration file:

```bash
neuland_ana --dump-config [your-filename.json]
```

The program will generate a JSON file, containing all default values, in the current folder. Users can open the generated JSON file and change the settings. If the file name is not given, the default filename "ana_config.json" will be used.

In the JSON file, the top section specifies the general information, needed by the program:

```json
    "general": {
        "run-id": 999,
        "number-of-events": -1,
        "log-level": "info",
        "input": {
            "data": [
                "simu.root"
            ],
            "tree-data": [
                "tree-file.root"
            ],
            "first-par": "para.root",
            "second-par": ""
        },
        "output": {
            "data": "output.root",
            "par": "output.par.root"
        }
    }
```
Further information:

- If `number-of-events` is less or equal to 0, the program will run through all events available in the data file.
- Input files and output files can be disabled by setting the file name empty.
- Each file name can be a [regex](https://users.cs.cf.ac.uk/Dave.Marshall/Internet/NEWS/regexp.html). The program will use all file names that can be represented by the regex.
- Users can give multiple values for the field `input.data` and `input.tree-data`. Each value must be inside a square bracket and separated by comma. 

For example:
```json
        "input": {
            "data": [
                "simu1.root",
                "other.root",
            ],
            "tree-data": [
                "tree.[0-9]+.root"
            ],
            "first-par": "para.root",
            "second-par": ""
        },
```

Options of each different tasks can be set after the `general` section. The tasks can be disabled or enabled by setting the field `enable` to be the `false` or `true`. **Be aware** that some tasks require the data generated from the other tasks. The program would fail if those parameters or data are not present in your input files. (_TODO: additional manuals required here_)

### Configuration through command line options

To check all available command line options:

```bash
neuland_ana -h
```

Output:

```text
Neuland Data analysis command line interface
Usage: neuland_ana [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -c,--config-file TEXT [ana_config.json]
                              set the json config file
  --paddle TEXT [neuland]     set the paddle name. e.g. "neuland"
  --channel TEXT [tamex]      set the channel name. e.g. "tamex"
  -i,--input-file TEXT [[input.root]]  ...
                              set the input filenames (regex)
  --input-tree-file TEXT ...  set the input filenames (regex) containing only root tree
  -p,--para TEXT [input.par.root]
                              set the filename of the input parameter root file
  --para-second TEXT          set the filename of the second input parameter root file
  -o,--output TEXT            set the output filename
  -n,--event-num INT [100]    set the event number
  --run-id INT [999]          set the run id
  -v,--verbose TEXT           set the verbose level
  --print-config              print default option value
  --dump-config TEXT [ana_config.json]
                              dump the config into a json file
```

Be aware that the values set from the command line options will override the values specified in the given JSON file.

