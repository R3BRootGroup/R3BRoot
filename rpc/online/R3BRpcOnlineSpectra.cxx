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

// -----------------------------------------------------------
// -----             R3BRpcOnlineSpectra                 -----
// -----   Created 23/01/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill RPC online histograms              -----
// -----------------------------------------------------------

/*
 * This task should fill histograms with RPC online data
 */

#include "R3BRpcOnlineSpectra.h"
#include "R3BLogger.h"
#include "R3BEventHeader.h"
#include "R3BRpcRefMappedData.h"
#include "R3BRpcPmtMappedData.h"
#include "R3BRpcStripMappedData.h"

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
#include "THttpServer.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

R3BRpcOnlineSpectra::R3BRpcOnlineSpectra()
    : R3BRpcOnlineSpectra("RpcOnlineSpectra", 1)
{
}

R3BRpcOnlineSpectra::R3BRpcOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fEventHeader(nullptr)
    , fRefMappedItems(NULL)
    , fPmtMappedItems(NULL)
    , fStripMappedItems(NULL)
    , fTrigger(-1)
    , fNEvents(0)
    , fNbDet(1)
{
}

R3BRpcOnlineSpectra::~R3BRpcOnlineSpectra()
{
    R3BLOG(DEBUG1, "");
    if (fEventHeader)
        delete fEventHeader;

    if (fRefMappedItems)
       delete fRefMappedItems;

    if (fStripMappedItems)
       delete fStripMappedItems;

    if (fPmtMappedItems)
       delete fPmtMappedItems;

}

