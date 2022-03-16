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
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BRpcPmtMappedData.h"
#include "R3BRpcPmtPreCalData.h"
#include "R3BRpcRefMappedData.h"
#include "R3BRpcStripCalData.h"
#include "R3BRpcStripHitData.h"
#include "R3BRpcStripMappedData.h"
#include "R3BRpcStripPreCalData.h"

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
    , fLosTriggerCalDataItems(NULL)
    , fLosCalDataItems(NULL)
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

    if (fLosTriggerCalDataItems)
        delete fLosTriggerCalDataItems;

    if (fLosCalDataItems)
        delete fLosCalDataItems;
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
        R3BLOG(INFO, "EventHeader. found");

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

    fStripHitDataItems = (TClonesArray*)mgr->GetObject("R3BRpcStripHitData");
    if (!fStripHitDataItems)
    {
        R3BLOG(FATAL, "RpcStripHitData not found");
        return kFATAL;
    }

    fLosCalDataItems = (TClonesArray*)mgr->GetObject("LosCal");
    if (!fLosCalDataItems)
    {
        R3BLOG(FATAL, "LOS Hit Data not found");
        return kFATAL;
    }

    fLosTriggerCalDataItems = (TClonesArray*)mgr->GetObject("LosTriggerCal");
    if (!fLosTriggerCalDataItems)
    {
        R3BLOG(FATAL, "LOS Data not found");
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

    TFolder* losCorrFolder = new TFolder("LOS Correlations", "LOS Correlations");
    mainfol->Add(losCorrFolder);

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

    rightStripCanvasCoarse = new TCanvas("Right Coarse Canvas", "rightStripCanvasCoarse");
    leftStripCanvasCoarse = new TCanvas("Left Coarse Canvas", "leftStripCanvasCoarse");

    rightStripCanvasFine = new TCanvas("Right Fine Canvas", "rightStripCanvasFine");
    leftStripCanvasFine = new TCanvas("Left Fine Canvas", "leftStripCanvasFine");

    rightStripCanvasFine->Divide(9, 5);
    leftStripCanvasFine->Divide(9, 5);

    rightStripCanvasCoarse->Divide(9, 5);
    leftStripCanvasCoarse->Divide(9, 5);

    pmtCoarseCanvas = new TCanvas("Pmt Coarse Canvas", "pmtCoarseCanvas");
    pmtCoarseCanvas->Divide(4, 2);

    pmtFineCanvas = new TCanvas("Pmt Fine Canvas", "pmtFineCanvas");
    pmtFineCanvas->Divide(4, 2);

    refFineCanvas = new TCanvas("Ref Fine Canvas", "refFineCanvas");
    refFineCanvas->Divide(2, 2);

    refCoarseCanvas = new TCanvas("Ref Coarse Canvas", "refCoarseCanvas");
    refCoarseCanvas->Divide(2, 2);

    stripCoarseLeftCorrCanvas = new TCanvas("Left : Strip Vs Coarse", "stripCoarseLeftCorrCanvas");
    stripCoarseRightCorrCanvas = new TCanvas("Right : Strip Vs Coarse", "stripCoarseRightCorrCanvas");

    stripFineLeftCorrCanvas = new TCanvas("Left : Strip Vs Fine", "stripFineLeftCorrCanvas");
    stripFineRightCorrCanvas = new TCanvas("Right : Strip Vs Fine", "stripFineRightCorrCanvas");

    /* ----- Pre Cal Canvases ----- */
    stripLeftTotCorrCanvas = new TCanvas("Left : Strip Vs Tot", "stripVsTot");
    stripRightTotCorrCanvas = new TCanvas("Right : Strip Vs Tot", "stripVsTot");
    stripLeftTimeCorrCanvas = new TCanvas("Left : Strip Vs Time", "stripVsTime");
    stripRightTimeCorrCanvas = new TCanvas("Right : Strip Vs Time", "stripVsTime");

    pmtPreCalTimeCanvas = new TCanvas("Pmt PreCal Time", "pmtPreCalTime");
    pmtPreCalTimeCanvas->Divide(4, 2);

    pmtPreCalTotCanvas = new TCanvas("Pmt PreCal ToT Time", "pmtPreCalToT");
    pmtPreCalTotCanvas->Divide(4, 2);

    /* ----- Cal Canvases ----- */
    stripCalTimeCorrCanvas = new TCanvas("Left Time Vs Right Time", "leftTimeVsRightTime");
    stripCalTotCorrCanvas = new TCanvas("Left ToT Vs Right ToT", "leftToTVsRightToT");

    /* ------ Hit Canvases ------*/
    hitMapCanvas = new TCanvas("Hit Map", "hitMapCanvas");
    hitMapCanvas->Divide(2, 1);

    /* ------ LOS Correlations -------*/
    losCorrCanvas = new TCanvas("losCorrCanvas", "LOS Correlations");
    stripLosCanvas = new TCanvas("stripLosCanvas", "Strip Los Correlations");
    stripLosCanvas->Divide(5, 9);

    /* ----- Map Histograms -----*/

    stripCoarseRightHisto = new TH1F*[41];
    stripFineRightHisto = new TH1F*[41];

    stripCoarseLeftHisto = new TH1F*[41];
    stripFineLeftHisto = new TH1F*[41];

    pmtCoarseHistoTop = new TH1F*[4];
    pmtFineHistoTop = new TH1F*[4];

    pmtCoarseHistoBottom = new TH1F*[4];
    pmtFineHistoBottom = new TH1F*[4];

    refCoarseHisto = new TH1F*[4];
    refFineHisto = new TH1F*[4];

    stripCoarseLeftCorr = new TH2F("stripCoarseLeft", " Strip Vs Coarse Time: Left", 41, 0.5, 41.5, 400, 0, 2500);
    stripCoarseRightCorr = new TH2F("stripCoarseRight", " Strip Vs Coarse Time: Right", 41, 0.5, 41.5, 400, 0, 2500);
    stripFineLeftCorr = new TH2F("stripFineLeft", " Strip Vs Fine Time: Left", 41, 0.5, 41.5, 400, 0, 600);
    stripFineRightCorr = new TH2F("stripFineRight", " Strip Vs Fine Time: Right", 41, 0.5, 41.5, 400, 0, 600);

    /* ----- Pre Cal Histograms ----- */
    pmtPreCalTimeHistoTop = new TH1F*[4];
    pmtPreCalTotHistoTop = new TH1F*[4];

    pmtPreCalTimeHistoBottom = new TH1F*[4];
    pmtPreCalTotHistoBottom = new TH1F*[4];

    stripLeftTotCorr = new TH2F("stripLeftTotCorr", "Strip Vs Tot : Left", 41, 0.5, 41.5, 400, 0, 500);
    stripRightTotCorr = new TH2F("stripRightTotCorr", "Strip Vs Tot : Right", 41, 0.5, 41.5, 400, 0, 500);
    stripLeftTimeCorr = new TH2F("stripLeftTimeCorr", "Strip Vs Time : Left", 41, 0.5, 41.5, 1000, -550, 400);
    stripRightTimeCorr = new TH2F("stripRightTimeCorr", "Strip Vs Time : Right", 41, 0.5, 41.5, 1000, -550, 400);

    /* ----- Cal Histograms ----- */
    stripCalTimeCorr = new TH2F("stripCalTimeCorr", "Strip: Time Left Vs Time Right", 800, -550, 400, 800, -550, 400);
    stripCalToTCorr = new TH2F("stripCalToTCorr", "Strip: ToT Left Vs ToT Right", 400, -100, 350, 400, -100, 350);

    /* ------------- HIT Histograms ------------ */
    stripPosHitCorr = new TH2F("stripPosHitCorr", "Strip Vs Position", 300, 0, 1500, 41, 0.5, 41.5);
    totalChargeHist = new TH1F("totalChargeHist", "Charge", 1000, -100, 400);
    meanChargeCorr = new TH2F("meanChargeCorr", "Heat Map : Mean Charge", 50, 0, 1500, 41, 0.5, 41.5);

    losTimeDiffCorr = new TH2F("losTimeDiffCorr", "Time Differences with LOS", 4000, 2000, 4000, 41, 0.5, 41.5);
    stripDiffLosHisto = new TH1F*[41];

    for (Int_t i = 0; i < 41; i++)
    {

        sprintf(name, "Left : Coarse Time Strip_%i", i + 1);
        stripCoarseLeftHisto[i] = new TH1F(name, name, 200, 0, 2200);

        sprintf(name, "Left : Fine Time Strip_%i", i + 1);
        stripFineLeftHisto[i] = new TH1F(name, name, 200, 0, 600);

        sprintf(name, "Right : Coarse Time Strip_%i", i + 1);
        stripCoarseRightHisto[i] = new TH1F(name, name, 200, 0, 2200);

        sprintf(name, "Right : Fine Time Strip_%i", i + 1);
        stripFineRightHisto[i] = new TH1F(name, name, 200, 0, 600);

        sprintf(name, "Time LOS - Time RPC Strip : Strip_%i", i + 1);
        stripDiffLosHisto[i] = new TH1F(name, name, 10000, 0, 4000);
    }

    for (Int_t i = 0; i < 4; i++)
    {

        sprintf(name, "Coarse Time Pmt_%i TOP", i + 1);
        pmtCoarseHistoTop[i] = new TH1F(name, name, 1000, 0, 3000);

        sprintf(name, "Fine Time Pmt_%i TOP", i + 1);
        pmtFineHistoTop[i] = new TH1F(name, name, 1000, 0, 3000);

        sprintf(name, "Time Pmt_%i TOP", i + 1);
        pmtPreCalTimeHistoTop[i] = new TH1F(name, name, 1000, -550, 400);

        sprintf(name, "ToT Pmt_%i TOP", i + 1);
        pmtPreCalTotHistoTop[i] = new TH1F(name, name, 1000, -550, 400);
    }

    for (Int_t i = 0; i < 4; i++)
    {

        sprintf(name, "Coarse Time Pmt_%i BOTTOM", i + 1);
        pmtCoarseHistoBottom[i] = new TH1F(name, name, 1000, 0, 3000);

        sprintf(name, "Fine Time Pmt_%i BOTTOM", i + 1);
        pmtFineHistoBottom[i] = new TH1F(name, name, 1000, 0, 3000);

        sprintf(name, "Time Pmt_%i BOTTOM", i + 1);
        pmtPreCalTimeHistoBottom[i] = new TH1F(name, name, 1000, -550, 400);

        sprintf(name, "ToT Pmt_%i BOTTOM", i + 1);
        pmtPreCalTotHistoBottom[i] = new TH1F(name, name, 1000, -550, 400);
    }

    for (Int_t i = 0; i < 4; i++)
    {

        sprintf(name, "Coarse Time Ref_%i", i + 1);
        refCoarseHisto[i] = new TH1F(name, name, 1000, 0, 3000);

        sprintf(name, "Fine Time Ref_%i", i + 1);
        refFineHisto[i] = new TH1F(name, name, 1000, 0, 3000);
    }

    for (Int_t i = 0; i < 41; i++)
    {

        stripCoarseLeftHisto[i]->GetXaxis()->SetTitle("Coarse Time");
        stripCoarseLeftHisto[i]->GetYaxis()->SetTitle("Counts");
        stripCoarseLeftHisto[i]->GetXaxis()->SetTitleOffset(1.1);
        stripCoarseLeftHisto[i]->GetYaxis()->SetTitleOffset(1.1);
        stripCoarseLeftHisto[i]->GetXaxis()->CenterTitle(true);
        stripCoarseLeftHisto[i]->GetYaxis()->CenterTitle(true);

        leftStripCanvasCoarse->cd(i + 1);
        stripCoarseLeftHisto[i]->Draw();

        stripFineRightHisto[i]->GetXaxis()->SetTitle("Fine Time");
        stripFineRightHisto[i]->GetYaxis()->SetTitle("Counts");
        stripFineRightHisto[i]->GetXaxis()->SetTitleOffset(1.1);
        stripFineRightHisto[i]->GetYaxis()->SetTitleOffset(1.1);
        stripFineRightHisto[i]->GetXaxis()->CenterTitle(true);
        stripFineRightHisto[i]->GetYaxis()->CenterTitle(true);

        rightStripCanvasFine->cd(i + 1);
        stripFineRightHisto[i]->Draw();

        stripFineLeftHisto[i]->GetXaxis()->SetTitle("Fine Time");
        stripFineLeftHisto[i]->GetYaxis()->SetTitle("Counts");
        stripFineLeftHisto[i]->GetXaxis()->SetTitleOffset(1.1);
        stripFineLeftHisto[i]->GetYaxis()->SetTitleOffset(1.1);
        stripFineLeftHisto[i]->GetXaxis()->CenterTitle(true);
        stripFineLeftHisto[i]->GetYaxis()->CenterTitle(true);

        leftStripCanvasFine->cd(i + 1);
        stripFineLeftHisto[i]->Draw();

        stripCoarseRightHisto[i]->GetXaxis()->SetTitle("Coarse Time");
        stripCoarseRightHisto[i]->GetYaxis()->SetTitle("Counts");
        stripCoarseRightHisto[i]->GetXaxis()->SetTitleOffset(1.1);
        stripCoarseRightHisto[i]->GetYaxis()->SetTitleOffset(1.1);
        stripCoarseRightHisto[i]->GetXaxis()->CenterTitle(true);
        stripCoarseRightHisto[i]->GetYaxis()->CenterTitle(true);

        rightStripCanvasCoarse->cd(i + 1);
        stripCoarseRightHisto[i]->Draw();

        stripDiffLosHisto[i]->GetXaxis()->SetTitle("Time Difference (ns)");
        stripDiffLosHisto[i]->GetYaxis()->SetTitle("Counts");
        stripLosCanvas->cd(i + 1);
        stripDiffLosHisto[i]->Draw();
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

    meanChargeCorr->GetXaxis()->SetTitle("Position");
    meanChargeCorr->GetYaxis()->SetTitle("Strip");
    hitMapCanvas->cd(2);
    meanChargeCorr->Draw("COLZ");

    losTimeDiffCorr->GetXaxis()->SetTitle("Time Difference (ns)");
    losTimeDiffCorr->GetYaxis()->SetTitle("Strip Number");
    losCorrCanvas->cd();
    losTimeDiffCorr->Draw("COLZ");
    losCorrFolder->Add(losCorrCanvas);
    losCorrFolder->Add(stripLosCanvas);

    for (Int_t i = 0; i < 4; i++)
    {

        pmtCoarseHistoTop[i]->GetXaxis()->SetTitle("Coarse Time");
        pmtCoarseHistoTop[i]->GetYaxis()->SetTitle("Counts");
        pmtCoarseCanvas->cd(i + 1);
        pmtCoarseHistoTop[i]->Draw();

        pmtFineHistoTop[i]->GetXaxis()->SetTitle("Fine Time");
        pmtFineHistoTop[i]->GetYaxis()->SetTitle("Counts");
        pmtFineCanvas->cd(i + 1);
        pmtFineHistoTop[i]->Draw();

        pmtPreCalTimeHistoTop[i]->GetXaxis()->SetTitle("Time");
        pmtPreCalTimeHistoTop[i]->GetYaxis()->SetTitle("Counts");
        pmtPreCalTimeCanvas->cd(i + 1);
        pmtPreCalTimeHistoTop[i]->Draw();

        pmtPreCalTotHistoTop[i]->GetXaxis()->SetTitle("ToT");
        pmtPreCalTotHistoTop[i]->GetYaxis()->SetTitle("Counts");
        pmtPreCalTotCanvas->cd(i + 1);
        pmtPreCalTotHistoTop[i]->Draw();

        pmtCoarseHistoBottom[i]->GetXaxis()->SetTitle("Coarse Time");
        pmtCoarseHistoBottom[i]->GetYaxis()->SetTitle("Counts");
        pmtCoarseCanvas->cd(i + 1 + 4);
        pmtCoarseHistoBottom[i]->Draw();

        pmtFineHistoBottom[i]->GetXaxis()->SetTitle("Fine Time");
        pmtFineHistoBottom[i]->GetYaxis()->SetTitle("Counts");
        pmtFineCanvas->cd(i + 1 + 4);
        pmtFineHistoBottom[i]->Draw();

        pmtPreCalTimeHistoBottom[i]->GetXaxis()->SetTitle("Time");
        pmtPreCalTimeHistoBottom[i]->GetYaxis()->SetTitle("Counts");
        pmtPreCalTimeCanvas->cd(i + 1 + 4);
        pmtPreCalTimeHistoBottom[i]->Draw();

        pmtPreCalTotHistoBottom[i]->GetXaxis()->SetTitle("ToT");
        pmtPreCalTotHistoBottom[i]->GetYaxis()->SetTitle("Counts");
        pmtPreCalTotCanvas->cd(i + 1 + 4);
        pmtPreCalTotHistoBottom[i]->Draw();
    }

    for (Int_t i = 0; i < 4; i++)
    {

        refFineHisto[i]->GetXaxis()->SetTitle("Fine Time");
        refFineHisto[i]->GetYaxis()->SetTitle("Counts");
        refFineCanvas->cd(i + 1);
        refFineHisto[i]->Draw();

        refCoarseHisto[i]->GetXaxis()->SetTitle("Coarse Time");
        refCoarseHisto[i]->GetYaxis()->SetTitle("Counts");
        refCoarseCanvas->cd(i + 1);
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
        stripDiffLosHisto[i]->Reset();
    }

    for (Int_t i = 0; i < 4; i++)
    {
        refFineHisto[i]->Reset();
        refCoarseHisto[i]->Reset();
    }

    for (Int_t i = 0; i < 4; i++)
    {

        pmtFineHistoTop[i]->Reset();
        pmtCoarseHistoTop[i]->Reset();
        pmtFineHistoBottom[i]->Reset();
        pmtCoarseHistoBottom[i]->Reset();
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
    meanChargeCorr->Reset();
    losTimeDiffCorr->Reset();

    return;
}

void R3BRpcOnlineSpectra::Exec(Option_t* option)
{
    if (fEventHeader->GetTrigger() != fTrigger && fTrigger > -1)
    {
        return;
    }

    if (fEventHeader->GetTpat() > 0)
    {
        for (Int_t i = 0; i < 16; i++)
        {
            tpatbin = (fEventHeader->GetTpat() & (1 << i));
            if (tpatbin != 0)
            {
                fTPat = i + 1;
            }
        }
    }

    auto nStripMappedHits = fStripMappedItems->GetEntriesFast();

    /*
      TRIG_LMU_OUT( 1) = BEAM_GATE_AUX and in_los_nrolu;
        TRIG_LMU_OUT( 2) = BEAM_GATE_AUX and in_los_nrolu and in_tofd and in_califa;
        TRIG_LMU_OUT( 3) = BEAM_GATE_AUX and in_los_nrolu and in_tofd and in_califa and in_rpc;
        TRIG_LMU_OUT( 4) = BEAM_GATE_AUX and in_los_nrolu and in_tofd and in_neuland;
        TRIG_LMU_OUT( 5) = BEAM_GATE_AUX and in_los_nrolu and in_tofd and in_califa and not in_califa_veto;

        TRIG_LMU_OUT( 9) = not BEAM_GATE_AUX and in_califa;
        TRIG_LMU_OUT(10) = not BEAM_GATE_AUX and in_neuland;
        TRIG_LMU_OUT(11) = not BEAM_GATE_AUX and in_tofd;
        TRIG_LMU_OUT(12) = not BEAM_GATE_AUX and in_rpc;
    */
    if (fTPat >= 1 && fTPat <= 5)
    {
        /* ------------------- Map EventLoop ------------------*/
        for (Int_t ihit = 0; ihit < nStripMappedHits; ihit++)
        {

            R3BRpcStripMappedData* hit = (R3BRpcStripMappedData*)fStripMappedItems->At(ihit);

            if (!hit)
                continue;

            Int_t side = hit->GetSide();

            if (side == 0)
            {

                stripFineLeftHisto[hit->GetStripId() - 1]->Fill(hit->GetFineTime());
                stripCoarseLeftHisto[hit->GetStripId() - 1]->Fill(hit->GetCoarseTime());
                stripCoarseLeftCorr->Fill(hit->GetStripId(), hit->GetCoarseTime());
                stripFineLeftCorr->Fill(hit->GetStripId(), hit->GetFineTime());
            }

            if (side == 1)
            {

                stripFineRightHisto[hit->GetStripId() - 1]->Fill(hit->GetFineTime());
                stripCoarseRightHisto[hit->GetStripId() - 1]->Fill(hit->GetCoarseTime());
                stripCoarseRightCorr->Fill(hit->GetStripId(), hit->GetCoarseTime());
                stripFineRightCorr->Fill(hit->GetStripId(), hit->GetFineTime());
            }
        }

        auto nRefMappedHits = fRefMappedItems->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nRefMappedHits; ihit++)
        {

            R3BRpcRefMappedData* hit = (R3BRpcRefMappedData*)fRefMappedItems->At(ihit);

            if (!hit)
                continue;

            refFineHisto[hit->GetChannelId() - 1]->Fill(hit->GetFineTime());
            refCoarseHisto[hit->GetChannelId() - 1]->Fill(hit->GetCoarseTime());
        }

        auto nPmtMappedHits = fPmtMappedItems->GetEntriesFast();
        Int_t pmtSide;
        for (Int_t ihit = 0; ihit < nPmtMappedHits; ihit++)
        {

            R3BRpcPmtMappedData* hit = (R3BRpcPmtMappedData*)fPmtMappedItems->At(ihit);
            if (!hit)
                continue;

            pmtSide = hit->GetSide();

            if (pmtSide == 0)
            {

                pmtFineHistoTop[hit->GetChannelId() - 1]->Fill(hit->GetFineTime());
                pmtCoarseHistoTop[hit->GetChannelId() - 1]->Fill(hit->GetCoarseTime());
            }

            if (pmtSide == 1)
            {

                pmtFineHistoBottom[hit->GetChannelId() - 1]->Fill(hit->GetFineTime());
                pmtCoarseHistoBottom[hit->GetChannelId() - 1]->Fill(hit->GetCoarseTime());
            }
        }

        /* ----------------- PreCal EventLoop ------------------*/
        auto nStripPreCalHits = fStripPreCalDataItems->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nStripPreCalHits; ihit++)
        {

            R3BRpcStripPreCalData* hit = (R3BRpcStripPreCalData*)fStripPreCalDataItems->At(ihit);

            Int_t side = hit->GetSide();

            if (side == 0)
            {
                stripLeftTotCorr->Fill(hit->GetChannelId(), hit->GetTot());
                stripLeftTimeCorr->Fill(hit->GetChannelId(), hit->GetTime());
            }

            if (side == 1)
            {
                stripRightTotCorr->Fill(hit->GetChannelId(), hit->GetTot());
                stripRightTimeCorr->Fill(hit->GetChannelId(), hit->GetTime());
            }
        }

        auto nPmtPreCalHits = fPmtPreCalItems->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nPmtPreCalHits; ihit++)
        {

            R3BRpcPmtPreCalData* hit = (R3BRpcPmtPreCalData*)fPmtPreCalItems->At(ihit);
            pmtSide = hit->GetSide();

            if (pmtSide == 0)
            {
                pmtPreCalTimeHistoTop[hit->GetChannelId() - 1]->Fill(hit->GetTime());
                pmtPreCalTotHistoTop[hit->GetChannelId() - 1]->Fill(hit->GetTot());
            }

            if (pmtSide == 1)
            {
                pmtPreCalTimeHistoBottom[hit->GetChannelId() - 1]->Fill(hit->GetTime());
                pmtPreCalTotHistoBottom[hit->GetChannelId() - 1]->Fill(hit->GetTot());
            }
        }

        /* -------------------- Cal EventLoop ---------------------*/
        auto nStripCalHits = fStripCalDataItems->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nStripCalHits; ihit++)
        {

            R3BRpcStripCalData* hit = (R3BRpcStripCalData*)fStripCalDataItems->At(ihit);

            stripCalTimeCorr->Fill(hit->GetTimeLeft(), hit->GetTimeRight());
            stripCalToTCorr->Fill(hit->GetTotLeft(), hit->GetTotRight());
        }

        /*---------------- HIT EventLoop ---------------*/
        auto nStripHits = fStripHitDataItems->GetEntriesFast();
        auto losNHits = fLosCalDataItems->GetEntriesFast();
        auto losNTriggerHits = fLosTriggerCalDataItems->GetEntriesFast();

        Int_t channelId;
        Float_t pos, charge;
        Int_t bin;

        Double_t losTime = 0., losTriggerTime = 0.;

        for (Int_t ihit = 0; ihit < losNHits; ihit++)
        {

            R3BLosCalData* losHit = (R3BLosCalData*)fLosCalDataItems->At(ihit);
            Int_t losChannel = losHit->GetDetector();
            losTime = losHit->GetTimeT_ns(losChannel);
        }

        for (Int_t ihit = 0; ihit < losNTriggerHits; ihit++)
        {

            R3BLosCalData* losTriggerHit = (R3BLosCalData*)fLosTriggerCalDataItems->At(ihit);
            Int_t losChannelTrigger = losTriggerHit->GetDetector();
            losTriggerTime = losTriggerHit->GetTimeL_ns(0);
        }

        for (Int_t ihit = 0; ihit < nStripHits; ihit++)
        {

            R3BRpcStripHitData* hit = (R3BRpcStripHitData*)fStripHitDataItems->At(ihit);
            channelId = hit->GetChannelId();
            pos = hit->GetPos();
            charge = hit->GetCharge();

            if (losNHits && losNTriggerHits)
            {

                int c = 2048 * 5;

                double time = fmod((losTime - losTriggerTime - hit->GetTime() + 10 * c + c / 2.), c) - c / 2.;
                stripDiffLosHisto[channelId - 1]->Fill(time);
                losTimeDiffCorr->Fill(time, channelId);
            }

            stripPosHitCorr->Fill(pos, channelId);
            totalChargeHist->Fill(hit->GetCharge());
            counts++;

            bin = Int_t(50 - (1500 - pos) / 30.0 + 1);

            if (charge >= 0.0 && bin <= 50 && bin > 0)
            {

                meanCharges[channelId - 1][bin - 1] = meanCharges[channelId - 1][bin - 1] +
                                                      (1.0 / counts) * (charge - meanCharges[channelId - 1][bin - 1]);

                meanChargeCorr->SetBinContent(bin, channelId, meanCharges[channelId - 1][bin - 1]);
            }
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

    if (fStripPreCalDataItems)
    {
        fStripPreCalDataItems->Clear();
    }

    if (fStripCalDataItems)
    {
        fStripCalDataItems->Clear();
    }

    if (fPmtPreCalItems)
    {
        fPmtPreCalItems->Clear();
    }

    if (fStripHitDataItems)
    {
        fStripHitDataItems->Clear();
    }

    if (fLosCalDataItems)
    {

        fLosCalDataItems->Clear();
    }
}

void R3BRpcOnlineSpectra::FinishTask()
{
    if (fRefMappedItems)
    {
        for (Int_t i = 0; i < 4; i++)
        {
            refFineHisto[i]->Write();
            refCoarseHisto[i]->Write();
        }
    }

    if (fPmtMappedItems)
    {
        for (Int_t i = 0; i < 4; i++)
        {
            pmtFineHistoTop[i]->Write();
            pmtCoarseHistoTop[i]->Write();
            pmtFineHistoBottom[i]->Write();
            pmtCoarseHistoBottom[i]->Write();
        }
    }

    if (fStripMappedItems)
    {
        for (Int_t i = 0; i < 41; i++)
        {

            stripFineLeftHisto[i]->Write();
            stripFineRightHisto[i]->Write();

            stripCoarseLeftHisto[i]->Write();
            stripCoarseRightHisto[i]->Write();
            stripDiffLosHisto[i]->Write();
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
