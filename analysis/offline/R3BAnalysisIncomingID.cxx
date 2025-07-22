/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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
// -----             R3BAnalysisIncomingID                    -----
// -----     Created 01/11/21 by M. Feijoo Fontan             -----
// ----------------------------------------------------------------

/*
 * This task should make the analysis of the incoming projectiles from FRS
 */

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include "R3BAnalysisIncomingID.h"
#include "R3BCoarseTimeStitch.h"
#include "R3BEventHeader.h"
#include "R3BFrsData.h"
#include "R3BFrsSciCalPar.h"
#include "R3BFrsSciTcalData.h"
#include "R3BIncomingIDPar.h"
#include "R3BLogger.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BLosMappedData.h"
#include "R3BLosTCalData.h"
#include "R3BMusicHitData.h"
#include "R3BMusicHitPar.h"
#include "R3BMusliHitData.h"
#include "R3BPspxHitData.h"

#include <TClonesArray.h>
#include <TMath.h>

R3BAnalysisIncomingID::R3BAnalysisIncomingID()
    : R3BAnalysisIncomingID("AnalysisIncomingID", 1)
{
}

R3BAnalysisIncomingID::R3BAnalysisIncomingID(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fPos_p0(-11)
    , fPos_p1(54.7)
    , fP0(-2.12371e7)
    , fP1(4.9473e7)
    , fP2(-2.87635e7)
    , fZprimary(50.)
    , fZoffset(-1.3)
{
}

R3BAnalysisIncomingID::~R3BAnalysisIncomingID()
{
    R3BLOG(debug1, "");
    if (fFrsHitData)
        delete fFrsHitData;
}

void R3BAnalysisIncomingID::SetParContainers()
{
    R3BLOG(info, "");
    // Reading IncomingIDPar from FairRuntimeDb
    auto rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fIncomingID_Par = dynamic_cast<R3BIncomingIDPar*>(rtdb->getContainer("IncomingIDPar"));
    R3BLOG_IF(fatal, !fIncomingID_Par, "Couldn't get handle on IncomingIDPar container");
    R3BLOG_IF(info, fIncomingID_Par, "IncomingIDPar container was found");

    fCalPar = dynamic_cast<R3BFrsSciCalPar*>(rtdb->getContainer("FrsSciCalPar"));
    R3BLOG_IF(fatal, !fCalPar, "Couldn't get handle on IncomingIDPar container");
    R3BLOG_IF(info, fCalPar, "IncomingIDPar container was found");

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

    fToFoffset = new TArrayF(fNumDet);
    fPosS2Left = new TArrayF(fNumDet);
    fPosS2Right = new TArrayF(fNumDet);
    fBrho0_S2toCC = new TArrayF(fNumDet);
    fDispersionS2 = new TArrayF(fNumDet);
    fTof2InvV_p0 = new TArrayF(fNumDet);
    fTof2InvV_p1 = new TArrayF(fNumDet);

    for (Int_t i = 1; i <= fNumDet; i++)
    {
        fToFoffset->AddAt(fIncomingID_Par->GetToFoffset(i), i - 1);
        fPosS2Left->AddAt(fIncomingID_Par->GetPosS2Left(i), i - 1);
        fPosS2Right->AddAt(fIncomingID_Par->GetPosS2Right(i), i - 1);
        fTof2InvV_p0->AddAt(fIncomingID_Par->GetTof2InvV_p0(i), i - 1);
        fTof2InvV_p1->AddAt(fIncomingID_Par->GetTof2InvV_p1(i), i - 1);
        fDispersionS2->AddAt(fIncomingID_Par->GetDispersionS2(i), i - 1);
        fBrho0_S2toCC->AddAt(fIncomingID_Par->GetBrho0_S2toCC(i), i - 1);
    }
    // fIncomingID_Par->printParams();
    // fCalPar->printParams();
    return;
}

InitStatus R3BAnalysisIncomingID::Init()
{
    R3BLOG(info, "");

    auto mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, nullptr == mgr, "FairRootManager not found");

    fHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));

    // Get access to hit data of the MUSIC
    fHitItemsMus = dynamic_cast<TClonesArray*>(mgr->GetObject("MusicHitData"));
    R3BLOG_IF(warn, !fHitItemsMus, "MusicHitData not found");

    fHitItemsMusli = dynamic_cast<TClonesArray*>(mgr->GetObject("MusliHitData"));
    R3BLOG_IF(warn, !fHitItemsMusli, "MusliHitData not found");

    fFrsSci_Tcal = dynamic_cast<TClonesArray*>(mgr->GetObject("FrsSciTcalData"));
    R3BLOG_IF(warn, !fFrsSci_Tcal, "FrsSciTcalData not found");

    // Get access to hit data of the LOS
    fHitLos = dynamic_cast<TClonesArray*>(mgr->GetObject("LosHit"));
    R3BLOG_IF(warn, !fHitLos, "LosHit not found");

    if (fUseTref)
    {
        // Get access to trigger data of the LOS
        fTriggerLos = dynamic_cast<TClonesArray*>(mgr->GetObject("LosTriggerTCal"));
        R3BLOG_IF(warn, !fTriggerLos, "LosTriggerTCal not found");
    }

    // Get access to hit data of PSPX1
    fHitPspx1_x = dynamic_cast<TClonesArray*>(mgr->GetObject("Pspx1_xHit"));
    R3BLOG_IF(warn, !fHitPspx1_x, "Pspx1_xHit not found");
    fHitPspx1_y = dynamic_cast<TClonesArray*>(mgr->GetObject("Pspx1_yHit"));
    R3BLOG_IF(warn, !fHitPspx1_y, "Pspx1_yHit not found");

    // Output data
    fFrsHitData = dynamic_cast<TClonesArray*>(mgr->GetObject("FrsData"));
    if (fFrsHitData == nullptr)
    {
        fFrsHitData = new TClonesArray("R3BFrsData");
        mgr->Register("FrsData", "Analysis FRS", fFrsHitData, !fOnline);
    }

    // Definition of a time stich object to correlate times coming from different systems
    fTimeStitch = new R3BCoarseTimeStitch();

    SetParameter();
    return kSUCCESS;
}