InitStatus R3BRpcOnlineSpectra::Init()
{
    R3BLOG(INFO, "");

    // Looking for FairRootManager
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    // Look for the R3BEventHeader
    fEventHeader = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!fEventHeader)
    {
        R3BLOG(WARNING, "EventHeader. not found");
        fEventHeader = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    }
    else
        R3BLOG(INFO,"EventHeader. found");

    // Get access to Mapped data
    fRefMappedItems = (TClonesArray*)mgr->GetObject("RpcRefMappedData");
    if (!fRefMappedItems)
    {
        R3BLOG(FATAL, "RpcMappedData not found");
        return kFATAL;
    }

    fPmtMappedItems = (TClonesArray*)mgr->GetObject("RpcPmtMappedData");
    if (!fPmtMappedItems)
    {
        R3BLOG(FATAL, "RpcMappedData not found");
        return kFATAL;
    }


    fStripMappedItems = (TClonesArray*)mgr->GetObject("RpcStripMappedData");
    if (!fStripMappedItems)
    {
        R3BLOG(FATAL, "RpcMappedData not found");
        return kFATAL;
    }

    char name[100];
    /*
      4 Pmt's
      41 Strips per side
      2 Sides
      4 Ref's
    */

     stripCoarseRightHisto = new TH1F*[41];
     stripFineRightHisto = new TH1F*[41];

     stripCoarseLeftHisto = new TH1F*[41];
     stripFineLeftHisto = new TH1F*[41];

     pmtCoarseHisto = new TH1F*[4];
     pmtFineHisto = new TH1F*[4];

     refCoarseHisto = new TH1F*[4];
     refFineHisto = new TH1F*[4];


    for ( Int_t i = 0 ; i < 41; i++){

      sprintf(name, "Left : Coarse Time Strip_%i",i+1);
      stripCoarseLeftHisto[i] = new TH1F(name,name,1000,0,2E12);

      sprintf(name, "Left : Fine Time Strip_%i",i+1);
      stripFineLeftHisto[i] = new TH1F(name,name,1000,0,2E12);

      sprintf(name, "Right : Coarse Time Strip_%i",i+1);
      stripCoarseRightHisto[i] = new TH1F(name,name,1000,0,2E12);

      sprintf(name, "Right : Fine Time Strip_%i",i+1);
      stripFineRightHisto[i] = new TH1F(name,name,1000,0,2E12);

    }


    for ( Int_t i = 0 ; i < 4; i++){

      sprintf(name, "Coarse Time Pmt_%i",i+1);
      pmtCoarseHisto[i] = new TH1F(name,name,1000,0,2E12);

      sprintf(name, "Fine Time Pmt_%i",i+1);
      pmtFineHisto[i] = new TH1F(name,name,1000,0,2E12);

      sprintf(name, "Coarse Time Ref_%i",i+1);
      refCoarseHisto[i] = new TH1F(name,name,1000,0,2E12);

      sprintf(name, "Fine Time Ref_%i",i+1);
      refFineHisto[i] = new TH1F(name,name,1000,0,2E12);

    }






    // MAIN FOLDER-RPC
    TFolder* mainfol = new TFolder("Rpc", "RPC info");
    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map RPC info");
    mainfol->Add(mapfol);

    // // Mapped data
    // fh2_EnergyVsStrip.resize(fNbDet);
    // for (Int_t i = 0; i < fNbDet; i++)
    // { // one histo per detector
    //     sprintf(Name1, "fh2_energy_vs_strip_rpc_%d", i + 1);
    //     sprintf(Name2, "Mapped energy vs strip number for RPC Det: %d", i + 1);
    //     fh2_EnergyVsStrip[i] = new TH2F(Name1, Name2, 640, 1, 641, binsE, minE, maxE);
    //     fh2_EnergyVsStrip[i]->GetXaxis()->SetTitle("Strip number");
    //     fh2_EnergyVsStrip[i]->GetYaxis()->SetTitle("Energy [channels]");
    //     fh2_EnergyVsStrip[i]->GetYaxis()->SetTitleOffset(1.4);
    //     fh2_EnergyVsStrip[i]->GetXaxis()->CenterTitle(true);
    //     fh2_EnergyVsStrip[i]->GetYaxis()->CenterTitle(true);
    //     fh2_EnergyVsStrip[i]->Draw("col");
    //     mapfol->Add(fh2_EnergyVsStrip[i]);
    // }
    //
    // // Cal data
    // if (fCalItems)
    // {
    //     fh2_EnergyVsStrip_cal.resize(fNbDet);
    //     for (Int_t i = 0; i < fNbDet; i++)
    //     { // one histo per detector
    //         sprintf(Name1, "fh2_energy_vs_strip_cal_rpc_%d", i + 1);
    //         sprintf(Name2, "Cal-energy vs strip number for RPC Det: %d", i + 1);
    //         fh2_EnergyVsStrip_cal[i] = new TH2F(Name1, Name2, 640, 1, 641, binsE, minE, maxE);
    //         fh2_EnergyVsStrip_cal[i]->GetXaxis()->SetTitle("Strip number");
    //         fh2_EnergyVsStrip_cal[i]->GetYaxis()->SetTitle("Energy [channels]");
    //         fh2_EnergyVsStrip_cal[i]->GetYaxis()->SetTitleOffset(1.4);
    //         fh2_EnergyVsStrip_cal[i]->GetXaxis()->CenterTitle(true);
    //         fh2_EnergyVsStrip_cal[i]->GetYaxis()->CenterTitle(true);
    //         fh2_EnergyVsStrip_cal[i]->Draw("col");
    //         calfol->Add(fh2_EnergyVsStrip_cal[i]);
    //     }
    // }
    //
    // // Hit data
    // if (fHitItems)
    // {
    //     fh1_pos.resize(fNbDet);
    //     for (Int_t i = 0; i < fNbDet; i++)
    //     { // one histo per detector
    //         sprintf(Name1, "fh1_pos_rpc_%d", i + 1);
    //         sprintf(Name2, "Cluster position for RPC Det: %d", i + 1);
    //         fh1_pos[i] = new TH1F(Name1, Name2, 600, -50., 50.);
    //         fh1_pos[i]->GetXaxis()->SetTitle("Position [mm]");
    //         fh1_pos[i]->GetYaxis()->SetTitle("Counts");
    //         fh1_pos[i]->GetYaxis()->SetTitleOffset(1.4);
    //         fh1_pos[i]->GetXaxis()->CenterTitle(true);
    //         fh1_pos[i]->GetYaxis()->CenterTitle(true);
    //         fh1_pos[i]->Draw("");
    //         hitfol->Add(fh1_pos[i]);
    //     }
    // }


    // Looking for FairRunOnline
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_RPC", Form("/Objects/%s/->Reset_RPC_Histo()", GetName()));
    return kSUCCESS;
}

