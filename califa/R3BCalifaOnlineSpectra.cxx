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

#include "R3BCalifaOnlineSpectra.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaHitData.h"
#include "R3BCalifaMappedData.h"
#include "R3BCalifaMappingPar.h"
#include "R3BEventHeader.h"
#include "R3BWRCalifaData.h"
#include "R3BWRMasterData.h"
#include "THttpServer.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

R3BCalifaOnlineSpectra::R3BCalifaOnlineSpectra()
    : FairTask("CALIFAOnlineSpectra", 1)
    , fMappedItemsCalifa(NULL)
    , fCalItemsCalifa(NULL)
    , fHitItemsCalifa(NULL)
    , fWRItemsCalifa(NULL)
    , fWRItemsMaster(NULL)
    , fMap_Par(NULL)
    , fNEvents(0)
    , fNbCalifaCrystals(4864)
    , fNumSides(Nb_Sides)
    , fNumRings(Nb_Rings)
    , fNumPreamps(Nb_Preamps)
    , fNumCrystalPreamp(Nb_PreampCh)
    , fMapHistos_bins(500)
    , fMapHistos_max(4000)
    , fBinsChannelFebex(5000)
    , fMaxBinChannelFebex(65535)
    , fMaxEnergyBarrel(10)
    , fMaxEnergyIphos(30)
    , fRaw2Cal(kFALSE)
    , fLogScale(kTRUE)
    , fFebex2Preamp(kTRUE)
{
    // Define Preamp vs Febex sequence for histograms
    fOrderFebexPreamp[0] = 6;
    fOrderFebexPreamp[1] = 5;
    fOrderFebexPreamp[2] = 4;
    fOrderFebexPreamp[3] = 3;
    fOrderFebexPreamp[4] = 2;
    fOrderFebexPreamp[5] = 1;
    fOrderFebexPreamp[6] = 0;
    fOrderFebexPreamp[7] = 7;
    fOrderFebexPreamp[8] = 8;
    fOrderFebexPreamp[9] = 15;
    fOrderFebexPreamp[10] = 14;
    fOrderFebexPreamp[11] = 13;
    fOrderFebexPreamp[12] = 12;
    fOrderFebexPreamp[13] = 11;
    fOrderFebexPreamp[14] = 10;
    fOrderFebexPreamp[15] = 9;

    for (Int_t s = 0; s < fNumSides; s++)
        for (Int_t r = 0; r < fNumRings; r++)
            for (Int_t p = 0; p < fNumPreamps; p++)
                for (Int_t i = 0; i < 4; i++)
                    fFebexInfo[s][r][p][i] = -1;
}

R3BCalifaOnlineSpectra::R3BCalifaOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItemsCalifa(NULL)
    , fCalItemsCalifa(NULL)
    , fHitItemsCalifa(NULL)
    , fWRItemsCalifa(NULL)
    , fWRItemsMaster(NULL)
    , fMap_Par(NULL)
    , fNEvents(0)
    , fNbCalifaCrystals(4864)
    , fNumSides(Nb_Sides)
    , fNumRings(Nb_Rings)
    , fNumPreamps(Nb_Preamps)
    , fNumCrystalPreamp(Nb_PreampCh)
    , fMapHistos_bins(500)
    , fMapHistos_max(4000)
    , fBinsChannelFebex(5000)
    , fMaxBinChannelFebex(65535)
    , fMaxEnergyBarrel(10)
    , fMaxEnergyIphos(30)
    , fRaw2Cal(kFALSE)
    , fLogScale(kTRUE)
    , fFebex2Preamp(kTRUE)
{
    // Define Preamp vs Febex sequence for histograms
    fOrderFebexPreamp[0] = 6;
    fOrderFebexPreamp[1] = 5;
    fOrderFebexPreamp[2] = 4;
    fOrderFebexPreamp[3] = 3;
    fOrderFebexPreamp[4] = 2;
    fOrderFebexPreamp[5] = 1;
    fOrderFebexPreamp[6] = 0;
    fOrderFebexPreamp[7] = 7;
    fOrderFebexPreamp[8] = 8;
    fOrderFebexPreamp[9] = 15;
    fOrderFebexPreamp[10] = 14;
    fOrderFebexPreamp[11] = 13;
    fOrderFebexPreamp[12] = 12;
    fOrderFebexPreamp[13] = 11;
    fOrderFebexPreamp[14] = 10;
    fOrderFebexPreamp[15] = 9;

    for (Int_t s = 0; s < fNumSides; s++)
        for (Int_t r = 0; r < fNumRings; r++)
            for (Int_t p = 0; p < fNumPreamps; p++)
                for (Int_t i = 0; i < 4; i++)
                    fFebexInfo[s][r][p][i] = -1;
}

R3BCalifaOnlineSpectra::~R3BCalifaOnlineSpectra()
{
    LOG(INFO) << "R3BCalifaOnlineSpectra: Delete instance";
    if (fMappedItemsCalifa)
        delete fMappedItemsCalifa;
    if (fCalItemsCalifa)
        delete fCalItemsCalifa;
    if (fHitItemsCalifa)
        delete fHitItemsCalifa;
    if (fWRItemsCalifa)
        delete fWRItemsCalifa;
    if (fWRItemsMaster)
        delete fWRItemsMaster;
}

void R3BCalifaOnlineSpectra::SetParContainers()
{
    // Parameter Container
    // Reading amsStripCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "FairRuntimeDb not opened!";
    }

    fMap_Par = (R3BCalifaMappingPar*)rtdb->getContainer("califaMappingPar");
    if (!fMap_Par)
    {
        LOG(ERROR) << "R3BCalifaOnlineSpectra::Couldn't get handle on califaMappingPar container";
    }
    else
    {
        LOG(INFO) << "R3BCalifaOnlineSpectra::CalifaMappingPar container open";
    }
}

void R3BCalifaOnlineSpectra::SetParameter()
{
    if (!fMap_Par)
    {
        LOG(ERROR) << "R3BCalifaOnlineSpectra::CalifaMappingPar container not found";
    }
    //--- Parameter Container ---
    fNbCalifaCrystals = fMap_Par->GetNumCrystals(); // Number of crystals
    LOG(INFO) << "R3BCalifaOnlineSpectra::NumCry " << fNbCalifaCrystals;
    // fMap_Par->printParams();

    for (Int_t c = 1; c <= fNbCalifaCrystals; c++)
    {

        if (c <= fNbCalifaCrystals / 2)
        {
            fFebexInfo[fMap_Par->GetHalf(c) - 1][fMap_Par->GetRing(c) - 1][fMap_Par->GetPreamp(c) - 1][0] =
                fMap_Par->GetFebexSlot(c);
            fFebexInfo[fMap_Par->GetHalf(c) - 1][fMap_Par->GetRing(c) - 1][fMap_Par->GetPreamp(c) - 1][1] =
                fMap_Par->GetFebexMod(c);
        }
        else
        {
            fFebexInfo[fMap_Par->GetHalf(c) - 1][fMap_Par->GetRing(c) - 1][fMap_Par->GetPreamp(c) - 1][2] =
                fMap_Par->GetFebexSlot(c);
            fFebexInfo[fMap_Par->GetHalf(c) - 1][fMap_Par->GetRing(c) - 1][fMap_Par->GetPreamp(c) - 1][3] =
                fMap_Par->GetFebexMod(c);
        }
    }
}