InitStatus R3BAnalysisIncomingID::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BAnalysisIncomingID::Exec(Option_t*)
{
    if (fHeader->GetExpId() == 249)
    {
        double Zcharge = 0.;
        size_t multLosHit = 0;
        if (fHitLos && fHitLos->GetEntriesFast() > 0)
        {
            auto nHits = fHitLos->GetEntriesFast();
            for (size_t ihit = 0; ihit < nHits; ihit++)
            {
                auto hit_los = dynamic_cast<R3BLosHitData*>(fHitLos->At(ihit));
                Zcharge = hit_los->GetZ();
                multLosHit++;
            }
        }
        // Only events with multi=1 are considered for analysis
        if (multLosHit != 1)
            return;
        // Only good charges are considered for analysis
        if (Zcharge < fMinLosCharge)
            return;

        auto fFrsSciNbDets = fCalPar->GetNumDets();
        auto fFrsSciNbPmts = fCalPar->GetNumPmts();

        std::vector<UInt_t> multFrsSciTcal(fFrsSciNbDets * fFrsSciNbPmts, 0);
        std::vector<UInt_t> multTofRaw(fFrsSciNbDets, 0);

        std::vector<std::vector<double>> FrsSciTraw;
        FrsSciTraw.resize(fFrsSciNbDets * fFrsSciNbPmts);

        for (auto& row : FrsSciTraw)
        {
            row.resize(64, 0.0);
        }

        Double_t StartTraw_atTcal = -1;

        for (size_t i = 0; i < fFrsSciNbDets; i++)
        {
            for (size_t j = 0; j < fFrsSciNbPmts; j++)
            {
                multFrsSciTcal[i * fFrsSciNbPmts + j] = 0;
                for (size_t k = 0; k < 63; k++)
                {
                    FrsSciTraw[i * fFrsSciNbPmts + j][k] = -1;
                }
            }
            multTofRaw[i] = 0;
        }

        if (fFrsSci_Tcal && fFrsSci_Tcal->GetEntriesFast() > 0)
        {
            auto nHits = fFrsSci_Tcal->GetEntriesFast();
            for (size_t ihit = 0; ihit < nHits; ihit++)
            {
                auto hitscitcal = dynamic_cast<R3BFrsSciTcalData*>(fFrsSci_Tcal->At(ihit));
                auto iDet = hitscitcal->GetDetector() - 1;
                auto iPmt = hitscitcal->GetPmt() - 1;
                FrsSciTraw[iDet * fFrsSciNbPmts + iPmt][multFrsSciTcal[iDet * fFrsSciNbPmts + iPmt]] =
                    hitscitcal->GetRawTimeNs();
                multFrsSciTcal[iDet * fFrsSciNbPmts + iPmt]++;
            }
        }

        std::vector<Int_t> indexl(2, -1);
        std::vector<Int_t> indexr(2, -1);

        for (size_t i = 0; i < fFrsSciNbDets; i++)
        {
            for (size_t hitr = 0; hitr < multFrsSciTcal[i * fFrsSciNbPmts]; hitr++)
            {
                for (size_t hitl = 0; hitl < multFrsSciTcal[i * fFrsSciNbPmts + i]; hitl++)
                {
                    StartTraw_atTcal = fTimeStitch->GetTime(
                        0.5 * (FrsSciTraw[i * fFrsSciNbPmts][hitr] + FrsSciTraw[i * fFrsSciNbPmts + 1][hitl]) -
                            FrsSciTraw[i * fFrsSciNbPmts + 2][0],
                        "vftx",
                        "vftx");

                    auto TofRaw = fTimeStitch->GetTime(fHeader->GetTStartMaster() - StartTraw_atTcal, "vftx", "vftx");
                    // Right - left
                    auto PosRaw = FrsSciTraw[i * fFrsSciNbPmts][hitr] - FrsSciTraw[i * fFrsSciNbPmts + 1][hitl];

                    if (fCalPar->GetMinTofAtRank(i) < TofRaw && TofRaw < fCalPar->GetMaxTofAtRank(i) &&
                        fCalPar->GetMinPosAtRank(i) < PosRaw && PosRaw < fCalPar->GetMaxPosAtRank(i))
                    {
                        indexr[i] = hitr;
                        indexl[i] = hitl;
                        multTofRaw[i]++;
                    }
                }
            }

            if (indexr[i] >= 0 && indexl[i] >= 0 && multTofRaw[i] == 1)
            {
                StartTraw_atTcal = fTimeStitch->GetTime(0.5 * (FrsSciTraw[i * fFrsSciNbPmts][indexr[i]] +
                                                               FrsSciTraw[i * fFrsSciNbPmts + 1][indexl[i]]) -
                                                            FrsSciTraw[i * fFrsSciNbPmts + 2][0],
                                                        "vftx",
                                                        "vftx") -
                                   (i == 1 ? 130. : 0.);

                auto TofRaw = fTimeStitch->GetTime(fHeader->GetTStartMaster() - StartTraw_atTcal, "vftx", "vftx");
                auto PosRaw = FrsSciTraw[i * fFrsSciNbPmts][indexr[i]] - FrsSciTraw[i * fFrsSciNbPmts + 1][indexl[i]] -
                              (i == 1 ? 45.51 : 0.);

                auto PosCal = fCalPar->GetPosCalGainAtRank(i) * PosRaw + fCalPar->GetPosCalOffsetAtRank(i);
                auto Velocity = 1. / (fTof2InvV_p0->GetAt(0) + fTof2InvV_p1->GetAt(0) * TofRaw);
                auto Beta = Velocity / 0.299792458;
                auto Gamma = 1. / TMath::Sqrt(1. - Beta * Beta);
                auto Brho = fBrho0_S2toCC->GetAt(0) * (1. - PosCal / fDispersionS2->GetAt(0));
                auto AoQcal = Brho / (3.10716 * Beta * Gamma);
                if (Beta > fIncomingID_Par->GetBeta_min() && Beta < fIncomingID_Par->GetBeta_max())
                    this->AddData(i + 1, 1, Zcharge, AoQcal, Beta, Brho, PosCal, 0., TofRaw);
            }
        }
    }
    else
    {
        double Zmusic = 0.; //, Music_ang = 0.;
        if (!fHitItemsMusli && fHitItemsMus && fHitItemsMus->GetEntriesFast() > 0)
        {
            Int_t nHits = fHitItemsMus->GetEntriesFast();
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                auto hit = dynamic_cast<R3BMusicHitData*>(fHitItemsMus->At(ihit));
                if (!hit)
                    continue;
                Zmusic = hit->GetZcharge();
                // Music_ang = hit->GetTheta() * 1000.; // mrad
            }
        }

        if (!fHitItemsMus && fHitItemsMusli && fHitItemsMusli->GetEntriesFast() > 0)
        {
            Int_t nHits = fHitItemsMusli->GetEntriesFast();
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                auto hit = dynamic_cast<R3BMusliHitData*>(fHitItemsMusli->At(ihit));
                if (!hit)
                    continue;
                if (hit->GetType() == 2)
                    Zmusic = hit->GetZcharge(); // for data with mean signals from 4 anodes
                // if (hit->GetType() == 1)
                // Music_ang = hit->GetTheta() * 1000.; // mrad, for data with mean signals from 2 anodes
            }
        }

        // --- local variables --- //
        UInt_t nHits = 0;
        std::vector<Double_t> Zlos(fNumDet, 0);
        std::vector<Double_t> posLosX_cm(fNumDet, 0);
        std::vector<Double_t> trigTimeV(fNumDet, 0);
        Double_t Gamma_m1 = 0., Brho_m1 = 0., AoQ_m1 = 0.;
        Double_t AoQ_m1_corr = 0.;
        std::vector<Int_t> multLos(fNumDet, 0);
        Double_t en_pspx = 0., ZPsp = 0.;

        // --- read Trigger data from LOS --- //
        if (fTriggerLos && fTriggerLos->GetEntriesFast() > 0)
        {
            Int_t numDet = 1;
            Int_t tHits = fTriggerLos->GetEntriesFast();
            for (Int_t ihit = 0; ihit < tHits; ihit++)
            {
                R3BLosTCalData* hittcal = dynamic_cast<R3BLosTCalData*>(fTriggerLos->At(ihit));
                numDet = hittcal->GetDetector();
                if (hittcal->GetType() == 0)
                    trigTimeV[numDet - 1] = hittcal->GetRawTimeNs();
            } // --- end of loop over hit data --- //
        }

        // --- read hit from LOS data --- //
        if (fHitLos && fHitLos->GetEntriesFast() > 0)
        {
            Int_t numDet = 1;
            nHits = fHitLos->GetEntriesFast();
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BLosHitData* hittcal = dynamic_cast<R3BLosHitData*>(fHitLos->At(ihit));
                numDet = hittcal->GetDetector();
                if (fUseTref)
                {
                    Double_t time = fTimeStitch->GetTime(hittcal->GetTime() - trigTimeV[numDet - 1], "vftx", "vftx");
                    if (time == fHeader->GetTStart())
                    {
                        posLosX_cm[numDet - 1] = hittcal->GetX_cm();
                        Zlos[numDet - 1] = hittcal->GetZ();
                    }
                }
                else
                {
                    if (multLos[numDet - 1] == 0)
                    {
                        posLosX_cm[numDet - 1] = hittcal->GetX_cm();
                        Zlos[numDet - 1] = hittcal->GetZ();
                    }
                }
                multLos[numDet - 1]++;
            } // --- end of loop over hit data --- //
        }

        // --- read hit from PSP data --- //

        if (fHitPspx1_x)
        {
            // For now only multiplicity = 1 events are taken. Proper
            // treatment of multihit events needs to be implemented.
            if (fHitPspx1_x && fHitPspx1_x->GetEntriesFast() == 1)
            {
                auto pspx1hitx = dynamic_cast<R3BPspxHitData*>(fHitPspx1_x->At(0));
                en_pspx = pspx1hitx->GetEnergy();
            }
            ZPsp = en_pspx;
        }

        for (int i = 0; i < fNumDet; i++)
        {
            // --- secondary beam identification ---

            // if X is increasing from left to right:
            //    Brho = fBhro0 * (1 - xMwpc0/fDCC + xS2/fDS2)
            // in R3BRoot, X is increasing from right to left
            //    Bro = fBrho0 * (1 + xMwpc0/fDCC - xS2/fDS2)

            if (multLos[i] > 0)
            {
                Double_t betaS2 = 0.;
                Double_t PosXS2 = 0.;
                R3BFrsData* hitfrs = nullptr;
                if (fFrsHitData && fFrsHitData->GetEntriesFast() > 0)
                {
                    nHits = fFrsHitData->GetEntriesFast();
                    R3BLOG_IF(error, nHits > 1, "Multiplicity from FRS detector larger than 1: " << nHits);
                    for (Int_t ihit = 0; ihit < nHits; ihit++)
                    {
                        hitfrs = dynamic_cast<R3BFrsData*>(fFrsHitData->At(ihit));
                        if (!hitfrs)
                            continue;
                        betaS2 = hitfrs->GetBeta();
                        PosXS2 = hitfrs->GetXS2();
                        if (TMath::IsNaN(PosXS2) && fHeader->GetExpId() == 509) // NaN indicator for one S2 pmt missing
                            PosXS2 = 0.;
                    }
                }

                if (betaS2 < fBeta_max && betaS2 > fBeta_min && hitfrs)
                {
                    Gamma_m1 = 1. / (TMath::Sqrt(1. - TMath::Power(betaS2, 2)));
                    Brho_m1 = fBrho0_S2toCC->GetAt(i) * (1. + PosXS2 / fDispersionS2->GetAt(i));
                    AoQ_m1 = Brho_m1 / (3.10716 * betaS2 * Gamma_m1);
                    AoQ_m1_corr = fy0_Aq + (posLosX_cm[i] - fx0_Aq) * sin(fang_Aq) + (AoQ_m1 - fy0_Aq) * cos(fang_Aq);

                    if (fCutS2 && fCutS2->IsInside(PosXS2, AoQ_m1_corr))
                    {
                        if (Zmusic > 0. && !fUseLOS && !fUsePspx1)
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

                        if (Zlos[i] > 0. && fUseLOS && !fUsePspx1)
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

                        if (ZPsp > 0. && !fUseLOS && fUsePspx1)
                        {
                            hitfrs->SetZ(ZPsp);
                            hitfrs->SetAq(AoQ_m1_corr);
                            hitfrs->SetBrho(Brho_m1);
                        }
                    }
                    else if (!fCutS2)
                    {
                        if (Zmusic > 0. && !fUseLOS && !fUsePspx1)
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

                        if (Zlos[i] > 0. && fUseLOS && !fUsePspx1)
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

                        if (ZPsp > 0. && !fUseLOS && fUsePspx1)
                        {
                            hitfrs->SetZ(ZPsp);
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
    {
        fHitLos->Clear();
    }
    if (fHitItemsMus)
    {
        fHitItemsMus->Clear();
    }
    if (fHitItemsMusli)
    {
        fHitItemsMusli->Clear();
    }
    if (fFrsSci_Tcal)
    {
        fFrsSci_Tcal->Clear();
    }
    if (fHitPspx1_x)
    {
        fHitPspx1_x->Clear();
    }
    if (fHitPspx1_y)
    {
        fHitPspx1_y->Clear();
    }
    if (fFrsHitData)
    {
        fFrsHitData->Clear();
    }
}

R3BFrsData* R3BAnalysisIncomingID::AddData(Int_t StaId,
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
    TClonesArray& clref = *fFrsHitData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BFrsData(StaId, StoId, z, aq, beta, brho, xs2, xc, tof);
}

ClassImp(R3BAnalysisIncomingID)
