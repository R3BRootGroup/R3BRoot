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

#include "R3BCalifavsTofDOnlineSpectra.h"
#include "R3BCalifaHitData.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BTofdHitData.h"
#include "R3BWRData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "TCanvas.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"
#include "TVector3.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

R3BCalifavsTofDOnlineSpectra::R3BCalifavsTofDOnlineSpectra()
    : R3BCalifavsTofDOnlineSpectra("CALIFAvsTofDOnlineSpectra", 1)
{
}

R3BCalifavsTofDOnlineSpectra::R3BCalifavsTofDOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fHitItemsCalifa(NULL)
    , fHitItemsTofd(NULL)
    , fNEvents(0)
    , fTpat(-1)
    , fMinProtonE(50000.) // 50MeV
    , fZselection(5.)
{
}

R3BCalifavsTofDOnlineSpectra::~R3BCalifavsTofDOnlineSpectra()
{
    R3BLOG(DEBUG1, "");
    if (fHitItemsCalifa)
        delete fHitItemsCalifa;
    if (fHitItemsTofd)
        delete fHitItemsTofd;
}

void R3BCalifavsTofDOnlineSpectra::SetParContainers()
{
    // Parameter Container
    // Reading amsStripCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(FATAL, !rtdb, "FairRuntimeDb not found");
}

InitStatus R3BCalifavsTofDOnlineSpectra::Init()
{
    R3BLOG(INFO, "");
    FairRootManager* mgr = FairRootManager::Instance();

    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to Hit data
    fHitItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaHitData");
    R3BLOG_IF(FATAL, !fHitItemsCalifa, "CalifaHitData not found");

    fHitItemsTofd = (TClonesArray*)mgr->GetObject("TofdHit");
    R3BLOG_IF(WARNING, !fHitItemsTofd, "TofdHit not found");

    // Create histograms for detectors

    // CANVAS Theta vs Phi
    cCalifa_angles = new TCanvas("Califa_Theta_vs_Phi_andTofD", "Theta vs Phi", 10, 10, 500, 500);
    cCalifa_angles->Divide(2, 1);
    for (int i = 0; i < 2; i++)
    {
        cCalifa_angles->cd(i + 1);
        char buf[512];
        snprintf(buf, 512, "%s%s", "fh2_Califa_theta_vs_phi", (i == 0) ? "" : "_withTofD");
        fh2_Califa_theta_phi[i] = new TH2F(buf, buf, 50, 0, 90, 180, -180, 180);
        fh2_Califa_theta_phi[i]->GetXaxis()->SetTitle("Theta [degrees]");
        fh2_Califa_theta_phi[i]->GetYaxis()->SetTitle("Phi [degrees]");
        fh2_Califa_theta_phi[i]->GetYaxis()->SetTitleOffset(1.2);
        fh2_Califa_theta_phi[i]->GetXaxis()->CenterTitle(true);
        fh2_Califa_theta_phi[i]->GetYaxis()->CenterTitle(true);
        fh2_Califa_theta_phi[i]->Draw("COLZ");
    }

    // MAIN FOLDER-Califa
    TFolder* mainfolCalifa = new TFolder("CALIFAvsTofD", "CALIFA vs TofD info");

    if (fHitItemsCalifa && fHitItemsTofd)
    {
        mainfolCalifa->Add(cCalifa_angles);
    }
    run->AddObject(mainfolCalifa);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_CalifavsTofD", Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BCalifavsTofDOnlineSpectra::ReInit()
{
    SetParContainers();
    // SetParameter();
    return kSUCCESS;
}

void R3BCalifavsTofDOnlineSpectra::Reset_Histo()
{
    R3BLOG(INFO, "");
    for (int i = 0; i < 2; i++)
    {
        fh2_Califa_theta_phi[i]->Reset();
    }
}

void R3BCalifavsTofDOnlineSpectra::Exec(Option_t* option)
{
    if ((fTpat >= 0) && (header) && ((header->GetTpat() & fTpat) != fTpat))
        return;

    bool fZminus1 = false;
    for (Int_t ihit = 0; ihit < fHitItemsTofd->GetEntriesFast(); ihit++)
    {
        auto hit = (R3BTofdHitData*)fHitItemsTofd->At(ihit);
        if (!hit)
            continue;
        if (hit->GetDetId() == 1 && hit->GetEloss() > (fZselection - 0.5) && hit->GetEloss() < (fZselection + 0.5))
            fZminus1 = true;
    }

    Int_t nHits = fHitItemsCalifa->GetEntriesFast();
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        auto hit = (R3BCalifaHitData*)fHitItemsCalifa->At(ihit);
        if (hit->GetEnergy() < fMinProtonE)
            continue;

        double theta = hit->GetTheta() * TMath::RadToDeg();
        double phi = hit->GetPhi() * TMath::RadToDeg();
        fh2_Califa_theta_phi[0]->Fill(theta, phi); // always
        if (fZminus1)
            fh2_Califa_theta_phi[1]->Fill(theta, phi); // only with TofD
    }
    fNEvents += 1;
}

void R3BCalifavsTofDOnlineSpectra::FinishEvent()
{
    if (fHitItemsCalifa)
    {
        fHitItemsCalifa->Clear();
    }
    if (fHitItemsTofd)
    {
        fHitItemsTofd->Clear();
    }
}

void R3BCalifavsTofDOnlineSpectra::FinishTask()
{
    // Write canvas for Hit data
    if (fHitItemsCalifa)
    {
        fh2_Califa_theta_phi[0]->Write();
        if (fHitItemsTofd)
            fh2_Califa_theta_phi[1]->Write();
    }
}

ClassImp(R3BCalifavsTofDOnlineSpectra);
