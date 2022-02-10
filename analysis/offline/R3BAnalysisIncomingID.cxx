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
 *
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
#include "R3BSamplerMappedData.h"
#include "R3BSci2HitData.h"
#include "R3BSci2TcalData.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <algorithm>
#include <array>
#include <vector>

R3BAnalysisIncomingID::R3BAnalysisIncomingID()
    : R3BAnalysisIncomingID("AnalysisIncomingID", 1)
{
}

R3BAnalysisIncomingID::R3BAnalysisIncomingID(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fHeader(NULL)
    , fHitSci2(NULL)
    , fHitItemsMus(NULL)
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
    LOG(INFO) << "R3BAnalysisIncomingID::SetParContainers()";
    // Parameter Container
    // Reading IncomingIDPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(FATAL, !rtdb, "FairRuntimeDb not found");

    fIncomingID_Par = (R3BIncomingIDPar*)rtdb->getContainer("IncomingIDPar");

    if (!fIncomingID_Par)
    {
        LOG(ERROR) << "R3BAnalysisIncomingIDPar:: Couldn't get handle on R3BincomingIDPar container";
    }
    else
    {
        LOG(INFO) << "R3BAnalysisIncomingIDPar:: R3BincomingIDParcontainer open";
    }

    R3BMusicHitPar* fCal_Par; /// **< Parameter container. >* //
    fCal_Par = (R3BMusicHitPar*)rtdb->getContainer("musicHitPar");
    if (!fCal_Par)
    {
        LOG(ERROR) << "R3BAnalysisIncomingIDPar::Init() Couldn't get handle on musicHitPar container";
    }
    else
    {
        LOG(INFO) << "R3BAnalysisIncomingIDPar:: musicHitPar container open";
    }

    //--- Parameter Container ---
    fNumMusicParams = fCal_Par->GetNumParZFit(); // Number of Parameters
    LOG(INFO) << "R3BAnalysisIncomingIDPar::R3BMusicCal2Hit: Nb parameters for charge-Z: " << (Int_t)fNumMusicParams;
    CalZParams = new TArrayF();
    CalZParams->Set(fNumMusicParams);
    CalZParams = fCal_Par->GetZHitPar(); // Array with the Cal parameters

    // Parameters detector
    if (fNumMusicParams == 2)
    {
        fZ0 = CalZParams->GetAt(0);
        fZ1 = CalZParams->GetAt(1);
    }
    else if (fNumMusicParams == 3)
    {
        fZ0 = CalZParams->GetAt(0);
        fZ1 = CalZParams->GetAt(1);
        fZ2 = CalZParams->GetAt(2);
    }
    else
        LOG(INFO)
            << "R3BAnalysisIncomingIDPar:: R3BMusicCal2Hit parameters for charge-Z cannot be used here, number of "
               "parameters: "
            << fNumMusicParams;
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
    return;
}

