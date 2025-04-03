# NeuLAND Calibration {#neuland_cal}

[TOC]

## General

NeuLAND calibration processes is to calculate the calibration relations and parameters that are used to convert the binary TDC values (Leading edge and time-over-threshold) to the physical data (`R3BNeulandHit`). The whole calibration processes can be divided into two parts:

- TDC calibration (see `R3B::Neuland::Map2CalParTask`)
  - input: mapped data (`std::vector<R3B::PaddleTamexMappedData>` and `std::vector<R3B::PaddleTamexTrigMappedData>`)
  - output: `R3B::Map2CalPar`
- Position, time and energy calibration (see `R3B::Neuland::Cal2HitParTask`)
  - input: cal data (`std::vector<R3B::Neuland::BarCalData>`)
  - output: `R3B::Neuland::Cal2HitPar`

The output parameters are used respectively for the following two data conversions:

- Mapped data to cal data (see `R3B::Neuland::Map2CalTask`)
  - input: mapped data (`std::vector<R3B::PaddleTamexMappedData>` and `std::vector<R3B::PaddleTamexTrigMappedData>`)
  - output: cal data (`std::vector<R3B::Neuland::BarCalData>`)
- Cal data to hit data (see `R3B::Neuland::Cal2HitTask`)
  - input: cal data (`std::vector<R3B::Neuland::BarCalData>`)
  - output: hit data (`std::vector<R3BNeulandHit>`)

Verification of the calibration processes can either be done via the real experimental data or through the simulations. The details of the verification through simulations can be found in the following documentation:

- \subpage neuland_sim_cal

The following sections introduce the details of data calibrations and data conversions.

## TDC calibration

The TDC calibrations converts various TDC values of a signal to the its real times, which include the time of the leading edge and the trailing edge. The time values has the unit of **nanoseconds**. The TDC values of a signal are incorporated into two different C++ structs:

- `R3B::PaddleTamexMappedData`
- `R3B::PaddleTamexTrigMappedData`

The first struct is used by data signals while the second struct is used by the trigger signal. The purpose of trigger signals is to eliminate spurious time jumps that could occur during the beam time. The trigger signals are emitted from the starter detector (LOS detector), which are synchronized through different detectors in R3B setup. By always using the difference between signal time and trigger signal time as the time value, the value will not be changed by a time jump as the time jump always (?) shifts the times of both data signals and trigger signals equally.

## Position, time and energy calibration

## Data conversions

### From mapped data to cal data

### From cal data to hit data
