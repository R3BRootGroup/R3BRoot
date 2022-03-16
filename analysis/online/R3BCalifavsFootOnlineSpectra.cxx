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

#include "R3BCalifavsFootOnlineSpectra.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaHitData.h"
#include "R3BEventHeader.h"
#include "R3BFootCalData.h"
#include "R3BFootHitData.h"
#include "R3BFootMappedData.h"
#include "R3BLogger.h"
#include "R3BWRData.h"

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
#include "THttpServer.h"
#include "TVector3.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"
#include <array>

using namespace std;

R3BCalifavsFootOnlineSpectra::R3BCalifavsFootOnlineSpectra()
    : R3BCalifavsFootOnlineSpectra("CALIFAvsFootOnlineSpectra", 1)
{
}

R3BCalifavsFootOnlineSpectra::R3BCalifavsFootOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsCalifa(NULL)
    , fHitItemsCalifa(NULL)
    , fMappedItemsFoot(NULL)
    , fCalItemsFoot(NULL)
    , fHitItemsFoot(NULL)
    , fNEvents(0)
    , fTpat(-1)
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
    , fMinProtonE(50000.)
    , fRaw2Cal(kFALSE)
    , fLogScale(kTRUE)
    , fTotHist(kFALSE)
{
}

R3BCalifavsFootOnlineSpectra::~R3BCalifavsFootOnlineSpectra()
{
    R3BLOG(DEBUG1, "");
    if (fHitItemsCalifa)
        delete fHitItemsCalifa;
    if (fMappedItemsFoot)
        delete fMappedItemsFoot;
}

void R3BCalifavsFootOnlineSpectra::SetParContainers()
{
    // Parameter Container
    // Reading amsStripCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(FATAL, !rtdb, "FairRuntimeDb not found");
}

InitStatus R3BCalifavsFootOnlineSpectra::Init()
{
    R3BLOG(INFO, "");
    FairRootManager* mgr = FairRootManager::Instance();

    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");

    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // get access to Cal data
    fCalItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaCrystalCalData");
    R3BLOG_IF(WARNING, !fCalItemsCalifa, "CalifaCrystalCalData not found");

    // get access to Hit data
    fHitItemsCalifa = (TClonesArray*)mgr->GetObject("CalifaHitData");
    R3BLOG_IF(WARNING, !fHitItemsCalifa, "CalifaHitData not found");

    fMappedItemsFoot = (TClonesArray*)mgr->GetObject("FootMappedData");
    R3BLOG_IF(WARNING, !fMappedItemsFoot, "FootMappedData not found");

    fCalItemsFoot = (TClonesArray*)mgr->GetObject("FootCalData");
    R3BLOG_IF(WARNING, !fCalItemsFoot, "FootCalData not found");

    fHitItemsFoot = (TClonesArray*)mgr->GetObject("FootHitData");
    R3BLOG_IF(WARNING, !fHitItemsFoot, "FootHitData not found");

    // Create histograms for detectors
    char Name1[255];
    char Name2[255];
    char Name3[255];

    // CANVAS Theta vs Phi
    auto cCalifa_angles = new TCanvas("Califa_Theta_vs_Phi_withFoot", "Theta vs Phi", 10, 10, 500, 500);
    fh2_Califa_theta_phi =
        new TH2F("fh2_Califa_theta_vs_phi_withFoot", "Califa theta vs phi with Foot", 50, 0, 90, 180, -180, 180);
    fh2_Califa_theta_phi->GetXaxis()->SetTitle("Theta [degrees]");
    fh2_Califa_theta_phi->GetYaxis()->SetTitle("Phi [degrees]");
    fh2_Califa_theta_phi->GetYaxis()->SetTitleOffset(1.2);
    fh2_Califa_theta_phi->GetXaxis()->CenterTitle(true);
    fh2_Califa_theta_phi->GetYaxis()->CenterTitle(true);
    fh2_Califa_theta_phi->Draw("COLZ");

    // MAIN FOLDER-Califa
    TFolder* mainfolCalifa = new TFolder("CALIFAvsFoot", "CALIFA vs Foot info");

    if (fHitItemsCalifa && fMappedItemsFoot)
    {
        mainfolCalifa->Add(cCalifa_angles);
    }
    run->AddObject(mainfolCalifa);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_CalifavsFoot",
                                          Form("/Objects/%s/->Reset_CALIFAFOOT_Histo()", GetName()));

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BCalifavsFootOnlineSpectra::ReInit()
{
    SetParContainers();
    // SetParameter();
    return kSUCCESS;
}

void R3BCalifavsFootOnlineSpectra::Reset_CALIFAFOOT_Histo()
{

    LOG(INFO) << "R3BCalifavsFootOnlineSpectra::Reset_CALIFAFOOT_Histo";

    fh2_Califa_theta_phi->Reset();
}

void R3BCalifavsFootOnlineSpectra::Exec(Option_t* option)
{
    if ((fTpat >= 0) && (header) && ((header->GetTpat() & 2) != fTpat))
        return;

    bool ffoot = false;
    if (fMappedItemsFoot->GetEntriesFast() > 0)
    {
        for (Int_t ihit = 0; ihit < fMappedItemsFoot->GetEntriesFast(); ihit++)
        {

            auto hit = (R3BFootMappedData*)fMappedItemsFoot->At(ihit);
            if (!hit)
                continue;
            if (hit->GetDetId() == 11)
                ffoot = true;
        }
    }

    // Hit data
    if (fHitItemsCalifa && fHitItemsCalifa->GetEntriesFast() > 0)
    {
        Int_t nHits = fHitItemsCalifa->GetEntriesFast();

        Double_t theta = 0., phi = 0.;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BCalifaHitData*)fHitItemsCalifa->At(ihit);
            if (!hit)
                continue;
            theta = hit->GetTheta() * TMath::RadToDeg();
            phi = hit->GetPhi() * TMath::RadToDeg();
            if (ffoot)
                fh2_Califa_theta_phi->Fill(theta, phi);
        }
    }

    fNEvents += 1;
}

void R3BCalifavsFootOnlineSpectra::FinishEvent()
{

    if (fCalItemsCalifa)
    {
        fCalItemsCalifa->Clear();
    }
    if (fHitItemsCalifa)
    {
        fHitItemsCalifa->Clear();
    }
    if (fMappedItemsFoot)
    {
        fMappedItemsFoot->Clear();
    }
    if (fCalItemsFoot)
    {
        fCalItemsFoot->Clear();
    }
    if (fHitItemsFoot)
    {
        fHitItemsFoot->Clear();
    }
}

void R3BCalifavsFootOnlineSpectra::FinishTask()
{
    // Write canvas for Hit data
    if (fHitItemsCalifa && fMappedItemsFoot)
    {
        fh2_Califa_theta_phi->Write();
    }
}

ClassImp(R3BCalifavsFootOnlineSpectra);
