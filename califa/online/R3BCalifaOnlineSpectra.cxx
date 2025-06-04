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

#include "R3BCalifaOnlineSpectra.h"
#include "R3BCalifaClusterData.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappedData.h"
#include "R3BCalifaMappingPar.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BWRData.h"

#include <FairRootManager.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TH1I.h>
#include <TH2F.h>
#include <THttpServer.h>
#include <TLatex.h>
#include <TMath.h>
#include <TRandom.h>
#include <TVector3.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

R3BCalifaOnlineSpectra::R3BCalifaOnlineSpectra()
    : R3BCalifaOnlineSpectra("CALIFAOnlineSpectra", 1)
{
}

R3BCalifaOnlineSpectra::R3BCalifaOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

void R3BCalifaOnlineSpectra::SetParContainers()
{
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fMap_Par = dynamic_cast<R3BCalifaMappingPar*>(rtdb->getContainer("califaMappingPar"));
    if (!fMap_Par)
    {
        R3BLOG(error, "Couldn't get handle on califaMappingPar container");
    }
    else
    {
        R3BLOG(info, "CalifaMappingPar container open");
    }
}

void R3BCalifaOnlineSpectra::SetParameter()
{
    R3BLOG_IF(error, !fMap_Par, "CalifaMappingPar container not found");

    //--- Parameter Container ---
    fNbCalifaCrystals = fMap_Par->GetNumCrystals(); // Number of crystals
    assert(std::clamp(fNbCalifaCrystals, 0, MaxNbCrystals) == fNbCalifaCrystals && "Number of crystals out of range");

    R3BLOG(info, "Number of crystals (gamma+proton): " << fNbCalifaCrystals);
    // fMap_Par->printParams();

    fFebexInfo.resize(extents[Nb_Sides][Nb_Rings][Nb_Preamps][Nb_SlotandModule]);
    std::fill(fFebexInfo.data(), fFebexInfo.data() + fFebexInfo.num_elements(), -1);

    for (Int_t c = 1; c <= fNbCalifaCrystals; c++)
    {
        if (c <= fNbCalifaCrystals / 2)
        {
            if (fMap_Par->GetInUse(c) == 1 && fMap_Par->GetHalf(c) > 0)
            { // only for installed crystals (see issue 681)
                fFebexInfo[fMap_Par->GetHalf(c) - 1][fMap_Par->GetRing(c) - 1][fMap_Par->GetPreamp(c) - 1][0] =
                    fMap_Par->GetFebexSlot(c);
                fFebexInfo[fMap_Par->GetHalf(c) - 1][fMap_Par->GetRing(c) - 1][fMap_Par->GetPreamp(c) - 1][1] =
                    fMap_Par->GetFebexMod(c);
            }
        }
        else
        {
            if (fMap_Par->GetInUse(c) == 1 && fMap_Par->GetHalf(c) > 0)
            {
                fFebexInfo[fMap_Par->GetHalf(c) - 1][fMap_Par->GetRing(c) - 1][fMap_Par->GetPreamp(c) - 1][2] =
                    fMap_Par->GetFebexSlot(c);
                fFebexInfo[fMap_Par->GetHalf(c) - 1][fMap_Par->GetRing(c) - 1][fMap_Par->GetPreamp(c) - 1][3] =
                    fMap_Par->GetFebexMod(c);
            }
        }
    }
}

