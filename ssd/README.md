# Overview of the Silicon tracker classes

TClonesArray data (typically FairMultiLinkedData)
Event data on various levels.

Generally in

- ../r3bdata/trackerData/. for simulation
- ../r3bdata/amsData/. for analysis of AMS silicon detectors

### (Main) FairTasks:
Classes generating new, higher level TClonesArray data from existing data.

Tasks for AMS detectors are

- ./calibration/R3BAmsMapped2StripCal
- ./calibration/R3BAmsStripCal2Hit

### Parameter containers

These files generally hold channel-specific data used for calibration and the like.

- ./pars/R3BAmsMappingPar
- ./pars/R3BAmsStripCalPar
- ./pars/R3BTraContFact for managing all the parameter containers

### Online FairTasks

The main online classes are

- ./online/R3BAmsOnlineSpectra
- ./online/R3BAmsCalifaCorrelatedOnlineSpectra, only for 2018 and 2023 experiments

### other

- ./TraLinkDef.h       -- linking directives for CINT.

#  Tracker processing chains

### Simulation branch

The main simulation class is

- ./sim/R3BTra

which generates a data structure called ../r3bdata/trackerData/R3BTraPoint.h

### Unpack / experiment data branch

The AMS data unpacker is ../r3bsource/R3BAmsReader, 
which is involved in parsing the lmd (with help from ucesb) to generate a TCloneArray of

- ../r3bdata/amsData/R3BAmsMappedData.h.

This is then read by ./calibration/R3BAmsMapped2StripCal.h to generate a TCloneArray of 

- ../r3bdata/amsData/R3BAmsStripCalData.h

by applying per-channel and per-experiment parameters from ./pars/R3BAmsStripCalPar.h 

### Processing to hit level clustering

This takes data from a TCloneArray of the class

- ../r3bdata/amsData/R3BAmsStripCalData.h

which is converted by

- ./calibration/R3BAmsStripCal2Hit

in clusters that provide the position and energy of each particle/ion going through the silicon detector 
and then it fills a TCloneArray of the class called

- ../r3bdata/amsData/R3BAmsHitData.h



