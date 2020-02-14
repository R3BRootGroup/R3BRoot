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
    kCADNEULAND,
    kCADVETO,
    kCADNEBULA,
    kCADNEBULAVETO,
//    kACTAR,
    kFI3a,
    kFI3b,
    kFI4,
    kFI6,
    kFI7,
    kFI8,
    kFI5,
    kFI10,
    kFI11,
    kSFI,
#ifdef SOFIA
    kSOFSCI,
    kSOFAT,
    kSOFTRIM,
    kSOFMWPC1,
    kSOFTWIM,
    kSOFMWPC2,
    kSOFTofWall,
#endif
#ifdef GTPC
    kGTPC,
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
    kCADNeulandPoint,
    kCADVETOPoint,
    kCADNEBULAPoint,
    kCADNEBULAVETOPoint,
    kFI3aPoint,
    kFI3bPoint,
    kFI4Point,
    kFI6Point,
    kFI7Point,
    kFI8Point,
    kFI5Point,
    kFI10Point,
    kFI11Point,
    kSFIPoint
#ifdef SOFIA
    ,kSOFSCIPoint
    ,kSOFATPoint
    ,kSOFTRIMPoint
    ,kSOFMWPC1Point
    ,kSOFTWIMPoint
    ,kSOFMWPC2Point
    ,kSOFTofWallPoint
#endif
#ifdef GTPC
    ,kGTPCPoint
#endif
};

enum SensorSide
{
    kTOP,
    kBOTTOM
};

#endif
