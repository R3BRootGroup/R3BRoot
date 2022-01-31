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
// -----      Modified 24/01/22 by G.García-Jiménez
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

     stripCoarseLeftCorr = new TH2F("stripCoarseLeft"," Strip Vs Coarse Time: Left",42,-0.5,41.5,400,0,2500);
     stripCoarseRightCorr = new TH2F("stripCoarseRight"," Strip Vs Coarse Time: Right",42,-0.5,41.5,400,0,2500);
     stripFineLeftCorr = new TH2F("stripFineLeft"," Strip Vs Fine Time: Left",42,-0.5,41.5,400,0,600);
     stripFineRightCorr = new TH2F("stripFineRight"," Strip Vs Fine Time: Right",42,-0.5,41.5,400,0,600);


    for ( Int_t i = 0 ; i < 41; i++){

      sprintf(name, "Left : Coarse Time Strip_%i",i+1);
      stripCoarseLeftHisto[i] = new TH1F(name,name,200,0,2200);

      sprintf(name, "Left : Fine Time Strip_%i",i+1);
      stripFineLeftHisto[i] = new TH1F(name,name,200,0,600);

      sprintf(name, "Right : Coarse Time Strip_%i",i+1);
      stripCoarseRightHisto[i] = new TH1F(name,name,200,0,2200);

      sprintf(name, "Right : Fine Time Strip_%i",i+1);
      stripFineRightHisto[i] = new TH1F(name,name,200,0,600);

    }


    for ( Int_t i = 0 ; i < 4; i++){

      sprintf(name, "Coarse Time Pmt_%i",i+1);
      pmtCoarseHisto[i] = new TH1F(name,name,1000,0,3000);

      sprintf(name, "Fine Time Pmt_%i",i+1);
      pmtFineHisto[i] = new TH1F(name,name,1000,0,3000);

      sprintf(name, "Coarse Time Ref_%i",i+1);
      refCoarseHisto[i] = new TH1F(name,name,1000,0,3000);

      sprintf(name, "Fine Time Ref_%i",i+1);
      refFineHisto[i] = new TH1F(name,name,1000,0,3000);

    }



    rightStripCanvasCoarse = new TCanvas("Right Coarse Canvas","rightStripCanvasCoarse");
    leftStripCanvasCoarse = new TCanvas("Left Coarse Canvas","leftStripCanvasCoarse");

    rightStripCanvasFine = new TCanvas("Right Fine Canvas","rightStripCanvasFine");
    leftStripCanvasFine = new TCanvas("Left Fine Canvas","leftStripCanvasFine");

    rightStripCanvasFine->Divide(9,5);
    leftStripCanvasFine->Divide(9,5);

    rightStripCanvasCoarse->Divide(9,5);
    leftStripCanvasCoarse->Divide(9,5);


    stripCoarseLeftCorrCanvas  =  new TCanvas("Left : Strip Vs Coarse","stripCoarseLeftCorrCanvas");
    stripCoarseRightCorrCanvas =  new TCanvas("Right : Strip Vs Coarse","stripCoarseRightCorrCanvas");

    stripFineLeftCorrCanvas    =  new TCanvas("Left : Strip Vs Fine","stripFineLeftCorrCanvas");
    stripFineRightCorrCanvas   =  new TCanvas("Right : Strip Vs Fine","stripFineRightCorrCanvas");



    // MAIN FOLDER-RPC

    TFolder* mainfol = new TFolder("Rpc", "RPC info");

    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map RPC info");
    mainfol->Add(mapfol);

    TFolder* stripLeftFolder = new TFolder("Strip Map : Left", "Strip Map : Left");
    mapfol->Add(stripLeftFolder);

    TFolder* stripRightFolder = new TFolder("Strip Map : Right", "Strip Map : Right");
    mapfol->Add(stripRightFolder);

    TFolder* pmtFolder = new TFolder("Pmt Map", "Pmt Map");
    mapfol->Add(pmtFolder);

    TFolder* refFolder = new TFolder("Ref Map", "Ref Map");
    mapfol->Add(refFolder);

     for (Int_t i = 0; i < 41; i++) {

        stripCoarseLeftHisto[i]->GetXaxis()->SetTitle("Coarse Time");
        stripCoarseLeftHisto[i]->GetYaxis()->SetTitle("Counts");
        stripCoarseLeftHisto[i]->GetXaxis()->SetTitleOffset(1.1);
        stripCoarseLeftHisto[i]->GetYaxis()->SetTitleOffset(1.1);
        stripCoarseLeftHisto[i]->GetXaxis()->CenterTitle(true);
        stripCoarseLeftHisto[i]->GetYaxis()->CenterTitle(true);

        leftStripCanvasCoarse->cd(i+1);
        stripCoarseLeftHisto[i]->Draw();


        stripFineRightHisto[i]->GetXaxis()->SetTitle("Fine Time");
        stripFineRightHisto[i]->GetYaxis()->SetTitle("Counts");
        stripFineRightHisto[i]->GetXaxis()->SetTitleOffset(1.1);
        stripFineRightHisto[i]->GetYaxis()->SetTitleOffset(1.1);
        stripFineRightHisto[i]->GetXaxis()->CenterTitle(true);
        stripFineRightHisto[i]->GetYaxis()->CenterTitle(true);

        rightStripCanvasFine->cd(i+1);
        stripFineRightHisto[i]->Draw();


        stripFineLeftHisto[i]->GetXaxis()->SetTitle("Fine Time");
        stripFineLeftHisto[i]->GetYaxis()->SetTitle("Counts");
        stripFineLeftHisto[i]->GetXaxis()->SetTitleOffset(1.1);
        stripFineLeftHisto[i]->GetYaxis()->SetTitleOffset(1.1);
        stripFineLeftHisto[i]->GetXaxis()->CenterTitle(true);
        stripFineLeftHisto[i]->GetYaxis()->CenterTitle(true);

        leftStripCanvasFine->cd(i+1);
        stripFineLeftHisto[i]->Draw();

        stripCoarseRightHisto[i]->GetXaxis()->SetTitle("Coarse Time");
        stripCoarseRightHisto[i]->GetYaxis()->SetTitle("Counts");
        stripCoarseRightHisto[i]->GetXaxis()->SetTitleOffset(1.1);
        stripCoarseRightHisto[i]->GetYaxis()->SetTitleOffset(1.1);
        stripCoarseRightHisto[i]->GetXaxis()->CenterTitle(true);
        stripCoarseRightHisto[i]->GetYaxis()->CenterTitle(true);

        rightStripCanvasCoarse->cd(i+1);
        stripCoarseRightHisto[i]->Draw();


    }

    stripLeftFolder->Add(leftStripCanvasCoarse);
    stripLeftFolder->Add(leftStripCanvasFine);


    stripRightFolder->Add(rightStripCanvasCoarse);
    stripRightFolder->Add(rightStripCanvasFine);


    for (Int_t i = 0; i < 4; i++) {

       pmtCoarseHisto[i]->GetXaxis()->SetTitle("Coarse Time");
       pmtCoarseHisto[i]->GetYaxis()->SetTitle("Counts");
       pmtCoarseHisto[i]->Draw();
       pmtFolder->Add(pmtCoarseHisto[i]);

       pmtFineHisto[i]->GetXaxis()->SetTitle("Fine Time");
       pmtFineHisto[i]->GetYaxis()->SetTitle("Counts");
       pmtFineHisto[i]->Draw();
       pmtFolder->Add(pmtFineHisto[i]);

       refFineHisto[i]->GetXaxis()->SetTitle("Fine Time");
       refFineHisto[i]->GetYaxis()->SetTitle("Counts");
       refFineHisto[i]->Draw();
       refFolder->Add(refFineHisto[i]);


       refCoarseHisto[i]->GetXaxis()->SetTitle("Coarse Time");
       refCoarseHisto[i]->GetYaxis()->SetTitle("Counts");
       refCoarseHisto[i]->Draw();
       refFolder->Add(refCoarseHisto[i]);

   }

    stripCoarseRightCorr->GetXaxis()->SetTitle("Strip Number");
    stripCoarseRightCorr->GetYaxis()->SetTitle("Coarse Time");
    stripCoarseRightCorrCanvas->cd();
    stripCoarseRightCorr->Draw("COLZ");
    stripRightFolder->Add(stripCoarseRightCorrCanvas);

    stripCoarseLeftCorr->GetXaxis()->SetTitle("Strip Number");
    stripCoarseLeftCorr->GetYaxis()->SetTitle("Coarse Time");
    stripCoarseLeftCorrCanvas->cd();
    stripCoarseLeftCorr->Draw("COLZ");
    stripLeftFolder->Add(stripCoarseLeftCorrCanvas);

    stripFineRightCorr->GetXaxis()->SetTitle("Strip Number");
    stripFineRightCorr->GetYaxis()->SetTitle("Fine Time");
    stripFineRightCorrCanvas->cd();
    stripFineRightCorr->Draw("COLZ");
    stripRightFolder->Add(stripFineRightCorrCanvas);

    stripFineLeftCorr->GetXaxis()->SetTitle("Strip Number");
    stripFineLeftCorr->GetYaxis()->SetTitle("Fine Time");
    stripFineLeftCorrCanvas->cd();
    stripFineLeftCorr->Draw("COLZ");
    stripLeftFolder->Add(stripFineLeftCorrCanvas);


    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);


    run->GetHttpServer()->RegisterCommand("Reset_RPC", Form("/Objects/%s/->Reset_RPC_Histo()", GetName()));
    return kSUCCESS;
}

