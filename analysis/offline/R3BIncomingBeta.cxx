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

// ------------------------------------------------------------
// -----               R3BIncomingBeta                    -----
// -----    Created 10/05/22 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------
#define MAXMULT 64

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "R3BEventHeader.h"
#include "R3BIncomingBeta.h"
#include "R3BIncomingIDPar.h"
#include "R3BLogger.h"
#include "R3BLosHitData.h"
#include "R3BSci2HitData.h"
#include "R3BTimeStitch.h"

#include "TClonesArray.h"
#include "TMath.h"

R3BIncomingBeta::R3BIncomingBeta()
    : R3BIncomingBeta("IncomingBeta", 1)
{
}

R3BIncomingBeta::R3BIncomingBeta(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fHeader(NULL)
    , fHitSci2(NULL)
    , fFrsDataCA(NULL)
    , fPos_p0(-11)
    , fPos_p1(54.7)
    , fP0(-2.12371e7)
    , fP1(4.9473e7)
    , fP2(-2.87635e7)
    , fZprimary(50.)
    , fZoffset(-1.3)
    , fOnline(kFALSE)
    , fTimeStitch(nullptr)
    , fIncomingID_Par(NULL)
    , fNumDet(1)
    , fUseTref(kFALSE)
{
    fToFoffset = new TArrayF(fNumDet);
    fPosS2Left = new TArrayF(fNumDet);
    fPosS2Right = new TArrayF(fNumDet);
    fTof2InvV_p0 = new TArrayF(fNumDet);
    fTof2InvV_p1 = new TArrayF(fNumDet);
}

R3BIncomingBeta::~R3BIncomingBeta()
{
    R3BLOG(DEBUG1, "");
    if (fFrsDataCA)
        delete fFrsDataCA;
}

void R3BIncomingBeta::SetParContainers()
{
    R3BLOG(INFO, "");
    // Reading IncomingIDPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(FATAL, !rtdb, "FairRuntimeDb not found");

    fIncomingID_Par = (R3BIncomingIDPar*)rtdb->getContainer("IncomingIDPar");
    R3BLOG_IF(FATAL, !fIncomingID_Par, "Couldn't get handle on IncomingIDPar container");
    R3BLOG_IF(INFO, fIncomingID_Par, "IncomingIDPar container was found");

    return;
}

void R3BIncomingBeta::SetParameter()
{
    //--- Parameter Container ---
    fBeta_min = fIncomingID_Par->GetBeta_min();
    fBeta_max = fIncomingID_Par->GetBeta_max();

    for (Int_t i = 1; i < fNumDet + 1; i++)
    {
        fToFoffset->AddAt(fIncomingID_Par->GetToFoffset(i), i - 1);
        fPosS2Left->AddAt(fIncomingID_Par->GetPosS2Left(i), i - 1);
        fPosS2Right->AddAt(fIncomingID_Par->GetPosS2Right(i), i - 1);
        fTof2InvV_p0->AddAt(fIncomingID_Par->GetTof2InvV_p0(i), i - 1);
        fTof2InvV_p1->AddAt(fIncomingID_Par->GetTof2InvV_p1(i), i - 1);
    }
    return;
}

InitStatus R3BIncomingBeta::Init()
{
    R3BLOG(INFO, "");
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    fHeader = (R3BEventHeader*)mgr->GetObject("EventHeader.");

    // Get access to Sci2 data at hit level
    fHitSci2 = (TClonesArray*)mgr->GetObject("Sci2Hit");
    R3BLOG_IF(WARNING, !fHitSci2, "Could not find Sci2Hit");

    // Get access to hit data of the LOS
    fHitLos = (TClonesArray*)mgr->GetObject("LosHit");
    R3BLOG_IF(WARNING, !fHitLos, "LosHit not found");

    // Output data
    fFrsDataCA = (TClonesArray*)mgr->GetObject("FrsData");
    if (fFrsDataCA == NULL)
    {
        fFrsDataCA = new TClonesArray("R3BFrsData");
        mgr->Register("FrsData", "Analysis FRS", fFrsDataCA, !fOnline);
        R3BLOG(INFO, "FrsData register done, it is OK");
    }

    SetParameter();

    // Definition of a time stich object to correlate times coming from different systems
    fTimeStitch = new R3BTimeStitch();

    return kSUCCESS;
}

InitStatus R3BIncomingBeta::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BIncomingBeta::Exec(Option_t* option)
{
    // Reset entries in output arrays
    Reset();

    // --- local variables --- //
    Double_t timeLosV[fNumDet][MAXMULT];
    Double_t posLosX_cm[fNumDet][MAXMULT];
    Double_t TimeSci2_m1[fNumDet][MAXMULT];
    Double_t TimeSci2wTref_m1[fNumDet][MAXMULT];
    Double_t PosSci2_m1[fNumDet][MAXMULT];
    UInt_t nHits = 0;
    Double_t ToFraw_m1 = 0.;
    Double_t ToFrawwTref_m1 = 0.;
    Double_t Velo_m1 = 0., VelowTref_m1 = 0., Beta_m1 = 0., Gamma_m1 = 0.;

    Int_t multSci2[fNumDet];
    Int_t multLos[fNumDet];

    for (Int_t i = 0; i < fNumDet; i++)
    {
        multSci2[i] = 0;
        multLos[i] = 0;
        for (Int_t m = 0; m < MAXMULT; m++)
        {
            PosSci2_m1[i][m] = 0.;
            TimeSci2_m1[i][m] = 0.;
            TimeSci2wTref_m1[i][m] = 0.;
            posLosX_cm[i][m] = 0.;
            timeLosV[i][m] = 0.;
        }
    }

    // --- read hit from Sci2 data --- //
    if (fHitSci2 && fHitSci2->GetEntriesFast() > 0)
    {
        Int_t numDet = 1;
        nHits = fHitSci2->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BSci2HitData* hittcal = (R3BSci2HitData*)fHitSci2->At(ihit);
            numDet = hittcal->GetSciId();
            if (numDet > fNumDet)
            {
                R3BLOG(WARNING, "Sci2 detector id:" << numDet << " is out of range!");
                continue;
            }
            if (multSci2[numDet - 1] >= MAXMULT)
                continue;
            PosSci2_m1[numDet - 1][multSci2[numDet - 1]] = hittcal->GetX();
            TimeSci2_m1[numDet - 1][multSci2[numDet - 1]] = hittcal->GetTime();
            TimeSci2wTref_m1[numDet - 1][multSci2[numDet - 1]] = hittcal->GetTimeWithTref();
            multSci2[numDet - 1]++;
        } // --- end of loop over hit data --- //
    }

    // --- read hit from LOS data --- //
    if (fHitLos && fHitLos->GetEntriesFast() > 0)
    {
        Int_t numDet = 1;

        nHits = fHitLos->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BLosHitData* hittcal = (R3BLosHitData*)fHitLos->At(ihit);
            numDet = hittcal->GetDetector();

            if (multLos[numDet - 1] >= MAXMULT)
                break;
            timeLosV[numDet - 1][multLos[numDet - 1]] = hittcal->GetTime();
            posLosX_cm[numDet - 1][multLos[numDet - 1]] = hittcal->GetX_cm();
            multLos[numDet - 1]++;
        } // --- end of loop over hit data --- //
    }

    Int_t num_tof_candidates = 0;
    for (int i = 0; i < fNumDet; i++)
    {
        for (Int_t i_L = 0; i_L < multLos[i]; i_L++)
        {
            // if (multLos[i] >= 1 && multSci2[i] >= 1)
            for (Int_t i_2 = 0; i_2 < multSci2[i]; i_2++)
            {
                if (num_tof_candidates > 0)
                    break;
                ToFraw_m1 = fTimeStitch->GetTime(timeLosV[i][i_L] - TimeSci2_m1[i][i_2], "vftx", "vftx");

                if (ToFraw_m1 > 0. && fHeader->GetExpId() == 515)
                    ToFraw_m1 = ToFraw_m1 - 40960.;

                ToFrawwTref_m1 = fTimeStitch->GetTime(fHeader->GetTStart() - TimeSci2wTref_m1[i][i_2], "vftx", "vftx");

                Velo_m1 = 1. / (fTof2InvV_p0->GetAt(i) +
                                fTof2InvV_p1->GetAt(i) * (fToFoffset->GetAt(i) + ToFraw_m1)); // [m/ns]
                VelowTref_m1 = 1. / (fTof2InvV_p0->GetAt(i) +
                                     fTof2InvV_p1->GetAt(i) * (fToFoffset->GetAt(i) + ToFrawwTref_m1)); // [m/ns]

                if (fUseTref)
                    Beta_m1 = VelowTref_m1 / 0.299792458;
                else
                    Beta_m1 = Velo_m1 / 0.299792458;

                if (fUseTref)
                    ToFraw_m1 = ToFrawwTref_m1;
                // Select good ToF hit with gating beta
                // At this moment, the multiplicity of FrsData (num_tof_candidates)
                // is restricted to be one. The beta conditions should be optimised.
                if (Beta_m1 < fBeta_max && Beta_m1 > fBeta_min)
                {
                    AddData(1, 2, 0., 0., Beta_m1, 0., PosSci2_m1[i][i_2], posLosX_cm[i][i_L], ToFraw_m1);
                    num_tof_candidates++;
                }
            }
        }
    }
}

void R3BIncomingBeta::FinishEvent()
{
    if (fHitSci2)
    {
        fHitSci2->Clear();
    }
    if (fHitLos)
    {
        fHitLos->Clear();
    }
}

void R3BIncomingBeta::Reset()
{
    R3BLOG(DEBUG1, "Clearing FrsData Structure");
    if (fFrsDataCA)
    {
        fFrsDataCA->Clear();
    }
}

// -----   Private method AddData  --------------------------------------------
R3BFrsData* R3BIncomingBeta::AddData(Int_t StaId,
                                     Int_t StoId,
                                     Double_t z,
                                     Double_t aq,
                                     Double_t beta,
                                     Double_t brho,
                                     Double_t xs2,
                                     Double_t xc,
                                     Double_t tof)
{
    // It fills the R3BFrsData
    TClonesArray& clref = *fFrsDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BFrsData(StaId, StoId, z, aq, beta, brho, xs2, xc, tof);
}

ClassImp(R3BIncomingBeta);
