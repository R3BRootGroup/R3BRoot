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

// ------------------------------------------------------------
// -----             R3BFootOnlineSpectra                 -----
// -----    Created 16/07/21 by J.L. Rodriguez-Sanchez    -----
// -----       Modified 05/2025 by Pablo Gonzalez Rusell  -----
// -----          Fill FOOT online histograms             -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with FOOT online data
 */

#include "R3BFootOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BFootCalData.h"
#include "R3BFootHitData.h"
#include "R3BFootHitPar.h"
#include "R3BFootMappedData.h"
#include "R3BFootMappingPar.h"
#include "R3BLogger.h"
#include "R3BShared.h"

#include <THttpServer.h>

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>
#include <TCanvas.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLine.h>

#include <TClonesArray.h>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

R3BFootOnlineSpectra::R3BFootOnlineSpectra()
    : R3BFootOnlineSpectra("FootOnlineSpectra", 1)
{
}

R3BFootOnlineSpectra::R3BFootOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

void R3BFootOnlineSpectra::SetParContainers()
{
    // Parameter Container
    // Reading footMappingPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fMap_Par = dynamic_cast<R3BFootMappingPar*>(rtdb->getContainer("footMappingPar"));
    if (!fMap_Par)
    {
        R3BLOG(error, "Couldn't get handle on footMappingPar container");
    }
    else
    {
        R3BLOG(info, "footMappingPar found");
    }

    fHit_Par = dynamic_cast<R3BFootHitPar*>(rtdb->getContainer("footHitPar"));
    if (!fHit_Par)
    {
        R3BLOG(warn,
               "Couldn't get handle on footHitgPar container. Eta correction and charge calibration will be disabled.");
    }
    else
    {
        R3BLOG(info, "footHitPar found");
    }
}

void R3BFootOnlineSpectra::SetParameter()
{
    if (!fMap_Par)
    {
        R3BLOG(warn, "Container footMappingPar not found.");
        return;
    }
    //--- Parameter Container ---
    fNbDet = fMap_Par->GetNumDets(); // Number of foot detectors

    R3BLOG(info, "NumDet from mapping " << fNbDet);

    fXNdx.clear();
    fYNdx.clear();

    for (int i = 0; i < fNbDet; i++)
    {

        // X coordinate
        if ((fMap_Par->GetAnglePhi(i + 1) == 0) || (fMap_Par->GetAnglePhi(i + 1) == 180))
        {
            fXNdx.push_back(i);
        }

        // Y coordinate
        if ((fMap_Par->GetAnglePhi(i + 1) == 90) || (fMap_Par->GetAnglePhi(i + 1) == 270))
        {
            fYNdx.push_back(i);
        }
    }

    if (!fHit_Par || fHit_Par->GetNumParsFit() == -1)
    {
        R3BLOG(error, "SetParameter(): fHit_Par is NULL");
        fHit_Par = nullptr;
        return;
    }
}