void R3BRpcOnlineSpectra::Reset_RPC_Histo()
{
    R3BLOG(INFO, "");

    // Mapped data
    for (Int_t i = 0; i < 44; i++)
    {
        stripCoarseLeftHisto[i]->Reset();
        stripFineLeftHisto[i]->Reset();
        stripCoarseRightHisto[i]->Reset();
        stripFineRightHisto[i]->Reset();
    }

    for (Int_t i = 0; i < 4; i++)
    {
        refFineHisto[i]->Reset();
        refCoarseHisto[i]->Reset();
        pmtFineHisto[i]->Reset();
        pmtCoarseHisto[i]->Reset();
    }

    return;
}

void R3BRpcOnlineSpectra::Exec(Option_t* option)
{
    if (fEventHeader->GetTrigger() != fTrigger && fTrigger > -1)
        return;
        
    // Fill mapped data

    if (fStripMappedItems && fStripMappedItems->GetEntriesFast() > 0)
    {
        auto nHits = fStripMappedItems->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRpcStripMappedData* hit = (R3BRpcStripMappedData*)fStripMappedItems->At(ihit);
            if (!hit)
                continue;
            Int_t side = hit->GetSide();

            if(side == 0){

             stripFineLeftHisto[hit->GetStripId() - 1]->Fill(hit->GetFineTime());
             stripCoarseLeftHisto[hit->GetStripId() - 1]->Fill(hit->GetCoarseTime());

           }

           if(side == 1){

            stripFineRightHisto[hit->GetStripId() - 1]->Fill(hit->GetFineTime());
            stripCoarseRightHisto[hit->GetStripId() - 1]->Fill(hit->GetCoarseTime());

          }
        }
    }



    if (fRefMappedItems && fRefMappedItems->GetEntriesFast() > 0)
    {
        auto nHits = fRefMappedItems->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRpcRefMappedData* hit = (R3BRpcRefMappedData*)fRefMappedItems->At(ihit);
            if (!hit)
                continue;

             refFineHisto[hit->GetChannelId() - 1]->Fill(hit->GetFineTime());
             refCoarseHisto[hit->GetChannelId() - 1]->Fill(hit->GetCoarseTime());

        }
    }


    if (fPmtMappedItems && fPmtMappedItems->GetEntriesFast() > 0)
    {
        auto nHits = fPmtMappedItems->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRpcPmtMappedData* hit = (R3BRpcPmtMappedData*)fPmtMappedItems->At(ihit);
            if (!hit)
                continue;

             pmtFineHisto[hit->GetChannelId() - 1]->Fill(hit->GetFineTime());
             pmtCoarseHisto[hit->GetChannelId() - 1]->Fill(hit->GetCoarseTime());

        }
    }


    fNEvents += 1;
    return;
}

void R3BRpcOnlineSpectra::FinishEvent()
{
    if (fRefMappedItems)
    {
        fRefMappedItems->Clear();
    }


    if (fPmtMappedItems)
    {
        fPmtMappedItems->Clear();
    }


    if (fStripMappedItems)
    {
        fStripMappedItems->Clear();
    }


    return;
}

void R3BRpcOnlineSpectra::FinishTask()
{
    if (fRefMappedItems)
    {
        for (Int_t i = 0; i < 4; i++){
            refFineHisto[i]->Write();
            refCoarseHisto[i]->Write();
     }
   }

   if (fPmtMappedItems)
   {
       for (Int_t i = 0; i < 4; i++){
           pmtFineHisto[i]->Write();
           pmtCoarseHisto[i]->Write();
    }
  }


  if (fStripMappedItems)
  {
      for (Int_t i = 0; i < 4; i++){

          stripFineLeftHisto[i]->Write();
          stripFineRightHisto[i]->Write();

          stripCoarseLeftHisto[i]->Write();
          stripCoarseRightHisto[i]->Write();
   }
 }

    return;
}

ClassImp(R3BRpcOnlineSpectra);
