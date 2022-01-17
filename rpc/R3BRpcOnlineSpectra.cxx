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

#include "R3BRpcMappedData.h"
#include "R3BRpcCalData.h"
#include "R3BRpcHitData.h"
#include "R3BRpcOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BWRData.h"
#include "THttpServer.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TVector3.h"

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

R3BRpcOnlineSpectra::R3BRpcOnlineSpectra()
    : FairTask("RPCOnlineSpectra", 1)
    , fMappedItems(NULL)
    , fCalItems(NULL)
    , fHitItems(NULL)
    , fNEvents(0)
    , fNumSides(Nb_Sides)
    , fNumChannels(Nb_Channels)
    , fMapHistos_bins(500)
    , fMapHistos_max(4000)
    , fLogScale(kTRUE)
    , fTotHist(kFALSE)
{
}

R3BRpcOnlineSpectra::R3BRpcOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fCalItems(NULL)
    , fHitItems(NULL)
    , fNEvents(0)
    , fNumSides(Nb_Sides)
    , fNumChannels(Nb_Channels)
    , fMapHistos_bins(500)
    , fMapHistos_max(4000)
    , fLogScale(kTRUE)
    , fTotHist(kFALSE)
{
}

R3BRpcOnlineSpectra::~R3BRpcOnlineSpectra()
{
    LOG(INFO) << "R3BRpcOnlineSpectra: Delete instance";
    if (fMappedItems)
        delete fMappedItems;
    if (fCalItems)
        delete fCalItems;
    if (fHitItems)
        delete fHitItems;
}

void R3BRpcOnlineSpectra::SetParContainers()
{
    // Parameter Container
    // Reading from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "FairRuntimeDb not opened!";
    }
}

void R3BRpcOnlineSpectra::SetParameter() {}

