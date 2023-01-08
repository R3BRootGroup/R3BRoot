/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStartrackSortRawAna                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.04.2014 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"

#include "R3BStartrackRawHit.h"
#include "R3BStartrackSortRawAna.h"

R3BStartrackSortRawAna::R3BStartrackSortRawAna()
    : fOutname("")
    , fOutfile(0)
    , fSortRawData(new TClonesArray("R3BStartrackRawHit"))
{
    fnEvents = 0;
}

R3BStartrackSortRawAna::~R3BStartrackSortRawAna()
{
    if (fMan)
    {
        delete fMan; // who is responsible
        fMan = 0;
    }
}

InitStatus R3BStartrackSortRawAna::Init()
{
    // FairRootManager *fMan = FairRootManager::Instance();
    fMan = FairRootManager::Instance();

    fRawData = (TClonesArray*)fMan->GetObject("StartrackRawHit");

    // Creating a new TClonearray of raw hit but time ordered
    fMan->Register("StartrackSortRawHit", "Ordered Raw data from R3B Si Tracker", fSortRawData, kTRUE);

    return kSUCCESS;
}

void R3BStartrackSortRawAna::Exec(Option_t* option)
{
    Int_t nHits = fRawData->GetEntries();
    // cout << "-I- R3BStartrackRawAna::Exec : event " << fnEvents << ", multiplicity " << nHits << endl;
    R3BStartrackRawHit* hit;

    for (Int_t i = 0; i < nHits; i++)
    {
        hit = (R3BStartrackRawHit*)fRawData->At(i);

        my_type = hit->GetWordType();
        my_hit = hit->GetHitBit();
        my_det_id = hit->GetModuleId();
        my_side = hit->GetSide();
        my_asic_id = hit->GetAsicId();
        my_ch_id = hit->GetStripId();
        my_adc_data = hit->GetADCdata();
        my_ts = hit->GetTime();
        my_info_field = hit->GetInfoField();
        my_info_code = hit->GetInfoCode();

        my_new_type = my_type;
        my_new_hit = my_hit;
        my_new_det_id = my_det_id;
        my_new_side = my_side;
        my_new_asic_id = my_asic_id;
        my_new_ch_id = my_ch_id;
        my_new_adc_data = my_adc_data;
        my_new_ts = my_ts;
        my_new_info_field = my_info_field;
        my_new_info_code = my_info_code;

        new ((*fSortRawData)[nHits]) R3BStartrackRawHit(my_new_type,
                                                        my_new_hit,
                                                        my_new_det_id,
                                                        my_new_side,
                                                        my_new_asic_id,
                                                        my_new_ch_id,
                                                        my_new_adc_data,
                                                        my_new_ts,
                                                        my_new_info_field,
                                                        my_new_info_code);
    }

    if (0 == (fnEvents % 100))
    {
        cout << fnEvents << "  " << nHits << endl;
    }
    fnEvents += 1;
}

void R3BStartrackSortRawAna::FinishTask() {}

//_____________________________________________________________________________
void R3BStartrackSortRawAna::SetOutputFile(const char* fname)
{
    fOutname = fname;
    fOutfile = fMan->OpenOutFile(fOutname);
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void R3BStartrackSortRawAna::SetOutputFile(TFile* f)
{
    fOutname = f->GetName();
    fMan->OpenOutFile(f);
    fOutfile = f;
}
//_____________________________________________________________________________

void R3BStartrackSortRawAna::CreateSortTree()
{

    /*
    FairRunOnline *run = FairRunOnline::Instance();
    run->AddObject(thw);
    run->AddObject(thh);
    run->AddObject(thm);
    run->AddObject(thsd);
    run->AddObject(tha);
    run->AddObject(thst);
    run->AddObject(the);
    run->AddObject(tht);
    run->AddObject(thif);
    run->AddObject(thic);
    */
}

void R3BStartrackSortRawAna::WriteSortTree() { LOG(info) << "R3BStartrackSortRawAna ---  writed to the Root File ..."; }

ClassImp(R3BStartrackSortRawAna)