InitStatus R3BFootOnlineSpectra::Init()
{
    R3BLOG(info, "");
    // Looking for FairRootManager
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, mgr == nullptr, "FairRootManager not found");

    // Look for the R3BEventHeader
    fEventHeader = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    if (fEventHeader == nullptr)
    {
        R3BLOG(error, "EventHeader. not found");
    }
    else
    {
        R3BLOG(info, "EventHeader. found");
    }

    // Get access to Mapped data
    fMappedItems = dynamic_cast<TClonesArray*>(mgr->GetObject("FootMappedData"));
    R3BLOG_IF(fatal, fMappedItems == nullptr, "FootMappedData not found");

    // Get access to Cal data
    fCalItems = dynamic_cast<TClonesArray*>(mgr->GetObject("FootCalData"));
    R3BLOG_IF(warn, fCalItems == nullptr, "FootCalData not found");

    // Get access to Hit data
    fHitItems = dynamic_cast<TClonesArray*>(mgr->GetObject("FootHitData"));
    R3BLOG_IF(warn, fHitItems == nullptr, "FootHitData not found");

    // Access parameter container
    SetParContainers();
    SetParameter();

    // Create histograms for all the detectors
    // Energy range for strips

    char Name1[255];
    char Name2[255];

    // MAIN FOLDER-FOOT
    TFolder* mainfol = new TFolder("Foot", "Foot-Si info");
    TFolder* mapfol = new TFolder("Map", "Map FOOT info");
    TFolder* calfol = new TFolder("Cal", "Cal FOOT info");
    TFolder* hitfol = new TFolder("Hit", "Hit FOOT info");
    mainfol->Add(mapfol);

    auto cMap = new TCanvas("FOOT_mapped", "mapped info", 10, 10, 500, 500);
    cMap->Divide(4, fNbDet / 4);
    mapfol->Add(cMap);

    //================  Mapped data =====================
    fh2_EnergyVsStrip.resize(fNbDet);
    int i_pad = 1; // pad iterator
    int i_pad_double = 1;
    int i_pad_corr = 1;
    for (Int_t i = 0; i < fNbDet; i++)
    { // one histo per detector
        sprintf(Name1, "fh2_energy_vs_strip_det_%d", i + 1);
        sprintf(Name2, "Mapped energy vs strip number for FOOT Det: %d", i + 1);
        fh2_EnergyVsStrip[i] = R3B::root_owned<TH2F>(Name1, Name2, 640, 1, 641, fBinsE, fMinE, fMaxE);
        fh2_EnergyVsStrip[i]->GetXaxis()->SetTitle("Strip number");
        fh2_EnergyVsStrip[i]->GetYaxis()->SetTitle("Energy [channels]");
        fh2_EnergyVsStrip[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_EnergyVsStrip[i]->GetXaxis()->CenterTitle(true);
        int foot_num = i + 1;

        cMap->cd(i_pad);
        fh2_EnergyVsStrip[i]->Draw("col");
        for (int i_asic = 1; i_asic < 10; i_asic++)
        {
            TLine* l = new TLine(64.5 * i_asic, fMinE, 64.5 * i_asic, fMaxE);
            l->Draw("same");
            l->SetLineStyle(7);
            l->SetLineWidth(1);
            l->SetLineColor(13);
        }
        i_pad++;
    }

    //============ CAL data ==================

    auto cCal = new TCanvas("FOOT_cal", "cal info", 10, 10, 500, 500);
    cCal->Divide(4, fNbDet / 4);
    calfol->Add(cCal);

    if (fCalItems)
    {
        i_pad = 1; // pad iterator
        fh2_EnergyVsStrip_cal.resize(fNbDet);
        for (Int_t i = 0; i < fNbDet; i++)
        {
            sprintf(Name1, "fh2_energy_vs_strip_cal_det_%d", i + 1);
            sprintf(Name2, "Cal-energy vs strip number for FOOT Det: %d", i + 1);
            fh2_EnergyVsStrip_cal[i] = R3B::root_owned<TH2F>(Name1, Name2, 640, 1, 641, fBinsE, fMinE, fMaxE);
            fh2_EnergyVsStrip_cal[i]->GetXaxis()->SetTitle("Strip number");
            fh2_EnergyVsStrip_cal[i]->GetYaxis()->SetTitle("Energy [channels]");
            fh2_EnergyVsStrip_cal[i]->GetYaxis()->SetTitleOffset(1.4);
            fh2_EnergyVsStrip_cal[i]->GetXaxis()->CenterTitle(true);
            fh2_EnergyVsStrip_cal[i]->GetYaxis()->CenterTitle(true);
            int foot_num = i + 1;
            // if (foot_num < 9)
            //{
            cCal->cd(i_pad);
            fh2_EnergyVsStrip_cal[i]->Draw("col");
            for (int i_asic = 1; i_asic < 10; i_asic++)
            {
                TLine* l = new TLine(64.5 * i_asic, fMinE, 64.5 * i_asic, fMaxE);
                l->Draw("same");
                l->SetLineStyle(7);
                l->SetLineWidth(1);
                l->SetLineColor(13);
            }
            i_pad++;
            //}
        }
    }

    //------------- SIGMA -------------------

    auto cSigma = new TCanvas("FOOT_sigma", "Sigma info", 10, 10, 500, 500);
    cSigma->Divide(4, fNbDet / 4);
    calfol->Add(cSigma);

    fh2_SigmaVsStrip.resize(fNbDet);
    i_pad = 1; // pad iterator

    for (Int_t i = 0; i < fNbDet; i++)
    {
        sprintf(Name1, "fh2_sigma_vs_strip_det_%d", i + 1);
        sprintf(Name2, "Sigma vs strip number for FOOT Det: %d", i + 1);
        fh2_SigmaVsStrip[i] = R3B::root_owned<TH2F>(Name1, Name2, 640, 1, 641, 100, 0, 15);
        fh2_SigmaVsStrip[i]->GetXaxis()->SetTitle("Strip number");
        fh2_SigmaVsStrip[i]->GetYaxis()->SetTitle("Sigma");
        fh2_SigmaVsStrip[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_SigmaVsStrip[i]->GetXaxis()->CenterTitle(true);
        fh2_SigmaVsStrip[i]->GetYaxis()->CenterTitle(true);

        int foot_num = i + 1;
        // if (foot_num < 9)
        //{
        cSigma->cd(i_pad);
        fh2_SigmaVsStrip[i]->Draw("col");
        for (int i_asic = 1; i_asic < 10; i_asic++)
        {
            TLine* l = new TLine(64.5 * i_asic, 0, 64.5 * i_asic, 15);
            l->Draw("same");
            l->SetLineStyle(7);
            l->SetLineWidth(1);
            l->SetLineColor(13);
        }
        i_pad++;
        //}
    }

    //================ HIT data ==========================

    int n = fXNdx.size();
    int r = 2;

    R3BLOG_IF(fatal, n > 100, Form("Number of FOOT is too high. Are you sure you have %d FOOTs?", 2 * n));

    dim = TMath::Factorial(n) / (TMath::Factorial(r) * TMath::Factorial(n - r));

    // General canvas info (cluster position and energy)
    auto cHit = new TCanvas("FOOT_hit", "hit general info", 10, 10, 500, 500);
    cHit->Divide(2 * fNbDet / 4, 4);

    // General canvas info (cluster position and energy)
    auto cHitMax = new TCanvas("FOOT_hitMax", "hit general info (max energy)", 10, 10, 500, 500);
    cHitMax->Divide(2 * fNbDet / 4, 4);

    // Correlation position between detectors
    auto cHit_PosCorr = new TCanvas("FOOT_posCorr", "hit position correlation info", 10, 10, 500, 500);
    cHit_PosCorr->Divide(fNbDet / 4, 2);

    // Eta
    auto cHit_Eta = new TCanvas("FOOT_eta", "hit eta plot info", 10, 10, 500, 500);
    cHit_Eta->Divide(fNbDet / 2, 2);

    // Eta
    auto cHit_EtaMax = new TCanvas("FOOT_etaMax", "hit eta plot info (max energy)", 10, 10, 500, 500);
    cHit_EtaMax->Divide(fNbDet / 2, 2);

    // Multiplicity and cluster size
    auto cHit_MultSize = new TCanvas("FOOT_mulSize", "hit multiplicity and size info", 10, 10, 500, 500);
    cHit_MultSize->Divide(2 * fNbDet / 4, 4);

    // Energy correlation
    auto cHit_EnergyCorr = new TCanvas("FOOT_energyCorr", "Energy correlation", 10, 10, 500, 500);
    cHit_EnergyCorr->Divide(fNbDet / 4, 2);

    // Max energy correlation
    auto cHit_MaxEnergyCorr = new TCanvas("FOOT_maxEnergyCorr", "Max energy correlation", 10, 10, 500, 500);
    cHit_MaxEnergyCorr->Divide(fNbDet / 4, 2);

    // Eta
    auto cHit_Charge = new TCanvas("FOOT_charge", "charge info", 10, 10, 500, 500);
    cHit_Charge->Divide(fNbDet / 4, 4);

    // position vs energy
    auto cHit_position_charge = new TCanvas("FOOT_position_charge", "charge info", 10, 10, 500, 500);
    cHit_position_charge->Divide(fNbDet / 4, 4);

    // Max pos correlation
    auto cHit_XYCorrMax = new TCanvas("FOOT_XYCorrMax", "Position correlation XY (max energy)", 10, 10, 500, 500);
    cHit_XYCorrMax->Divide(fNbDet / 4, 2);

    // Max pos correlation
    auto cHit_SameCorrMax =
        new TCanvas("FOOT_SameCorrMax", "Position correlation XX and YY (max energy)", 10, 10, 500, 500);

    // Different combinations depending on the setup
    if (dim % 8 == 0)
        cHit_SameCorrMax->Divide(dim / 4, 8);
    else if (dim % 4 == 0)
        cHit_SameCorrMax->Divide(dim / 2, 4);
    else if (dim % 5 == 0)
        cHit_SameCorrMax->Divide(dim / 5 * 2, 5);
    else
        cHit_SameCorrMax->Divide(dim, 2);

    hitfol->Add(cHit);
    hitfol->Add(cHitMax);
    hitfol->Add(cHit_PosCorr);
    hitfol->Add(cHit_Eta);
    hitfol->Add(cHit_EtaMax);
    hitfol->Add(cHit_MultSize);
    hitfol->Add(cHit_EnergyCorr);
    hitfol->Add(cHit_MaxEnergyCorr);
    hitfol->Add(cHit_Charge);
    hitfol->Add(cHit_XYCorrMax);
    hitfol->Add(cHit_position_charge);
    hitfol->Add(cHit_SameCorrMax);

    if (fHitItems)
    {
        i_pad = 1; // pad iterator
        i_pad_double = 1;
        i_pad_corr = 1;
        fh1_pos.resize(fNbDet);
        fh1_ene.resize(fNbDet);
        fh1_posMax.resize(fNbDet);
        fh1_eneMax.resize(fNbDet);
        fh1_mult.resize(fNbDet);
        fh1_size.resize(fNbDet);
        fh1_charge.resize(fNbDet);
        fh2_pos_charge.resize(fNbDet);
        fh2_etaMax.resize(fNbDet);
        fh2_eta.resize(fNbDet);
        fh2_foot_corr.resize(fNbDet / 2);
        fh2_energy_corr.resize(fNbDet / 2);
        fh2_energy_corr_max.resize(fNbDet / 2);

        fh2_XY_max_corr.resize(fNbDet / 2);

        fh2_XX_max_corr.resize(dim);
        fh2_YY_max_corr.resize(dim);

        // If there is no charge calibration charge ~ energy
        if (!fHit_Par)
        {
            fMinZ = fMinE;
            fMaxZ = fMaxE;
        }

        R3BLOG_IF(fatal,
                  fXNdx.size() != fYNdx.size(),
                  "You don't have the same number of X detector than Y detectors... Is it ok?");

        for (int i = 0; i < fXNdx.size() - 1; i++)
        {
            for (int j = i + 1; j < fXNdx.size(); j++)
            {
                fCorrNdxX.push_back(fXNdx[i]);
                fCorrNdxX.push_back(fXNdx[j]);

                fCorrNdxY.push_back(fYNdx[i]);
                fCorrNdxY.push_back(fYNdx[j]);
            }
        }

        for (int i = 0; i < dim; i++)
        {

            int det1x = fCorrNdxX[2 * i] + 1;
            int det2x = fCorrNdxX[2 * i + 1] + 1;

            int det1y = fCorrNdxY[2 * i] + 1;
            int det2y = fCorrNdxY[2 * i + 1] + 1;

            sprintf(Name1, "fh2_XX_corr_max_dets_%d_%d", det1x, det2x);
            sprintf(Name2, "Cluster XX correlation for FOOTs: %d and %d (max frag)", det1x, det2x);
            fh2_XX_max_corr[i] = R3B::root_owned<TH2F>(Name1, Name2, 100, -50., 50., 100, -50, 50);
            fh2_XX_max_corr[i]->GetXaxis()->SetTitle(Form("x - Position [mm] (det %d)", det1x));
            fh2_XX_max_corr[i]->GetYaxis()->SetTitle(Form("x - Position [mm] (det %d)", det2x));
            fh2_XX_max_corr[i]->GetYaxis()->SetTitleOffset(1.4);
            fh2_XX_max_corr[i]->GetXaxis()->CenterTitle(true);
            fh2_XX_max_corr[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh2_YY_corr_max_dets_%d_%d", det1y, det2y);
            sprintf(Name2, "Cluster YY correlation for FOOTs: %d and %d (max frag)", det1y, det2y);
            fh2_YY_max_corr[i] = R3B::root_owned<TH2F>(Name1, Name2, 100, -50., 50., 100, -50, 50);
            fh2_YY_max_corr[i]->GetXaxis()->SetTitle(Form("y - Position [mm] (det %d)", det1y));
            fh2_YY_max_corr[i]->GetYaxis()->SetTitle(Form("y - Position [mm] (det %d)", det2y));
            fh2_YY_max_corr[i]->GetYaxis()->SetTitleOffset(1.4);
            fh2_YY_max_corr[i]->GetXaxis()->CenterTitle(true);
            fh2_YY_max_corr[i]->GetYaxis()->CenterTitle(true);

            cHit_SameCorrMax->cd(i + 1);
            fh2_XX_max_corr[i]->Draw();

            cHit_SameCorrMax->cd(i + 1 + dim);
            fh2_YY_max_corr[i]->Draw();
        }

        for (Int_t i = 0; i < fNbDet; i++)
        { // one histo per detector
            sprintf(Name1, "fh1_pos_det_%d", i + 1);
            sprintf(Name2, "Cluster position for FOOT Det: %d", i + 1);
            fh1_pos[i] = R3B::root_owned<TH1F>(Name1, Name2, 600, -50., 50.);
            fh1_pos[i]->GetXaxis()->SetTitle("Position [mm]");
            fh1_pos[i]->GetYaxis()->SetTitle("Counts");
            fh1_pos[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_pos[i]->GetXaxis()->CenterTitle(true);
            fh1_pos[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh1_ene_det_%d", i + 1);
            sprintf(Name2, "Cluster energy for FOOT Det: %d", i + 1);
            fh1_ene[i] = R3B::root_owned<TH1F>(Name1, Name2, fBinsE, fMinE, fMaxE);
            fh1_ene[i]->GetXaxis()->SetTitle("Energy");
            fh1_ene[i]->GetYaxis()->SetTitle("Counts");
            fh1_ene[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_ene[i]->GetXaxis()->CenterTitle(true);
            fh1_ene[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh1_pos_det_max_%d", i + 1);
            sprintf(Name2, "Cluster position for FOOT Det (max energy): %d", i + 1);
            fh1_posMax[i] = R3B::root_owned<TH1F>(Name1, Name2, 600, -50., 50.);
            fh1_posMax[i]->GetXaxis()->SetTitle("Position [mm]");
            fh1_posMax[i]->GetYaxis()->SetTitle("Counts");
            fh1_posMax[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_posMax[i]->GetXaxis()->CenterTitle(true);
            fh1_posMax[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh1_ene_det_max_%d", i + 1);
            sprintf(Name2, "Cluster energy for FOOT Det (max energy): %d", i + 1);
            fh1_eneMax[i] = R3B::root_owned<TH1F>(Name1, Name2, fBinsE, fMinE, fMaxE);
            fh1_eneMax[i]->GetXaxis()->SetTitle("Energy");
            fh1_eneMax[i]->GetYaxis()->SetTitle("Counts");
            fh1_eneMax[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_eneMax[i]->GetXaxis()->CenterTitle(true);
            fh1_eneMax[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh2_eta_vs_strip_det_%d", i + 1);
            sprintf(Name2, "Eta parameter for FOOT Det: %d", i + 1);
            fh2_eta[i] = R3B::root_owned<TH2F>(Name1, Name2, 100, 0, 1, fBinsE, 0, fMaxE);
            fh2_eta[i]->GetXaxis()->SetTitle("Eta");
            fh2_eta[i]->GetYaxis()->SetTitle("Energy [channels]");
            fh2_eta[i]->GetYaxis()->SetTitleOffset(1.4);
            fh2_eta[i]->GetXaxis()->CenterTitle(true);
            fh2_eta[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh2_eta_vs_strip_det_max_%d", i + 1);
            sprintf(Name2, "Eta parameter for FOOT Det (max energy): %d", i + 1);
            fh2_etaMax[i] = R3B::root_owned<TH2F>(Name1, Name2, 100, 0, 1, fBinsE, 0, fMaxE);
            fh2_etaMax[i]->GetXaxis()->SetTitle("Eta");
            fh2_etaMax[i]->GetYaxis()->SetTitle("Energy [channels]");
            fh2_etaMax[i]->GetYaxis()->SetTitleOffset(1.4);
            fh2_etaMax[i]->GetXaxis()->CenterTitle(true);
            fh2_etaMax[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh1_mult_det_%d", i + 1);
            sprintf(Name2, "Cluster multiplicity for FOOT Det: %d", i + 1);
            fh1_mult[i] = R3B::root_owned<TH1F>(Name1, Name2, 20, 0, 20);
            fh1_mult[i]->GetXaxis()->SetTitle("Multiplicity");
            fh1_mult[i]->GetYaxis()->SetTitle("Counts");
            fh1_mult[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_mult[i]->GetXaxis()->CenterTitle(true);
            fh1_mult[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh1_size_det_%d", i + 1);
            sprintf(Name2, "Cluster size for FOOT Det: %d", i + 1);
            fh1_size[i] = R3B::root_owned<TH1F>(Name1, Name2, 50, 0, 50);
            fh1_size[i]->GetXaxis()->SetTitle("Size");
            fh1_size[i]->GetYaxis()->SetTitle("Counts");
            fh1_size[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_size[i]->GetXaxis()->CenterTitle(true);
            fh1_size[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh1_charge_det_%d", i + 1);
            sprintf(Name2, "Charge for FOOT Det: %d", i + 1);
            fh1_charge[i] = R3B::root_owned<TH1F>(Name1, Name2, fBinsE, fMinZ, fMaxZ);
            fh1_charge[i]->GetXaxis()->SetTitle("Charge");
            fh1_charge[i]->GetYaxis()->SetTitle("Counts");
            fh1_charge[i]->GetYaxis()->SetTitleOffset(1.4);
            fh1_charge[i]->GetXaxis()->CenterTitle(true);
            fh1_charge[i]->GetYaxis()->CenterTitle(true);

            sprintf(Name1, "fh2_pos_charge_det_%d", i + 1);
            sprintf(Name2, " Position vs Energy for FOOT Det: %d", i + 1);
            fh2_pos_charge[i] = R3B::root_owned<TH2F>(Name1, Name2, 200, -50., 50., fBinsE / 2., fMinZ, fMaxZ);
            fh2_pos_charge[i]->GetXaxis()->SetTitle("Position [mm]");
            fh2_pos_charge[i]->GetYaxis()->SetTitle("Charge");
            fh2_pos_charge[i]->GetYaxis()->SetTitleOffset(1.4);
            fh2_pos_charge[i]->GetXaxis()->CenterTitle(true);
            fh2_pos_charge[i]->GetYaxis()->CenterTitle(true);

            if ((i % 2) == 0 && (i / 2 + 1 <= fXNdx.size()))
            {
                sprintf(Name1, "fh2_pos_corr_dets_%d_%d", fXNdx[i / 2] + 1, fYNdx[i / 2] + 1);
                sprintf(Name2, "Cluster position correlation for FOOTs: %d and %d", fXNdx[i / 2] + 1, fYNdx[i / 2] + 1);
                fh2_foot_corr[i / 2] = R3B::root_owned<TH2F>(Name1, Name2, 100, -50., 50., 100, -50, 50);
                fh2_foot_corr[i / 2]->GetXaxis()->SetTitle("x - Position [mm]");
                fh2_foot_corr[i / 2]->GetYaxis()->SetTitle("y - Position [mm]");
                fh2_foot_corr[i / 2]->GetYaxis()->SetTitleOffset(1.4);
                fh2_foot_corr[i / 2]->GetXaxis()->CenterTitle(true);
                fh2_foot_corr[i / 2]->GetYaxis()->CenterTitle(true);

                sprintf(Name1, "fh2_XY_corr_max_dets_%d_%d", fXNdx[i / 2] + 1, fYNdx[i / 2] + 1);
                sprintf(Name2,
                        "Cluster XY correlation for FOOTs: %d and %d (max frag)",
                        fXNdx[i / 2] + 1,
                        fYNdx[i / 2] + 1);
                fh2_XY_max_corr[i / 2] = R3B::root_owned<TH2F>(Name1, Name2, 100, -50., 50., 100, -50, 50);
                fh2_XY_max_corr[i / 2]->GetXaxis()->SetTitle("x - Position [mm]");
                fh2_XY_max_corr[i / 2]->GetYaxis()->SetTitle("y - Position [mm]");
                fh2_XY_max_corr[i / 2]->GetYaxis()->SetTitleOffset(1.4);
                fh2_XY_max_corr[i / 2]->GetXaxis()->CenterTitle(true);
                fh2_XY_max_corr[i / 2]->GetYaxis()->CenterTitle(true);

                sprintf(Name1, "fh2_energ_corr_dets_%d_%d", fXNdx[i / 2] + 1, fYNdx[i / 2]);
                sprintf(Name2, "Cluster energy correlation for FOOTs: %d and %d", fXNdx[i / 2] + 1, fYNdx[i / 2] + 1);
                fh2_energy_corr[i / 2] =
                    R3B::root_owned<TH2F>(Name1, Name2, fBinsE, fMinZ, fMaxZ, fBinsE, fMinZ, fMaxZ);

                sprintf(Name1, "Energy det %d [channels]", fXNdx[i / 2] + 1);
                fh2_energy_corr[i / 2]->GetXaxis()->SetTitle(Name1);
                sprintf(Name1, "Energy det %d [channels]", fYNdx[i / 2] + 1);
                fh2_energy_corr[i / 2]->GetYaxis()->SetTitle(Name1);
                fh2_energy_corr[i / 2]->GetYaxis()->SetTitleOffset(1.4);
                fh2_energy_corr[i / 2]->GetXaxis()->CenterTitle(true);
                fh2_energy_corr[i / 2]->GetYaxis()->CenterTitle(true);

                sprintf(Name1, "fh2_max_energ_corr_dets_%d_%d", fXNdx[i / 2] + 1, fYNdx[i / 2] + 1);
                sprintf(Name2,
                        "Max energy correlation correlation for FOOTs: %d and %d",
                        fXNdx[i / 2] + 1,
                        fYNdx[i / 2] + 1);
                fh2_energy_corr_max[i / 2] =
                    R3B::root_owned<TH2F>(Name1, Name2, fBinsE / 2, fMinZ, fMaxZ, fBinsE / 2, fMinZ, fMaxZ);

                sprintf(Name1, "Max energy det %d [channels]", fXNdx[i / 2] + 1);
                fh2_energy_corr_max[i / 2]->GetXaxis()->SetTitle(Name1);
                sprintf(Name1, "Max energy det %d [channels]", fYNdx[i / 2] + 1);
                fh2_energy_corr_max[i / 2]->GetYaxis()->SetTitle(Name1);
                fh2_energy_corr_max[i / 2]->GetYaxis()->SetTitleOffset(1.4);
                fh2_energy_corr_max[i / 2]->GetXaxis()->CenterTitle(true);
                fh2_energy_corr_max[i / 2]->GetYaxis()->CenterTitle(true);
            }

            int foot_num = i + 1;

            cHit->cd(i_pad_double);
            fh1_ene[i]->Draw();
            cHit->cd(i_pad_double + 1);
            fh1_pos[i]->Draw();

            cHitMax->cd(i_pad_double);
            fh1_eneMax[i]->Draw();
            cHitMax->cd(i_pad_double + 1);
            fh1_posMax[i]->Draw();

            if ((foot_num % 2) == 1 && (i / 2 + 1 <= fXNdx.size()))
            {

                cHit_PosCorr->cd(i_pad_corr);
                fh2_foot_corr[i / 2]->Draw();

                cHit_EnergyCorr->cd(i_pad_corr);
                fh2_energy_corr[i / 2]->Draw();

                cHit_MaxEnergyCorr->cd(i_pad_corr);
                fh2_energy_corr_max[i / 2]->Draw();

                cHit_XYCorrMax->cd(i_pad_corr);
                fh2_XY_max_corr[i / 2]->Draw();

                i_pad_corr++;
            }

            cHit_Eta->cd(i_pad);
            fh2_eta[i]->Draw();

            cHit_EtaMax->cd(i_pad);
            fh2_etaMax[i]->Draw();

            cHit_Charge->cd(i_pad);
            fh1_charge[i]->Draw();

            cHit_MultSize->cd(i_pad_double);
            fh1_size[i]->Draw();
            cHit_MultSize->cd(i_pad_double + 1);
            fh1_mult[i]->Draw();

            cHit_position_charge->cd(i_pad);
            fh2_pos_charge[i]->Draw();

            i_pad_double++;
            i_pad_double++;

            i_pad++;
        }
    }

    if (fCalItems)
    {
        mainfol->Add(calfol);
    }
    if (fHitItems)
    {
        mainfol->Add(hitfol);
    }

    // Looking for FairRunOnline
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_FOOT", Form("/Objects/%s/->Reset_FOOT_Histo()", GetName()));

    return kSUCCESS;
}

void R3BFootOnlineSpectra::Reset_FOOT_Histo()
{
    LOG(info) << "R3BFootOnlineSpectra::Reset_FOOT_Histo";

    // Mapped data
    for (Int_t i = 0; i < fNbDet; i++)
    {
        fh2_EnergyVsStrip[i]->Reset();
    }

    // Cal data
    if (fCalItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh2_EnergyVsStrip_cal[i]->Reset();
            fh2_SigmaVsStrip[i]->Reset();
        }
    }

    // Hit data

    if (fHitItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh1_pos[i]->Reset();
            fh1_ene[i]->Reset();
            fh1_posMax[i]->Reset();
            fh1_eneMax[i]->Reset();
            fh1_mult[i]->Reset();
            fh1_size[i]->Reset();
            fh2_eta[i]->Reset();
            fh2_etaMax[i]->Reset();
            fh1_charge[i]->Reset();
            fh2_pos_charge[i]->Reset();

            if ((i % 2) == 0 && (i / 2 + 1 <= fXNdx.size()))
            {
                fh2_foot_corr[i / 2]->Reset();
                fh2_energy_corr[i / 2]->Reset();
                fh2_energy_corr_max[i / 2]->Reset();
                fh2_XY_max_corr[i / 2]->Reset();
            }
        }

        for (Int_t i = 0; i < dim; i++)
        {
            fh2_XX_max_corr[i]->Reset();
            fh2_YY_max_corr[i]->Reset();
        }
    }

    return;
}

void R3BFootOnlineSpectra::Exec(Option_t* option)
{

    if (fEventHeader->GetTrigger() != fTrigger && fTrigger > -1)
        return;

    if (fTpat > 0 && (fEventHeader->GetTpat() & fTpat) != fTpat)
        return;

    // Fill mapped data
    if (fMappedItems && fMappedItems->GetEntriesFast() > 0)
    {
        auto nHits = fMappedItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFootMappedData* hit = dynamic_cast<R3BFootMappedData*>(fMappedItems->At(ihit));
            if (!hit)
                continue;
            fh2_EnergyVsStrip[hit->GetDetId() - 1]->Fill(hit->GetStripId(), hit->GetEnergy());
        }
    }

    // Fill cal data
    if (fCalItems && fCalItems->GetEntriesFast() > 0)
    {
        auto nHits = fCalItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFootCalData* hit = dynamic_cast<R3BFootCalData*>(fCalItems->At(ihit));
            if (!hit)
                continue;
            fh2_EnergyVsStrip_cal[hit->GetDetId() - 1]->Fill(hit->GetStripId(), hit->GetEnergy());

            if ((eventNumber % fSigmaRefreshRate) == 0)
            {
                fh2_SigmaVsStrip[hit->GetDetId() - 1]->Fill(hit->GetStripId(), hit->GetSigma());
            }
        }
    }

    // Vector to store the multiplicity of each event
    std::vector<int> mult(fNbDet, 0);
    std::vector<std::vector<double>> energies(fNbDet);
    std::vector<std::vector<double>> positions(fNbDet);
    std::vector<std::vector<double>> etas(fNbDet);

    // Fill hit data
    if (fHitItems && fHitItems->GetEntriesFast() > 0)
    {
        auto nHits = fHitItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFootHitData* hit = dynamic_cast<R3BFootHitData*>(fHitItems->At(ihit));
            if (!hit)
                continue;

            if ((hit->GetMulStrip() > fMaxSize) && (fMaxSize > 0))
                continue;

            int XorY = -1;
            // X coordinate
            if (std::find(fXNdx.begin(), fXNdx.end(), hit->GetDetId() - 1) != fXNdx.end())
            {
                XorY = 0;
            }

            // Y coordinate
            if (std::find(fYNdx.begin(), fYNdx.end(), hit->GetDetId() - 1) != fYNdx.end())
            {
                XorY = 1;
            }

            fh1_pos[hit->GetDetId() - 1]->Fill(hit->GetPosLab()[XorY]);
            fh1_ene[hit->GetDetId() - 1]->Fill(hit->GetEnergy());
            fh1_size[hit->GetDetId() - 1]->Fill(hit->GetMulStrip());

            // Only calculate charge when it is on a constant eta-energy profile
            if (fHit_Par || ((hit->GetEta() < 0.7) && (hit->GetEta() > 0.3)))
            {
                fh1_charge[hit->GetDetId() - 1]->Fill(hit->GetZCharge());
                fh2_pos_charge[hit->GetDetId() - 1]->Fill(hit->GetPosLab()[XorY], hit->GetZCharge());
            }

            energies[hit->GetDetId() - 1].push_back(hit->GetZCharge());
            positions[hit->GetDetId() - 1].push_back(hit->GetPosLab()[XorY]);
            mult[hit->GetDetId() - 1]++;
            fh2_eta[hit->GetDetId() - 1]->Fill(hit->GetEta(), hit->GetEnergy());
            etas[hit->GetDetId() - 1].push_back(hit->GetEta());
        }
    }

    // Fill the multiplicity of the cluster per event
    for (int i = 0; i < fNbDet; i++)
    {
        if (mult[i] == 0)
        {
            continue;
        }

        fh1_mult[i]->Fill(mult[i]);
    }

    std::vector<double> maxEnergy(fNbDet, 0.0);
    std::vector<int> maxNdx(fNbDet, -1);

    for (int i = 0; i < fNbDet; i++)
    {
        for (int j = 0; j < energies[i].size(); j++)
        {
            if (energies[i][j] > maxEnergy[i])
            {
                maxEnergy[i] = energies[i][j];
                maxNdx[i] = j;
            }
        }
    }

    for (int i = 0; i < fNbDet; i++)
    {

        if (maxNdx[i] > -1)
        {

            if (fHit_Par || ((etas[i][maxNdx[i]] > 0.3) && (etas[i][maxNdx[i]] < 0.7)))
            {
                fh1_eneMax[i]->Fill(energies[i][maxNdx[i]]);
            }

            fh1_posMax[i]->Fill(positions[i][maxNdx[i]]);
            fh2_etaMax[i]->Fill(etas[i][maxNdx[i]], energies[i][maxNdx[i]]);
        }
    }

    for (int i = 0; i < fXNdx.size(); i++)
    {

        // Max energy
        if ((maxEnergy[fXNdx[i]] > 0) && (maxEnergy[fYNdx[i]] > 0))
        {
            if (fHit_Par || ((etas[fXNdx[i]][maxNdx[fXNdx[i]]] > 0.3) && (etas[fXNdx[i]][maxNdx[fXNdx[i]]] < 0.7) &&
                             (etas[fYNdx[i]][maxNdx[fYNdx[i]]] > 0.3) && (etas[fYNdx[i]][maxNdx[fYNdx[i]]] < 0.7)))
            {
                fh2_energy_corr_max[i]->Fill(maxEnergy[fXNdx[i]], maxEnergy[fYNdx[i]]);
            }
        }

        //  Correlation between pairs of FOOTs
        if ((!energies[fXNdx[i]].empty()) && (!energies[fYNdx[i]].empty()))
        {

            for (int j1 = 0; j1 < energies[fXNdx[i]].size(); j1++)
            {
                for (int j2 = 0; j2 < energies[fYNdx[i]].size(); j2++)
                {

                    // Only fill if energy is the central eta region
                    if (fHit_Par || ((etas[fXNdx[i]][j1] > 0.3) && (etas[fXNdx[i]][j1] < 0.7) &&
                                     (etas[fYNdx[i]][j2] > 0.3) && (etas[fYNdx[i]][j2] < 0.7)))
                    {
                        fh2_energy_corr[i]->Fill(energies[fXNdx[i]][j1], energies[fYNdx[i]][j2]);
                    }

                    fh2_foot_corr[i]->Fill(positions[fXNdx[i]][j1], positions[fYNdx[i]][j2]);
                }
            }

            // X vs Y
            fh2_XY_max_corr[i]->Fill(positions[fXNdx[i]][maxNdx[fXNdx[i]]], positions[fYNdx[i]][maxNdx[fYNdx[i]]]);
        }
    }

    for (int i = 0; i < dim; i++)
    {

        int det1x = fCorrNdxX[2 * i];
        int det2x = fCorrNdxX[2 * i + 1];

        int det1y = fCorrNdxY[2 * i];
        int det2y = fCorrNdxY[2 * i + 1];

        if ((positions[det1x].size() > 0) && (positions[det2x].size() > 0))
        {
            fh2_XX_max_corr[i]->Fill(positions[det1x][maxNdx[det1x]], positions[det2x][maxNdx[det2x]]);
        }

        if ((positions[det1y].size() > 0) && (positions[det2y].size() > 0))
        {

            fh2_YY_max_corr[i]->Fill(positions[det1y][maxNdx[det1y]], positions[det2y][maxNdx[det2y]]);
        }
    }

    fNEvents += 1;
    return;
}

void R3BFootOnlineSpectra::FinishEvent()
{
    if (fMappedItems)
    {
        fMappedItems->Clear();
    }
    if (fCalItems)
    {
        fCalItems->Clear();
    }

    if (fHitItems)
    {
        fHitItems->Clear();
    }

    // Increase the event number variable
    eventNumber++;
    return;
}

void R3BFootOnlineSpectra::FinishTask()
{
    if (fMappedItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
            fh2_EnergyVsStrip[i]->Write();
    }
    if (fCalItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh2_EnergyVsStrip_cal[i]->Write();
            fh2_SigmaVsStrip[i]->Write();
        }
    }

    if (fHitItems)
    {
        for (Int_t i = 0; i < fNbDet; i++)
        {
            fh1_pos[i]->Write();
            fh1_ene[i]->Write();
            fh1_posMax[i]->Write();
            fh1_eneMax[i]->Write();
            fh1_mult[i]->Write();
            fh1_size[i]->Write();
            fh2_eta[i]->Write();
            fh2_etaMax[i]->Write();
            fh1_charge[i]->Write();

            if ((i % 2) == 0 && (i / 2 + 1 <= fXNdx.size()))
            {
                fh2_foot_corr[i / 2]->Write();
                fh2_energy_corr[i / 2]->Write();
                fh2_energy_corr_max[i / 2]->Write();
                fh2_XY_max_corr[i / 2]->Write();
            }
        }

        for (Int_t i = 0; i < dim; i++)
        {
            fh2_XX_max_corr[i]->Write();
            fh2_YY_max_corr[i]->Write();
        }
    }

    return;
}

ClassImp(R3BFootOnlineSpectra)
