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

// ----------------------------------------------------------------
// -----         R3BAnalysisIncomingID source file            -----
// -----     Created 01/11/21 by M. Feijoo Fontan             -----
// ----------------------------------------------------------------

/*
 * This task should make the analysis of the incoming projectiles from FRS
 */

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "R3BAnalysisIncomingID.h"
#include "R3BEventHeader.h"
#include "R3BIncomingIDPar.h"
#include "R3BLogger.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BLosMappedData.h"
#include "R3BMusicHitData.h"
#include "R3BMusicHitPar.h"
#include "R3BMusliHitData.h"

#include "TClonesArray.h"
#include "TMath.h"

R3BAnalysisIncomingID::R3BAnalysisIncomingID()
    : R3BAnalysisIncomingID("AnalysisIncomingID", 1)
{
}

R3BAnalysisIncomingID::R3BAnalysisIncomingID(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fHeader(NULL)
    , fHitItemsMus(NULL)
    , fHitItemsMusli(NULL)
    , fFrsDataCA(NULL)
    , fPos_p0(-11)
    , fPos_p1(54.7)
    , fP0(-2.12371e7)
    , fP1(4.9473e7)
    , fP2(-2.87635e7)
    , fZprimary(50.)
    , fZoffset(-1.3)
    , fOnline(kFALSE)
    , fIncomingID_Par(NULL)
    , fNumDet(1)
    , fUseLOS(kTRUE)
    , fCutS2(NULL)
{
    fToFoffset = new TArrayF(fNumDet);
    fPosS2Left = new TArrayF(fNumDet);
    fPosS2Right = new TArrayF(fNumDet);
    fBrho0_S2toCC = new TArrayF(fNumDet);
    fDispersionS2 = new TArrayF(fNumDet);
    fTof2InvV_p0 = new TArrayF(fNumDet);
    fTof2InvV_p1 = new TArrayF(fNumDet);
}

R3BAnalysisIncomingID::~R3BAnalysisIncomingID()
{
    R3BLOG(DEBUG1, "");
    if (fFrsDataCA)
        delete fFrsDataCA;
}

void R3BAnalysisIncomingID::SetParContainers()
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

void R3BAnalysisIncomingID::SetParameter()
{
    //--- Parameter Container ---
    fx0_point = fIncomingID_Par->Getx0_point();
    fy0_point = fIncomingID_Par->Gety0_point();
    frot_ang = fIncomingID_Par->Getrot_ang();
    fx0_Aq = fIncomingID_Par->Getx0_Aq();
    fy0_Aq = fIncomingID_Par->Gety0_Aq();
    fang_Aq = fIncomingID_Par->Getang_Aq();
    fBeta_min = fIncomingID_Par->GetBeta_min();
    fBeta_max = fIncomingID_Par->GetBeta_max();

    fCutS2 = fIncomingID_Par->GetCutS2();
    fCutCave = fIncomingID_Par->GetCutCave();

    for (Int_t i = 1; i < fNumDet + 1; i++)
    {
        fToFoffset->AddAt(fIncomingID_Par->GetToFoffset(i), i - 1);
        fPosS2Left->AddAt(fIncomingID_Par->GetPosS2Left(i), i - 1);
        fPosS2Right->AddAt(fIncomingID_Par->GetPosS2Right(i), i - 1);
        fTof2InvV_p0->AddAt(fIncomingID_Par->GetTof2InvV_p0(i), i - 1);
        fTof2InvV_p1->AddAt(fIncomingID_Par->GetTof2InvV_p1(i), i - 1);
        fDispersionS2->AddAt(fIncomingID_Par->GetDispersionS2(i), i - 1);
        fBrho0_S2toCC->AddAt(fIncomingID_Par->GetBrho0_S2toCC(i), i - 1);
    }
    R3BLOG(INFO, "Brho:" << fBrho0_S2toCC->GetAt(0));
    return;
}

InitStatus R3BAnalysisIncomingID::Init()
{
    R3BLOG(INFO, "");

    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    fHeader = (R3BEventHeader*)mgr->GetObject("EventHeader.");

    // Get access to hit data of the MUSIC
    fHitItemsMus = (TClonesArray*)mgr->GetObject("MusicHitData");
    R3BLOG_IF(WARNING, !fHitItemsMus, "MusicHitData not found");

    fHitItemsMusli = (TClonesArray*)mgr->GetObject("MusliHitData");
    R3BLOG_IF(WARNING, !fHitItemsMusli, "MusliHitData not found");

    // Get access to hit data of the LOS
    fHitLos = (TClonesArray*)mgr->GetObject("LosHit");
    R3BLOG_IF(WARNING, !fHitLos, "LosHit not found");

    // Output data
    fFrsDataCA = (TClonesArray*)mgr->GetObject("FrsData");
    if (fFrsDataCA == NULL)
    {
        fFrsDataCA = new TClonesArray("R3BFrsData");
        mgr->Register("FrsData", "Analysis FRS", fFrsDataCA, !fOnline);
    }

    SetParameter();

    return kSUCCESS;
}

InitStatus R3BAnalysisIncomingID::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BAnalysisIncomingID::Exec(Option_t* option)
{
    double Zmusic = 0., Music_ang = 0.;
    if (!fHitItemsMusli && fHitItemsMus && fHitItemsMus->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsMus->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BMusicHitData*)fHitItemsMus->At(ihit);
            if (!hit)
                continue;
            Zmusic = hit->GetZcharge();
            Music_ang = hit->GetTheta() * 1000.; // mrad
        }
    }

    if (!fHitItemsMus && fHitItemsMusli && fHitItemsMusli->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsMusli->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BMusliHitData*)fHitItemsMusli->At(ihit);
            if (!hit)
                continue;
            if (hit->GetType() == 2)
                Zmusic = hit->GetZcharge(); // for data with mean signals from 4 anodes
            if (hit->GetType() == 1)
                Music_ang = hit->GetTheta() * 1000.; // mrad, for data with mean signals from 2 anodes
        }
    }

    // --- local variables --- //
    Double_t Zlos[fNumDet];
    UInt_t nHits = 0;
    Double_t posLosX_cm[fNumDet];
    Double_t Gamma_m1 = 0., Brho_m1 = 0., AoQ_m1 = 0.;
    Double_t AoQ_m1_corr = 0.;
    Int_t multLos[fNumDet];

    for (Int_t i = 0; i < fNumDet; i++)
    {
        multLos[i] = 0;
        Zlos[i] = 0.;
        posLosX_cm[i] = 0.;
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
            if (multLos[numDet - 1] == 0)
            {
                posLosX_cm[numDet - 1] = hittcal->GetX_cm();
                Zlos[numDet - 1] = hittcal->GetZ();
                multLos[numDet - 1]++;
            }
        } // --- end of loop over hit data --- //
    }

    for (int i = 0; i < fNumDet; i++)
    {
        // --- secondary beam identification ---

        // if X is increasing from left to right:
        //    Brho = fBhro0 * (1 - xMwpc0/fDCC + xS2/fDS2)
        // in R3BRoot, X is increasing from right to left
        //    Bro = fBrho0 * (1 + xMwpc0/fDCC - xS2/fDS2)

        if (multLos[i] >= 1)
        {
            Double_t betaS2 = 0.;
            Double_t PosXS2 = 0.;
            R3BFrsData* hitfrs = nullptr;
            if (fFrsDataCA && fFrsDataCA->GetEntriesFast() > 0)
            {
                nHits = fFrsDataCA->GetEntriesFast();
                R3BLOG_IF(ERROR, nHits > 1, "Multiplicity from FRS detector larger than 1: " << nHits);
                for (Int_t ihit = 0; ihit < nHits; ihit++)
                {
                    hitfrs = (R3BFrsData*)fFrsDataCA->At(ihit);
                    if (!hitfrs)
                        continue;
                    betaS2 = hitfrs->GetBeta();
                    PosXS2 = hitfrs->GetXS2();
                }
            }

            if (betaS2 < fBeta_max && betaS2 > fBeta_min)
            {
                Gamma_m1 = 1. / (TMath::Sqrt(1. - TMath::Power(betaS2, 2)));
                Brho_m1 = fBrho0_S2toCC->GetAt(i) * (1. - PosXS2 / fDispersionS2->GetAt(i));
                AoQ_m1 = Brho_m1 / (3.10716 * betaS2 * Gamma_m1);
                AoQ_m1_corr = fy0_Aq + (posLosX_cm[i] - fx0_Aq) * sin(fang_Aq) + (AoQ_m1 - fy0_Aq) * cos(fang_Aq);

                if (fCutS2 && fCutS2->IsInside(PosXS2, AoQ_m1_corr))
                {
                    if (Zmusic > 0. && !fUseLOS)
                    {
                        // double Emus = ((Zmusic + 4.7) / 0.28) * ((Zmusic + 4.7) / 0.28);
                        // double zcor = sqrt(Emus * Beta_m1) * 0.277;
                        if (fCutCave && fCutCave->IsInside(AoQ_m1_corr, Zmusic))
                        {
                            hitfrs->SetZ(Zmusic);
                            hitfrs->SetAq(AoQ_m1_corr);
                            hitfrs->SetBrho(Brho_m1);
                        }
                        else if (!fCutCave)
                        {
                            hitfrs->SetZ(Zmusic);
                            hitfrs->SetAq(AoQ_m1_corr);
                            hitfrs->SetBrho(Brho_m1);
                        }
                    }

                    if (Zlos[i] > 0. && fUseLOS)
                    {
                        if (fCutCave && fCutCave->IsInside(AoQ_m1_corr, Zlos[i]))
                        {
                            hitfrs->SetZ(Zlos[i]);
                            hitfrs->SetAq(AoQ_m1_corr);
                            hitfrs->SetBrho(Brho_m1);
                        }
                        else if (!fCutCave)
                        {
                            hitfrs->SetZ(Zlos[i]);
                            hitfrs->SetAq(AoQ_m1_corr);
                            hitfrs->SetBrho(Brho_m1);
                        }
                    }
                }
                else if (!fCutS2)
                {
                    if (Zmusic > 0. && !fUseLOS)
                    {
                        // double Emus = ((Zmusic + 4.7) / 0.28) * ((Zmusic + 4.7) / 0.28);
                        // double zcor = sqrt(Emus * Beta_m1) * 0.277;
                        if (fCutCave && fCutCave->IsInside(AoQ_m1_corr, Zmusic))
                        {
                            hitfrs->SetZ(Zmusic);
                            hitfrs->SetAq(AoQ_m1_corr);
                            hitfrs->SetBrho(Brho_m1);
                        }
                        else if (!fCutCave)
                        {
                            hitfrs->SetZ(Zmusic);
                            hitfrs->SetAq(AoQ_m1_corr);
                            hitfrs->SetBrho(Brho_m1);
                        }
                    }

                    if (Zlos[i] > 0. && fUseLOS)
                    {
                        if (fCutCave && fCutCave->IsInside(AoQ_m1_corr, Zlos[i]))
                        {
                            hitfrs->SetZ(Zlos[i]);
                            hitfrs->SetAq(AoQ_m1_corr);
                            hitfrs->SetBrho(Brho_m1);
                        }
                        else if (!fCutCave)
                        {
                            hitfrs->SetZ(Zlos[i]);
                            hitfrs->SetAq(AoQ_m1_corr);
                            hitfrs->SetBrho(Brho_m1);
                        }
                    }
                }
            }
        }
    }
}

void R3BAnalysisIncomingID::FinishEvent()
{
    if (fHitLos)
        fHitLos->Clear();
    if (fHitItemsMus)
        fHitItemsMus->Clear();
    if (fHitItemsMusli)
        fHitItemsMusli->Clear();
    if (fFrsDataCA)
        fFrsDataCA->Clear();
}

ClassImp(R3BAnalysisIncomingID);