InitStatus R3BCalifaOnlineSpectra::Init()
{
    LOG(INFO) << "R3BCalifaOnlineSpectra::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BCalifaOnlineSpectra::Init FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to Mapped data
    fMappedItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaMappedData");
    if (!fMappedItemsCalifa)
    {
        LOG(ERROR) << "R3BCalifaOnlineSpectra::CalifaCrystalMappedData not found";
        return kFATAL;
    }

    // get access to Cal data
    fCalItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaCrystalCalData");
    if (!fCalItemsCalifa)
    {
        LOG(WARNING) << "R3BCalifaOnlineSpectra::CalifaCrystalCalData not found";
    }

    // get access to Hit data
    fHitItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaHitData");
    if (!fHitItemsCalifa)
    {
        LOG(WARNING) << "R3BCalifaOnlineSpectra::CalifaHitData not found";
    }

    // get access to WR-Califa data
    fWRItemsCalifa = (TClonesArray*)mgr->GetObject("WRCalifaData");
    if (!fWRItemsCalifa)
    {
        LOG(WARNING) << "R3BCalifaOnlineSpectra::WRCalifaData not found";
    }

    // get access to WR-Master data
    fWRItemsMaster = (TClonesArray*)mgr->GetObject("WRMasterData");
    if (!fWRItemsMaster)
    {
        LOG(WARNING) << "R3BCalifaOnlineSpectra::WRMasterData not found";
    }

    SetParameter();

    // reading the file
    Bool_t noFile = kFALSE;
    ifstream* FileHistos = new ifstream(fCalifaFile);
    if (!FileHistos->is_open())
    {
        LOG(WARNING) << "R3BCalifaOnlineSpectra:  No Histogram definition file";
        noFile = kTRUE;
    }

    Double_t arry_bins[fNumSides][fNumRings][fNumPreamps][fNumCrystalPreamp];
    Double_t arry_maxE[fNumSides][fNumRings][fNumPreamps][fNumCrystalPreamp];
    Double_t arry_minE[fNumSides][fNumRings][fNumPreamps][fNumCrystalPreamp];

    if (!noFile)
    {
        for (Int_t s = 0; s < fNumSides; s++)
            for (Int_t r = 0; r < fNumRings; r++)
                for (Int_t p = 0; p < fNumPreamps; p++)
                    for (Int_t ch = 0; ch < fNumCrystalPreamp; ch++)
                        *FileHistos >> arry_bins[s][r][p][ch] >> arry_minE[s][r][p][ch] >> arry_maxE[s][r][p][ch];
    }
    else
    {
        for (Int_t s = 0; s < fNumSides; s++)
            for (Int_t r = 0; r < fNumRings; r++)
                for (Int_t p = 0; p < fNumPreamps; p++)
                    for (Int_t ch = 0; ch < fNumCrystalPreamp; ch++)
                    {
                        arry_bins[s][r][p][ch] = fMapHistos_bins;
                        arry_minE[s][r][p][ch] = 0;
                        arry_maxE[s][r][p][ch] = fMapHistos_max;
                    }
    }

    // Create histograms for detectors
    char Name1[255];
    char Name2[255];
    char Name3[255];
    TString Xaxis;
    Double_t bins = fMapHistos_bins;
    Double_t maxE = fMapHistos_max;
    Double_t minE = 0.;

    // CANVAS Crystal_ID vs energy
    cCalifa_cry_energy = new TCanvas("Califa_Map_cryID_energy", "Califa_Map energy vs cryID", 10, 10, 500, 500);
    fh2_Califa_cryId_energy = new TH2F("fh2_Califa_Map_cryID_energy",
                                       "Califa: CryID vs energy",
                                       fMap_Par->GetNumCrystals(),
                                       0.5,
                                       fMap_Par->GetNumCrystals() + 0.5,
                                       fBinsChannelFebex,
                                       0.0,
                                       fMaxBinChannelFebex);
    fh2_Califa_cryId_energy->GetXaxis()->SetTitle("Crystal ID");
    fh2_Califa_cryId_energy->GetYaxis()->SetTitle("Energy [Channels]");
    fh2_Califa_cryId_energy->GetYaxis()->SetTitleOffset(1.4);
    fh2_Califa_cryId_energy->GetXaxis()->CenterTitle(true);
    fh2_Califa_cryId_energy->GetYaxis()->CenterTitle(true);
    gPad->SetLogz();
    fh2_Califa_cryId_energy->Draw("COLZ");

    // CANVAS Crystal_ID vs energy
    cCalifa_cry_energy_cal = new TCanvas("Califa_Cal_cryID_energy", "Califa_Cal energy vs cryID", 10, 10, 500, 500);
    fh2_Califa_cryId_energy_cal = new TH2F("fh2_Califa_Cal_cryID_energy",
                                           "Califa: CryID vs calibrated energy",
                                           fMap_Par->GetNumCrystals(),
                                           0.5,
                                           fMap_Par->GetNumCrystals() + 0.5,
                                           bins,
                                           minE,
                                           maxE);
    fh2_Califa_cryId_energy_cal->GetXaxis()->SetTitle("Crystal ID");
    fh2_Califa_cryId_energy_cal->GetYaxis()->SetTitle("Energy [keV]");
    fh2_Califa_cryId_energy_cal->GetYaxis()->SetTitleOffset(1.4);
    fh2_Califa_cryId_energy_cal->GetXaxis()->CenterTitle(true);
    fh2_Califa_cryId_energy_cal->GetYaxis()->CenterTitle(true);
    gPad->SetLogz();
    fh2_Califa_cryId_energy_cal->Draw("COLZ");

    // Map data
    for (Int_t i = 0; i < fNumRings; i++)
    {
        // Left side
        sprintf(Name1, "Ring_%d_Left", i + 1);
        cMap_RingL[i] = new TCanvas(Name1, Name1, 10, 10, 800, 700);

        sprintf(Name1, "fh2_Ring_%d_Left", i + 1);
        sprintf(Name2, "Ring %d left: Preamp number vs channel number", i + 1);
        fh2_Preamp_vs_ch_L[i] = new TH2F(
            Name1, Name2, fNumPreamps, 0.5, fNumPreamps + 0.5, fNumCrystalPreamp, 0.5, fNumCrystalPreamp + 0.5);
        sprintf(Name3, "Channel number [1-%d]", fNumCrystalPreamp);
        fh2_Preamp_vs_ch_L[i]->GetXaxis()->SetTitle("Preamp number [1-16]");
        fh2_Preamp_vs_ch_L[i]->GetYaxis()->SetTitle(Name3);
        fh2_Preamp_vs_ch_L[i]->GetYaxis()->SetTitleOffset(1.2);
        fh2_Preamp_vs_ch_L[i]->GetXaxis()->CenterTitle(true);
        fh2_Preamp_vs_ch_L[i]->GetYaxis()->CenterTitle(true);
        fh2_Preamp_vs_ch_L[i]->Draw("colz");

        // Right side
        sprintf(Name1, "Ring_%d_Right", i + 1);
        cMap_RingR[i] = new TCanvas(Name1, Name1, 10, 10, 800, 700);

        sprintf(Name1, "fh2_Ring_%d_Right", i + 1);
        sprintf(Name2, "Ring %d right: Preamp number vs channel number", i + 1);
        fh2_Preamp_vs_ch_R[i] = new TH2F(
            Name1, Name2, fNumPreamps, 0.5, fNumPreamps + 0.5, fNumCrystalPreamp, 0.5, fNumCrystalPreamp + 0.5);
        fh2_Preamp_vs_ch_R[i]->GetXaxis()->SetTitle("Preamp number [1-16]");
        fh2_Preamp_vs_ch_R[i]->GetYaxis()->SetTitle(Name3);
        fh2_Preamp_vs_ch_R[i]->GetYaxis()->SetTitleOffset(1.2);
        fh2_Preamp_vs_ch_R[i]->GetXaxis()->CenterTitle(true);
        fh2_Preamp_vs_ch_R[i]->GetYaxis()->CenterTitle(true);
        fh2_Preamp_vs_ch_R[i]->Draw("colz");
    }

    char Side[255];
    for (Int_t s = 0; s < fNumSides; s++) // Side
    {
        if (s == 1)
            sprintf(Side, "Left");
        else
            sprintf(Side, "Right");
        for (Int_t r = 0; r < fNumRings; r++)       // Ring
            for (Int_t p = 0; p < fNumPreamps; p++) // Preamp
            {
                if (fFebexInfo[s][r][p][0] != -1)
                {
                    sprintf(Name1, "Ring_%d_%s_Preamp_%d", r + 1, Side, p + 1);
                    sprintf(Name2, "Ring %d, %s side, Preamp %d", r + 1, Side, p + 1);
                    cMapCry[s][r][p] = new TCanvas(Name1, Name2, 10, 10, 500, 500);
                    cMapCry[s][r][p]->Divide(4, 4);
                    // for TOT correlations
                    sprintf(Name1, "Ring_%d_%s_Preamp_%d_tot", r + 1, Side, p + 1);
                    sprintf(Name2, "Ring %d, %s side, Preamp %d", r + 1, Side, p + 1);
                    cMapCryTot[s][r][p] = new TCanvas(Name1, Name2, 10, 10, 500, 500);
                    cMapCryTot[s][r][p]->Divide(4, 4);
                }
                if (fFebexInfo[s][r][p][2] != -1)
                {
                    sprintf(Name1, "Ring_%d_%s_Preamp_%d_pr", r + 1, Side, p + 1);
                    sprintf(Name2, "Ring %d, %s side, Preamp %d for PR", r + 1, Side, p + 1);
                    cMapCryP[s][r][p] = new TCanvas(Name1, Name2, 10, 10, 500, 500);
                    cMapCryP[s][r][p]->Divide(4, 4);
                    // for TOT correlations
                    sprintf(Name1, "Ring_%d_%s_Preamp_%d_tot_pr", r + 1, Side, p + 1);
                    sprintf(Name2, "Ring %d, %s side, Preamp %d for PR", r + 1, Side, p + 1);
                    cMapCryPTot[s][r][p] = new TCanvas(Name1, Name2, 10, 10, 500, 500);
                    cMapCryPTot[s][r][p]->Divide(4, 4);
                }
                for (Int_t j = 0; j < fNumCrystalPreamp; j++)
                { // Channel
                    Xaxis = "Energy [channels]";

                    if (fFebexInfo[s][r][p][0] != -1)
                    {
                        sprintf(Name1, "fh1_Map_Side_%s_Ring_%d_Preamp_%d_Ch_%d_energy", Side, r + 1, p + 1, j + 1);
                        sprintf(Name2, "Map level, Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);

                        fh1_crystals[s][r][p][j] = new TH1F(Name1, Name2, fBinsChannelFebex, 0, fMaxBinChannelFebex);
                        fh1_crystals[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh1_crystals[s][r][p][j]->GetXaxis()->SetTitle(Xaxis);
                        fh1_crystals[s][r][p][j]->GetXaxis()->SetLabelSize(0.06);
                        fh1_crystals[s][r][p][j]->GetYaxis()->SetLabelSize(0.07);
                        fh1_crystals[s][r][p][j]->GetXaxis()->SetTitleSize(0.05);
                        fh1_crystals[s][r][p][j]->GetXaxis()->CenterTitle(true);
                        fh1_crystals[s][r][p][j]->GetYaxis()->CenterTitle(true);
                        fh1_crystals[s][r][p][j]->GetXaxis()->SetTitleOffset(1.);
                        fh1_crystals[s][r][p][j]->SetFillColor(45);
                        cMapCry[s][r][p]->cd(j + 1);
                        gPad->SetLogy();
                        fh1_crystals[s][r][p][j]->Draw();

                        // for TOT correlations
                        sprintf(Name1, "fh2_Map_Side_%s_Ring_%d_Preamp_%d_Ch_%d_evstot", Side, r + 1, p + 1, j + 1);
                        sprintf(Name2, "Map level, Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);

                        fh2_crystalsETot[s][r][p][j] = new TH2F(Name1,
                                                                Name2,
                                                                fBinsChannelFebex,
                                                                0,
                                                                fMaxBinChannelFebex,
                                                                fBinsChannelFebex,
                                                                0,
                                                                fMaxBinChannelFebex);
                        fh2_crystalsETot[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->SetTitle(Xaxis);
                        fh2_crystalsETot[s][r][p][j]->GetYaxis()->SetTitle("Tot");
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->SetLabelSize(0.06);
                        fh2_crystalsETot[s][r][p][j]->GetYaxis()->SetLabelSize(0.06);
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->SetTitleSize(0.05);
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->CenterTitle(true);
                        fh2_crystalsETot[s][r][p][j]->GetYaxis()->CenterTitle(true);
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->SetTitleOffset(1.);
                        cMapCryTot[s][r][p]->cd(j + 1);
                        fh2_crystalsETot[s][r][p][j]->Draw();
                    }
                    if (fFebexInfo[s][r][p][2] != -1)
                    {
                        // histograms for proton range
                        sprintf(Name1, "fh1_Map_Side_%s_Ring_%d_Preamp_%d_Ch_%d_energy_pr", Side, r + 1, p + 1, j + 1);
                        sprintf(Name2, "Map level (PR), Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                        fh1_crystals_p[s][r][p][j] = new TH1F(Name1, Name2, fBinsChannelFebex, 0, fMaxBinChannelFebex);
                        fh1_crystals_p[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh1_crystals_p[s][r][p][j]->GetXaxis()->SetTitle(Xaxis);
                        fh1_crystals_p[s][r][p][j]->GetXaxis()->SetLabelSize(0.06);
                        fh1_crystals_p[s][r][p][j]->GetYaxis()->SetLabelSize(0.07);
                        fh1_crystals_p[s][r][p][j]->GetXaxis()->SetTitleSize(0.05);
                        fh1_crystals_p[s][r][p][j]->GetXaxis()->CenterTitle(true);
                        fh1_crystals_p[s][r][p][j]->GetYaxis()->CenterTitle(true);
                        fh1_crystals_p[s][r][p][j]->GetXaxis()->SetTitleOffset(1.);
                        fh1_crystals_p[s][r][p][j]->SetFillColor(45);
                        cMapCryP[s][r][p]->cd(j + 1);
                        gPad->SetLogy();
                        fh1_crystals_p[s][r][p][j]->Draw();

                        // for TOT correlations
                        sprintf(Name1, "fh2_Map_Side_%s_Ring_%d_Preamp_%d_Ch_%d_pr_evstot", Side, r + 1, p + 1, j + 1);
                        sprintf(Name2, "Map level (PR), Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                        fh2_crystalsETot_p[s][r][p][j] = new TH2F(Name1,
                                                                  Name2,
                                                                  fBinsChannelFebex,
                                                                  0,
                                                                  fMaxBinChannelFebex,
                                                                  fBinsChannelFebex,
                                                                  0,
                                                                  fMaxBinChannelFebex);
                        fh2_crystalsETot_p[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->SetTitle(Xaxis);
                        fh2_crystalsETot_p[s][r][p][j]->GetYaxis()->SetTitle("Tot");
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->SetLabelSize(0.06);
                        fh2_crystalsETot_p[s][r][p][j]->GetYaxis()->SetLabelSize(0.06);
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->SetTitleSize(0.05);
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->CenterTitle(true);
                        fh2_crystalsETot_p[s][r][p][j]->GetYaxis()->CenterTitle(true);
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->SetTitleOffset(1.);
                        cMapCryPTot[s][r][p]->cd(j + 1);
                        fh2_crystalsETot_p[s][r][p][j]->Draw();
                    }
                }
            }
    }

    // Cal data
    for (Int_t s = 0; s < fNumSides; s++) // Side
    {
        if (s == 1)
            sprintf(Side, "Left");
        else
            sprintf(Side, "Right");
        for (Int_t r = 0; r < fNumRings; r++) // Ring

            for (Int_t p = 0; p < fNumPreamps; p++) // Preamp
            {
                if (fFebexInfo[s][r][p][0] != -1)
                {
                    sprintf(Name1, "Ring_%d_%s_Preamp_%d_cal", r + 1, Side, p + 1);
                    sprintf(Name2, "Ring %d, %s side, Preamp %d Cal", r + 1, Side, p + 1);
                    cMapCryCal[s][r][p] = new TCanvas(Name1, Name2, 10, 10, 500, 500);
                    cMapCryCal[s][r][p]->Divide(4, 4);
                }
                if (fFebexInfo[s][r][p][2] != -1)
                {
                    sprintf(Name1, "Ring_%d_%s_Preamp_%d_calpr", r + 1, Side, p + 1);
                    sprintf(Name2, "Ring %d, %s side, Preamp %d for PR Cal", r + 1, Side, p + 1);
                    cMapCryPCal[s][r][p] = new TCanvas(Name1, Name2, 10, 10, 500, 500);
                    cMapCryPCal[s][r][p]->Divide(4, 4);
                }
                for (Int_t j = 0; j < fNumCrystalPreamp; j++)
                { // Channel
                    Xaxis = "Energy [keV]";

                    if (fFebexInfo[s][r][p][0] != -1)
                    {
                        sprintf(Name1, "fh1_Cal_Side_%s_Ring_%d_Preamp_%d_Ch_%d_energy", Side, r + 1, p + 1, j + 1);
                        sprintf(Name2, "Cal level, Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);

                        fh1_crystals_cal[s][r][p][j] =
                            new TH1F(Name1, Name2, arry_bins[s][r][p][j], arry_minE[s][r][p][j], arry_maxE[s][r][p][j]);
                        fh1_crystals_cal[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh1_crystals_cal[s][r][p][j]->GetXaxis()->SetTitle(Xaxis);
                        fh1_crystals_cal[s][r][p][j]->GetXaxis()->SetLabelSize(0.06);
                        fh1_crystals_cal[s][r][p][j]->GetYaxis()->SetLabelSize(0.07);
                        fh1_crystals_cal[s][r][p][j]->GetXaxis()->SetTitleSize(0.05);
                        fh1_crystals_cal[s][r][p][j]->GetXaxis()->CenterTitle(true);
                        fh1_crystals_cal[s][r][p][j]->GetYaxis()->CenterTitle(true);
                        fh1_crystals_cal[s][r][p][j]->GetXaxis()->SetTitleOffset(1.);
                        fh1_crystals_cal[s][r][p][j]->SetFillColor(45);
                        cMapCryCal[s][r][p]->cd(j + 1);
                        gPad->SetLogy();
                        fh1_crystals_cal[s][r][p][j]->Draw();
                    }
                    if (fFebexInfo[s][r][p][2] != -1)
                    { // histograms for proton range
                        sprintf(Name1, "fh1_Cal_Side_%s_Ring_%d_Preamp_%d_Ch_%d_energy_pr", Side, r + 1, p + 1, j + 1);
                        sprintf(Name2, "Cal level (PR), Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                        fh1_crystals_p_cal[s][r][p][j] =
                            new TH1F(Name1, Name2, arry_bins[s][r][p][j], arry_minE[s][r][p][j], arry_maxE[s][r][p][j]);
                        fh1_crystals_p_cal[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh1_crystals_p_cal[s][r][p][j]->GetXaxis()->SetTitle(Xaxis);
                        fh1_crystals_p_cal[s][r][p][j]->GetXaxis()->SetLabelSize(0.06);
                        fh1_crystals_p_cal[s][r][p][j]->GetYaxis()->SetLabelSize(0.07);
                        fh1_crystals_p_cal[s][r][p][j]->GetXaxis()->SetTitleSize(0.05);
                        fh1_crystals_p_cal[s][r][p][j]->GetXaxis()->CenterTitle(true);
                        fh1_crystals_p_cal[s][r][p][j]->GetYaxis()->CenterTitle(true);
                        fh1_crystals_p_cal[s][r][p][j]->GetXaxis()->SetTitleOffset(1.);
                        fh1_crystals_p_cal[s][r][p][j]->SetFillColor(45);
                        cMapCryPCal[s][r][p]->cd(j + 1);
                        gPad->SetLogy();
                        fh1_crystals_p_cal[s][r][p][j]->Draw();
                    }
                }
            }
    }

    // CANVAS Multiplicity
    cCalifaMult = new TCanvas("Califa_Multiplicity", "Califa_Multiplicity", 10, 10, 500, 500);
    fh1_Califa_Mult = new TH1F("fh1_Califa_Mult", "Califa multiplicity (crystal:blue, cluster:red)", 21, -0.5, 20.5);
    fh1_Califa_MultHit = new TH1F("fh1_Califa_MultHit", "Califa multiplicity", 21, -0.5, 20.5);
    fh1_Califa_Mult->GetXaxis()->SetTitle("Multiplicity");
    fh1_Califa_Mult->GetXaxis()->CenterTitle(true);
    fh1_Califa_Mult->GetYaxis()->CenterTitle(true);
    fh1_Califa_Mult->GetYaxis()->SetTitleOffset(1.2);
    fh1_Califa_Mult->GetXaxis()->SetTitleOffset(1.1);
    gPad->SetLogy();
    fh1_Califa_Mult->Draw();
    fh1_Califa_MultHit->SetLineColor(kRed);
    fh1_Califa_MultHit->Draw("SAME");

    // CANVAS Energy correlations between hits
    cCalifaCoinE = new TCanvas("Califa_energy_correlation_hits", "Energy correlations, hit level", 10, 10, 500, 500);

    fh2_Califa_coinE =
        new TH2F("fh2_Califa_energy_correlations", "Califa energy correlations", bins, minE, maxE, bins, minE, maxE);
    fh2_Califa_coinE->GetXaxis()->SetTitle("Energy (keV)");
    fh2_Califa_coinE->GetYaxis()->SetTitle("Energy (keV)");
    fh2_Califa_coinE->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_coinE->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinE->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinE->Draw("COLZ");

    // CANVAS Theta correlations between hits
    cCalifaCoinTheta = new TCanvas("Califa_theta_correlation_hits", "Theta correlations, hit level", 10, 10, 500, 500);

    fh2_Califa_coinTheta =
        new TH2F("fh2_Califa_theta_correlations", "Califa theta correlations", 50, 0, 100, 50, 0, 100);
    fh2_Califa_coinTheta->GetXaxis()->SetTitle("Theta [degrees]");
    fh2_Califa_coinTheta->GetYaxis()->SetTitle("Theta [degrees]");
    fh2_Califa_coinTheta->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_coinTheta->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinTheta->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinTheta->Draw("COLZ");

    // CANVAS Phi correlations between hits
    cCalifaCoinPhi = new TCanvas("Califa_phi_correlation_hits", "Phi correlations, hit level", 10, 10, 500, 500);

    fh2_Califa_coinPhi =
        new TH2F("fh2_Califa_phi_correlations", "Califa phi correlations", 90, -180, 180, 90, -180, 180);
    fh2_Califa_coinPhi->GetXaxis()->SetTitle("Phi [degrees]");
    fh2_Califa_coinPhi->GetYaxis()->SetTitle("Phi [degrees]");
    fh2_Califa_coinPhi->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_coinPhi->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinPhi->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinPhi->Draw("COLZ");

    // CANVAS Theta vs Phi
    cCalifa_angles = new TCanvas("Califa_Theta_vs_Phi", "Theta vs Phi", 10, 10, 500, 500);
    fh2_Califa_theta_phi = new TH2F("fh2_Califa_theta_vs_phi", "Califa theta vs phi", 50, 0, 90, 180, -180, 180);
    fh2_Califa_theta_phi->GetXaxis()->SetTitle("Theta [degrees]");
    fh2_Califa_theta_phi->GetYaxis()->SetTitle("Phi [degrees]");
    fh2_Califa_theta_phi->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_theta_phi->GetXaxis()->CenterTitle(true);
    fh2_Califa_theta_phi->GetYaxis()->CenterTitle(true);
    fh2_Califa_theta_phi->Draw("COLZ");

    // CANVAS Theta vs energy
    sprintf(Name1, "Califa_calorimeter_theta_vs_energy");
    sprintf(Name2, "fh_Califa_theta_vs_total_energy");
    sprintf(Name3, "Califa theta vs energy for full calorimeter");
    cCalifa_theta_energy = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    fh2_Califa_theta_energy = new TH2F(Name2, Name3, 360, 0, 90, bins, minE, maxE);
    fh2_Califa_theta_energy->GetXaxis()->SetTitle("Theta [degrees]");
    fh2_Califa_theta_energy->GetYaxis()->SetTitle("Energy [keV]");
    fh2_Califa_theta_energy->GetYaxis()->SetTitleOffset(1.4);
    fh2_Califa_theta_energy->GetXaxis()->CenterTitle(true);
    fh2_Califa_theta_energy->GetYaxis()->CenterTitle(true);
    fh2_Califa_theta_energy->Draw("COLZ");

    // CANVAS Total energy
    sprintf(Name1, "Califa_calorimeter_total_energy_per_hit");
    sprintf(Name2, "fh_Califa_total_energy");
    sprintf(Name3, "Califa total energy per hit for the full calorimeter");
    cCalifa_hitenergy = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    fh1_Califa_total_energy = new TH1F(Name2, Name3, bins, minE, maxE);
    fh1_Califa_total_energy->GetXaxis()->SetTitle("Energy [keV]");
    fh1_Califa_total_energy->GetYaxis()->SetTitle("Counts");
    fh1_Califa_total_energy->GetYaxis()->SetTitleOffset(1.4);
    fh1_Califa_total_energy->GetXaxis()->CenterTitle(true);
    fh1_Califa_total_energy->GetYaxis()->CenterTitle(true);
    fh1_Califa_total_energy->SetFillColor(29);
    fh1_Califa_total_energy->SetLineColor(1);
    fh1_Califa_total_energy->SetLineWidth(2);
    fh1_Califa_total_energy->Draw("");
    gPad->SetLogy();

    // Difference between Califa WRs
    sprintf(Name1, "WR_Califa");
    sprintf(Name2, "fh1_WR_Califa");
    sprintf(Name3, "WR-Wixhausen - WR-Messel");
    cCalifa_wr = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    fh1_Califa_wr = new TH1F(Name2, Name3, 1200, -4100, 4100);
    fh1_Califa_wr->GetXaxis()->SetTitle("Difference of Califa WRs");
    fh1_Califa_wr->GetYaxis()->SetTitle("Counts");
    fh1_Califa_wr->GetYaxis()->SetTitleOffset(1.3);
    fh1_Califa_wr->GetXaxis()->CenterTitle(true);
    fh1_Califa_wr->GetYaxis()->CenterTitle(true);
    fh1_Califa_wr->SetFillColor(29);
    fh1_Califa_wr->SetLineColor(1);
    fh1_Califa_wr->SetLineWidth(2);
    fh1_Califa_wr->Draw("");

    // Difference between Califa-Master WRs
    sprintf(Name1, "WR_Master_Califa");
    cWrs = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    sprintf(Name2, "fh1_WR_Master_Califa");
    sprintf(Name3, "WR-Master - WR-Califa: Messel (blue), Wixhausen (red)");
    fh1_wrs[0] = new TH1F(Name2, Name3, 1200, -4100, 4100);
    fh1_wrs[0]->GetXaxis()->SetTitle("WRs difference");
    fh1_wrs[0]->GetYaxis()->SetTitle("Counts");
    fh1_wrs[0]->GetYaxis()->SetTitleOffset(1.3);
    fh1_wrs[0]->GetXaxis()->CenterTitle(true);
    fh1_wrs[0]->GetYaxis()->CenterTitle(true);
    fh1_wrs[0]->SetLineColor(4);
    fh1_wrs[0]->SetLineWidth(3);
    fh1_wrs[0]->Draw("");
    fh1_wrs[1] = new TH1F("fh1_WR_Master_Califa_Messel", "", 1200, -4100, 4100);
    fh1_wrs[1]->SetLineColor(2);
    fh1_wrs[1]->SetLineWidth(3);
    fh1_wrs[1]->Draw("same");

    // FOLDERS for Califa
    TFolder* folder_sta = new TFolder("Statistics_per_ring", "Statistics info");
    for (Int_t i = 2; i < fNumRings; i++)
    { // FIXME in the future
        folder_sta->Add(cMap_RingR[i]);
        folder_sta->Add(cMap_RingL[i]);
    }

    TFolder* folder_el = new TFolder("Energy_Map_per_crystal_Left", "Energy per crystal, left side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][0] != -1)
                folder_el->Add(cMapCry[1][r][p]);

    TFolder* folder_etotl = new TFolder("Energy_Tot_per_crystal_Left", "Energy vs Tot per crystal, left side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][0] != -1)
                folder_etotl->Add(cMapCryTot[1][r][p]);

    TFolder* folder_eprl = new TFolder("Energy_Map_per_crystal_Left_PR", "Energy per crystal, left side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][2] != -1)
                folder_eprl->Add(cMapCryP[1][r][p]);

    TFolder* folder_eprtotl =
        new TFolder("Energy_Tot_per_crystal_Left_PR", "Energy vs Tot per crystal, left side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][2] != -1)
                folder_eprtotl->Add(cMapCryPTot[1][r][p]);

    TFolder* folder_er = new TFolder("Energy_Map_per_crystal_Right", "Energy per crystal, right side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][0] != -1)
                folder_er->Add(cMapCry[0][r][p]);

    TFolder* folder_etotr = new TFolder("Energy_Tot_per_crystal_Right", "Energy vs Tot per crystal, right side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][0] != -1)
                folder_etotr->Add(cMapCryTot[0][r][p]);

    TFolder* folder_eprr = new TFolder("Energy_Map_per_crystal_Right_PR", "Energy per crystal, right side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][2] != -1)
                folder_eprr->Add(cMapCryP[0][r][p]);

    TFolder* folder_eprtotr =
        new TFolder("Energy_Tot_per_crystal_Right_PR", "Energy vs Tot per crystal, right side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][2] != -1)
                folder_eprtotr->Add(cMapCryPTot[0][r][p]);

    TFolder* folder_ecall = new TFolder("Energy_Cal_per_crystal_Left", "Energy Cal per crystal, left side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][0] != -1)
                folder_ecall->Add(cMapCryCal[1][r][p]);

    TFolder* folder_eprcall =
        new TFolder("Energy_Cal_per_crystal_Left_PR", "Energy Cal per crystal, left side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][2] != -1)
                folder_eprcall->Add(cMapCryPCal[1][r][p]);

    TFolder* folder_ecalr = new TFolder("Energy_Cal_per_crystal_Right", "Energy Cal per crystal, right side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][0] != -1)
                folder_ecalr->Add(cMapCryCal[0][r][p]);

    TFolder* folder_eprcalr =
        new TFolder("Energy_Cal_per_crystal_Right_PR", "Energy Cal per crystal, right side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][2] != -1)
                folder_eprcalr->Add(cMapCryPCal[0][r][p]);

    // MAIN FOLDER-Califa
    TFolder* mainfolCalifa = new TFolder("CALIFA", "CALIFA info");
    if (fWRItemsCalifa)
        mainfolCalifa->Add(cCalifa_wr);
    if (fWRItemsCalifa && fWRItemsMaster)
        mainfolCalifa->Add(cWrs);
    mainfolCalifa->Add(cCalifaMult);
    mainfolCalifa->Add(cCalifa_cry_energy);
    mainfolCalifa->Add(folder_sta);
    mainfolCalifa->Add(folder_el);
    mainfolCalifa->Add(folder_eprl);
    mainfolCalifa->Add(folder_er);
    mainfolCalifa->Add(folder_eprr);

    mainfolCalifa->Add(folder_etotl);
    mainfolCalifa->Add(folder_eprtotl);
    mainfolCalifa->Add(folder_etotr);
    mainfolCalifa->Add(folder_eprtotr);

    if (fCalItemsCalifa)
    {
        mainfolCalifa->Add(cCalifa_cry_energy_cal);
        mainfolCalifa->Add(folder_ecall);
        mainfolCalifa->Add(folder_eprcall);
        mainfolCalifa->Add(folder_ecalr);
        mainfolCalifa->Add(folder_eprcalr);
    }
    if (fHitItemsCalifa)
    {
        mainfolCalifa->Add(cCalifaCoinE);
        mainfolCalifa->Add(cCalifaCoinTheta);
        mainfolCalifa->Add(cCalifaCoinPhi);
        mainfolCalifa->Add(cCalifa_angles);
        mainfolCalifa->Add(cCalifa_theta_energy);
        mainfolCalifa->Add(cCalifa_hitenergy);
    }
    run->AddObject(mainfolCalifa);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Califa", Form("/Objects/%s/->Reset_CALIFA_Histo()", GetName()));
    // Register command for moving between Febex and Preamp channels
    run->GetHttpServer()->RegisterCommand("Febex2Preamp_Califa",
                                          Form("/Objects/%s/->Febex2Preamp_CALIFA_Histo()", GetName()));
    // Register command to change the histogram scales (Log/Lineal)
    run->GetHttpServer()->RegisterCommand("Log_Califa", Form("/Objects/%s/->Log_CALIFA_Histo()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BCalifaOnlineSpectra::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BCalifaOnlineSpectra::Reset_CALIFA_Histo()
{
    LOG(INFO) << "R3BCalifaOnlineSpectra::Reset_CALIFA_Histo";

    if (fWRItemsCalifa)
        fh1_Califa_wr->Reset();

    if (fWRItemsCalifa && fWRItemsMaster)
    {
        fh1_wrs[0]->Reset();
        fh1_wrs[1]->Reset();
    }

    if (fMappedItemsCalifa)
    {
        fh1_Califa_Mult->Reset();
        fh2_Califa_cryId_energy->Reset();
        for (Int_t i = 0; i < fNumRings; i++)
        {
            fh2_Preamp_vs_ch_R[i]->Reset();
            fh2_Preamp_vs_ch_L[i]->Reset();
        }
        for (Int_t s = 0; s < fNumSides; s++)
            for (Int_t r = 0; r < fNumRings; r++)
                for (Int_t p = 0; p < fNumPreamps; p++)
                    for (Int_t ch = 0; ch < fNumCrystalPreamp; ch++)
                    {
                        if (fFebexInfo[s][r][p][0] != -1)
                        {
                            fh1_crystals[s][r][p][ch]->Reset();
                            fh2_crystalsETot[s][r][p][ch]->Reset();
                        }
                        if (fFebexInfo[s][r][p][2] != -1)
                        {
                            fh1_crystals_p[s][r][p][ch]->Reset();
                            fh2_crystalsETot_p[s][r][p][ch]->Reset();
                        }
                    }
    }

    if (fCalItemsCalifa)
    {
        fh2_Califa_cryId_energy_cal->Reset();
        for (Int_t s = 0; s < fNumSides; s++)
            for (Int_t r = 0; r < fNumRings; r++)
                for (Int_t p = 0; p < fNumPreamps; p++)
                    for (Int_t ch = 0; ch < fNumCrystalPreamp; ch++)
                    {
                        if (fFebexInfo[s][r][p][0] != -1)
                            fh1_crystals_cal[s][r][p][ch]->Reset();
                        if (fFebexInfo[s][r][p][2] != -1)
                            fh1_crystals_p_cal[s][r][p][ch]->Reset();
                    }
    }

    if (fHitItemsCalifa)
    {
        fh1_Califa_MultHit->Reset();
        fh2_Califa_coinE->Reset();
        fh2_Califa_coinTheta->Reset();
        fh2_Califa_coinPhi->Reset();
        fh2_Califa_theta_phi->Reset();
        fh2_Califa_theta_energy->Reset();
        fh1_Califa_total_energy->Reset();
    }
}

void R3BCalifaOnlineSpectra::Log_CALIFA_Histo()
{

    LOG(INFO) << "R3BCalifaOnlineSpectra::Log_CALIFA_Histo";

    cCalifa_cry_energy->cd();
    if (fLogScale)
    {
        gPad->SetLogz(0);
    }
    else
    {
        gPad->SetLogz(1);
    }

    cCalifaMult->cd();
    if (fLogScale)
    {
        gPad->SetLogy(0);
    }
    else
    {
        gPad->SetLogy(1);
    }

    for (Int_t s = 0; s < fNumSides; s++)
    {
        for (Int_t r = 0; r < fNumRings; r++)
        {
            for (Int_t p = 0; p < fNumPreamps; p++)
            {

                for (Int_t j = 0; j < fNumCrystalPreamp; j++)
                {
                    if (fFebexInfo[s][r][p][0] != -1)
                    {
                        cMapCry[s][r][p]->cd(j + 1);
                        if (fLogScale)
                        {
                            gPad->SetLogy(0);
                        }
                        else
                        {
                            gPad->SetLogy(1);
                        }
                    }
                    if (fFebexInfo[s][r][p][2] != -1)
                    { // histograms for proton range
                        cMapCryP[s][r][p]->cd(j + 1);
                        if (fLogScale)
                        {
                            gPad->SetLogy(0);
                        }
                        else
                        {
                            gPad->SetLogy(1);
                        }
                    }
                    if (fCalItemsCalifa)
                    {
                        if (fFebexInfo[s][r][p][0] != -1)
                        {
                            cMapCryCal[s][r][p]->cd(p + 1);
                            if (fLogScale)
                            {
                                gPad->SetLogy(0);
                            }
                            else
                            {
                                gPad->SetLogy(1);
                            }
                        }
                        if (fFebexInfo[s][r][p][2] != -1)
                        { // histograms for proton range
                            cMapCryPCal[s][r][p]->cd(j + 1);
                            if (fLogScale)
                            {
                                gPad->SetLogy(0);
                            }
                            else
                            {
                                gPad->SetLogy(1);
                            }
                        }
                    }
                }
            }
        }
    }

    if (fCalItemsCalifa)
    {
        cCalifa_cry_energy_cal->cd();
        if (fLogScale)
        {
            gPad->SetLogz(0);
        }
        else
        {
            gPad->SetLogz(1);
        }
    }

    if (fHitItemsCalifa)
    {
        cCalifa_hitenergy->cd();
        if (fLogScale)
        {
            gPad->SetLogy(0);
        }
        else
        {
            gPad->SetLogy(1);
        }
    }

    if (fLogScale)
        fLogScale = kFALSE;
    else
        fLogScale = kTRUE;
}

void R3BCalifaOnlineSpectra::Febex2Preamp_CALIFA_Histo()
{
    LOG(INFO) << "R3BCalifaOnlineSpectra::Febex2Preamp_CALIFA_Histo";

    char Name[255];
    char Side[255];

    if (fFebex2Preamp)
    { // Preamp to Febex sequence
        for (Int_t s = 0; s < fNumSides; s++)
        {
            if (s == 1)
                sprintf(Side, "Left");
            else
                sprintf(Side, "Right");
            for (Int_t r = 0; r < fNumRings; r++)
            {
                for (Int_t p = 0; p < fNumPreamps; p++)
                {
                    for (Int_t j = 0; j < fNumCrystalPreamp; j++)
                    {
                        if (fFebexInfo[s][r][p][0] != -1)
                        {
                            cMapCry[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                            fh1_crystals[s][r][p][j]->SetFillColor(kGreen);
                            sprintf(Name,
                                    "Map level, Side %s Ring %d, Slot %d, Febex %d, ch. %d",
                                    Side,
                                    r + 1,
                                    fFebexInfo[s][r][p][0],
                                    fFebexInfo[s][r][p][1],
                                    fOrderFebexPreamp[j]);
                            fh1_crystals[s][r][p][j]->SetTitle(Name);
                            fh1_crystals[s][r][p][j]->Draw();

                            cMapCryTot[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                            sprintf(Name,
                                    "Map level, Side %s Ring %d, Slot %d, Febex %d, ch. %d",
                                    Side,
                                    r + 1,
                                    fFebexInfo[s][r][p][0],
                                    fFebexInfo[s][r][p][1],
                                    fOrderFebexPreamp[j]);
                            fh2_crystalsETot[s][r][p][j]->SetTitle(Name);
                            fh2_crystalsETot[s][r][p][j]->Draw();
                        }
                        if (fFebexInfo[s][r][p][2] != -1)
                        {
                            cMapCryP[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                            fh1_crystals_p[s][r][p][j]->SetFillColor(kGreen);
                            sprintf(Name,
                                    "Map level (PR), Side %s Ring %d, Slot %d, Febex %d, ch. %d",
                                    Side,
                                    r + 1,
                                    fFebexInfo[s][r][p][2],
                                    fFebexInfo[s][r][p][3],
                                    fOrderFebexPreamp[j]);
                            fh1_crystals_p[s][r][p][j]->SetTitle(Name);
                            fh1_crystals_p[s][r][p][j]->Draw();

                            cMapCryPTot[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                            sprintf(Name,
                                    "Map level (PR), Side %s Ring %d, Slot %d, Febex %d, ch. %d",
                                    Side,
                                    r + 1,
                                    fFebexInfo[s][r][p][2],
                                    fFebexInfo[s][r][p][3],
                                    fOrderFebexPreamp[j]);
                            fh2_crystalsETot_p[s][r][p][j]->SetTitle(Name);
                            fh2_crystalsETot_p[s][r][p][j]->Draw();
                        }

                        if (fCalItemsCalifa)
                        {
                            if (fFebexInfo[s][r][p][0] != -1)
                            {
                                cMapCryCal[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                                fh1_crystals_cal[s][r][p][j]->SetFillColor(kGreen);
                                sprintf(Name,
                                        "Cal level, Side %s Ring %d, Slot %d, Febex %d, ch. %d",
                                        Side,
                                        r + 1,
                                        fFebexInfo[s][r][p][0],
                                        fFebexInfo[s][r][p][1],
                                        fOrderFebexPreamp[j]);
                                fh1_crystals_cal[s][r][p][j]->SetTitle(Name);
                                fh1_crystals_cal[s][r][p][j]->Draw();
                            }
                            if (fFebexInfo[s][r][p][2] != -1)
                            {
                                cMapCryPCal[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                                fh1_crystals_p_cal[s][r][p][j]->SetFillColor(kGreen);
                                sprintf(Name,
                                        "Cal level (PR), Side %s Ring %d, Slot %d, Febex %d, ch. %d",
                                        Side,
                                        r + 1,
                                        fFebexInfo[s][r][p][2],
                                        fFebexInfo[s][r][p][3],
                                        fOrderFebexPreamp[j]);
                                fh1_crystals_p_cal[s][r][p][j]->SetTitle(Name);
                                fh1_crystals_p_cal[s][r][p][j]->Draw();
                            }
                        }
                    }
                }
            }
        }
        fFebex2Preamp = kFALSE;
    }
    else
    { // Febex to Preamp sequence
        for (Int_t s = 0; s < fNumSides; s++)
        {
            if (s == 1)
                sprintf(Side, "Left");
            else
                sprintf(Side, "Right");
            for (Int_t r = 0; r < fNumRings; r++)
            {
                for (Int_t p = 0; p < fNumPreamps; p++)
                {
                    for (Int_t j = 0; j < fNumCrystalPreamp; j++)
                    {
                        if (fFebexInfo[s][r][p][0] != -1)
                        {
                            cMapCry[s][r][p]->cd(j + 1);
                            fh1_crystals[s][r][p][j]->SetFillColor(45);
                            sprintf(Name, "Map level, Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                            fh1_crystals[s][r][p][j]->SetTitle(Name);
                            fh1_crystals[s][r][p][j]->Draw();

                            cMapCryTot[s][r][p]->cd(j + 1);
                            sprintf(Name, "Map level, Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                            fh2_crystalsETot[s][r][p][j]->SetTitle(Name);
                            fh2_crystalsETot[s][r][p][j]->Draw();
                        }
                        if (fFebexInfo[s][r][p][2] != -1)
                        {
                            cMapCryP[s][r][p]->cd(j + 1);
                            fh1_crystals_p[s][r][p][j]->SetFillColor(45);
                            sprintf(
                                Name, "Map level (PR), Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                            fh1_crystals_p[s][r][p][j]->SetTitle(Name);
                            fh1_crystals_p[s][r][p][j]->Draw();

                            cMapCryPTot[s][r][p]->cd(j + 1);
                            sprintf(
                                Name, "Map level (PR), Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                            fh2_crystalsETot_p[s][r][p][j]->SetTitle(Name);
                            fh2_crystalsETot_p[s][r][p][j]->Draw();
                        }

                        if (fCalItemsCalifa)
                        {
                            if (fFebexInfo[s][r][p][0] != -1)
                            {
                                cMapCryCal[s][r][p]->cd(j + 1);
                                fh1_crystals_cal[s][r][p][j]->SetFillColor(45);
                                sprintf(
                                    Name, "Cal level, Side %s Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                                fh1_crystals_cal[s][r][p][j]->SetTitle(Name);
                                fh1_crystals_cal[s][r][p][j]->Draw();
                            }

                            if (fFebexInfo[s][r][p][2] != -1)
                            {
                                cMapCryPCal[s][r][p]->cd(j + 1);
                                fh1_crystals_p_cal[s][r][p][j]->SetFillColor(45);
                                sprintf(Name,
                                        "Cal level (PR), Side %s Ring %d, Preamp %d, ch. %d",
                                        Side,
                                        r + 1,
                                        p + 1,
                                        j + 1);
                                fh1_crystals_p_cal[s][r][p][j]->SetTitle(Name);
                                fh1_crystals_p_cal[s][r][p][j]->Draw();
                            }
                        }
                    }
                }
            }
        }
        fFebex2Preamp = kTRUE;
    }
}

void R3BCalifaOnlineSpectra::Exec(Option_t* option)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BCalifaOnlineSpectra::Exec FairRootManager not found";

    // WR data
    if (fWRItemsCalifa && fWRItemsCalifa->GetEntriesFast() > 0)
    {
        // Califa
        Int_t nHits = fWRItemsCalifa->GetEntriesFast();
        int64_t wr[nHits];
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BWRCalifaData* hit = (R3BWRCalifaData*)fWRItemsCalifa->At(ihit);
            if (!hit)
                continue;
            wr[ihit] = hit->GetTimeStamp();
        }
        if (nHits == 2)
            fh1_Califa_wr->Fill(wr[1] - wr[0]);

        // Master from SOFIA (exp. 2020)
        if (fWRItemsMaster && fWRItemsMaster->GetEntriesFast() > 0)
        {
            nHits = fWRItemsMaster->GetEntriesFast();
            int64_t wrm;
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BWRMasterData* hit = (R3BWRMasterData*)fWRItemsMaster->At(ihit);
                if (!hit)
                    continue;
                wrm = hit->GetTimeStamp();
            }
            fh1_wrs[0]->Fill(wrm - wr[0]); // messel
            fh1_wrs[1]->Fill(wrm - wr[1]); // wixhausen
        }
    }

    // Mapped data
    if (fMappedItemsCalifa && fMappedItemsCalifa->GetEntriesFast() > 0)
    {
        Int_t nHits = fMappedItemsCalifa->GetEntriesFast();
        Int_t Crymult = 0;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BCalifaMappedData* hit = (R3BCalifaMappedData*)fMappedItemsCalifa->At(ihit);
            if (!hit)
                continue;

            Int_t cryId = hit->GetCrystalId();
            if ((fMap_Par->GetInUse(cryId) == 1 && cryId <= fNbCalifaCrystals / 2) ||
                (cryId > fNbCalifaCrystals / 2 &&
                 fMap_Par->GetInUse(cryId) != fMap_Par->GetInUse(cryId - fNbCalifaCrystals / 2)))
                Crymult++;

            fh2_Califa_cryId_energy->Fill(cryId, hit->GetEnergy());

            if (fMap_Par->GetHalf(cryId) == 2)
                fh2_Preamp_vs_ch_L[fMap_Par->GetRing(cryId) - 1]->Fill(fMap_Par->GetPreamp(cryId),
                                                                       fMap_Par->GetChannel(cryId));
            if (fMap_Par->GetHalf(cryId) == 1)
                fh2_Preamp_vs_ch_R[fMap_Par->GetRing(cryId) - 1]->Fill(fMap_Par->GetPreamp(cryId),
                                                                       fMap_Par->GetChannel(cryId));

            if (fMap_Par->GetInUse(cryId) == 1 && cryId <= fNbCalifaCrystals / 2)
            {
                fh1_crystals[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1][fMap_Par->GetPreamp(cryId) - 1]
                            [fMap_Par->GetChannel(cryId) - 1]
                                ->Fill(hit->GetEnergy());
                fh2_crystalsETot[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                    ->Fill(hit->GetEnergy(), hit->GetTot());
            }

            else if (fMap_Par->GetInUse(cryId) == 1 && cryId > fNbCalifaCrystals / 2)
            {
                fh1_crystals_p[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                              [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                  ->Fill(hit->GetEnergy());
                fh2_crystalsETot_p[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                  [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                      ->Fill(hit->GetEnergy(), hit->GetTot());
            }
        }
        fh1_Califa_Mult->Fill(Crymult);
    }

    // Cal data
    if (fCalItemsCalifa && fCalItemsCalifa->GetEntriesFast() > 0)
    {
        Int_t nHits = fCalItemsCalifa->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BCalifaCrystalCalData* hit = (R3BCalifaCrystalCalData*)fCalItemsCalifa->At(ihit);
            if (!hit)
                continue;

            Int_t cryId = hit->GetCrystalId();

            fh2_Califa_cryId_energy_cal->Fill(cryId, hit->GetEnergy());

            if (fMap_Par->GetInUse(cryId) == 1 && cryId <= fNbCalifaCrystals / 2)
                fh1_crystals_cal[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                    ->Fill(hit->GetEnergy());
            else if (fMap_Par->GetInUse(cryId) == 1 && cryId > fNbCalifaCrystals / 2)
                fh1_crystals_p_cal[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                  [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                      ->Fill(hit->GetEnergy());
        }
    }

    // Hit data
    if (fHitItemsCalifa && fHitItemsCalifa->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsCalifa->GetEntriesFast();
        fh1_Califa_MultHit->Fill(nHits);

        Double_t theta = 0., phi = 0.;
        Double_t califa_theta[nHits];
        Double_t califa_phi[nHits];
        Double_t califa_e[nHits];
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BCalifaHitData* hit = (R3BCalifaHitData*)fHitItemsCalifa->At(ihit);
            if (!hit)
                continue;
            theta = hit->GetTheta() * TMath::RadToDeg();
            phi = hit->GetPhi() * TMath::RadToDeg();
            califa_theta[ihit] = theta;
            califa_phi[ihit] = phi;
            califa_e[ihit] = hit->GetEnergy();
            fh2_Califa_theta_phi->Fill(theta, phi);
            fh2_Califa_theta_energy->Fill(theta + gRandom->Uniform(-1.5, 1.5), hit->GetEnergy());
            fh1_Califa_total_energy->Fill(hit->GetEnergy());
        }
        // Comparison of hits to get energy, theta and phi correlations between them
        for (Int_t i1 = 0; i1 < nHits; i1++)
            for (Int_t i2 = i1 + 1; i2 < nHits; i2++)
                if (gRandom->Uniform(0., 1.) < 0.5)
                {
                    fh2_Califa_coinE->Fill(califa_e[i1], califa_e[i2]);
                    fh2_Califa_coinTheta->Fill(califa_theta[i1], califa_theta[i2]);
                    fh2_Califa_coinPhi->Fill(califa_phi[i1], califa_phi[i2]);
                }
                else
                {
                    fh2_Califa_coinE->Fill(califa_e[i2], califa_e[i1]);
                    fh2_Califa_coinTheta->Fill(califa_theta[i2], califa_theta[i1]);
                    fh2_Califa_coinPhi->Fill(califa_phi[i2], califa_phi[i1]);
                }
    }

    fNEvents += 1;
}

void R3BCalifaOnlineSpectra::FinishEvent()
{
    if (fMappedItemsCalifa)
    {
        fMappedItemsCalifa->Clear();
    }
    if (fCalItemsCalifa)
    {
        fCalItemsCalifa->Clear();
    }
    if (fHitItemsCalifa)
    {
        fHitItemsCalifa->Clear();
    }
    if (fWRItemsCalifa)
    {
        fWRItemsCalifa->Clear();
    }
    if (fWRItemsMaster)
    {
        fWRItemsMaster->Clear();
    }
}

void R3BCalifaOnlineSpectra::FinishTask()
{
    // Write canvas for Califa WR data
    if (fWRItemsCalifa)
    {
        cCalifa_wr->Write();
    }

    // Write canvas for Master-Califa WR data
    if (fWRItemsMaster && fWRItemsCalifa)
    {
        cWrs->Write();
    }

    // Write canvas for Mapped data
    if (fMappedItemsCalifa)
    {
        cCalifaMult->Write();
        cCalifa_cry_energy->Write();
        for (Int_t i = 0; i < fNumRings; i++)
        {
            cMap_RingR[i]->Write();
            cMap_RingL[i]->Write();
        }
        for (Int_t s = 0; s < fNumSides; s++)
            for (Int_t r = 0; r < fNumRings; r++)
                for (Int_t p = 0; p < fNumPreamps; p++)
                {
                    if (fFebexInfo[s][r][p][0] != -1)
                        cMapCry[s][r][p]->Write();
                    if (fFebexInfo[s][r][p][2] != -1)
                        cMapCryP[s][r][p]->Write();
                }
    }

    // Write canvas for Cal data
    if (fCalItemsCalifa)
    {
        cCalifa_cry_energy_cal->Write();
        for (Int_t s = 0; s < fNumSides; s++)
            for (Int_t r = 0; r < fNumRings; r++)
                for (Int_t p = 0; p < fNumPreamps; p++)
                {
                    if (fFebexInfo[s][r][p][0] != -1)
                        cMapCryCal[s][r][p]->Write();
                    if (fFebexInfo[s][r][p][2] != -1)
                        cMapCryPCal[s][r][p]->Write();
                }
    }

    // Write canvas for Hit data
    if (fHitItemsCalifa)
    {
        cCalifaCoinE->Write();
        cCalifaCoinTheta->Write();
        cCalifaCoinPhi->Write();
        cCalifa_angles->Write();
        cCalifa_theta_energy->Write();
        cCalifa_hitenergy->Write();
    }
}

ClassImp(R3BCalifaOnlineSpectra)
