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
#include "R3BRpcStripPreCalData.h"
#include "R3BRpcStripCalData.h"
#include "R3BRpcPmtPreCalData.h"
#include "R3BRpcStripHitData.h"





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

    if (fStripPreCalDataItems)
       delete fStripPreCalDataItems;

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

    fStripPreCalDataItems = (TClonesArray*)mgr->GetObject("R3BRpcStripPreCalData");
    if (!fStripPreCalDataItems)
    {
        R3BLOG(FATAL, "R3BRpcStripPreCalData not found");
        return kFATAL;
    }

    fPmtPreCalItems = (TClonesArray*)mgr->GetObject("R3BRpcPmtPreCalData");
    if (!fPmtPreCalItems)
    {
        R3BLOG(FATAL, "R3BRpcPmtPreCalData not found");
        return kFATAL;
    }


    fStripCalDataItems = (TClonesArray*)mgr->GetObject("R3BRpcStripCalData");
    if (!fStripCalDataItems)
    {
        R3BLOG(FATAL, "R3BRpcStripCalData not found");
        return kFATAL;
    }


    fStripHitDataItems = (TClonesArray*)mgr->GetObject("RpcStripHitData");
    if (!fStripHitDataItems)
    {
        R3BLOG(FATAL, "RpcStripHitData not found");
        return kFATAL;
    }



    char name[100];
    /*
      6 Pmt's
      41 Strips per side
      2 Sides
      4 Ref's
    */

    /* ------------- FOLDERS --------------*/

    TFolder* mainfol = new TFolder("Rpc", "RPC info");

    // Folder for mapped data
    TFolder* mapfol = new TFolder("Map", "Map RPC info");
    mainfol->Add(mapfol);

    // Folder for PreCal Data
    TFolder* preCalFolder = new TFolder("Pre Cal", "Pre Cal RPC info");
    mainfol->Add(preCalFolder);

    // Folder for Cal Data
    TFolder* calFolder = new TFolder("Cal", "Cal RPC info");
    mainfol->Add(calFolder);

    // Folder for Hit Data
    TFolder* hitFolder = new TFolder("Hit", "Hit RPC info");
    mainfol->Add(hitFolder);



    TFolder* stripLeftFolder = new TFolder("Strip Map : Left", "Strip Map : Left");
    mapfol->Add(stripLeftFolder);

    TFolder* stripRightFolder = new TFolder("Strip Map : Right", "Strip Map : Right");
    mapfol->Add(stripRightFolder);

    TFolder* pmtFolder = new TFolder("Pmt Map", "Pmt Map");
    mapfol->Add(pmtFolder);

    TFolder* refFolder = new TFolder("Ref Map", "Ref Map");
    mapfol->Add(refFolder);


    TFolder* stripLeftPreCalFolder = new TFolder("Strip Pre Cal : Left", "Strip Pre Cal : Left");
    preCalFolder->Add(stripLeftPreCalFolder);

    TFolder* pmtPreCalFolder = new TFolder("Pmt Pre Cal", "Pmt Pre Cal");
    preCalFolder->Add(pmtPreCalFolder);


    TFolder* stripRightPreCalFolder = new TFolder("Strip Pre Cal : Right", "Strip Pre Cal : Right");
    preCalFolder->Add(stripRightPreCalFolder);

    TFolder* stripCalFolder = new TFolder("Strip Cal", "Strip Cal");
    calFolder->Add(stripCalFolder);

    TFolder* pmtCalFolder = new TFolder("Pmt Cal", "Pmt Cal");
    calFolder->Add(pmtCalFolder);




    /* ----------- CANVASES ------------- */

    rightStripCanvasCoarse = new TCanvas("Right Coarse Canvas","rightStripCanvasCoarse");
    leftStripCanvasCoarse = new TCanvas("Left Coarse Canvas","leftStripCanvasCoarse");

    rightStripCanvasFine = new TCanvas("Right Fine Canvas","rightStripCanvasFine");
    leftStripCanvasFine = new TCanvas("Left Fine Canvas","leftStripCanvasFine");

    rightStripCanvasFine->Divide(9,5);
    leftStripCanvasFine->Divide(9,5);

    rightStripCanvasCoarse->Divide(9,5);
    leftStripCanvasCoarse->Divide(9,5);

    pmtCoarseCanvas = new TCanvas("Pmt Coarse Canvas","pmtCoarseCanvas");
    pmtCoarseCanvas->Divide(3,2);

    pmtFineCanvas = new TCanvas("Pmt Fine Canvas","pmtFineCanvas");
    pmtFineCanvas->Divide(3,2);

    refFineCanvas = new TCanvas("Ref Fine Canvas","refFineCanvas");
    refFineCanvas->Divide(2,2);

    refCoarseCanvas = new TCanvas("Ref Coarse Canvas","refCoarseCanvas");
    refCoarseCanvas->Divide(2,2);



    stripCoarseLeftCorrCanvas  =  new TCanvas("Left : Strip Vs Coarse","stripCoarseLeftCorrCanvas");
    stripCoarseRightCorrCanvas =  new TCanvas("Right : Strip Vs Coarse","stripCoarseRightCorrCanvas");

    stripFineLeftCorrCanvas    =  new TCanvas("Left : Strip Vs Fine","stripFineLeftCorrCanvas");
    stripFineRightCorrCanvas   =  new TCanvas("Right : Strip Vs Fine","stripFineRightCorrCanvas");


    /* ----- Pre Cal Canvases ----- */
    stripLeftTotCorrCanvas     = new TCanvas("Left : Strip Vs Tot","stripVsTot");
    stripRightTotCorrCanvas    = new TCanvas("Right : Strip Vs Tot","stripVsTot");
    stripLeftTimeCorrCanvas    = new TCanvas("Left : Strip Vs Time","stripVsTime");
    stripRightTimeCorrCanvas   = new TCanvas("Right : Strip Vs Time","stripVsTime");

    pmtPreCalTimeCanvas        = new TCanvas("Pmt PreCal Time","pmtPreCalTime");
    pmtPreCalTimeCanvas->Divide(3,2);

    pmtPreCalTotCanvas         = new TCanvas("Pmt PreCal ToT Time","pmtPreCalToT");
    pmtPreCalTotCanvas->Divide(3,2);

    /* ----- Cal Canvases ----- */
    stripCalTimeCorrCanvas = new TCanvas("Left Time Vs Right Time","leftTimeVsRightTime");
    stripCalTotCorrCanvas  = new TCanvas("Left ToT Vs Right ToT","leftToTVsRightToT");

    /* ------ Hit Canvases ------*/
    hitMapCanvas = new TCanvas("Hit Map","hitMapCanvas");
    hitMapCanvas->Divide(2,1);


    /* ----- Map Histograms -----*/

     stripCoarseRightHisto = new TH1F*[41];
     stripFineRightHisto = new TH1F*[41];

     stripCoarseLeftHisto = new TH1F*[41];
     stripFineLeftHisto = new TH1F*[41];

     pmtCoarseHisto = new TH1F*[6];
     pmtFineHisto = new TH1F*[6];

     refCoarseHisto = new TH1F*[4];
     refFineHisto = new TH1F*[4];

     stripCoarseLeftCorr = new TH2F("stripCoarseLeft"," Strip Vs Coarse Time: Left",42,-0.5,41.5,400,0,2500);
     stripCoarseRightCorr = new TH2F("stripCoarseRight"," Strip Vs Coarse Time: Right",42,-0.5,41.5,400,0,2500);
     stripFineLeftCorr = new TH2F("stripFineLeft"," Strip Vs Fine Time: Left",42,-0.5,41.5,400,0,600);
     stripFineRightCorr = new TH2F("stripFineRight"," Strip Vs Fine Time: Right",42,-0.5,41.5,400,0,600);

     /* ----- Pre Cal Histograms ----- */
     pmtPreCalTimeHisto = new TH1F*[6];
     pmtPreCalTotHisto  = new TH1F*[6];

     stripLeftTotCorr = new TH2F("stripLeftTotCorr","Strip Vs Tot : Left",42,-0.5,41.5,400,0,500);
     stripRightTotCorr = new TH2F("stripRightTotCorr","Strip Vs Tot : Right",42,-0.5,41.5,400,0,500);
     stripLeftTimeCorr = new TH2F("stripLeftTimeCorr","Strip Vs Time : Left",42,-0.5,41.5,1000,-550,400);
     stripRightTimeCorr = new TH2F("stripRightTimeCorr","Strip Vs Time : Right",42,-0.5,41.5,1000,-550,400);

     /* ----- Cal Histograms ----- */
     stripCalTimeCorr = new TH2F("stripCalTimeCorr","Strip: Time Left Vs Time Right",800,-550,400,800,-550,400);
     stripCalToTCorr = new TH2F("stripCalToTCorr","Strip: ToT Left Vs ToT Right",400,-100,350,400,-100,350);

     /* ------------- HIT Histograms ------------ */
     stripPosHitCorr = new TH2F("stripPosHitCorr","Strip Vs Position",500,-200,300,42,-0.5,41.5);
     totalChargeHist = new TH1F("totalChargeHist","Charge",1000,-100,400);


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


    for ( Int_t i = 0 ; i < 6; i++){

      sprintf(name, "Coarse Time Pmt_%i",i+1);
      pmtCoarseHisto[i] = new TH1F(name,name,1000,0,3000);

      sprintf(name, "Fine Time Pmt_%i",i+1);
      pmtFineHisto[i] = new TH1F(name,name,1000,0,3000);

      sprintf(name, "Time Pmt_%i",i+1);
      pmtPreCalTimeHisto[i] = new TH1F(name,name,1000,-550,400);

      sprintf(name, "ToT Pmt_%i",i+1);
      pmtPreCalTotHisto[i] = new TH1F(name,name,1000,-550,400);

    }

    for ( Int_t i = 0 ; i < 4; i++){

      sprintf(name, "Coarse Time Ref_%i",i+1);
      refCoarseHisto[i] = new TH1F(name,name,1000,0,3000);

      sprintf(name, "Fine Time Ref_%i",i+1);
      refFineHisto[i] = new TH1F(name,name,1000,0,3000);

    }




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


    stripLeftTotCorr->GetXaxis()->SetTitle("Strip Number");
    stripLeftTotCorr->GetYaxis()->SetTitle("ToT");
    stripLeftTotCorrCanvas->cd();
    stripLeftTotCorr->Draw("COLZ");
    stripLeftPreCalFolder->Add(stripLeftTotCorrCanvas);

    stripRightTotCorr->GetXaxis()->SetTitle("Strip Number");
    stripRightTotCorr->GetYaxis()->SetTitle("ToT");
    stripRightTotCorrCanvas->cd();
    stripRightTotCorr->Draw("COLZ");
    stripRightPreCalFolder->Add(stripRightTotCorrCanvas);


    stripLeftTimeCorr->GetXaxis()->SetTitle("Strip Number");
    stripLeftTimeCorr->GetYaxis()->SetTitle("Time");
    stripLeftTimeCorrCanvas->cd();
    stripLeftTimeCorr->Draw("COLZ");
    stripLeftPreCalFolder->Add(stripLeftTimeCorrCanvas);

    stripRightTimeCorr->GetXaxis()->SetTitle("Strip Number");
    stripRightTimeCorr->GetYaxis()->SetTitle("Time");
    stripRightTimeCorrCanvas->cd();
    stripRightTimeCorr->Draw("COLZ");
    stripRightPreCalFolder->Add(stripRightTimeCorrCanvas);

    stripCalTimeCorr->GetXaxis()->SetTitle("Left Time");
    stripCalTimeCorr->GetYaxis()->SetTitle("Right Time");
    stripCalTimeCorrCanvas->cd();
    stripCalTimeCorr->Draw("COLZ");
    stripCalFolder->Add(stripCalTimeCorrCanvas);

    stripCalToTCorr->GetXaxis()->SetTitle("Left ToT");
    stripCalToTCorr->GetYaxis()->SetTitle("Right ToT");
    stripCalTotCorrCanvas->cd();
    stripCalToTCorr->Draw("COLZ");
    stripCalFolder->Add(stripCalTotCorrCanvas);

    stripPosHitCorr->GetXaxis()->SetTitle("Position");
    stripPosHitCorr->GetYaxis()->SetTitle("Strip");
    hitMapCanvas->cd(1);
    stripPosHitCorr->Draw("COLZ");
    hitFolder->Add(hitMapCanvas);

    totalChargeHist->GetXaxis()->SetTitle("Charge");
    totalChargeHist->GetYaxis()->SetTitle("Counts");
    hitFolder->Add(totalChargeHist);


    for (Int_t i = 0; i < 6; i++) {

       pmtCoarseHisto[i]->GetXaxis()->SetTitle("Coarse Time");
       pmtCoarseHisto[i]->GetYaxis()->SetTitle("Counts");
       pmtCoarseCanvas->cd(i+1);
       pmtCoarseHisto[i]->Draw();

       pmtFineHisto[i]->GetXaxis()->SetTitle("Fine Time");
       pmtFineHisto[i]->GetYaxis()->SetTitle("Counts");
       pmtFineCanvas->cd(i+1);
       pmtFineHisto[i]->Draw();

       pmtPreCalTimeHisto[i]->GetXaxis()->SetTitle("Time");
       pmtPreCalTimeHisto[i]->GetYaxis()->SetTitle("Counts");
       pmtPreCalTimeCanvas->cd(i+1);
       pmtPreCalTimeHisto[i]->Draw();

       pmtPreCalTotHisto[i]->GetXaxis()->SetTitle("ToT");
       pmtPreCalTotHisto[i]->GetYaxis()->SetTitle("Counts");
       pmtPreCalTotCanvas->cd(i+1);
       pmtPreCalTotHisto[i]->Draw();

    }


    for (Int_t i = 0; i < 4; i++) {

       refFineHisto[i]->GetXaxis()->SetTitle("Fine Time");
       refFineHisto[i]->GetYaxis()->SetTitle("Counts");
       refFineCanvas->cd(i+1);
       refFineHisto[i]->Draw();


       refCoarseHisto[i]->GetXaxis()->SetTitle("Coarse Time");
       refCoarseHisto[i]->GetYaxis()->SetTitle("Counts");
       refCoarseCanvas->cd(i+1);
       refCoarseHisto[i]->Draw();
   }

    refFolder->Add(refFineCanvas);
    refFolder->Add(refCoarseCanvas);
    pmtFolder->Add(pmtCoarseCanvas);
    pmtFolder->Add(pmtFineCanvas);

    pmtPreCalFolder->Add(pmtPreCalTimeCanvas);
    pmtPreCalFolder->Add(pmtPreCalTotCanvas);



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

    }


    for (Int_t i = 0; i < 6; i++) {

        pmtFineHisto[i]->Reset();
        pmtCoarseHisto[i]->Reset();
    }


    stripCoarseLeftCorr->Reset();
    stripCoarseRightCorr->Reset();
    stripFineLeftCorr->Reset();
    stripFineRightCorr->Reset();
    stripCoarseLeftCorr->Reset();
    stripCoarseRightCorr->Reset();
    stripFineLeftCorr->Reset();
    stripFineRightCorr->Reset();
    stripCalTimeCorr->Reset();
    stripCalToTCorr->Reset();
    totalChargeHist->Reset();
    stripPosHitCorr->Reset();




    return;
}

