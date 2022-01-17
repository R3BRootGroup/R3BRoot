/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

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
    kTOFD,
    kTOFI,
    kTOF,
    kTRA,
    kCALIFA,
    kMFI,
    kPSP,
    kVETO,
    kSTARTRACK,
    kLUMON,
    kNEULAND,
    kDNNNEULAND,
    kDNNVETO,
    kDNNNEBULA,
    kDNNNEBULAVETO,
    kFI3a,
    kFI3b,
    kFI4,
    kFI6,
    kFI7,
    kFI8,
    kFI5,
    kFI10,
    kFI11,
    kFI12,
    kFI13,
    kFI23a,
    kFI23b,
    kFI30,
    kFI31,
    kFI32,
    kFI33,
    kSFI,
    kMUSIC,
    kLOS,
#ifdef SOFIA
    kSOFSCI,
    kSOFAT,
    kSOFTRIM,
    kSOFMWPC0,
    kSOFMWPC1,
    kSOFTWIM,
    kSOFMWPC2,
    kSOFMWPC3,
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
    kUnknownDetector,
    kDchPoint,
    kCalPoint,
    kLandPoint,
    kGfiPoint,
    kmTofPoint,
    kTofdPoint,
    kTofiPoint,
    kTofPoint,
    kTraPoint,
    kCalifaPoint,
    kMfiPoint,
    kPspPoint,
    kVetoPoint,
    kStartrackPoint,
    kLuMonPoint,
    kNeulandPoint,
    kDNNNeulandPoint,
    kDNNVETOPoint,
    kDNNNEBULAPoint,
    kDNNNEBULAVETOPoint,
    kFI3aPoint,
    kFI3bPoint,
    kFI4Point,
    kFI6Point,
    kFI7Point,
    kFI8Point,
    kFI5Point,
    kFI10Point,
    kFI11Point,
    kFI12Point,
    kFI13Point,
    kFI23aPoint,
    kFI23bPoint,
    kFI30Point,
    kFI31Point,
    kFI32Point,
    kFI33Point,
    kSFIPoint,
    kMUSICPoint,
    kLOSPoint
#ifdef SOFIA
    ,
    kSOFSCIPoint,
    kSOFATPoint,
    kSOFTRIMPoint,
    kSOFMWPC0Point,
    kSOFMWPC1Point,
    kSOFTWIMPoint,
    kSOFMWPC2Point,
    kSOFMWPC3Point,
    kSOFTofWallPoint
#endif
#ifdef GTPC
    /* note that the value of kGTPCPoint will depend on
     * whether SOFIA is defined. --pklenze
     */
    ,
    kGTPCPoint
#endif
};

enum SensorSide
{
    kTOP,
    kBOTTOM
};

#endif
