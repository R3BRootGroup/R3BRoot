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

#include "R3BBeamMonitorMappedData.h"
#include "FairLogger.h"
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
#include <iostream>

using namespace std;

R3BBeamMonitorMappedData::R3BBeamMonitorMappedData()
    : fIC(-1)      // IC
    , fSEETRAM(-1) // SEETRAM
    , fTOFDOR(-1)  // TOFD or
    , fSROLU1(-1)
    , fSROLU2(-1)
{
}

R3BBeamMonitorMappedData::R3BBeamMonitorMappedData(UInt_t IC,
                                                   UInt_t SEETRAM,
                                                   UInt_t TOFDOR,
                                                   UInt_t SROLU1,
                                                   UInt_t SROLU2)
    : fIC(IC)
    , fSEETRAM(SEETRAM)
    , fTOFDOR(TOFDOR)
    , fSROLU1(SROLU1)
    , fSROLU2(SROLU2)
{
    /*
     LOG(INFO)<<"LosBeamMonitorData: "<<IC<<",  "<<SEETRAM<<", "<<TOFDOR;

    */
}

UInt_t R3BBeamMonitorMappedData::GetIC() const { return fIC; }

UInt_t R3BBeamMonitorMappedData::GetSEETRAM() const { return fSEETRAM; }

UInt_t R3BBeamMonitorMappedData::GetTOFDOR() const { return fTOFDOR; }

UInt_t R3BBeamMonitorMappedData::GetSROLU1() const { return fSROLU1; }

UInt_t R3BBeamMonitorMappedData::GetSROLU2() const { return fSROLU2; }

ClassImp(R3BBeamMonitorMappedData)