void R3BRpcOnlineSpectra::Exec(Option_t* option)
{
    if (fEventHeader->GetTrigger() != fTrigger && fTrigger > -1){
        return;
    }
        auto nStripMappedHits = fStripMappedItems->GetEntriesFast();


        /* ------------------- Map EventLoop ------------------*/
        for (Int_t ihit = 0; ihit < nStripMappedHits; ihit++) {

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



        auto nRefMappedHits = fRefMappedItems->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nRefMappedHits; ihit++) {

            R3BRpcRefMappedData* hit = (R3BRpcRefMappedData*)fRefMappedItems->At(ihit);

            if (!hit)
                continue;

             refFineHisto[hit->GetChannelId() - 1]->Fill(hit->GetFineTime());
             refCoarseHisto[hit->GetChannelId() - 1]->Fill(hit->GetCoarseTime());

        }



        auto nPmtMappedHits = fPmtMappedItems->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nPmtMappedHits; ihit++) {

            R3BRpcPmtMappedData* hit = (R3BRpcPmtMappedData*)fPmtMappedItems->At(ihit);
            if (!hit)
                continue;

             pmtFineHisto[hit->GetChannelId() - 1]->Fill(hit->GetFineTime());
             pmtCoarseHisto[hit->GetChannelId() - 1]->Fill(hit->GetCoarseTime());


    }

    /* ----------------- PreCal EventLoop ------------------*/
    auto nStripPreCalHits = fStripPreCalDataItems->GetEntriesFast();

    for( Int_t ihit = 0; ihit < nStripPreCalHits; ihit++) {

      R3BRpcStripPreCalData* hit = (R3BRpcStripPreCalData*)fStripPreCalDataItems->At(ihit);

         Int_t side = hit->GetSide();

         if(side == 0){
           stripLeftTotCorr->Fill(hit->GetChannelId(),hit->GetTot());
           stripLeftTimeCorr->Fill(hit->GetChannelId(),hit->GetTime());
         }

         if(side == 1){
           stripRightTotCorr->Fill(hit->GetChannelId(),hit->GetTot());
           stripRightTimeCorr->Fill(hit->GetChannelId(),hit->GetTime());

         }

    }

    auto nPmtPreCalHits = fPmtPreCalItems->GetEntriesFast();

    for( Int_t ihit = 0; ihit < nPmtPreCalHits; ihit++) {

      R3BRpcPmtPreCalData* hit = (R3BRpcPmtPreCalData*)fPmtPreCalItems->At(ihit);

        pmtPreCalTimeHisto[hit->GetChannelId()-1]->Fill(hit->GetTime());
        pmtPreCalTotHisto[hit->GetChannelId()-1]->Fill(hit->GetTot());

    }




    /* -------------------- Cal EventLoop ---------------------*/
    auto nStripCalHits = fStripCalDataItems->GetEntriesFast();

    for( Int_t ihit = 0; ihit < nStripCalHits; ihit++) {

     R3BRpcStripCalData* hit = (R3BRpcStripCalData*)fStripCalDataItems->At(ihit);

     stripCalTimeCorr->Fill(hit->GetTimeLeft(),hit->GetTimeRight());
     stripCalToTCorr->Fill(hit->GetTotLeft(),hit->GetTotRight());




    }

    /*---------------- HIT EventLoop ---------------*/
    auto nStripHits = fStripHitDataItems->GetEntriesFast();

    for( Int_t ihit = 0; ihit < nStripHits; ihit++) {

     R3BRpcStripHitData* hit = (R3BRpcStripHitData*)fStripHitDataItems->At(ihit);

     stripPosHitCorr->Fill(hit->GetChannelId(),hit->GetPos());
     totalChargeHist->Fill(hit->GetCharge());

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

    if(fStripPreCalDataItems)
    {
        fStripPreCalDataItems->Clear();
    }


    if(fStripCalDataItems)
    {
        fStripCalDataItems->Clear();
    }

    if(fPmtPreCalItems)
    {
        fPmtPreCalItems->Clear();
    }

    if(fStripHitDataItems)
    {
        fStripHitDataItems->Clear();
    }



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

   leftStripCanvasCoarse->Write();
   rightStripCanvasCoarse->Write();

   leftStripCanvasFine->Write();
   rightStripCanvasFine->Write();

   stripCoarseLeftCorrCanvas->Write();
   stripCoarseRightCorrCanvas->Write();

   stripFineLeftCorrCanvas->Write();
   stripFineRightCorrCanvas->Write();

   pmtCoarseCanvas->Write();
   pmtFineCanvas->Write();
   refFineCanvas->Write();
   refCoarseCanvas->Write();

   stripLeftTotCorrCanvas->Write();
   stripRightTotCorrCanvas->Write();
   stripLeftTimeCorrCanvas->Write();
   stripRightTimeCorrCanvas->Write();




 }
}

ClassImp(R3BRpcOnlineSpectra);