InitStatus R3BCalifaOnlineSpectra::Init()
{
    R3BLOG(info, "");

    FairRootManager* mgr = FairRootManager::Instance();

    R3BLOG_IF(fatal, mgr == nullptr, "FairRootManager not found");

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to Mapped data
    fMappedItemsCalifa = dynamic_cast<TClonesArray*>(mgr->GetObject("CalifaMappedData"));
    R3BLOG_IF(fatal, fMappedItemsCalifa == nullptr, "CalifaMappedData not found");

    // get access to trigger Mapped data
    fTrigMappedItemsCalifa = dynamic_cast<TClonesArray*>(mgr->GetObject("CalifaMappedtrigData"));
    R3BLOG_IF(warn, !fTrigMappedItemsCalifa, "CalifaMappedtrigData not found");

    // get access to Cal data
    fCalItemsCalifa = dynamic_cast<TClonesArray*>(mgr->GetObject("CalifaCrystalCalData"));
    R3BLOG_IF(warn, !fCalItemsCalifa, "CalifaCrystalCalData not found");

    // get access to Hit data
    fHitItemsCalifa = dynamic_cast<TClonesArray*>(mgr->GetObject("CalifaClusterData"));
    R3BLOG_IF(warn, !fHitItemsCalifa, "CalifaClusterData not found");

    // get access to WR-Califa data
    fWRItemsCalifa = dynamic_cast<TClonesArray*>(mgr->GetObject("WRCalifaData"));
    R3BLOG_IF(warn, !fWRItemsCalifa, "WRCalifaData not found");

    // get access to WR-Master data
    fWRItemsMaster = dynamic_cast<TClonesArray*>(mgr->GetObject("WRMasterData"));
    R3BLOG_IF(warn, !fWRItemsMaster, "WRMasterData not found");

    SetParameter();

    // reading the file
    ifstream* FileHistos = new ifstream(fCalifaFile);
    if (FileHistos->is_open() == false)
    {
        R3BLOG(warn, "No Histogram definition file");
    }

    Double_t arry_bins[fNumSides][fNumRings][fNumPreamps][fNumCrystalPreamp];
    Double_t arry_maxE[fNumSides][fNumRings][fNumPreamps][fNumCrystalPreamp];
    Double_t arry_minE[fNumSides][fNumRings][fNumPreamps][fNumCrystalPreamp];

    if (FileHistos->is_open())
    {
        for (int s = 0; s < fNumSides; s++)
            for (int r = 0; r < fNumRings; r++)
                for (int p = 0; p < fNumPreamps; p++)
                    for (int ch = 0; ch < fNumCrystalPreamp; ch++)
                        *FileHistos >> arry_bins[s][r][p][ch] >> arry_minE[s][r][p][ch] >> arry_maxE[s][r][p][ch];
    }
    else
    {
        for (int s = 0; s < fNumSides; s++)
            for (int r = 0; r < fNumRings; r++)
                for (int p = 0; p < fNumPreamps; p++)
                    for (int ch = 0; ch < fNumCrystalPreamp; ch++)
                    {
                        arry_bins[s][r][p][ch] = fMapHistos_bins;
                        arry_minE[s][r][p][ch] = 0;
                        arry_maxE[s][r][p][ch] = fMapHistos_max;
                    }
    }

    // Create histograms for detectors
    std::string Name1;
    std::string Name2;
    std::string Name3;
    std::string Xaxis;
    double bins = fMapHistos_bins;
    double maxE = fMapHistos_max;
    double minE = 0.;

    // CANVAS Crystal_ID vs energy
    cCalifa_cry_energy = new TCanvas("Califa_Map_energy_vs_cryID", "Califa_Map energy vs cryID", 10, 10, 500, 500);
    fh2_Califa_cryId_energy = R3B::root_owned<TH2F>("fh2_Califa_Map_cryID_energy",
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
    fh2_Califa_cryId_energy->SetStats(0);
    gPad->SetLogz();
    fh2_Califa_cryId_energy->Draw("COLZ");

    // CANVAS Crystal_ID vs energy
    cCalifa_cry_energy_cal = new TCanvas("Califa_Cal_energy_vs_cryID", "Califa_Cal energy vs cryID", 10, 10, 500, 500);
    fh2_Califa_cryId_energy_cal = R3B::root_owned<TH2F>("fh2_Califa_Cal_cryID_energy",
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
    fh2_Califa_cryId_energy_cal->SetStats(0);
    gPad->SetLogz();
    fh2_Califa_cryId_energy_cal->Draw("COLZ");

    cCalifa_NsNf = new TCanvas("Califa_Cal_NsNf", "Califa_Cal Ns vs Nf", 10, 10, 500, 500);
    cCalifa_NsNf->Divide(1, 3);

    std::vector<std::string> region = { "Barrel", "iPhos", "CEPA" };
    for (Int_t i = 0; i < region.size(); i++)
    {
        std::stringstream ss1;
        ss1 << "PID_" << region[i];
        std::stringstream ss2;
        ss2 << "Califa " << region[i] << " PID: Ns and Nf energies";
        fh2_Califa_NsNf.push_back(
            R3B::root_owned<TH2F>(ss1.str().c_str(), ss2.str().c_str(), 500, 0., 500., 600, 0., 1.));
        fh2_Califa_NsNf[i]->GetXaxis()->SetTitle("Ns+Nf Energies [MeV]");
        fh2_Califa_NsNf[i]->GetYaxis()->SetTitle("Nf/(Nf+Ns)");
        fh2_Califa_NsNf[i]->GetYaxis()->SetTitleOffset(1.4);
        fh2_Califa_NsNf[i]->GetXaxis()->CenterTitle(true);
        fh2_Califa_NsNf[i]->GetYaxis()->CenterTitle(true);
        cCalifa_NsNf->cd(i + 1);
        gPad->SetLogz();
        fh2_Califa_NsNf[i]->Draw("COLZ");
    }

    // Map data
    for (Int_t i = 0; i < fNumRings; i++)
    {
        // Left side
        std::stringstream ss1;
        ss1 << "Ring_" << i + 1 << "_Left";
        cMap_RingL[i] = new TCanvas(ss1.str().c_str(), ss1.str().c_str(), 10, 10, 800, 700);
        std::stringstream ss2;
        ss2 << "fh2_Ring_" << i + 1 << "_Left";
        std::stringstream ss3;
        ss3 << "Ring " << i + 1 << " left: Preamp number vs channel number";

        fh2_Preamp_vs_ch_L.push_back(R3B::root_owned<TH2F>(ss2.str().c_str(),
                                                           ss3.str().c_str(),
                                                           fNumPreamps,
                                                           0.5,
                                                           fNumPreamps + 0.5,
                                                           fNumCrystalPreamp,
                                                           0.5,
                                                           fNumCrystalPreamp + 0.5));
        fh2_Preamp_vs_ch_L[i]->GetXaxis()->SetTitle("Preamp number [1-16]");
        std::stringstream ss4;
        ss4 << "Channel number [1-" << fNumCrystalPreamp << "]";
        fh2_Preamp_vs_ch_L[i]->GetYaxis()->SetTitle(ss4.str().c_str());
        fh2_Preamp_vs_ch_L[i]->GetYaxis()->SetTitleOffset(1.2);
        fh2_Preamp_vs_ch_L[i]->GetXaxis()->CenterTitle(true);
        fh2_Preamp_vs_ch_L[i]->GetYaxis()->CenterTitle(true);
        fh2_Preamp_vs_ch_L[i]->SetStats(0);
        fh2_Preamp_vs_ch_L[i]->Draw("colz");

        // Right side
        std::stringstream ss5;
        ss5 << "Ring_" << i + 1 << "_Right";
        cMap_RingR[i] = new TCanvas(ss5.str().c_str(), ss5.str().c_str(), 10, 10, 800, 700);
        std::stringstream ss6;
        ss6 << "fh2_Ring_" << i + 1 << "_Right";
        std::stringstream ss7;
        ss7 << "Ring " << i + 1 << " right: Preamp number vs channel number";

        fh2_Preamp_vs_ch_R.push_back(R3B::root_owned<TH2F>(ss6.str().c_str(),
                                                           ss7.str().c_str(),
                                                           fNumPreamps,
                                                           0.5,
                                                           fNumPreamps + 0.5,
                                                           fNumCrystalPreamp,
                                                           0.5,
                                                           fNumCrystalPreamp + 0.5));
        fh2_Preamp_vs_ch_R[i]->GetXaxis()->SetTitle("Preamp number [1-16]");
        fh2_Preamp_vs_ch_R[i]->GetYaxis()->SetTitle(ss4.str().c_str());
        fh2_Preamp_vs_ch_R[i]->GetYaxis()->SetTitleOffset(1.2);
        fh2_Preamp_vs_ch_R[i]->GetXaxis()->CenterTitle(true);
        fh2_Preamp_vs_ch_R[i]->GetYaxis()->CenterTitle(true);
        fh2_Preamp_vs_ch_R[i]->SetStats(0);
        fh2_Preamp_vs_ch_R[i]->Draw("colz");
    }

    auto cMap_ECor = new TCanvas("Trigger_ECor", "Trigger_ECor", 10, 10, 800, 700);
    cMap_ECor->Divide(4, 2);

    fh2_Califa_EtrigCor.push_back(R3B::root_owned<TH2F>(
        "fh2Trigger_ECor_m0m1", "Correlation of trigger energies (mes0 vs mes1)", 2000, 0., 4000., 2000, 0., 4000.));
    fh2_Califa_EtrigCor[0]->GetXaxis()->SetTitle("Energy mes0");
    fh2_Califa_EtrigCor[0]->GetYaxis()->SetTitle("Energy mes1");
    fh2_Califa_EtrigCor[0]->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_EtrigCor[0]->GetXaxis()->CenterTitle(true);
    fh2_Califa_EtrigCor[0]->GetYaxis()->CenterTitle(true);
    cMap_ECor->cd(1);
    fh2_Califa_EtrigCor[0]->Draw("colz");

    fh2_Califa_EtrigCor.push_back(R3B::root_owned<TH2F>(
        "fh2Trigger_ECor_m0w0", "Correlation of trigger energies (mes0 vs wix0)", 2000, 0., 4000., 2000, 0., 4000.));
    fh2_Califa_EtrigCor[1]->GetXaxis()->SetTitle("Energy mes0");
    fh2_Califa_EtrigCor[1]->GetYaxis()->SetTitle("Energy wix0");
    fh2_Califa_EtrigCor[1]->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_EtrigCor[1]->GetXaxis()->CenterTitle(true);
    fh2_Califa_EtrigCor[1]->GetYaxis()->CenterTitle(true);
    cMap_ECor->cd(2);
    fh2_Califa_EtrigCor[1]->Draw("colz");

    fh2_Califa_EtrigCor.push_back(R3B::root_owned<TH2F>(
        "fh2Trigger_ECor_m0w1", "Correlation of trigger energies (mes0 vs wix1)", 2000, 0., 4000., 2000, 0., 4000.));
    fh2_Califa_EtrigCor[2]->GetXaxis()->SetTitle("Energy mes0");
    fh2_Califa_EtrigCor[2]->GetYaxis()->SetTitle("Energy wix1");
    fh2_Califa_EtrigCor[2]->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_EtrigCor[2]->GetXaxis()->CenterTitle(true);
    fh2_Califa_EtrigCor[2]->GetYaxis()->CenterTitle(true);
    cMap_ECor->cd(3);
    fh2_Califa_EtrigCor[2]->Draw("colz");

    fh2_Califa_EtrigCor.push_back(R3B::root_owned<TH2F>(
        "fh2Trigger_ECor_m1w1", "Correlation of trigger energies (mes1 vs wix1)", 2000, 0., 4000., 2000, 0., 4000.));
    fh2_Califa_EtrigCor[3]->GetXaxis()->SetTitle("Energy mes1");
    fh2_Califa_EtrigCor[3]->GetYaxis()->SetTitle("Energy wix1");
    fh2_Califa_EtrigCor[3]->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_EtrigCor[3]->GetXaxis()->CenterTitle(true);
    fh2_Califa_EtrigCor[3]->GetYaxis()->CenterTitle(true);
    cMap_ECor->cd(4);
    fh2_Califa_EtrigCor[3]->Draw("colz");

    fh1_Califa_Etrig.push_back(
        R3B::root_owned<TH1F>("fh1Trigger_Emessel0", "Messel0 trigger energies (mes0)", 2000, 0., 4000.));
    fh1_Califa_Etrig[0]->GetXaxis()->SetTitle("Energy mes0");
    fh1_Califa_Etrig[0]->GetYaxis()->SetTitle("Counts");
    fh1_Califa_Etrig[0]->GetYaxis()->SetTitleOffset(1.2);
    fh1_Califa_Etrig[0]->GetXaxis()->CenterTitle(true);
    fh1_Califa_Etrig[0]->GetYaxis()->CenterTitle(true);
    fh1_Califa_Etrig[0]->SetFillColor(31);
    cMap_ECor->cd(5);
    fh1_Califa_Etrig[0]->Draw();

    fh1_Califa_Etrig.push_back(
        R3B::root_owned<TH1F>("fh1Trigger_Emessel1", "Messel1 trigger energies (mes1)", 2000, 0., 4000.));
    fh1_Califa_Etrig[1]->GetXaxis()->SetTitle("Energy mes1");
    fh1_Califa_Etrig[1]->GetYaxis()->SetTitle("Counts");
    fh1_Califa_Etrig[1]->GetYaxis()->SetTitleOffset(1.2);
    fh1_Califa_Etrig[1]->GetXaxis()->CenterTitle(true);
    fh1_Califa_Etrig[1]->GetYaxis()->CenterTitle(true);
    fh1_Califa_Etrig[1]->SetFillColor(31);
    cMap_ECor->cd(6);
    fh1_Califa_Etrig[1]->Draw();

    fh1_Califa_Etrig.push_back(
        R3B::root_owned<TH1F>("fh1Trigger_Ewixhausen0", "Wixhausen trigger energies (wix0)", 2000, 0., 4000.));
    fh1_Califa_Etrig[2]->GetXaxis()->SetTitle("Energy wix0");
    fh1_Califa_Etrig[2]->GetYaxis()->SetTitle("Counts");
    fh1_Califa_Etrig[2]->GetYaxis()->SetTitleOffset(1.2);
    fh1_Califa_Etrig[2]->GetXaxis()->CenterTitle(true);
    fh1_Califa_Etrig[2]->GetYaxis()->CenterTitle(true);
    fh1_Califa_Etrig[2]->SetFillColor(31);
    cMap_ECor->cd(7);
    fh1_Califa_Etrig[2]->Draw();

    fh1_Califa_Etrig.push_back(
        R3B::root_owned<TH1F>("fh1Trigger_Ewixhausen1", "Wixhausen trigger energies (wix1)", 2000, 0., 4000.));
    fh1_Califa_Etrig[3]->GetXaxis()->SetTitle("Energy wix1");
    fh1_Califa_Etrig[3]->GetYaxis()->SetTitle("Counts");
    fh1_Califa_Etrig[3]->GetYaxis()->SetTitleOffset(1.2);
    fh1_Califa_Etrig[3]->GetXaxis()->CenterTitle(true);
    fh1_Califa_Etrig[3]->GetYaxis()->CenterTitle(true);
    fh1_Califa_Etrig[3]->SetFillColor(31);
    cMap_ECor->cd(8);
    fh1_Califa_Etrig[3]->Draw();

    /*
    fh2_Califa_EtrigCor.push_back(R3B::root_owned<TH2F>(
        "fh2Trigger_ECor_trg1", "Correlation of trigger energies (trigger 1)", 2000, 0., 4000., 2000, 0., 4000.));
    fh2_Califa_EtrigCor[1]->GetXaxis()->SetTitle("Energy messel side");
    fh2_Califa_EtrigCor[1]->GetYaxis()->SetTitle("Energy wixhausen side");
    fh2_Califa_EtrigCor[1]->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_EtrigCor[1]->GetXaxis()->CenterTitle(true);
    fh2_Califa_EtrigCor[1]->GetYaxis()->CenterTitle(true);
    cMap_ECor->cd(4);
    fh2_Califa_EtrigCor[1]->Draw("colz");
    */

    std::vector<std::string> side = { "Right", "Left" };
    for (int s = 0; s < fNumSides; s++) // Side
    {
        for (Int_t r = 0; r < fNumRings; r++)       // Ring
            for (Int_t p = 0; p < fNumPreamps; p++) // Preamp
            {
                if (fFebexInfo[s][r][p][0] != -1)
                {
                    std::stringstream ss1;
                    ss1 << "Ring_" << r + 1 << "_" << side[s] << "_Preamp_" << p + 1;

                    std::stringstream ss2;
                    ss2 << "Ring " << r + 1 << ", " << side[s] << " side, Preamp " << p + 1;

                    cMapCry[s][r][p] = new TCanvas(ss1.str().c_str(), ss2.str().c_str(), 10, 10, 500, 500);
                    cMapCry[s][r][p]->Divide(4, 4);
                    // for TOT correlations
                    std::stringstream ss3;
                    ss3 << "Ring_" << r + 1 << "_" << side[s] << "_Preamp_" << p + 1 << "_tot";

                    cMapCryTot[s][r][p] = new TCanvas(ss3.str().c_str(), ss2.str().c_str(), 10, 10, 500, 500);
                    cMapCryTot[s][r][p]->Divide(4, 4);
                }
                if (fFebexInfo[s][r][p][2] != -1)
                {
                    std::stringstream ss1;
                    ss1 << "Ring_" << r + 1 << "_" << side[s] << "_Preamp_" << p + 1 << "_pr";

                    std::stringstream ss2;
                    ss2 << "Ring " << r + 1 << ", " << side[s] << " side, Preamp " << p + 1 << " for PR";

                    cMapCryP[s][r][p] = new TCanvas(ss1.str().c_str(), ss2.str().c_str(), 10, 10, 500, 500);
                    cMapCryP[s][r][p]->Divide(4, 4);
                    // for TOT correlations
                    std::stringstream ss3;
                    ss3 << "Ring_" << r + 1 << "_" << side[s] << "_Preamp_" << p + 1 << "_tot_pr";

                    std::stringstream ss4;
                    ss4 << "Ring " << r + 1 << ", " << side[s] << " side, Preamp " << p + 1 << " for ToT-PR";

                    cMapCryPTot[s][r][p] = new TCanvas(ss3.str().c_str(), ss4.str().c_str(), 10, 10, 500, 500);
                    cMapCryPTot[s][r][p]->Divide(4, 4);
                }
                for (Int_t j = 0; j < fNumCrystalPreamp; j++)
                { // Channel
                    Xaxis = "Energy [channels]";

                    if (fFebexInfo[s][r][p][0] != -1)
                    {
                        std::stringstream ss1;
                        ss1 << "fh1_Map_Side_" << side[s] << "_Ring_" << r + 1 << "_Preamp_" << p + 1 << "_Ch_" << j + 1
                            << "_energy";

                        std::stringstream ss2;
                        ss2 << "Map level, Side " << side[s] << ", Ring " << r + 1 << ", Preamp " << p + 1 << ", ch. "
                            << j + 1;

                        fh1_crystals[s][r][p][j] = R3B::root_owned<TH1F>(
                            ss1.str().c_str(), ss2.str().c_str(), fBinsChannelFebex, 0, fMaxBinChannelFebex);
                        fh1_crystals[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh1_crystals[s][r][p][j]->GetXaxis()->SetTitle(Xaxis.c_str());
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
                        std::stringstream ss3;
                        ss3 << "fh2_Map_Side_" << side[s] << "_Ring_" << r + 1 << "_Preamp_" << p + 1 << "_Ch_" << j + 1
                            << "_evstot";

                        fh2_crystalsETot[s][r][p][j] = R3B::root_owned<TH2F>(ss3.str().c_str(),
                                                                             ss2.str().c_str(),
                                                                             fBinsChannelFebex / 20,
                                                                             0,
                                                                             30000,
                                                                             fBinsChannelFebex / 20,
                                                                             0,
                                                                             fMaxBinChannelFebex * 2);
                        fh2_crystalsETot[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->SetTitle(Xaxis.c_str());
                        fh2_crystalsETot[s][r][p][j]->GetYaxis()->SetTitle("Tot");
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->SetLabelSize(0.06);
                        fh2_crystalsETot[s][r][p][j]->GetYaxis()->SetLabelSize(0.06);
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->SetTitleSize(0.05);
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->CenterTitle(true);
                        fh2_crystalsETot[s][r][p][j]->GetYaxis()->CenterTitle(true);
                        fh2_crystalsETot[s][r][p][j]->GetXaxis()->SetTitleOffset(1.);
                        cMapCryTot[s][r][p]->cd(j + 1);
                        fh2_crystalsETot[s][r][p][j]->Draw("col");
                    }
                    if (fFebexInfo[s][r][p][2] != -1)
                    {
                        // histograms for proton range
                        std::stringstream ss1;
                        ss1 << "fh1_Map_Side_" << side[s] << "_Ring_" << r + 1 << "_Preamp_" << p + 1 << "_Ch_" << j + 1
                            << "_energy_pr";

                        std::stringstream ss2;
                        ss2 << "Map level (PR), Side " << side[s] << ", Ring " << r + 1 << ", Preamp " << p + 1
                            << ", ch. " << j + 1;

                        fh1_crystals_p[s][r][p][j] = R3B::root_owned<TH1F>(
                            ss1.str().c_str(), ss2.str().c_str(), fBinsChannelFebex, 0, fMaxBinChannelFebex);
                        fh1_crystals_p[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh1_crystals_p[s][r][p][j]->GetXaxis()->SetTitle(Xaxis.c_str());
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
                        std::stringstream ss3;
                        ss3 << "fh2_Map_Side_" << side[s] << "_Ring_" << r + 1 << "_Preamp_" << p + 1 << "_Ch_" << j + 1
                            << "_pr_evstot";
                        fh2_crystalsETot_p[s][r][p][j] = R3B::root_owned<TH2F>(ss3.str().c_str(),
                                                                               ss2.str().c_str(),
                                                                               fBinsChannelFebex / 20,
                                                                               0,
                                                                               30000,
                                                                               fBinsChannelFebex / 20,
                                                                               0,
                                                                               fMaxBinChannelFebex * 2);
                        fh2_crystalsETot_p[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->SetTitle(Xaxis.c_str());
                        fh2_crystalsETot_p[s][r][p][j]->GetYaxis()->SetTitle("Tot");
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->SetLabelSize(0.06);
                        fh2_crystalsETot_p[s][r][p][j]->GetYaxis()->SetLabelSize(0.06);
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->SetTitleSize(0.05);
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->CenterTitle(true);
                        fh2_crystalsETot_p[s][r][p][j]->GetYaxis()->CenterTitle(true);
                        fh2_crystalsETot_p[s][r][p][j]->GetXaxis()->SetTitleOffset(1.);
                        cMapCryPTot[s][r][p]->cd(j + 1);
                        fh2_crystalsETot_p[s][r][p][j]->Draw("col");
                    }
                }
            }
    }
    TCanvas* cFebex_overview = new TCanvas("Febex overview", "Febex overview", 700, 500);
    fh2_febex_side_febex_pc_sfp =
        R3B::root_owned<TH2F>("Febex overview", "Entries febex overview", 100, 0, 100, 36, 0, 36);
    fh2_febex_side_febex_pc_sfp->GetXaxis()->SetTitle("febex module");
    fh2_febex_side_febex_pc_sfp->GetYaxis()->SetTitle("febex channel (top:mes, bottom:wix)");
    fh2_febex_side_febex_pc_sfp->GetYaxis()->CenterTitle(true);
    fh2_febex_side_febex_pc_sfp->Draw("colz");
    TLatex latex;
    latex.SetTextSize(0.05);
    latex.SetTextAlign(13);
    latex.DrawLatex(34, 19, "iphos");
    latex.DrawLatex(4, 19, "barrel");
    latex.DrawLatex(64, 19, "barrel");
    latex.DrawLatex(80, 19, "cepa");
    latex.DrawLatex(89, 20, "back");
    latex.DrawLatex(88, 18, "barrel");

    TCanvas* cWr_diff_febex = new TCanvas("WR diff febex", "WR diff febex", 700, 500);
    fh2_wrts_diff_febex_pc_sfp = R3B::root_owned<TH2F>(
        "WRTS difference (pc, sfp, febex)", "WR TS difference (pc, sfp, febex)", 200, 0, 200, 1000, -4000, 4000);
    fh2_wrts_diff_febex_pc_sfp->GetXaxis()->SetTitle("febex_side_pc_sfp_mod_t");
    fh2_wrts_diff_febex_pc_sfp->GetYaxis()->SetTitle("wrts_diff");

    fh2_wrts_diff_febex_pc_sfp->Draw("colz");
    // Cal data
    for (Int_t s = 0; s < fNumSides; s++) // Side
    {
        for (Int_t r = 0; r < fNumRings; r++) // Ring

            for (Int_t p = 0; p < fNumPreamps; p++) // Preamp
            {
                if (fFebexInfo[s][r][p][0] != -1)
                {
                    std::stringstream ss1;
                    ss1 << "Ring_" << r + 1 << "_" << side[s] << "_Preamp_" << p + 1 << "_cal";

                    std::stringstream ss2;
                    ss2 << "Ring " << r + 1 << ", " << side[s] << " side, Preamp " << p + 1 << " for Cal";

                    cMapCryCal[s][r][p] = new TCanvas(ss1.str().c_str(), ss2.str().c_str(), 10, 10, 500, 500);
                    cMapCryCal[s][r][p]->Divide(4, 4);
                }
                if (fFebexInfo[s][r][p][2] != -1)
                {
                    std::stringstream ss1;
                    ss1 << "Ring_" << r + 1 << "_" << side[s] << "_Preamp_" << p + 1 << "_calpr";

                    std::stringstream ss2;
                    ss2 << "Ring " << r + 1 << ", " << side[s] << " side, Preamp " << p + 1 << "for PR Cal";

                    cMapCryPCal[s][r][p] = new TCanvas(ss1.str().c_str(), ss2.str().c_str(), 10, 10, 500, 500);
                    cMapCryPCal[s][r][p]->Divide(4, 4);
                }
                for (Int_t j = 0; j < fNumCrystalPreamp; j++)
                { // Channel
                    if (fFebexInfo[s][r][p][0] != -1)
                    {
                        // histograms for gamma range
                        Xaxis = "Energy [keV]";
                        std::stringstream ss1;
                        ss1 << "fh1_Cal_Side_" << side[s] << "_Ring_" << r + 1 << "_Preamp_" << p + 1 << "_Ch_" << j + 1
                            << "_energy";
                        std::stringstream ss2;
                        ss2 << "Cal level, Side " << side[s] << ", Ring " << r + 1 << ", Preamp " << p + 1 << ", ch. "
                            << j + 1;

                        fh1_crystals_cal[s][r][p][j] = R3B::root_owned<TH1F>(
                            ss1.str().c_str(), ss2.str().c_str(), fMaxEnergyGR / 10., 0, fMaxEnergyGR);
                        fh1_crystals_cal[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh1_crystals_cal[s][r][p][j]->GetXaxis()->SetTitle(Xaxis.c_str());
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
                    {
                        // histograms for proton range
                        Xaxis = "Energy [MeV]";
                        std::stringstream ss1;
                        ss1 << "fh1_Cal_Side_" << side[s] << "_Ring_" << r + 1 << "_Preamp_" << p + 1 << "_Ch_" << j + 1
                            << "_energy_pr";
                        std::stringstream ss2;
                        ss2 << "Cal level (PR), Side" << side[s] << ", Ring " << r + 1 << ", Preamp " << p + 1
                            << ", ch. " << j + 1;

                        fh1_crystals_p_cal[s][r][p][j] =
                            R3B::root_owned<TH1F>(ss1.str().c_str(), ss2.str().c_str(), fMaxEnergyPR, 0., fMaxEnergyPR);
                        fh1_crystals_p_cal[s][r][p][j]->SetTitleSize(1.6, "t");
                        fh1_crystals_p_cal[s][r][p][j]->GetXaxis()->SetTitle(Xaxis.c_str());
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
    fh1_Califa_Mult =
        R3B::root_owned<TH1F>("fh1_Califa_Mult", "Califa multiplicity (crystal:blue, cluster:red)", 341, -0.5, 340.5);
    fh1_Califa_MultHit = R3B::root_owned<TH1F>("fh1_Califa_MultHit", "Califa multiplicity", 341, -0.5, 340.5);
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
    cCalifaCoinE->Divide(2, 1);
    cCalifaCoinE->cd(1);

    fh2_Califa_coinE = R3B::root_owned<TH2F>("fh2_Califa_energy_correlations",
                                             "Califa energy correlations",
                                             (maxE - minE) / 1000.,
                                             minE / 1000.,
                                             maxE / 1000.,
                                             (maxE - minE) / 1000.,
                                             minE / 1000.,
                                             maxE / 1000.);
    fh2_Califa_coinE->GetXaxis()->SetTitle("Energy (MeV)");
    fh2_Califa_coinE->GetYaxis()->SetTitle("Energy (MeV)");
    fh2_Califa_coinE->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_coinE->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinE->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinE->Draw("COLZ");

    cCalifaCoinE->cd(2);
    fh2_Califa_coinE_p2p = R3B::root_owned<TH2F>("fh2_Califa_energy_correlations_p2p",
                                                 "Califa energy correlations for p2p",
                                                 (maxE - minE) / 1000.,
                                                 minE / 1000.,
                                                 maxE / 1000.,
                                                 (maxE - minE) / 1000.,
                                                 minE / 1000.,
                                                 maxE / 1000.);
    fh2_Califa_coinE_p2p->GetXaxis()->SetTitle("Energy (MeV)");
    fh2_Califa_coinE_p2p->GetYaxis()->SetTitle("Energy (MeV)");
    fh2_Califa_coinE_p2p->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_coinE_p2p->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinE_p2p->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinE_p2p->Draw("COLZ");

    // CANVAS Theta correlations between hits
    cCalifaCoinTheta = new TCanvas("Califa_theta_correlation_hits", "Theta correlations, hit level", 10, 10, 500, 500);
    cCalifaCoinTheta->Divide(2, 1);
    cCalifaCoinTheta->cd(1);
    fh2_Califa_coinTheta =
        R3B::root_owned<TH2F>("fh2_Califa_theta_correlations", "Califa theta correlations", 500, 0, 100, 500, 0, 100);
    fh2_Califa_coinTheta->GetXaxis()->SetTitle("Theta [deg]");
    fh2_Califa_coinTheta->GetYaxis()->SetTitle("Theta [deg]");
    fh2_Califa_coinTheta->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_coinTheta->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinTheta->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinTheta->Draw("COLZ");

    cCalifaCoinTheta->cd(2);
    fh2_Califa_coinTheta_cutOPA = R3B::root_owned<TH2F>(
        "fh2_Califa_theta_correlations_cutOPA", "Califa theta correlations OPA cut", 500, 0, 100, 500, 0, 100);
    fh2_Califa_coinTheta_cutOPA->GetXaxis()->SetTitle("Theta [deg]");
    fh2_Califa_coinTheta_cutOPA->GetYaxis()->SetTitle("Theta [deg]");
    fh2_Califa_coinTheta_cutOPA->SetTitleOffset(1.2);
    fh2_Califa_coinTheta_cutOPA->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinTheta_cutOPA->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinTheta_cutOPA->Draw("colz");

    // CANVAS Phi correlations between hits
    cCalifaCoinPhi = new TCanvas("Califa_phi_correlation_hits", "Phi correlations, hit level", 10, 10, 500, 500);

    fh2_Califa_coinPhi =
        R3B::root_owned<TH2F>("fh2_Califa_phi_correlations", "Califa phi correlations", 600, -190, 190, 600, -190, 190);
    fh2_Califa_coinPhi->GetXaxis()->SetTitle("Phi [deg]");
    fh2_Califa_coinPhi->GetYaxis()->SetTitle("Phi [deg]");
    fh2_Califa_coinPhi->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_coinPhi->GetXaxis()->CenterTitle(true);
    fh2_Califa_coinPhi->GetYaxis()->CenterTitle(true);
    fh2_Califa_coinPhi->Draw("COLZ");

    // CANVAS Theta vs Phi
    cCalifa_angles = new TCanvas("Califa_Theta_vs_Phi", "Theta vs Phi", 10, 10, 500, 500);
    fh2_Califa_theta_phi =
        R3B::root_owned<TH2F>("fh2_Califa_theta_vs_phi", "Califa theta vs phi", 90, 0, 90, 380, -190, 190);
    fh2_Califa_theta_phi->GetXaxis()->SetTitle("Theta [deg]");
    fh2_Califa_theta_phi->GetYaxis()->SetTitle("Phi [deg]");
    fh2_Califa_theta_phi->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_theta_phi->GetXaxis()->CenterTitle(true);
    fh2_Califa_theta_phi->GetYaxis()->CenterTitle(true);
    fh2_Califa_theta_phi->SetStats(0);
    fh2_Califa_theta_phi->Draw("COLZ");
    gPad->SetLogz();

    // CANVAS Theta vs energy
    Name1 = "Calorimeter_energy_vs_theta";
    Name2 = "fh2_Califa_total_energy_vs_theta_pr";
    Name3 = "Califa energy vs theta for full calorimeter in PR";
    cCalifa_theta_energy = new TCanvas(Name1.c_str(), Name1.c_str(), 10, 10, 500, 500);
    cCalifa_theta_energy->Divide(1, 2);
    cCalifa_theta_energy->cd(1);
    fh2_Califa_theta_energy_pr =
        R3B::root_owned<TH2F>(Name2.c_str(), Name3.c_str(), 500, 0, 92, fMaxEnergyPR, 0, fMaxEnergyPR);
    fh2_Califa_theta_energy_pr->GetXaxis()->SetTitle("Theta [deg]");
    fh2_Califa_theta_energy_pr->GetYaxis()->SetTitle("Energy [MeV]");
    fh2_Califa_theta_energy_pr->GetYaxis()->SetTitleOffset(1.4);
    fh2_Califa_theta_energy_pr->GetXaxis()->CenterTitle(true);
    fh2_Califa_theta_energy_pr->GetYaxis()->CenterTitle(true);
    fh2_Califa_theta_energy_pr->SetStats(0);
    fh2_Califa_theta_energy_pr->Draw("COLZ");
    gPad->SetLogz();

    Name2 = "fh2_Califa_total_energy_vs_theta_gr";
    Name3 = "Califa energy vs theta for full calorimeter in GR";
    cCalifa_theta_energy->cd(2);
    fh2_Califa_theta_energy_gr =
        R3B::root_owned<TH2F>(Name2.c_str(), Name3.c_str(), 500, 0, 92, fMaxEnergyGR / 10., 0, fMaxEnergyGR);
    fh2_Califa_theta_energy_gr->GetXaxis()->SetTitle("Theta [deg]");
    fh2_Califa_theta_energy_gr->GetYaxis()->SetTitle("Energy [keV]");
    fh2_Califa_theta_energy_gr->GetYaxis()->SetTitleOffset(1.4);
    fh2_Califa_theta_energy_gr->GetXaxis()->CenterTitle(true);
    fh2_Califa_theta_energy_gr->GetYaxis()->CenterTitle(true);
    fh2_Califa_theta_energy_gr->SetStats(0);
    fh2_Califa_theta_energy_gr->Draw("COLZ");
    gPad->SetLogz();

    // CANVAS Total energy
    Name1 = "Calorimeter_total_energy_per_hit";
    Name2 = "fh1_Califa_total_energy_pr";
    Name3 = "Califa total energy per hit for the full calorimeter in PR";
    cCalifa_hitenergy = new TCanvas(Name1.c_str(), Name1.c_str(), 10, 10, 500, 500);
    cCalifa_hitenergy->Divide(1, 2);
    cCalifa_hitenergy->cd(1);
    fh1_Califa_total_energy_pr = R3B::root_owned<TH1F>(Name2.c_str(), Name3.c_str(), fMaxEnergyPR, 0, fMaxEnergyPR);
    fh1_Califa_total_energy_pr->GetXaxis()->SetTitle("Energy [MeV]");
    fh1_Califa_total_energy_pr->GetYaxis()->SetTitle("Counts");
    fh1_Califa_total_energy_pr->GetYaxis()->SetTitleOffset(1.4);
    fh1_Califa_total_energy_pr->GetXaxis()->CenterTitle(true);
    fh1_Califa_total_energy_pr->GetYaxis()->CenterTitle(true);
    fh1_Califa_total_energy_pr->SetFillColor(29);
    fh1_Califa_total_energy_pr->SetLineColor(1);
    fh1_Califa_total_energy_pr->SetLineWidth(2);
    fh1_Califa_total_energy_pr->Draw("");
    gPad->SetLogy();

    cCalifa_hitenergy->cd(2);
    Name2 = "fh1_Califa_total_energy_gr";
    Name3 = "Califa total energy per hit for the full calorimeter in GR";
    fh1_Califa_total_energy_gr =
        R3B::root_owned<TH1F>(Name2.c_str(), Name3.c_str(), fMaxEnergyGR / 10., 0, fMaxEnergyGR);
    fh1_Califa_total_energy_gr->GetXaxis()->SetTitle("Energy [keV]");
    fh1_Califa_total_energy_gr->GetYaxis()->SetTitle("Counts");
    fh1_Califa_total_energy_gr->GetYaxis()->SetTitleOffset(1.4);
    fh1_Califa_total_energy_gr->GetXaxis()->CenterTitle(true);
    fh1_Califa_total_energy_gr->GetYaxis()->CenterTitle(true);
    fh1_Califa_total_energy_gr->SetFillColor(29);
    fh1_Califa_total_energy_gr->SetLineColor(1);
    fh1_Califa_total_energy_gr->SetLineWidth(2);
    fh1_Califa_total_energy_gr->Draw("");
    gPad->SetLogy();

    // CANVAS opening angle
    Name1 = "Califa_opening_angle_hit";
    Name2 = "fh1_Califa_opening";
    Name3 = "Califa opening angle";
    cCalifa_opening = new TCanvas(Name1.c_str(), Name1.c_str(), 10, 10, 500, 500);
    fh1_openangle = R3B::root_owned<TH1F>(Name2.c_str(), Name3.c_str(), 160, 10, 170);
    fh1_openangle->GetXaxis()->SetTitle("Opening angle [deg]");
    fh1_openangle->GetYaxis()->SetTitle("Counts");
    fh1_openangle->GetXaxis()->CenterTitle(true);
    fh1_openangle->GetYaxis()->CenterTitle(true);
    fh1_openangle->GetYaxis()->SetTitleOffset(1.2);
    fh1_openangle->GetXaxis()->SetTitleOffset(1.2);
    fh1_openangle->SetFillColor(8);
    fh1_openangle->SetLineColor(1);
    fh1_openangle->SetLineWidth(2);
    fh1_openangle->Draw("");

    // CANVAS opening angle
    Name1 = "Califa_opening_angle_tpats";
    Name2 = "fh1_Califa_opening_tpats";
    Name3 = "Califa opening angle vs tpat";
    cCalifa_opening_tpat = new TCanvas(Name1.c_str(), Name1.c_str(), 10, 10, 500, 500);
    fh2_openangle_tpat = R3B::root_owned<TH2F>(Name2.c_str(), Name3.c_str(), 17, -0.5, 16.5, 160, 10, 170);
    fh2_openangle_tpat->GetXaxis()->SetTitle("Tpat");
    fh2_openangle_tpat->GetYaxis()->SetTitle("Opening angle [deg]");
    fh2_openangle_tpat->GetXaxis()->CenterTitle(true);
    fh2_openangle_tpat->GetYaxis()->CenterTitle(true);
    fh2_openangle_tpat->GetYaxis()->SetTitleOffset(1.2);
    fh2_openangle_tpat->GetXaxis()->SetTitleOffset(1.2);
    fh2_openangle_tpat->Draw("colz");

    // Difference between Califa WRs
    Name1 = "WR_Califa";
    Name2 = "fh1_WR_Califa";
    Name3 = "WR-Wixhausen - WR-Messel";
    cCalifa_wr = new TCanvas(Name1.c_str(), Name1.c_str(), 10, 10, 500, 500);
    fh1_Califa_wr[0] = R3B::root_owned<TH1I>(
        Name2.c_str(), "black:mes0-mes1, red:wix0-wix1, green:mes0-wix0, blue:mes1-wix1", 400, -400, 400);
    fh1_Califa_wr[0]->GetXaxis()->SetTitle("ts diff");
    fh1_Califa_wr[0]->GetYaxis()->SetTitle("Counts");
    fh1_Califa_wr[0]->GetYaxis()->SetTitleOffset(1.3);
    fh1_Califa_wr[0]->GetXaxis()->CenterTitle(true);
    fh1_Califa_wr[0]->GetYaxis()->CenterTitle(true);
    // fh1_Califa_wr[0]->SetFillColor(29);
    fh1_Califa_wr[0]->SetLineColor(1);
    fh1_Califa_wr[0]->SetLineWidth(2);
    fh1_Califa_wr[0]->Draw("");
    fh1_Califa_wr[1] = R3B::root_owned<TH1I>(Name2.c_str(), "wix0-wix1", 400, -400, 400);
    fh1_Califa_wr[2] = R3B::root_owned<TH1I>(Name2.c_str(), "mes0-wix0", 400, -400, 400);
    fh1_Califa_wr[3] = R3B::root_owned<TH1I>(Name2.c_str(), "mes1-wix1", 400, -400, 400);
    fh1_Califa_wr[1]->SetLineColor(2);
    fh1_Califa_wr[2]->SetLineColor(3);
    fh1_Califa_wr[3]->SetLineColor(4);
    fh1_Califa_wr[1]->SetLineWidth(2);
    fh1_Califa_wr[2]->SetLineWidth(2);
    fh1_Califa_wr[3]->SetLineWidth(2);
    fh1_Califa_wr[1]->Draw("same");
    fh1_Califa_wr[2]->Draw("same");
    fh1_Califa_wr[3]->Draw("same");

    // Difference between Califa-Master WRs
    Name1 = "WR_Master_Califa";
    cWrs = new TCanvas(Name1.c_str(), Name1.c_str(), 10, 10, 500, 500);
    Name2 = "fh1_WR_Master_Califa";
    Name3 = "WRCalifa - WRMaster mes0:red, mes1:blue, wix0:black, wix1:green";
    fh1_wrs[0] = R3B::root_owned<TH1I>(Name2.c_str(), Name3.c_str(), 4000, -4000, 4000);
    fh1_wrs[0]->SetStats(1);
    fh1_wrs[0]->GetXaxis()->SetTitle("WRTs difference");
    fh1_wrs[0]->GetYaxis()->SetTitle("Counts");
    fh1_wrs[0]->GetYaxis()->SetTitleOffset(1.3);
    fh1_wrs[0]->GetXaxis()->CenterTitle(true);
    fh1_wrs[0]->GetYaxis()->CenterTitle(true);
    fh1_wrs[0]->SetLineColor(2);
    fh1_wrs[0]->SetLineWidth(3);
    fh1_wrs[0]->Draw("");
    fh1_wrs[1] = R3B::root_owned<TH1I>("fh1_WR_Master_Califa_Messel", "Mes0", 4000, -4000, 4000);
    fh1_wrs[1]->SetLineColor(4);
    fh1_wrs[1]->SetLineWidth(3);
    fh1_wrs[1]->Draw("same");
    fh1_wrs[2] = R3B::root_owned<TH1I>("fh1_WR_Master_Califa_Wix1", "Wix1", 4000, -4000, 4000);
    fh1_wrs[2]->SetLineColor(1);
    fh1_wrs[2]->SetLineWidth(3);
    fh1_wrs[2]->Draw("same");
    fh1_wrs[3] = R3B::root_owned<TH1I>("fh1_WR_Master_Califa_Messel1", "Mes1", 4000, -4000, 4000);
    fh1_wrs[3]->SetLineColor(3);
    fh1_wrs[3]->SetLineWidth(3);
    fh1_wrs[3]->Draw("same");

    // CANVAS energy vs wrs
    Name1 = "Califa_wr_vs_energy";
    Name2 = "fh2_wr_vs_energy_left";
    Name3 = "Califa WR vs hit-energy left side";
    cCalifa_wr_energy = new TCanvas(Name1.c_str(), Name1.c_str(), 10, 10, 500, 500);
    cCalifa_wr_energy->Divide(1, 2);
    fh2_Cal_wr_energy_l = R3B::root_owned<TH2F>(Name2.c_str(), Name3.c_str(), 700, -4000, 4000, bins, minE, maxE);
    fh2_Cal_wr_energy_l->GetXaxis()->SetTitle("WR difference (Master-Califa)");
    fh2_Cal_wr_energy_l->GetYaxis()->SetTitle("Energy [keV]");
    fh2_Cal_wr_energy_l->GetYaxis()->SetTitleOffset(1.4);
    fh2_Cal_wr_energy_l->GetXaxis()->CenterTitle(true);
    fh2_Cal_wr_energy_l->GetYaxis()->CenterTitle(true);
    cCalifa_wr_energy->cd(1);
    fh2_Cal_wr_energy_l->Draw("COLZ");
    Name2 = "fh2_wr_vs_energy_right";
    Name3 = "Califa WR vs hit-energy right side";
    fh2_Cal_wr_energy_r = R3B::root_owned<TH2F>(Name2.c_str(), Name3.c_str(), 700, -4000, 4000, bins, minE, maxE);
    fh2_Cal_wr_energy_r->GetXaxis()->SetTitle("WR difference (Master-Califa)");
    fh2_Cal_wr_energy_r->GetYaxis()->SetTitle("Energy [keV]");
    fh2_Cal_wr_energy_r->GetYaxis()->SetTitleOffset(1.4);
    fh2_Cal_wr_energy_r->GetXaxis()->CenterTitle(true);
    fh2_Cal_wr_energy_r->GetYaxis()->CenterTitle(true);
    cCalifa_wr_energy->cd(2);
    fh2_Cal_wr_energy_r->Draw("COLZ");

    // FOLDERS for Califa
    auto folder_sta = new TFolder("Statistics_per_ring", "Statistics info");
    for (Int_t i = 1; i < fNumRings; i++)
    { // FIXME in the future
        folder_sta->Add(cMap_RingR[i]);
        folder_sta->Add(cMap_RingL[i]);
    }

    auto folder_el = new TFolder("Energy_Map_per_crystal_Left", "Energy per crystal, left side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][0] != -1)
                folder_el->Add(cMapCry[1][r][p]);

    auto folder_etotl = new TFolder("Energy_Tot_per_crystal_Left", "Energy vs Tot per crystal, left side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][0] != -1)
                folder_etotl->Add(cMapCryTot[1][r][p]);

    auto folder_eprl = new TFolder("Energy_Map_per_crystal_Left_PR", "Energy per crystal, left side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][2] != -1)
                folder_eprl->Add(cMapCryP[1][r][p]);

    auto folder_eprtotl =
        new TFolder("Energy_Tot_per_crystal_Left_PR", "Energy vs Tot per crystal, left side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][2] != -1)
                folder_eprtotl->Add(cMapCryPTot[1][r][p]);

    auto folder_er = new TFolder("Energy_Map_per_crystal_Right", "Energy per crystal, right side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][0] != -1)
                folder_er->Add(cMapCry[0][r][p]);

    auto folder_etotr = new TFolder("Energy_Tot_per_crystal_Right", "Energy vs Tot per crystal, right side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][0] != -1)
                folder_etotr->Add(cMapCryTot[0][r][p]);

    auto folder_eprr = new TFolder("Energy_Map_per_crystal_Right_PR", "Energy per crystal, right side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][2] != -1)
                folder_eprr->Add(cMapCryP[0][r][p]);

    auto folder_eprtotr =
        new TFolder("Energy_Tot_per_crystal_Right_PR", "Energy vs Tot per crystal, right side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][2] != -1)
                folder_eprtotr->Add(cMapCryPTot[0][r][p]);

    auto folder_ecall = new TFolder("Energy_Cal_per_crystal_Left", "Energy Cal per crystal, left side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][0] != -1)
                folder_ecall->Add(cMapCryCal[1][r][p]);

    auto folder_eprcall =
        new TFolder("Energy_Cal_per_crystal_Left_PR", "Energy Cal per crystal, left side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[1][r][p][2] != -1)
                folder_eprcall->Add(cMapCryPCal[1][r][p]);

    auto folder_ecalr = new TFolder("Energy_Cal_per_crystal_Right", "Energy Cal per crystal, right side info");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][0] != -1)
                folder_ecalr->Add(cMapCryCal[0][r][p]);

    auto folder_eprcalr =
        new TFolder("Energy_Cal_per_crystal_Right_PR", "Energy Cal per crystal, right side info for PR");
    for (Int_t r = 0; r < fNumRings; r++)
        for (Int_t p = 0; p < fNumPreamps; p++)
            if (fFebexInfo[0][r][p][2] != -1)
                folder_eprcalr->Add(cMapCryPCal[0][r][p]);

    // MAIN FOLDER-Califa
    auto mainfolCalifa = new TFolder("CALIFA", "CALIFA info");
    mainfolCalifa->Add(cCalifaMult);
    mainfolCalifa->Add(cCalifa_cry_energy);
    mainfolCalifa->Add(cFebex_overview);

    auto folder_wrs = new TFolder("WRs", "CALIFA white-rabbit info");
    folder_wrs->Add(cWr_diff_febex);

    auto tpatfolder = new TFolder("Corr_with_tpat", "Correlations with tpat info");

    if (fWRItemsCalifa && fWRItemsMaster)
    {
        folder_wrs->Add(cWrs);
    }
    if (fTrigMappedItemsCalifa)
    {
        folder_wrs->Add(cMap_ECor);
    }
    if (fWRItemsCalifa)
    {
        folder_wrs->Add(cCalifa_wr);
    }
    if (fWRItemsCalifa)
        mainfolCalifa->Add(folder_wrs);

    auto mappedfolder = new TFolder("Mapped_data", "Mapped data");
    mappedfolder->Add(folder_sta);
    mappedfolder->Add(folder_el);
    mappedfolder->Add(folder_eprl);
    mappedfolder->Add(folder_er);
    mappedfolder->Add(folder_eprr);

    if (fTotHist)
    {
        mappedfolder->Add(folder_etotl);
        mappedfolder->Add(folder_eprtotl);
        mappedfolder->Add(folder_etotr);
        mappedfolder->Add(folder_eprtotr);
    }
    mainfolCalifa->Add(mappedfolder);

    if (fCalItemsCalifa)
    {
        auto calfolder = new TFolder("Cal_data", "Calibrated data");
        calfolder->Add(cCalifa_cry_energy_cal);
        calfolder->Add(folder_ecall);
        calfolder->Add(folder_eprcall);
        calfolder->Add(folder_ecalr);
        calfolder->Add(folder_eprcalr);
        mainfolCalifa->Add(calfolder);
    }
    if (fHitItemsCalifa)
    {
        auto hitfolder = new TFolder("Hit_data", "Hit data");
        hitfolder->Add(cCalifaCoinE);
        hitfolder->Add(cCalifaCoinTheta);
        hitfolder->Add(cCalifaCoinPhi);
        hitfolder->Add(cCalifa_angles);
        hitfolder->Add(cCalifa_opening);
        hitfolder->Add(cCalifa_theta_energy);
        hitfolder->Add(cCalifa_hitenergy);
        tpatfolder->Add(cCalifa_opening_tpat);
        if (fWRItemsCalifa && fWRItemsMaster)
            hitfolder->Add(cCalifa_wr_energy);
        mainfolCalifa->Add(hitfolder);
        mainfolCalifa->Add(tpatfolder);
        auto pidfolder = new TFolder("PID", "PID data");
        pidfolder->Add(cCalifa_NsNf);
        mainfolCalifa->Add(pidfolder);
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
    SetParameter();
    return kSUCCESS;
}

void R3BCalifaOnlineSpectra::Reset_CALIFA_Histo()
{
    R3BLOG(info, "");
    if (fWRItemsCalifa)
    {
        fh1_Califa_wr[0]->Reset();
        fh1_Califa_wr[1]->Reset();
        fh1_Califa_wr[2]->Reset();
        fh1_Califa_wr[3]->Reset();
    }

    if (fWRItemsCalifa && fWRItemsMaster)
    {
        fh1_wrs[0]->Reset();
        fh1_wrs[1]->Reset();
        fh1_wrs[2]->Reset();
        fh1_wrs[3]->Reset();
        fh2_wrts_diff_febex_pc_sfp->Reset();
        fh2_febex_side_febex_pc_sfp->Reset();
        if (fHitItemsCalifa)
        {
            fh2_Cal_wr_energy_r->Reset();
            fh2_Cal_wr_energy_l->Reset();
        }
    }

    if (fTrigMappedItemsCalifa)
    {
        for (const auto& hist : fh2_Califa_EtrigCor)
        {
            hist->Reset();
        }
        for (const auto& hist : fh1_Califa_Etrig)
        {
            hist->Reset();
        }
    }

    if (fMappedItemsCalifa)
    {
        fh1_Califa_Mult->Reset();
        fh2_Califa_cryId_energy->Reset();
        for (const auto& hist : fh2_Preamp_vs_ch_R)
        {
            hist->Reset();
        }
        for (const auto& hist : fh2_Preamp_vs_ch_L)
        {
            hist->Reset();
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
        for (const auto& hist : fh2_Califa_NsNf)
        {
            hist->Reset();
        }
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
        fh2_Califa_coinE_p2p->Reset();
        fh2_Califa_coinTheta->Reset();
        fh2_Califa_coinTheta_cutOPA->Reset();
        fh2_Califa_coinPhi->Reset();
        fh2_Califa_theta_phi->Reset();
        fh2_Califa_theta_energy_pr->Reset();
        fh2_Califa_theta_energy_gr->Reset();
        fh1_Califa_total_energy_pr->Reset();
        fh1_Califa_total_energy_gr->Reset();
        fh1_openangle->Reset();
        fh2_openangle_tpat->Reset();
    }
}

void R3BCalifaOnlineSpectra::Log_CALIFA_Histo()
{
    R3BLOG(info, "");
    cCalifa_cry_energy->cd();
    gPad->SetLogz(fLogScale ? 1 : 0);

    cCalifaMult->cd();
    gPad->SetLogy(fLogScale ? 1 : 0);

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
                        gPad->SetLogy(fLogScale ? 1 : 0);
                    }
                    if (fFebexInfo[s][r][p][2] != -1)
                    { // histograms for proton range
                        cMapCryP[s][r][p]->cd(j + 1);
                        gPad->SetLogy(fLogScale ? 1 : 0);
                    }
                    if (fCalItemsCalifa)
                    {
                        if (fFebexInfo[s][r][p][0] != -1)
                        {
                            cMapCryCal[s][r][p]->cd(j + 1);
                            gPad->SetLogy(fLogScale ? 1 : 0);
                        }
                        if (fFebexInfo[s][r][p][2] != -1)
                        { // histograms for proton range
                            cMapCryPCal[s][r][p]->cd(j + 1);
                            gPad->SetLogy(fLogScale ? 1 : 0);
                        }
                    }
                }
            }
        }
    }

    if (fCalItemsCalifa)
    {
        cCalifa_cry_energy_cal->cd();
        gPad->SetLogz(fLogScale ? 1 : 0);
        for (int padN = 1; padN <= fh2_Califa_NsNf.size(); ++padN)
        {
            cCalifa_NsNf->cd(padN);
            gPad->SetLogz(fLogScale ? 1 : 0);
        }
    }

    if (fHitItemsCalifa)
    {
        for (int padN : { 1, 2 })
        {
            cCalifa_hitenergy->cd(padN);
            gPad->SetLogy(fLogScale ? 1 : 0);
            cCalifa_theta_energy->cd(padN);
            gPad->SetLogz(fLogScale ? 1 : 0);
        }
        cCalifa_angles->cd();
        gPad->SetLogz(fLogScale ? 1 : 0);
    }
    fLogScale = fLogScale ? kFALSE : kTRUE;
}

void R3BCalifaOnlineSpectra::Febex2Preamp_CALIFA_Histo()
{
    R3BLOG(info, "");

    if (fFebex2Preamp)
    { // Preamp to Febex sequence
        for (Int_t s = 0; s < fNumSides; s++)
        {
            const char* Side = (s == 1) ? "Left" : "Right";
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
                            std::stringstream Name;
                            Name << "Map level, Side " << Side << ", Ring " << (r + 1) << ", Slot "
                                 << fFebexInfo[s][r][p][0] << ", Febex " << fFebexInfo[s][r][p][1] << ", ch. "
                                 << fOrderFebexPreamp[j];
                            fh1_crystals[s][r][p][j]->SetTitle(Name.str().c_str());
                            fh1_crystals[s][r][p][j]->Draw();

                            cMapCryTot[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                            fh2_crystalsETot[s][r][p][j]->SetTitle(Name.str().c_str());
                            fh2_crystalsETot[s][r][p][j]->Draw();
                        }
                        if (fFebexInfo[s][r][p][2] != -1)
                        {
                            cMapCryP[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                            fh1_crystals_p[s][r][p][j]->SetFillColor(kGreen);
                            std::stringstream Name1;
                            Name1 << "Map level (PR), Side " << Side << ", Ring " << (r + 1) << ", Slot "
                                  << fFebexInfo[s][r][p][2] << ", Febex " << fFebexInfo[s][r][p][3] << ", ch. "
                                  << fOrderFebexPreamp[j];
                            fh1_crystals_p[s][r][p][j]->SetTitle(Name1.str().c_str());
                            fh1_crystals_p[s][r][p][j]->Draw();

                            cMapCryPTot[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                            std::stringstream Name2;
                            Name2 << "Cal level (PR), Side " << Side << ", Ring " << (r + 1) << ", Slot "
                                  << fFebexInfo[s][r][p][2] << ", Febex " << fFebexInfo[s][r][p][3] << ", ch. "
                                  << fOrderFebexPreamp[j];
                            fh2_crystalsETot_p[s][r][p][j]->SetTitle(Name2.str().c_str());
                            fh2_crystalsETot_p[s][r][p][j]->Draw();
                        }

                        if (fCalItemsCalifa)
                        {
                            if (fFebexInfo[s][r][p][0] != -1)
                            {
                                cMapCryCal[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                                fh1_crystals_cal[s][r][p][j]->SetFillColor(kGreen);
                                std::stringstream Name;
                                Name << "Cal level (PR), Side " << Side << ", Ring " << (r + 1) << ", Slot "
                                     << fFebexInfo[s][r][p][0] << ", Febex " << fFebexInfo[s][r][p][1] << ", ch. "
                                     << fOrderFebexPreamp[j];
                                fh1_crystals_cal[s][r][p][j]->SetTitle(Name.str().c_str());
                                fh1_crystals_cal[s][r][p][j]->Draw();
                            }
                            if (fFebexInfo[s][r][p][2] != -1)
                            {
                                cMapCryPCal[s][r][p]->cd(fOrderFebexPreamp[j] + 1);
                                fh1_crystals_p_cal[s][r][p][j]->SetFillColor(kGreen);
                                std::stringstream Name;
                                Name << "Cal level (PR), Side " << Side << ", Ring " << (r + 1) << ", Slot "
                                     << fFebexInfo[s][r][p][2] << ", Febex " << fFebexInfo[s][r][p][3] << ", ch. "
                                     << fOrderFebexPreamp[j];
                                fh1_crystals_p_cal[s][r][p][j]->SetTitle(Name.str().c_str());
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
            const char* Side = (s == 1) ? "Left" : "Right";
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
                            std::stringstream Name;
                            Name << "Map level, Side " << Side << ", Ring " << (r + 1) << ", Preamp " << (p + 1)
                                 << ", ch. " << (j + 1);
                            fh1_crystals[s][r][p][j]->SetTitle(Name.str().c_str());
                            fh1_crystals[s][r][p][j]->Draw();

                            cMapCryTot[s][r][p]->cd(j + 1);
                            fh2_crystalsETot[s][r][p][j]->SetTitle(Name.str().c_str());
                            fh2_crystalsETot[s][r][p][j]->Draw();
                        }
                        if (fFebexInfo[s][r][p][2] != -1)
                        {
                            cMapCryP[s][r][p]->cd(j + 1);
                            fh1_crystals_p[s][r][p][j]->SetFillColor(45);
                            std::stringstream Name;
                            Name << "Map level (PR), Side " << Side << ", Ring " << (r + 1) << ", Preamp " << (p + 1)
                                 << ", ch. " << (j + 1);
                            fh1_crystals_p[s][r][p][j]->SetTitle(Name.str().c_str());
                            fh1_crystals_p[s][r][p][j]->Draw();

                            cMapCryPTot[s][r][p]->cd(j + 1);
                            fh2_crystalsETot_p[s][r][p][j]->SetTitle(Name.str().c_str());
                            fh2_crystalsETot_p[s][r][p][j]->Draw();
                        }

                        if (fCalItemsCalifa)
                        {
                            if (fFebexInfo[s][r][p][0] != -1)
                            {
                                cMapCryCal[s][r][p]->cd(j + 1);
                                fh1_crystals_cal[s][r][p][j]->SetFillColor(45);
                                std::stringstream Name;
                                Name << "Cal level, Side " << Side << ", Ring " << (r + 1) << ", Preamp " << (p + 1)
                                     << ", ch. " << (j + 1);
                                fh1_crystals_cal[s][r][p][j]->SetTitle(Name.str().c_str());
                                fh1_crystals_cal[s][r][p][j]->Draw();
                            }

                            if (fFebexInfo[s][r][p][2] != -1)
                            {
                                cMapCryPCal[s][r][p]->cd(j + 1);
                                fh1_crystals_p_cal[s][r][p][j]->SetFillColor(45);
                                std::stringstream Name;
                                Name << "Cal level (PR), Side " << Side << ", Ring " << (r + 1) << ", Preamp "
                                     << (p + 1) << ", ch. " << (j + 1);
                                fh1_crystals_p_cal[s][r][p][j]->SetTitle(Name.str().c_str());
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

void R3BCalifaOnlineSpectra::Exec(Option_t* /*option*/)
{
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;
    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit1 = fTpat1 - 1;
    Int_t fTpat_bit2 = fTpat2 - 1;
    Int_t tpatbin = 0;
    std::vector<int> tpatindex;
    if (header && fTpat1 >= 0 && fTpat2 >= 0)
    {
        for (int i = 0; i < 16; i++)
        {
            tpatbin = (header->GetTpat() & (1 << i));

            if (tpatbin != 0)
                tpatindex.push_back(i + 1);

            if (tpatbin != 0 && (i < fTpat_bit1 || i > fTpat_bit2))
            {
                return;
            }
        }
    }
    else if (header)
    {
        for (Int_t i = 0; i < 16; i++)
        {
            tpatbin = (header->GetTpat() & (1 << i));
            if (tpatbin != 0)
                tpatindex.push_back(i + 1);
        }
    }

    int64_t wr[4];
    int64_t wrm = 0.0;
    for (int i = 0; i < 4; i++)
        wr[i] = 0;
    // WR data
    if (fWRItemsCalifa && fWRItemsCalifa->GetEntriesFast() > 0)
    {
        // Califa
        Int_t nHits = fWRItemsCalifa->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BWRData* hit = dynamic_cast<R3BWRData*>(fWRItemsCalifa->At(ihit));
            if (!hit)
                continue;
            wr[ihit] = hit->GetTimeStamp(); // ihit 0,1,2,3: mes0, mes1, wix0, wix1
                                            // std::cout << ihit << " " << wr[ihit] << std::endl;
        }
        if (nHits == 4)
        {
            fh1_Califa_wr[0]->Fill(wr[0] - wr[1]);
            fh1_Califa_wr[1]->Fill(wr[2] - wr[3]);
            fh1_Califa_wr[2]->Fill(wr[0] - wr[2]);
            fh1_Califa_wr[3]->Fill(wr[1] - wr[3]);
        }

        // Master Ref. (exp. 2020)
        if (fWRItemsMaster && fWRItemsMaster->GetEntriesFast() > 0)
        {
            nHits = fWRItemsMaster->GetEntriesFast();
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BWRData* hit = dynamic_cast<R3BWRData*>(fWRItemsMaster->At(ihit));
                if (!hit)
                    continue;
                wrm = hit->GetTimeStamp();
            }
        }
    }

    // Mapped trigger data
    if (fTrigMappedItemsCalifa && fTrigMappedItemsCalifa->GetEntriesFast() > 0)
    {
        Int_t nHits = fTrigMappedItemsCalifa->GetEntriesFast();
        Double_t e[4];
        for (Int_t i = 0; i < 4; i++)
            e[i] = 0.;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = dynamic_cast<R3BCalifaMappedData*>(fTrigMappedItemsCalifa->At(ihit));
            if (!hit)
                continue;
            Int_t ch = hit->GetCrystalId() - 1;
            e[ch] = hit->GetEnergy();
        }
        if (e[0] > 0 || e[1] > 0)
            fh2_Califa_EtrigCor[0]->Fill(e[0], e[1]);
        if (e[0] > 0 || e[2] > 0)
            fh2_Califa_EtrigCor[1]->Fill(e[0], e[2]);
        if (e[0] > 0 || e[3] > 0)
            fh2_Califa_EtrigCor[2]->Fill(e[0], e[3]);
        if (e[1] > 0 || e[3] > 0)
            fh2_Califa_EtrigCor[3]->Fill(e[1], e[3]);
        // if (e[0] > 0 && e[1] > 0 && header && header->GetTrigger() == 1)
        //     fh2_Califa_EtrigCor[1]->Fill(e[0], e[1]);

        if (e[0] > 0)
            fh1_Califa_Etrig[0]->Fill(e[0]);
        if (e[1] > 0)
            fh1_Califa_Etrig[1]->Fill(e[1]);
        if (e[2] > 0)
            fh1_Califa_Etrig[2]->Fill(e[2]);
        if (e[3] > 0)
            fh1_Califa_Etrig[3]->Fill(e[3]);
    }

    // Mapped data
    if (fMappedItemsCalifa && fMappedItemsCalifa->GetEntriesFast() > 0)
    {
        Int_t nHits = fMappedItemsCalifa->GetEntriesFast();
        Int_t Crymult = 0;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = dynamic_cast<R3BCalifaMappedData*>(fMappedItemsCalifa->At(ihit));
            if (!hit)
                continue;

            Int_t cryId = hit->GetCrystalId();

            bool side = !(fMap_Par->GetHalf(cryId) % 2);
            // std::cout << fMap_Par->GetHalf(cryId) << std::endl;
            int sfp = fMap_Par->GetFebexSlot(cryId);
            int pc = fMap_Par->GetFebexPC(cryId);
            int febex_ch = fMap_Par->GetFebexChannel(cryId);
            int febex_mod = fMap_Par->GetFebexMod(cryId);
            // compensate slave exploder delays:
            int64_t wrc = hit->GetWrts();
            if (wrm > 0.)
            {
                float this_califa_wr = 0;
                if (side == 1 && pc == 0)
                    this_califa_wr = wr[0];
                if (side == 1 && pc == 1)
                    this_califa_wr = wr[1];
                if (side == 0 && pc == 0)
                    this_califa_wr = wr[2];
                if (side == 0 && pc == 1)
                    this_califa_wr = wr[3];

                fh1_wrs[side + pc * 2]->Fill(wrc - wrm);
                // fh1_wrs[side + pc*2]->Fill(this_califa_wr - wrm);
                fh2_wrts_diff_febex_pc_sfp->Fill(side * 100 + sfp * 20 + pc * 80 + febex_mod, wrc - wrm);
                // fh2_wrts_diff_febex_pc_sfp->Fill(side*100 + sfp*20 + pc*80 +febex_ch, this_califa_wr-wrm);
            }

            if ((fMap_Par->GetInUse(cryId) == 1 && cryId <= fNbCalifaCrystals / 2) ||
                (cryId > fNbCalifaCrystals / 2 &&
                 fMap_Par->GetInUse(cryId) != fMap_Par->GetInUse(cryId - fNbCalifaCrystals / 2)))
                Crymult++;

            fh2_Califa_cryId_energy->Fill(cryId, hit->GetEnergy());
            fh2_febex_side_febex_pc_sfp->Fill(febex_mod + sfp * 20 + pc * 80, side * 20 + febex_ch);

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
            auto hit = dynamic_cast<R3BCalifaCrystalCalData*>(fCalItemsCalifa->At(ihit));
            if (!hit)
                continue;

            Int_t cryId = hit->GetCrystalId();

            fh2_Califa_cryId_energy_cal->Fill(cryId, hit->GetEnergy());

            auto nf_ns = (hit->GetNs() + hit->GetNf());
            if (nf_ns > 0. && hit->GetNs() > 0 && hit->GetNf() > 0)
            {
                if (cryId > fNbCalifaCrystals / 2)
                {
                    if (cryId <= BarrelCrystals)
                    {
                        fh2_Califa_NsNf[0]->Fill(nf_ns / 1000., hit->GetNf() / nf_ns);
                    }
                    else if (cryId <= iPhosCrystals)
                    {
                        fh2_Califa_NsNf[1]->Fill(nf_ns / 1000., hit->GetNf() / nf_ns);
                    }
                    else
                    {
                        fh2_Califa_NsNf[2]->Fill(nf_ns / 1000., hit->GetNf() / nf_ns);
                    }
                }
            }

            if (fMap_Par->GetInUse(cryId) == 1 && cryId <= fNbCalifaCrystals / 2)
                fh1_crystals_cal[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                    ->Fill(hit->GetEnergy()); // keV
            else if (fMap_Par->GetInUse(cryId) == 1 && cryId > fNbCalifaCrystals / 2)
                fh1_crystals_p_cal[fMap_Par->GetHalf(cryId) - 1][fMap_Par->GetRing(cryId) - 1]
                                  [fMap_Par->GetPreamp(cryId) - 1][fMap_Par->GetChannel(cryId) - 1]
                                      ->Fill(hit->GetEnergy() / 1000.); // MeV
        }
    }

    // Hit data
    if (fHitItemsCalifa && fHitItemsCalifa->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsCalifa->GetEntriesFast();
        fh1_Califa_MultHit->Fill(nHits);

        double theta = 0., phi = 0.;
        std::vector<double> califa_theta;
        std::vector<double> califa_phi;
        std::vector<double> califa_e;

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = dynamic_cast<R3BCalifaClusterData*>(fHitItemsCalifa->At(ihit));
            if (!hit)
                continue;
            theta = hit->GetTheta() * TMath::RadToDeg();
            phi = hit->GetPhi() * TMath::RadToDeg();
            fh2_Califa_theta_phi->Fill(theta, phi);
            if (!(hit->GetEnergy() < 20e3))
            {
                califa_theta.push_back(theta);
                califa_phi.push_back(phi);
                califa_e.push_back(hit->GetEnergy());
                fh2_Califa_theta_energy_pr->Fill(theta, hit->GetEnergy() / 1000.); // MeV
                fh1_Califa_total_energy_pr->Fill(hit->GetEnergy() / 1000.);        // MeV
            }
            else
            {
                fh2_Califa_theta_energy_gr->Fill(theta, hit->GetEnergy());
                fh1_Califa_total_energy_gr->Fill(hit->GetEnergy());
            }
        }

        TVector3 master[2];
        Double_t maxEL = 0., maxER = 0.;
        for (Int_t i1 = 0; i1 < califa_theta.size(); i1++)
        {

            if (califa_e[i1] > maxER && TMath::Abs(califa_phi[i1]) > 90.) // wixhausen
            {
                master[0].SetMagThetaPhi(1., califa_theta[i1] * TMath::DegToRad(), califa_phi[i1] * TMath::DegToRad());
                maxER = califa_e[i1];
            }
            if (califa_e[i1] > maxEL && TMath::Abs(califa_phi[i1]) < 90.)
            { // messel
                master[1].SetMagThetaPhi(1., califa_theta[i1] * TMath::DegToRad(), califa_phi[i1] * TMath::DegToRad());
                maxEL = califa_e[i1];
            }
        }
        if (maxEL > fMinProtonE && maxER > fMinProtonE)
        {
            auto opa = master[0].Angle(master[1]) * TMath::RadToDeg();
            fh1_openangle->Fill(opa);
            if (opa < 90. && opa > 68.)
                fh2_Califa_coinE_p2p->Fill(maxEL / 1000., maxER / 1000.);
            for (const auto& itpat : tpatindex)
                fh2_openangle_tpat->Fill(itpat, master[0].Angle(master[1]) * TMath::RadToDeg());
        }

        // Comparison of hits to get energy, theta and phi correlations between them
        for (Int_t i1 = 0; i1 < califa_theta.size(); i1++)
        {
            for (Int_t i2 = i1 + 1; i2 < califa_theta.size(); i2++)
            {
                if (gRandom->Uniform(0., 1.) < 0.5)
                {
                    fh2_Califa_coinE->Fill(califa_e[i1] / 1000., califa_e[i2] / 1000.);
                    fh2_Califa_coinTheta->Fill(califa_theta[i1], califa_theta[i2]);
                    fh2_Califa_coinPhi->Fill(califa_phi[i1], califa_phi[i2]);

                    if (master[0].Angle(master[1]) * TMath::RadToDeg() > 68 &&
                        master[0].Angle(master[1]) * TMath::RadToDeg() < 90)
                    {
                        fh2_Califa_coinTheta_cutOPA->Fill(califa_theta[i1], califa_theta[i2]);
                    }
                }
                else
                {
                    fh2_Califa_coinE->Fill(califa_e[i2] / 1000., califa_e[i1] / 1000.);
                    fh2_Califa_coinTheta->Fill(califa_theta[i2], califa_theta[i1]);
                    fh2_Califa_coinPhi->Fill(califa_phi[i2], califa_phi[i1]);

                    if (master[0].Angle(master[1]) * TMath::RadToDeg() > 68 &&
                        master[0].Angle(master[1]) * TMath::RadToDeg() < 90)
                    {
                        fh2_Califa_coinTheta_cutOPA->Fill(califa_theta[i2], califa_theta[i1]);
                    }
                }
            }
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
    if (fTrigMappedItemsCalifa)
    {
        fTrigMappedItemsCalifa->Clear();
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
        if (fHitItemsCalifa)
            cCalifa_wr_energy->Write();
    }

    if (fTrigMappedItemsCalifa)
    {
        fh2_Califa_EtrigCor[0]->Write();
        fh2_Califa_EtrigCor[1]->Write();
        fh2_Califa_EtrigCor[2]->Write();
        fh2_Califa_EtrigCor[3]->Write();
        fh1_Califa_Etrig[0]->Write();
        fh1_Califa_Etrig[1]->Write();
        fh1_Califa_Etrig[2]->Write();
        fh1_Califa_Etrig[3]->Write();
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
        for (const auto& hist : fh2_Califa_NsNf)
        {
            hist->Write();
        }
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
        cCalifa_opening->Write();
        cCalifa_opening_tpat->Write();
    }
}

ClassImp(R3BCalifaOnlineSpectra)