void R3BRpcOnlineSpectra::Reset_RPC_Histo()
{
    R3BLOG(INFO, "");

    // Mapped data
    for (Int_t i = 0; i < 41; i++)
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

    stripCoarseRightCorr->Reset();
    stripCoarseLeftCorr->Reset();

    stripFineRightCorr->Reset();
    stripFineLeftCorr->Reset();

    return;
}

void R3BRpcOnlineSpectra::Exec(Option_t* option)
{
    if (fEventHeader->GetTrigger() != fTrigger && fTrigger > -1)
        return;

    // // Fill mapped data

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
             stripCoarseLeftCorr->Fill(hit->GetStripId(),hit->GetCoarseTime());
             stripFineLeftCorr->Fill(hit->GetStripId(),hit->GetFineTime());

           }

           if(side == 1){

            stripFineRightHisto[hit->GetStripId() - 1]->Fill(hit->GetFineTime());
            stripCoarseRightHisto[hit->GetStripId() - 1]->Fill(hit->GetCoarseTime());
            stripCoarseRightCorr->Fill(hit->GetStripId(),hit->GetCoarseTime());
            stripFineRightCorr->Fill(hit->GetStripId(),hit->GetFineTime());


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
      for (Int_t i = 0; i < 41; i++){

          stripFineLeftHisto[i]->Write();
          stripFineRightHisto[i]->Write();

          stripCoarseLeftHisto[i]->Write();
          stripCoarseRightHisto[i]->Write();
   }

   stripCoarseLeftCorr->Write();
   stripCoarseRightCorr->Write();

   stripFineLeftCorr->Write();
   stripFineRightCorr->Write();

   rightStripCanvasCoarse->Write();
   rightStripCanvasFine->Write();

   leftStripCanvasCoarse->Write();
   leftStripCanvasFine->Write();

   stripCoarseLeftCorrCanvas->Write();
   stripCoarseRightCorrCanvas->Write();
   stripFineLeftCorrCanvas->Write();
   stripFineRightCorrCanvas->Write();
 }

    return;
}

ClassImp(R3BRpcOnlineSpectra);
