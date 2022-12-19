# R3BRoot Software [![license](https://alfa-ci.gsi.de/shields/badge/license-GPL--3.0-orange.svg)](COPYRIGHT)

## The R3BRoot Framework
The R3BRoot software is based on the FairRoot framework and can be used to perform Monte Carlo simulations and experimental data analysis of the R3B (Reactions with Relativistic Radioactive Beams) nuclear physics experiments at the GSI-FAIR research center (Facility for Antiproton and Ion Research). The user can create simulated data and/or perform analysis with the same framework. Geant3 and Geant4 transport engines are supported, however, the implemented tasks that create simulated data do not depend on a particular Monte Carlo engine. The framework delivers base classes which enable the users to construct their detectors and/or analysis tasks in a simple way, it also delivers some general functionality like track visualization. Moreover, an interface for reading experimental and/or simulated magnetic field maps is also implemented.

## Discussion Forum
For the software-related user support you can post a new topic on our [forum](https://forum.gsi.de/index.php?t=index&cat=40&).

## License
R3BRoot is distributed under the terms of the GNU General Public Licence version 3 ([GPLv3](https://github.com/R3BRootGroup/R3BRoot/blob/dev/LICENSE)).

## Release Information
Please visit [releases](https://github.com/R3BRootGroup/R3BRoot/releases)

## Download

~~~bash
git clone -b dev https://github.com/R3BRootGroup/R3BRoot.git
cd R3BRoot
git clone -b dev https://github.com/R3BRootGroup/macros.git
~~~

## Step by Step Installation

### Required Software
First, you will need to install FairSoft and FairRoot. For more details:

1. Install [FairSoft](https://github.com/FairRootGroup/FairSoft)

2. Install [FairRoot](http://fairroot.gsi.de)

### Configure and Compile

~~~bash
from %R3BRoot_DIRECTORY%
cd ..
mkdir build
cd build
export SIMPATH=%PATH_TO_FAIRSOFT%
export FAIRROOTPATH=%PATH_TO_FAIRROOT%
cmake ../R3BRoot
. config.sh
make -j6
~~~

To run the detector tests do:

~~~bash
 # To run test: make new shell, do not define SIMPATH neither FAIRROOTPATH
cd ~/R3BRoot_build_dir
make -j8 test
~~~

## Simulations and Data Analysis
This is performed from the GitHub parameter and data analysis repository, which contains all the macros and parameter files needed by the user to carry out the simulations and data analysis of each experiment. There is one repository per experiment, please, visit the [R3B-Wiki](https://wiki.r3b-nustar.de) for more details.

