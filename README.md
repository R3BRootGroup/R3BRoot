# Purpose

The R3BRoot software is based on the FairRoot framework and can be used to perform Monte Carlo simulations and experimental data analysis of the R3B (Reactions with Relativistic Radioactive Beams) nuclear physics experiments at the FAIR research center (Facility for Antiproton and Ion Research).

For the software-related user support you can post a new topic on our [forum](https://forum.gsi.de/index.php?t=index&cat=40&).

# Download

~~~bash
git clone https://github.com/R3BRootGroup/R3BRoot.git
cd R3BRoot
git checkout dev
~~~

# Install

### Required software

First, you will need to install FairSoft and FairRoot. For more details: [instructions](https://www.r3broot.gsi.de/installation).

### Configure and compile

~~~bash
cd ..
mkdir build
cd build
export SIMPATH=%PATH_TO_FAIRSOFT%
export FAIRROOTPATH=%PATH_TO_FAIRROOT%
cmake ../R3BRoot/
make
~~~

# Simulations

~~~bash
cd %BUILD_DIRECTORY%
. ./config.sh
cd ../R3BRoot/macros/r3b/
root -l r3bsim.C
~~~

This will create output file `r3bsim.root` with the simulation results and parameter file `r3bpar.root` with geometry and magnetic field parameters.

After the simulation run:

1. To start an event display:

~~~bash
root -l eventDisplay.C
~~~

2. To perform a quick analysis with GUI:

~~~bash
root -l r3bsim.root
[] evt->StartViewer();
~~~

3. To convert the MC results into detector-like signals and open the output file:

~~~bash
root -l run_digi.C
[] .q
root -l r3bhits.root
[] evt->StartViewer();
~~~

# Data Analysis

> ...Under development...