InitStatus R3BAnalysisIncomingID::Init()
{
    R3BLOG(INFO, "");

    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    // fHeader = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    // Get access to Sci2 data at hit level
    fHitSci2 = (TClonesArray*)mgr->GetObject("Sci2Hit");
    R3BLOG_IF(WARNING, !fHitSci2, "Could not find Sci2Hit");

    // Get access to hit data of the MUSIC
    fHitItemsMus = (TClonesArray*)mgr->GetObject("MusicHitData");
    R3BLOG_IF(WARNING, !fHitItemsMus, "MusicHitData not found");

    // Get access to hit data of the LOS
    fHitLos = (TClonesArray*)mgr->GetObject("LosHit");
    R3BLOG_IF(WARNING, !fHitLos, "LosHit not found");

    // Output data
    fFrsDataCA = new TClonesArray("R3BFrsData");
    mgr->Register("FrsData", "Analysis FRS", fFrsDataCA, !fOnline);

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
    // Reset entries in output arrays
    Reset();

    double Zmusic = 0., Music_ang = 0.;
    if (fHitItemsMus && fHitItemsMus->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsMus->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BMusicHitData* hit = (R3BMusicHitData*)fHitItemsMus->At(ihit);
            if (!hit)
                continue;
            Zmusic = hit->GetZcharge();
            Music_ang = hit->GetTheta() * 1000.; // mrad
        }
    }

    // --- local variables --- //
    Double_t timeLosV[fNumDet];
    Double_t posLosX_cm[fNumDet];
    Double_t Zlos[fNumDet];
    Double_t TimeSci2_m1[fNumDet];
    Double_t PosSci2_m1[fNumDet];
    UInt_t nHits = 0;
    Double_t ToFraw_m1 = 0., PosCal_m1 = 0.;
    Double_t Velo_m1 = 0., Beta_m1 = 0., Gamma_m1 = 0., Brho_m1 = 0., AoQ_m1 = 0.;
    Double_t AoQ_m1_corr = 0.;

    Int_t multSci2[fNumDet];
    Int_t multLos[fNumDet];

    for (Int_t i = 0; i < fNumDet; i++)
    {
        multSci2[i] = 0;
        multLos[i] = 0;
    }

    // --- read hit from Sci2 data --- //
    if (fHitSci2 && fHitSci2->GetEntriesFast())
    {
        Int_t numDet;
        for (Int_t i = 0; i < fNumDet; i++)
        {
            PosSci2_m1[i] = 0.;
            TimeSci2_m1[i] = 0.;
        }

        nHits = fHitSci2->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BSci2HitData* hittcal = (R3BSci2HitData*)fHitSci2->At(ihit);
            numDet = hittcal->GetSciId();
            if (multSci2[numDet - 1] == 0)
            {
                PosSci2_m1[numDet - 1] = hittcal->GetX();
                TimeSci2_m1[numDet - 1] = hittcal->GetTime();
                multSci2[numDet - 1]++;
            }
        } // --- end of loop over hit data --- //
    }

    // --- read hit from LOS data --- //
    if (fHitLos && fHitLos->GetEntriesFast())
    {
        Int_t numDet;
        for (Int_t i = 0; i < fNumDet; i++)
        {
            timeLosV[i] = 0.;
        }

        nHits = fHitLos->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BLosHitData* hittcal = (R3BLosHitData*)fHitLos->At(ihit);
            numDet = hittcal->GetDetector();

            if (multLos[numDet - 1] == 0)
            {
                timeLosV[numDet - 1] = hittcal->GetTime();
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

        if (multLos[i] >= 1 && multSci2[i] >= 1)
        {
            ToFraw_m1 = timeLosV[i] - TimeSci2_m1[i];
            if (ToFraw_m1 > 0)
                ToFraw_m1 = ToFraw_m1 - 40960.;
            Velo_m1 =
                1. / (fTof2InvV_p0->GetAt(i) + fTof2InvV_p1->GetAt(i) * (fToFoffset->GetAt(i) + ToFraw_m1)); // [m/ns]
            Beta_m1 = Velo_m1 / 0.299792458;

            if (Beta_m1 < fBeta_max && Beta_m1 > fBeta_min)
            {
                Gamma_m1 = 1. / (TMath::Sqrt(1. - TMath::Power(Beta_m1, 2)));
                PosCal_m1 = PosSci2_m1[i]; // [mm] at S2
                Brho_m1 = fBrho0_S2toCC->GetAt(i) * (1. - PosCal_m1 / fDispersionS2->GetAt(i));
                AoQ_m1 = Brho_m1 / (3.10716 * Beta_m1 * Gamma_m1);
                AoQ_m1_corr = fy0_Aq + (posLosX_cm[i] - fx0_Aq) * sin(fang_Aq) + (AoQ_m1 - fy0_Aq) * cos(fang_Aq);

                if (fCutS2 && fCutS2->IsInside(PosSci2_m1[i], AoQ_m1_corr))
                {
                    if (Zmusic > 0. && !fUseLOS)
                    {
                        double Emus = ((Zmusic + 4.7) / 0.28) * ((Zmusic + 4.7) / 0.28);
                        double zcor = sqrt(Emus * Beta_m1) * 0.277;

                        AddData(1, 2, zcor, AoQ_m1_corr, Beta_m1, Brho_m1, PosCal_m1, 0.);
                    }

                    if (Zlos[i] > 0. && fUseLOS)
                    {
                        if (fCutCave && fCutCave->IsInside(AoQ_m1_corr, Zlos[i]))
                        {
                            AddData(1, 2, Zlos[i], AoQ_m1_corr, Beta_m1, Brho_m1, PosCal_m1, 0.);
                        }
                        else if (!fCutCave)
                        {
                            AddData(1, 2, Zlos[i], AoQ_m1_corr, Beta_m1, Brho_m1, PosCal_m1, 0.);
                        }
                    }
                }

                else if (!fCutS2)
                {
                    if (Zmusic > 0. && !fUseLOS)
                    {
                        double Emus = ((Zmusic + 4.7) / 0.28) * ((Zmusic + 4.7) / 0.28);
                        double zcor = sqrt(Emus * Beta_m1) * 0.277;

                        AddData(1, 2, zcor, AoQ_m1_corr, Beta_m1, Brho_m1, PosCal_m1, 0.);
                    }

                    if (Zlos[i] > 0. && fUseLOS)
                    {
                        if (fCutCave && fCutCave->IsInside(AoQ_m1_corr, Zlos[i]))
                        {
                            AddData(1, 2, Zlos[i], AoQ_m1_corr, Beta_m1, Brho_m1, PosCal_m1, 0.);
                        }
                        else if (!fCutCave)
                        {
                            AddData(1, 2, Zlos[i], AoQ_m1_corr, Beta_m1, Brho_m1, PosCal_m1, 0.);
                        }
                    }
                }
            }
        }
    }
}

void R3BAnalysisIncomingID::FinishEvent()
{
    if (fHitSci2)
        fHitSci2->Clear();
    if (fHitLos)
        fHitLos->Clear();
    if (fHitItemsMus)
        fHitItemsMus->Clear();
}

void R3BAnalysisIncomingID::Reset()
{
    R3BLOG(DEBUG1, "Clearing FrsData Structure");
    if (fFrsDataCA)
        fFrsDataCA->Clear();
}

// -----   Private method AddData  --------------------------------------------
R3BFrsData* R3BAnalysisIncomingID::AddData(Int_t StaId,
                                           Int_t StoId,
                                           Double_t z,
                                           Double_t aq,
                                           Double_t beta,
                                           Double_t brho,
                                           Double_t xs2,
                                           Double_t xc)
{
    // It fills the R3BFrsData
    TClonesArray& clref = *fFrsDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BFrsData(StaId, StoId, z, aq, beta, brho, xs2, xc);
}

ClassImp(R3BAnalysisIncomingID);
