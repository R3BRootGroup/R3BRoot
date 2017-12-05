// -------------------------------------------------------------------------
// -----                  R3BDetectorList.header file                  -----
// -----                 Created 11/02/09  by D.Bertini                -----
// -------------------------------------------------------------------------

/** Unique identifier for all R3B detector systems **/

#ifndef R3BDETECTORLIST_H
#define R3BDETECTORLIST_H 1

enum DetectorId
{
    kREF,
    kDCH,
    kCAL,
    kLAND,
    kGFI,
    kMTOF,
    kDTOF,
    kTOF,
    kTRA,
    kCALIFA,
    kMFI,
    kPSP,
    kVETO,
    kSTARTRACK,
    kLUMON,
    kNEULAND,
    kACTAR,
    kFI4,
    kFI6,
    kFI5,
    kSFI,
#ifdef SOFIA
    kSOFSCI,
    kSOFAT,
#endif
    kLAST
};

/** Unique identifier for all R3B Point and Hit types **/

enum fDetectorType
{
    kUnknown,
    kDchPoint,
    kCalPoint,
    kLandPoint,
    kGfiPoint,
    kmTofPoint,
    kdTofPoint,
    kTofPoint,
    kTraPoint,
    kCalifaPoint,
    kMfiPoint,
    kPspPoint,
    kVetoPoint,
    kStartrackPoint,
    kLuMonPoint,
    kNeulandPoint,
    kFI4Point,
    kFI6Point,
    kFI5Point,
    kSFIPoint
#ifdef SOFIA
    ,kSOFSCIPoint
    ,kSOFATPoint
#endif
};

enum SensorSide
{
    kTOP,
    kBOTTOM
};

#endif