InitStatus R3BRpcOnlineSpectra::Init()
{
    LOG(INFO) << "R3BRpcOnlineSpectra::Init()";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BRpcOnlineSpectra::Init FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to Mapped data
    fMappedItems = (TClonesArray*)mgr->GetObject("RPCMappedData");
    if (!fMappedItems)
    {
        LOG(ERROR) << "R3BRpcOnlineSpectra::RPCMappedData not found";
        return kFATAL;
    }

    // get access to Cal data
    fCalItems = (TClonesArray*)mgr->GetObject("RPCCalData");
    if (!fCalItems)
    {
        LOG(WARNING) << "R3BRpcOnlineSpectra::RPCCalData not found";
    }

    // get access to Hit data
    fHitItems = (TClonesArray*)mgr->GetObject("RPCHitData");
    if (!fHitItems)
    {
        LOG(WARNING) << "R3BRpcOnlineSpectra::RPCHitData not found";
    }

    SetParameter();

    // Create histograms for detectors
    char Name1[255];
    char Name2[255];
    char Name3[255];
    TString Xaxis;
    Double_t bins = fMapHistos_bins;
    Double_t maxE = fMapHistos_max;
    Double_t minE = 0.;

    // CANVAS Crystal_ID vs energy
    cRpc_cry_energy = new TCanvas("Rpc_Map_cryID_energy", "Rpc_Map energy vs cryID", 10, 10, 500, 500);
    fh2_Rpc_cryId_energy = new TH2F("fh2_Rpc_Map_cryID_energy",
                                       "Rpc: CryID vs energy",
                                       fMap_Par->GetNumCrystals(),
                                       0.5,
                                       fMap_Par->GetNumCrystals() + 0.5,
                                       fBinsChannelFebex,
                                       0.0,
                                       fMaxBinChannelFebex);
    fh2_Rpc_cryId_energy->GetXaxis()->SetTitle("Crystal ID");
    fh2_Rpc_cryId_energy->GetYaxis()->SetTitle("Energy [Channels]");
    fh2_Rpc_cryId_energy->GetYaxis()->SetTitleOffset(1.4);
    fh2_Rpc_cryId_energy->GetXaxis()->CenterTitle(true);
    fh2_Rpc_cryId_energy->GetYaxis()->CenterTitle(true);
    gPad->SetLogz();
    fh2_Rpc_cryId_energy->Draw("COLZ");

    // CANVAS Crystal_ID vs energy
    cRpc_cry_energy_cal = new TCanvas("Rpc_Cal_cryID_energy", "Rpc_Cal energy vs cryID", 10, 10, 500, 500);
    fh2_Rpc_cryId_energy_cal = new TH2F("fh2_Rpc_Cal_cryID_energy",
                                           "Rpc: CryID vs calibrated energy",
                                           fMap_Par->GetNumCrystals(),
                                           0.5,
                                           fMap_Par->GetNumCrystals() + 0.5,
                                           bins,
                                           minE,
                                           maxE);
    fh2_Rpc_cryId_energy_cal->GetXaxis()->SetTitle("Crystal ID");
    fh2_Rpc_cryId_energy_cal->GetYaxis()->SetTitle("Energy [keV]");
    fh2_Rpc_cryId_energy_cal->GetYaxis()->SetTitleOffset(1.4);
    fh2_Rpc_cryId_energy_cal->GetXaxis()->CenterTitle(true);
    fh2_Rpc_cryId_energy_cal->GetYaxis()->CenterTitle(true);
    gPad->SetLogz();
    fh2_Rpc_cryId_energy_cal->Draw("COLZ");

    cRpc_NsNf = new TCanvas("Rpc_Cal_NsNf", "Rpc_Cal Ns vs Nf", 10, 10, 500, 500);
    fh2_Rpc_NsNf =
        new TH2F("fh2_Rpc_Cal_NsNf", "Rpc PID: Ns vs Nf energies", bins, minE, maxE, bins, minE, maxE);
    fh2_Rpc_NsNf->GetXaxis()->SetTitle("Nf Energy [keV]");
    fh2_Rpc_NsNf->GetYaxis()->SetTitle("Ns Energy [keV]");
    fh2_Rpc_NsNf->GetYaxis()->SetTitleOffset(1.4);
    fh2_Rpc_NsNf->GetXaxis()->CenterTitle(true);
    fh2_Rpc_NsNf->GetYaxis()->CenterTitle(true);
    gPad->SetLogz();
    fh2_Rpc_NsNf->Draw("COLZ");

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

    char Side[50];
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
                        sprintf(Name2, "Map level, Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);

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
                        sprintf(Name2, "Map level, Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);

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
                        sprintf(
                            Name2, "Map level (PR), Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
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
                        sprintf(
                            Name2, "Map level (PR), Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
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
                        sprintf(Name2, "Cal level, Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);

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
                        sprintf(
                            Name2, "Cal level (PR), Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
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
    cPRCMult = new TCanvas("RPC_Multiplicity", "RPC_Multiplicity", 10, 10, 500, 500);
    fh1_Rpc_Mult = new TH1F("fh1_Rpc_Mult", "RPC multiplicity", 141, -0.5, 140.5);
    fh1_Rpc_MultHit = new TH1F("fh1_Rpc_MultHit", "Rpc multiplicity", 141, -0.5, 140.5);
    fh1_Rpc_Mult->GetXaxis()->SetTitle("Multiplicity");
    fh1_Rpc_Mult->GetXaxis()->CenterTitle(true);
    fh1_Rpc_Mult->GetYaxis()->CenterTitle(true);
    fh1_Rpc_Mult->GetYaxis()->SetTitleOffset(1.2);
    fh1_Rpc_Mult->GetXaxis()->SetTitleOffset(1.1);
    gPad->SetLogy();
    fh1_Rpc_Mult->Draw();
    fh1_Rpc_MultHit->SetLineColor(kRed);
    fh1_Rpc_MultHit->Draw("SAME");

    // CANVAS Energy correlations between hits
    cRpcCoinE = new TCanvas("Rpc_energy_correlation_hits", "Energy correlations, hit level", 10, 10, 500, 500);

    fh2_Rpc_coinE =
        new TH2F("fh2_Rpc_energy_correlations", "Rpc energy correlations", bins, minE, maxE, bins, minE, maxE);
    fh2_Rpc_coinE->GetXaxis()->SetTitle("Energy (keV)");
    fh2_Rpc_coinE->GetYaxis()->SetTitle("Energy (keV)");
    fh2_Rpc_coinE->GetYaxis()->SetTitleOffset(1.2);
    fh2_Rpc_coinE->GetXaxis()->CenterTitle(true);
    fh2_Rpc_coinE->GetYaxis()->CenterTitle(true);
    fh2_Rpc_coinE->Draw("COLZ");

    // CANVAS Theta correlations between hits
    cRpcCoinTheta = new TCanvas("Rpc_theta_correlation_hits", "Theta correlations, hit level", 10, 10, 500, 500);

    fh2_Rpc_coinTheta =
        new TH2F("fh2_Rpc_theta_correlations", "Rpc theta correlations", 50, 0, 100, 50, 0, 100);
    fh2_Rpc_coinTheta->GetXaxis()->SetTitle("Theta [degrees]");
    fh2_Rpc_coinTheta->GetYaxis()->SetTitle("Theta [degrees]");
    fh2_Rpc_coinTheta->GetYaxis()->SetTitleOffset(1.2);
    fh2_Rpc_coinTheta->GetXaxis()->CenterTitle(true);
    fh2_Rpc_coinTheta->GetYaxis()->CenterTitle(true);
    fh2_Rpc_coinTheta->Draw("COLZ");

    // CANVAS Phi correlations between hits
    cRpcCoinPhi = new TCanvas("Rpc_phi_correlation_hits", "Phi correlations, hit level", 10, 10, 500, 500);

    fh2_Rpc_coinPhi =
        new TH2F("fh2_Rpc_phi_correlations", "Rpc phi correlations", 90, -180, 180, 90, -180, 180);
    fh2_Rpc_coinPhi->GetXaxis()->SetTitle("Phi [degrees]");
    fh2_Rpc_coinPhi->GetYaxis()->SetTitle("Phi [degrees]");
    fh2_Rpc_coinPhi->GetYaxis()->SetTitleOffset(1.2);
    fh2_Rpc_coinPhi->GetXaxis()->CenterTitle(true);
    fh2_Rpc_coinPhi->GetYaxis()->CenterTitle(true);
    fh2_Rpc_coinPhi->Draw("COLZ");

    // CANVAS Theta vs Phi
    cRpc_angles = new TCanvas("Rpc_Theta_vs_Phi", "Theta vs Phi", 10, 10, 500, 500);
    fh2_Rpc_theta_phi = new TH2F("fh2_Rpc_theta_vs_phi", "Rpc theta vs phi", 50, 0, 90, 180, -180, 180);
    fh2_Rpc_theta_phi->GetXaxis()->SetTitle("Theta [degrees]");
    fh2_Rpc_theta_phi->GetYaxis()->SetTitle("Phi [degrees]");
    fh2_Rpc_theta_phi->GetYaxis()->SetTitleOffset(1.2);
    fh2_Rpc_theta_phi->GetXaxis()->CenterTitle(true);
    fh2_Rpc_theta_phi->GetYaxis()->CenterTitle(true);
    fh2_Rpc_theta_phi->Draw("COLZ");

    // CANVAS Theta vs energy
    sprintf(Name1, "Rpc_calorimeter_theta_vs_energy");
    sprintf(Name2, "fh_Rpc_theta_vs_total_energy");
    sprintf(Name3, "Rpc theta vs energy for full calorimeter");
    cRpc_theta_energy = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    fh2_Rpc_theta_energy = new TH2F(Name2, Name3, 360, 0, 90, bins, minE, maxE);
    fh2_Rpc_theta_energy->GetXaxis()->SetTitle("Theta [degrees]");
    fh2_Rpc_theta_energy->GetYaxis()->SetTitle("Energy [keV]");
    fh2_Rpc_theta_energy->GetYaxis()->SetTitleOffset(1.4);
    fh2_Rpc_theta_energy->GetXaxis()->CenterTitle(true);
    fh2_Rpc_theta_energy->GetYaxis()->CenterTitle(true);
    fh2_Rpc_theta_energy->Draw("COLZ");

    // CANVAS Total energy
    sprintf(Name1, "Rpc_calorimeter_total_energy_per_hit");
    sprintf(Name2, "fh_Rpc_total_energy");
    sprintf(Name3, "Rpc total energy per hit for the full calorimeter");
    cRpc_hitenergy = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    fh1_Rpc_total_energy = new TH1F(Name2, Name3, bins, minE, maxE);
    fh1_Rpc_total_energy->GetXaxis()->SetTitle("Energy [keV]");
    fh1_Rpc_total_energy->GetYaxis()->SetTitle("Counts");
    fh1_Rpc_total_energy->GetYaxis()->SetTitleOffset(1.4);
    fh1_Rpc_total_energy->GetXaxis()->CenterTitle(true);
    fh1_Rpc_total_energy->GetYaxis()->CenterTitle(true);
    fh1_Rpc_total_energy->SetFillColor(29);
    fh1_Rpc_total_energy->SetLineColor(1);
    fh1_Rpc_total_energy->SetLineWidth(2);
    fh1_Rpc_total_energy->Draw("");
    gPad->SetLogy();

    // CANVAS opening angle
    sprintf(Name1, "Rpc_opening_angle_hit");
    sprintf(Name2, "fh1_Rpc_opening");
    sprintf(Name3, "Rpc opening angle");
    cRpc_opening = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    fh1_openangle = new TH1F(Name2, Name3, 160, 10, 170);
    fh1_openangle->GetXaxis()->SetTitle("Opening angle [degrees]");
    fh1_openangle->GetYaxis()->SetTitle("Counts");
    fh1_openangle->GetXaxis()->CenterTitle(true);
    fh1_openangle->GetYaxis()->CenterTitle(true);
    fh1_openangle->GetYaxis()->SetTitleOffset(1.2);
    fh1_openangle->GetXaxis()->SetTitleOffset(1.2);
    fh1_openangle->SetFillColor(8);
    fh1_openangle->SetLineColor(1);
    fh1_openangle->SetLineWidth(2);
    fh1_openangle->Draw("");

    // Difference between Rpc WRs
    sprintf(Name1, "WR_Rpc");
    sprintf(Name2, "fh1_WR_Rpc");
    sprintf(Name3, "WR-Wixhausen - WR-Messel");
    cRpc_wr = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    fh1_Rpc_wr = new TH1I(Name2, Name3, 4000, -4000, 4000);
    fh1_Rpc_wr->GetXaxis()->SetTitle("Difference of Rpc WRs");
    fh1_Rpc_wr->GetYaxis()->SetTitle("Counts");
    fh1_Rpc_wr->GetYaxis()->SetTitleOffset(1.3);
    fh1_Rpc_wr->GetXaxis()->CenterTitle(true);
    fh1_Rpc_wr->GetYaxis()->CenterTitle(true);
    fh1_Rpc_wr->SetFillColor(29);
    fh1_Rpc_wr->SetLineColor(1);
    fh1_Rpc_wr->SetLineWidth(2);
    fh1_Rpc_wr->Draw("");

    // Difference between Rpc-Master WRs
    sprintf(Name1, "WR_Master_Rpc");
    cWrs = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    sprintf(Name2, "fh1_WR_Master_Rpc");
    sprintf(Name3, "WR-Master - WR-Rpc: Messel (blue), Wixhausen (red) - WR-Master");
    fh1_wrs[0] = new TH1I(Name2, Name3, 4000, -4000, 4000);
    fh1_wrs[0]->SetStats(1);
    fh1_wrs[0]->GetXaxis()->SetTitle("WRTs difference");
    fh1_wrs[0]->GetYaxis()->SetTitle("Counts");
    fh1_wrs[0]->GetYaxis()->SetTitleOffset(1.3);
    fh1_wrs[0]->GetXaxis()->CenterTitle(true);
    fh1_wrs[0]->GetYaxis()->CenterTitle(true);
    fh1_wrs[0]->SetLineColor(2);
    fh1_wrs[0]->SetLineWidth(3);
    fh1_wrs[0]->Draw("");
    fh1_wrs[1] = new TH1I("fh1_WR_Master_Rpc_Messel", "", 4000, -4000, 4000);
    fh1_wrs[1]->SetLineColor(4);
    fh1_wrs[1]->SetLineWidth(3);
    fh1_wrs[1]->Draw("same");

    // CANVAS energy vs wrs
    sprintf(Name1, "Rpc_wr_vs_energy");
    sprintf(Name2, "fh2_wr_vs_energy_left");
    sprintf(Name3, "Rpc WR vs hit-energy left side");
    cRpc_wr_energy = new TCanvas(Name1, Name1, 10, 10, 500, 500);
    cRpc_wr_energy->Divide(1, 2);
    fh2_Cal_wr_energy_l = new TH2F(Name2, Name3, 700, -4000, 4000, bins, minE, maxE);
    fh2_Cal_wr_energy_l->GetXaxis()->SetTitle("WR difference (Master-Rpc)");
    fh2_Cal_wr_energy_l->GetYaxis()->SetTitle("Energy [keV]");
    fh2_Cal_wr_energy_l->GetYaxis()->SetTitleOffset(1.4);
    fh2_Cal_wr_energy_l->GetXaxis()->CenterTitle(true);
    fh2_Cal_wr_energy_l->GetYaxis()->CenterTitle(true);
    cRpc_wr_energy->cd(1);
    fh2_Cal_wr_energy_l->Draw("COLZ");
    sprintf(Name2, "fh2_wr_vs_energy_right");
    sprintf(Name3, "Rpc WR vs hit-energy right side");
    fh2_Cal_wr_energy_r = new TH2F(Name2, Name3, 700, -4000, 4000, bins, minE, maxE);
    fh2_Cal_wr_energy_r->GetXaxis()->SetTitle("WR difference (Master-Rpc)");
    fh2_Cal_wr_energy_r->GetYaxis()->SetTitle("Energy [keV]");
    fh2_Cal_wr_energy_r->GetYaxis()->SetTitleOffset(1.4);
    fh2_Cal_wr_energy_r->GetXaxis()->CenterTitle(true);
    fh2_Cal_wr_energy_r->GetYaxis()->CenterTitle(true);
    cRpc_wr_energy->cd(2);
    fh2_Cal_wr_energy_r->Draw("COLZ");

    // FOLDERS for Rpc
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

    // MAIN FOLDER-Rpc
    TFolder* mainfolRpc = new TFolder("Rpc", "Rpc info");
    mainfolRpc->Add(cRpcMult);
    mainfolRpc->Add(cRpc_cry_energy);

    TFolder* folder_wrs = new TFolder("WRs", "Rpc white-rabbit info");

    if (fWRItemsRpc && fWRItemsMaster)
        folder_wrs->Add(cWrs);
    if (fWRItemsRpc)
    {
        folder_wrs->Add(cRpc_wr);
        folder_wrs->Add(cRpc_sync);
    }
    if (fWRItemsRpc)
        mainfolRpc->Add(folder_wrs);

    mainfolRpc->Add(folder_sta);
    mainfolRpc->Add(folder_el);
    mainfolRpc->Add(folder_eprl);
    mainfolRpc->Add(folder_er);
    mainfolRpc->Add(folder_eprr);

    if (fTotHist)
    {
        mainfolRpc->Add(folder_etotl);
        mainfolRpc->Add(folder_eprtotl);
        mainfolRpc->Add(folder_etotr);
        mainfolRpc->Add(folder_eprtotr);
    }

    if (fCalItemsRpc)
    {
        mainfolRpc->Add(cRpc_cry_energy_cal);
        mainfolRpc->Add(cRpc_NsNf);
        mainfolRpc->Add(folder_ecall);
        mainfolRpc->Add(folder_eprcall);
        mainfolRpc->Add(folder_ecalr);
        mainfolRpc->Add(folder_eprcalr);
    }
    if (fHitItemsRpc)
    {
        mainfolRpc->Add(cRpcCoinE);
        mainfolRpc->Add(cRpcCoinTheta);
        mainfolRpc->Add(cRpcCoinPhi);
        mainfolRpc->Add(cRpc_angles);
        mainfolRpc->Add(cRpc_opening);
        mainfolRpc->Add(cRpc_theta_energy);
        mainfolRpc->Add(cRpc_hitenergy);
        if (fWRItemsRpc && fWRItemsMaster)
            mainfolRpc->Add(cRpc_wr_energy);
    }
    run->AddObject(mainfolRpc);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Rpc", Form("/Objects/%s/->Reset_Rpc_Histo()", GetName()));
    // Register command for moving between Febex and Preamp channels
    run->GetHttpServer()->RegisterCommand("Febex2Preamp_Rpc",
                                          Form("/Objects/%s/->Febex2Preamp_Rpc_Histo()", GetName()));
    // Register command to change the histogram scales (Log/Lineal)
    run->GetHttpServer()->RegisterCommand("Log_Rpc", Form("/Objects/%s/->Log_Rpc_Histo()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BRpcOnlineSpectra::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BRpcOnlineSpectra::Reset_Rpc_Histo()
{
    LOG(INFO) << "R3BRpcOnlineSpectra::Reset_Rpc_Histo";

    if (fWRItemsRpc)
        fh1_Rpc_wr->Reset();

    if (fWRItemsRpc && fWRItemsMaster)
    {
        fh1_wrs[0]->Reset();
        fh1_wrs[1]->Reset();
        if (fHitItemsRpc)
        {
            fh2_Cal_wr_energy_r->Reset();
            fh2_Cal_wr_energy_l->Reset();
        }
    }

    if (fMappedItemsRpc)
    {
        fh1_Rpc_Mult->Reset();
        for (Int_t s = 0; s < 3; s++)
            fh1_Rpc_sync[s]->Reset();
        fh2_Rpc_cryId_energy->Reset();
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

    if (fCalItemsRpc)
    {
        fh2_Rpc_cryId_energy_cal->Reset();
        fh2_Rpc_NsNf->Reset();
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

    if (fHitItemsRpc)
    {
        fh1_Rpc_MultHit->Reset();
        fh2_Rpc_coinE->Reset();
        fh2_Rpc_coinTheta->Reset();
        fh2_Rpc_coinPhi->Reset();
        fh2_Rpc_theta_phi->Reset();
        fh2_Rpc_theta_energy->Reset();
        fh1_Rpc_total_energy->Reset();
        fh1_openangle->Reset();
    }
}

void R3BRpcOnlineSpectra::Log_Rpc_Histo()
{

    LOG(INFO) << "R3BRpcOnlineSpectra::Log_Rpc_Histo";

    cRpc_cry_energy->cd();
    if (fLogScale)
    {
        gPad->SetLogz(0);
    }
    else
    {
        gPad->SetLogz(1);
    }

    cRpcMult->cd();
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
                    if (fCalItemsRpc)
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

    if (fCalItemsRpc)
    {
        cRpc_cry_energy_cal->cd();
        if (fLogScale)
        {
            gPad->SetLogz(0);
        }
        else
        {
            gPad->SetLogz(1);
        }
    }

    if (fHitItemsRpc)
    {
        cRpc_hitenergy->cd();
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

void R3BRpcOnlineSpectra::Febex2Preamp_Rpc_Histo()
{
    LOG(INFO) << "R3BRpcOnlineSpectra::Febex2Preamp_Rpc_Histo";

    char Name[255];
    char Side[50];

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
                                    "Map level, Side %s, Ring %d, Slot %d, Febex %d, ch. %d",
                                    Side,
                                    r + 1,
                                    fFebexInfo[s][r][p][0],
                                    fFebexInfo[s][r][p][1],
                                    fOrderFebexPreamp[j]);
                            fh1_crystals[s][r][p][j]->SetTitle(Name);
                            fh1_crystals[s][r][p][j]->Draw();

                            cMapCryTot[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                            sprintf(Name,
                                    "Map level, Side %s, Ring %d, Slot %d, Febex %d, ch. %d",
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
                                    "Map level (PR), Side %s, Ring %d, Slot %d, Febex %d, ch. %d",
                                    Side,
                                    r + 1,
                                    fFebexInfo[s][r][p][2],
                                    fFebexInfo[s][r][p][3],
                                    fOrderFebexPreamp[j]);
                            fh1_crystals_p[s][r][p][j]->SetTitle(Name);
                            fh1_crystals_p[s][r][p][j]->Draw();

                            cMapCryPTot[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                            sprintf(Name,
                                    "Map level (PR), Side %s, Ring %d, Slot %d, Febex %d, ch. %d",
                                    Side,
                                    r + 1,
                                    fFebexInfo[s][r][p][2],
                                    fFebexInfo[s][r][p][3],
                                    fOrderFebexPreamp[j]);
                            fh2_crystalsETot_p[s][r][p][j]->SetTitle(Name);
                            fh2_crystalsETot_p[s][r][p][j]->Draw();
                        }

                        if (fCalItemsRpc)
                        {
                            if (fFebexInfo[s][r][p][0] != -1)
                            {
                                cMapCryCal[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                                fh1_crystals_cal[s][r][p][j]->SetFillColor(kGreen);
                                sprintf(Name,
                                        "Cal level, Side %s, Ring %d, Slot %d, Febex %d, ch. %d",
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
                                        "Cal level (PR), Side %s, Ring %d, Slot %d, Febex %d, ch. %d",
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
                            sprintf(Name, "Map level, Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                            fh1_crystals[s][r][p][j]->SetTitle(Name);
                            fh1_crystals[s][r][p][j]->Draw();

                            cMapCryTot[s][r][p]->cd(j + 1);
                            sprintf(Name, "Map level, Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                            fh2_crystalsETot[s][r][p][j]->SetTitle(Name);
                            fh2_crystalsETot[s][r][p][j]->Draw();
                        }
                        if (fFebexInfo[s][r][p][2] != -1)
                        {
                            cMapCryP[s][r][p]->cd(j + 1);
                            fh1_crystals_p[s][r][p][j]->SetFillColor(45);
                            sprintf(
                                Name, "Map level (PR), Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                            fh1_crystals_p[s][r][p][j]->SetTitle(Name);
                            fh1_crystals_p[s][r][p][j]->Draw();

                            cMapCryPTot[s][r][p]->cd(j + 1);
                            sprintf(
                                Name, "Map level (PR), Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                            fh2_crystalsETot_p[s][r][p][j]->SetTitle(Name);
                            fh2_crystalsETot_p[s][r][p][j]->Draw();
                        }

                        if (fCalItemsRpc)
                        {
                            if (fFebexInfo[s][r][p][0] != -1)
                            {
                                cMapCryCal[s][r][p]->cd(j + 1);
                                fh1_crystals_cal[s][r][p][j]->SetFillColor(45);
                                sprintf(
                                    Name, "Cal level, Side %s, Ring %d, Preamp %d, ch. %d", Side, r + 1, p + 1, j + 1);
                                fh1_crystals_cal[s][r][p][j]->SetTitle(Name);
                                fh1_crystals_cal[s][r][p][j]->Draw();
                            }

                            if (fFebexInfo[s][r][p][2] != -1)
                            {
                                cMapCryPCal[s][r][p]->cd(j + 1);
                                fh1_crystals_p_cal[s][r][p][j]->SetFillColor(45);
                                sprintf(Name,
                                        "Cal level (PR), Side %s, Ring %d, Preamp %d, ch. %d",
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

void R3BRpcOnlineSpectra::Exec(Option_t* option)
{
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(FATAL) << "R3BRpcOnlineSpectra::Exec FairRootManager not found";

    // int64_t wrdif[fNumSides];
    // Int_t wrdifinUse = 0;
    int64_t wr[2];
    int64_t wrm = 0.0;
    for (int i = 0; i < 2; i++)
        wr[i] = 0;
    // WR data
    if (fWRItemsRpc && fWRItemsRpc->GetEntriesFast() > 0)
    {
        // Rpc
        Int_t nHits = fWRItemsRpc->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BWRData* hit = (R3BWRData*)fWRItemsRpc->At(ihit);
            if (!hit)
                continue;
            wr[ihit] = hit->GetTimeStamp();
        }
        if (nHits == 2)
            fh1_Rpc_wr->Fill(wr[1] - wr[0]);

        // Master Ref. (exp. 2020)
        if (fWRItemsMaster && fWRItemsMaster->GetEntriesFast() > 0)
        {
            nHits = fWRItemsMaster->GetEntriesFast();
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BWRData* hit = (R3BWRData*)fWRItemsMaster->At(ihit);
                if (!hit)
                    continue;
                wrm = hit->GetTimeStamp();
            }
            // fh1_wrs[0]->Fill(wrm - wr[0]); // messel
            // fh1_wrs[1]->Fill(wrm - wr[1]); // wixhausen
            // wrdif[0] = wrm - wr[0];
            // wrdif[1] = wrm - wr[1];
            // wrdifinUse = 1;
        }
    }

    // Mapped data
    if (fMappedItemsRpc && fMappedItemsRpc->GetEntriesFast() > 0)
    {
        Int_t nHits = fMappedItemsRpc->GetEntriesFast();
        Int_t Crymult = 0;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRpcMappedData* hit = (R3BRpcMappedData*)fMappedItemsRpc->At(ihit);
            if (!hit)
                continue;

            Int_t cryId = hit->GetCrystalId();

            // compensate slave exploder delays:
            int64_t wrc = hit->GetWrts() + 245 * (fMap_Par->GetPreamp(cryId) > 8);
            if (wrm > 0.)
            {
                bool side = !(fMap_Par->GetHalf(cryId) % 2);
                fh1_wrs[side]->Fill(wrc - wrm);
            }

            if ((fMap_Par->GetInUse(cryId) == 1 && cryId <= fNbRpcCrystals / 2) ||
                (cryId > fNbRpcCrystals / 2 &&
                 fMap_Par->GetInUse(cryId) != fMap_Par->GetInUse(cryId - fNbRpcCrystals / 2)))
                Crymult++;

            fh2_Rpc_cryId_energy->Fill(cryId, hit->GetEnergy());

            if (fMap_Par->GetHalf(cryId) == 2)
                fh2_Preamp_vs_ch_L[fMap_Par->GetRing(cryId) - 1]->Fill(fMap_Par->GetPreamp(cryId),
                                                                       fMap_Par->GetChannel(cryId));
            if (fMap_Par->GetHalf(cryId) == 1)
                fh2_Preamp_vs_ch_R[fMap_Par->GetRing(cryId) - 1]->Fill(fMap_Par->GetPreamp(cryId),
                                                                       fMap_Par->GetChannel(cryId));

            if (fMap_Par->GetInUse(cryId) == 1 && cryId <= fNbRpcCrystals / 2)
            {
                fh1_crystals[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1][fMap_Par->GetPreamp(cryId) - 1]
                            [fMap_Par->GetChannel(cryId) - 1]
                                ->Fill(hit->GetEnergy());
                fh2_crystalsETot[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                    ->Fill(hit->GetEnergy(), hit->GetTot());
            }

            else if (fMap_Par->GetInUse(cryId) == 1 && cryId > fNbRpcCrystals / 2)
            {
                fh1_crystals_p[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                              [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                  ->Fill(hit->GetEnergy());
                fh2_crystalsETot_p[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                  [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                      ->Fill(hit->GetEnergy(), hit->GetTot());
            }
        }
        fh1_Rpc_Mult->Fill(Crymult);
    }

    // Cal data
    if (fCalItemsRpc && fCalItemsRpc->GetEntriesFast() > 0)
    {
        Int_t nHits = fCalItemsRpc->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRpcCrystalCalData* hit = (R3BRpcCrystalCalData*)fCalItemsRpc->At(ihit);
            if (!hit)
                continue;

            Int_t cryId = hit->GetCrystalId();

            fh2_Rpc_cryId_energy_cal->Fill(cryId, hit->GetEnergy());

            fh2_Rpc_NsNf->Fill(hit->GetNf(), hit->GetNs());

            if (fMap_Par->GetInUse(cryId) == 1 && cryId <= fNbRpcCrystals / 2)
                fh1_crystals_cal[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                    ->Fill(hit->GetEnergy());
            else if (fMap_Par->GetInUse(cryId) == 1 && cryId > fNbRpcCrystals / 2)
                fh1_crystals_p_cal[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                  [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                      ->Fill(hit->GetEnergy());
        }
    }

    // Hit data
    if (fHitItemsRpc && fHitItemsRpc->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsRpc->GetEntriesFast();
        fh1_Rpc_MultHit->Fill(nHits);

        Double_t theta = 0., phi = 0.;
        Double_t Rpc_theta[nHits];
        Double_t Rpc_phi[nHits];
        Double_t Rpc_e[nHits];
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRpcHitData* hit = (R3BRpcHitData*)fHitItemsRpc->At(ihit);
            if (!hit)
                continue;
            theta = hit->GetTheta() * TMath::RadToDeg();
            phi = hit->GetPhi() * TMath::RadToDeg();
            Rpc_theta[ihit] = theta;
            Rpc_phi[ihit] = phi;
            Rpc_e[ihit] = hit->GetEnergy();
            fh2_Rpc_theta_phi->Fill(theta, phi);
            fh2_Rpc_theta_energy->Fill(theta + gRandom->Uniform(-1.5, 1.5), hit->GetEnergy());
            fh1_Rpc_total_energy->Fill(hit->GetEnergy());
        }

        TVector3 master[2];
        Double_t maxEL = 0., maxER = 0.;
        for (Int_t i1 = 0; i1 < nHits; i1++)
        {

            if (Rpc_e[i1] > maxER && TMath::Abs(Rpc_phi[i1]) > 150.) // wixhausen
            {
                master[0].SetMagThetaPhi(1., Rpc_theta[i1] * TMath::DegToRad(), Rpc_phi[i1] * TMath::DegToRad());
                maxER = Rpc_e[i1];
            }
            if (Rpc_e[i1] > maxEL && TMath::Abs(Rpc_phi[i1]) < 60.)
            { // messel
                master[1].SetMagThetaPhi(1., Rpc_theta[i1] * TMath::DegToRad(), Rpc_phi[i1] * TMath::DegToRad());
                maxEL = Rpc_e[i1];
            }
        }
        if (maxEL > fMinProtonE && maxER > fMinProtonE)
        {
            fh1_openangle->Fill(master[0].Angle(master[1]) * TMath::RadToDeg());
        }

        // Comparison of hits to get energy, theta and phi correlations between them
        for (Int_t i1 = 0; i1 < nHits; i1++)
        {
            /*if (wrdifinUse == 1)
            {
                if (TMath::Abs(Rpc_phi[i1]) > 90.)
                    fh2_Cal_wr_energy_r->Fill(wrdif[1], Rpc_e[i1]); // wixhausen
                else
                    fh2_Cal_wr_energy_l->Fill(wrdif[0], Rpc_e[i1]); // messel
            }*/
            for (Int_t i2 = i1 + 1; i2 < nHits; i2++)
            {
                if (gRandom->Uniform(0., 1.) < 0.5)
                {
                    fh2_Rpc_coinE->Fill(Rpc_e[i1], Rpc_e[i2]);
                    fh2_Rpc_coinTheta->Fill(Rpc_theta[i1], Rpc_theta[i2]);
                    fh2_Rpc_coinPhi->Fill(Rpc_phi[i1], Rpc_phi[i2]);
                }
                else
                {
                    fh2_Rpc_coinE->Fill(Rpc_e[i2], Rpc_e[i1]);
                    fh2_Rpc_coinTheta->Fill(Rpc_theta[i2], Rpc_theta[i1]);
                    fh2_Rpc_coinPhi->Fill(Rpc_phi[i2], Rpc_phi[i1]);
                }
            }
        }
    }

    fNEvents += 1;
}

void R3BRpcOnlineSpectra::FinishEvent()
{
    if (fMappedItemsRpc)
    {
        fMappedItemsRpc->Clear();
    }
    if (fCalItemsRpc)
    {
        fCalItemsRpc->Clear();
    }
    if (fHitItemsRpc)
    {
        fHitItemsRpc->Clear();
    }
    if (fWRItemsRpc)
    {
        fWRItemsRpc->Clear();
    }
    if (fWRItemsMaster)
    {
        fWRItemsMaster->Clear();
    }
}

void R3BRpcOnlineSpectra::FinishTask()
{
    // Write canvas for Rpc WR data
    if (fWRItemsRpc)
    {
        cRpc_wr->Write();
    }

    // Write canvas for Master-Rpc WR data
    if (fWRItemsMaster && fWRItemsRpc)
    {
        cWrs->Write();
        if (fHitItemsRpc)
            cRpc_wr_energy->Write();
    }

    // Write canvas for Mapped data
    if (fMappedItemsRpc)
    {
        cRpcMult->Write();
        cRpc_sync->Write();
        cRpc_cry_energy->Write();
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
    if (fCalItemsRpc)
    {
        cRpc_cry_energy_cal->Write();
        cRpc_NsNf->Write();
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
    if (fHitItemsRpc)
    {
        cRpcCoinE->Write();
        cRpcCoinTheta->Write();
        cRpcCoinPhi->Write();
        cRpc_angles->Write();
        cRpc_theta_energy->Write();
        cRpc_hitenergy->Write();
        cRpc_opening->Write();
    }
}

ClassImp(R3BRpcOnlineSpectra)
