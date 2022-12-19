# R3BRoot [![license](https://alfa-ci.gsi.de/shields/badge/license-GPL--3.0-orange.svg)](COPYRIGHT)

## The R3BRoot framework
The R3BRoot software is based on the FairRoot framework and can be used to perform Monte Carlo simulations and experimental data analysis of the R3B (Reactions with Relativistic Radioactive Beams) nuclear physics experiments at the FAIR research center (Facility for Antiproton and Ion Research).

For the software-related user support you can post a new topic on our [forum](https://forum.gsi.de/index.php?t=index&cat=40&).

## License
R3BRoot is distributed under the terms of the GNU General Public Licence version 3 (GPLv3).

## Release information
Please visit [releases](https://github.com/R3BRootGroup/R3BRoot/releases)

## Download

~~~bash
git clone -b dev https://github.com/R3BRootGroup/R3BRoot.git
cd R3BRoot
git clone -b dev https://github.com/R3BRootGroup/macros.git
~~~

## Step by step installation

### Required software
First, you will need to install FairSoft and FairRoot. For more details:

1. Install [FairSoft](https://github.com/FairRootGroup/FairSoft)

2. Install [FairRoot](http://fairroot.gsi.de)

### Configure and compile

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

## Simulations and Data Analysis
This is performed from the GitHub parameter and data analysis repository, which contains all the macros and parameter files needed by the user to carry out the simulations and data analysis of each experiment. There is one repository per experiment, please, visit the R3B Wiki for more details.

