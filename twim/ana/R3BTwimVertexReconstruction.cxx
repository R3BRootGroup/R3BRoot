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

// -------------------------------------------------------------
// -----           R3BTwimVertexReconstruction source file              -----
// -----    Created 30/11/19 by J.L. Rodriguez-Sanchez     -----
// -------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TF1.h"
#include "TMath.h"
#include "TVector3.h"
#include "TVectorD.h"

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Twim headers
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BTwimHitData.h"
#include "R3BTwimVertexReconstruction.h"

// R3BTwimVertexReconstruction: Default Constructor --------------------------
R3BTwimVertexReconstruction::R3BTwimVertexReconstruction()
    : R3BTwimVertexReconstruction("R3BTwimVertexReconstruction", 1)
{
}

// R3BTwimVertexReconstructionPar: Standard Constructor --------------------------
R3BTwimVertexReconstruction::R3BTwimVertexReconstruction(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTwimHitDataCA(NULL)
    , f1(NULL)
    , f2(NULL)
    , fTpat(-1)
{
}

// Virtual R3BTwimVertexReconstruction: Destructor
R3BTwimVertexReconstruction::~R3BTwimVertexReconstruction()
{
    R3BLOG(DEBUG1, "Destructor");
    if (fTwimHitDataCA)
        delete fTwimHitDataCA;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BTwimVertexReconstruction::Init()
{
    R3BLOG(INFO, "");
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        R3BLOG(FATAL, "FairRootManager not found");
        return kFATAL;
    }

    header = (R3BEventHeader*)rootManager->GetObject("EventHeader.");
    if (!header)
        header = (R3BEventHeader*)rootManager->GetObject("R3BEventHeader");

    // INPUT DATA
    // get access to cal data of the Twim
    fTwimHitDataCA = (TClonesArray*)rootManager->GetObject("TwimHitData");
    if (!fTwimHitDataCA)
    {
        R3BLOG(FATAL, "TwimHitData not found");
        return kFATAL;
    }

    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BTwimVertexReconstruction::Exec(Option_t* option)
{
    if ((fTpat >= 0) && (header) && ((header->GetTpat() & fTpat) != fTpat))
        return;

    Int_t nHitTwim = fTwimHitDataCA->GetEntriesFast();

    TVector3 trajparams[2];
    trajparams[0].SetXYZ(-1000., -1000., 0.);
    trajparams[1].SetXYZ(-1000., -1000., 0.);

    if (nHitTwim == 2)
    {
        R3BTwimHitData** HitDat = new R3BTwimHitData*[nHitTwim];

        for (Int_t i = 0; i < nHitTwim; i++)
        {
            HitDat[i] = (R3BTwimHitData*)(fTwimHitDataCA->At(i));
            trajparams[i].SetXYZ(HitDat[i]->GetSecID(), HitDat[i]->GetOffset(), HitDat[i]->GetTheta());
        }

        f1 = new TF1("f1", "pol1", -4000, 4000);
        f2 = new TF1("f2", "pol1", -4000, 4000);
        f1->SetParameter(0, trajparams[0].Y());
        f1->SetParameter(1, trajparams[0].Z());
        f2->SetParameter(0, trajparams[1].Y());
        f2->SetParameter(1, trajparams[1].Z());
        Double_t min = 2000;
        for (Int_t i = -4000; i < -20; i++)
        {
            if (min > TMath::Abs(f1->Eval(i) - f2->Eval(i)))
            {
                min = TMath::Abs(f1->Eval(i) - f2->Eval(i));
                HitDat[0]->SetVertexZ(i);
                HitDat[1]->SetVertexZ(i);
                HitDat[0]->SetVertexX(f1->Eval(i));
                HitDat[1]->SetVertexX(f2->Eval(i));
            }
        }

        if (HitDat)
            delete[] HitDat;

    } // if nHitTwim == 2

    return;
}

ClassImp(R3BTwimVertexReconstruction);
