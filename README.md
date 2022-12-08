# Purpose

The R3BRoot software is based on the FairRoot framework and can be used to perform Monte Carlo simulations and experimental data analysis of the R3B (Reactions with Relativistic Radioactive Beams) nuclear physics experiments at the FAIR research center (Facility for Antiproton and Ion Research).

For the software-related user support you can post a new topic on our [forum](https://forum.gsi.de/index.php?t=index&cat=40&).

# Download

~~~bash
git clone https://github.com/R3BRootGroup/R3BRoot.git
cd R3BRoot
git checkout dev
git clone https://github.com/R3BRootGroup/macros.git
cd macros
git checkout dev
~~~

# Install

### Required software

First, you will need to install FairSoft and FairRoot. For more details: [instructions](https://www.r3broot.gsi.de/installation).

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

# Simulations and Data Analysis

This is performed from the GitHub parameter and data analysis repository, which contains all the macros and parameter files needed by the user to carry out the simulations and data analysis of each experiment. There is one repository per experiment, please, visit the R3B Wiki for more details.
