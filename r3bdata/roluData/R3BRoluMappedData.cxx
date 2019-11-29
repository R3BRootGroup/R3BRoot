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

#include "R3BRoluMappedData.h"
#include "FairLogger.h"
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
#include <iostream>

using namespace std;

// for the data analysis of ROLU following LOS

R3BRoluMappedData::R3BRoluMappedData()
    : fDetector(-1)   // Detector number: 1...n
    , fChannel(-1)    // Channel number:  1...n (n = 4 )
    , fType(-1)       // Type: 0 =  TAMEX leading edge, 1 = TAMEX trailing edge
    , fTimeFine(-1)   // Fine time
    , fTimeCoarse(-1) // Coarse time
{
}

R3BRoluMappedData::R3BRoluMappedData(UInt_t detector, UInt_t channel, UInt_t type, UInt_t timeFine, UInt_t timeCoarse)
    : fDetector(detector)
    , fChannel(channel)
    , fType(type)
    , fTimeFine(timeFine)
    , fTimeCoarse(timeCoarse)
{
    // cout<<detector<<", "<<channel<<"; "<<type<<", "<<timeFine<<"; "<<timeCoarse<<endl;
}

UInt_t R3BRoluMappedData::GetDetector() const { return fDetector; }

UInt_t R3BRoluMappedData::GetChannel() const { return fChannel; }

UInt_t R3BRoluMappedData::GetType() const { return fType; }
UInt_t R3BRoluMappedData::GetTimeCoarse() const { return fTimeCoarse; }

UInt_t R3BRoluMappedData::GetTimeFine() const { return fTimeFine; }

ClassImp(R3BRoluMappedData)
