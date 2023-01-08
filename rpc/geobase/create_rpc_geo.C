/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "TGeoManager.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>

TGeoManager* gGeoMan = NULL;

/** Detector parameters.*/
Double_t det_xGlassPos = 150; //!  x-size of Active Glass plates
Double_t det_yGlassPos = 120; //!  y-size of Active Glass plates
Double_t det_zGlassPos = 0.1; //!  z-size of Active Glass plates

Double_t det_xFreonSF6Pos = 150;  //!  x-size of gas gap
Double_t det_yFreonSF6Pos = 120;  //!  y-size of gas gap
Double_t det_zFreonSF6Pos = 0.03; //!  z-size of gas gap

Double_t det_xStripPos = 154;   //!  x-size of Strip
Double_t det_yStripPos = 2.9;   //!  y-size of Strip
Double_t det_zStripPos = 0.003; //!  z-size of Strip

Double_t det_latxPMMAPos = 1.5;  //!  x-size of PMMA box
Double_t det_latyPMMAPos = 125;  //!  y-size of PMMA box
Double_t det_latzPMMAPos = 0.88; //!  z-size of PMMA box

Double_t det_frontdxPMMAPos = 155; //!  x-thickness of PMMA box
Double_t det_frontdyPMMAPos = 125; //!  y-thickness of PMMA box
Double_t det_frontdzPMMAPos = 0.1; //!  z-thickness of PMMA box

Double_t det_topdxPMMAPos = 155;  //!  x-thickness of PMMA box
Double_t det_topdyPMMAPos = 1.5;  //!  y-thickness of PMMA box
Double_t det_topdzPMMAPos = 0.88; //!  z-thickness of PMMA box

Double_t det_xFreonSF6Pos_2 = 0.5; //!  x-size of gas gap
Double_t det_yFreonSF6Pos_2 = 0.5; //!  y-size of gas gap
Double_t det_zFreonSF6Pos_2 = 0.8; //!  z-size of gas gap

Double_t det_xFR4Pos = 157;  //!  x-size of FR4 box
Double_t det_yFR4Pos = 125;  //!  y-size of FR4 box
Double_t det_zFR4Pos = 0.15; //!  z-size of FR4 box

Double_t det_xAlPos = 177; //!  x-size of Aluminium box
Double_t det_yAlPos = 131; //!  y-size of Aluminium box
Double_t det_zAlPos = 0.3; //!  z-size of Aluminium box

void create_rpc_geo(const char* geoTag = "v22")
{

    // -------   Load media from media file   -----------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media_r3b.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    TString geoFileName = geoPath + "/geometry/tof_rpc_";
    geoFileName = geoFileName + geoTag + ".geo.root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("Air");
    if (!mAir)
        Fatal("Main", "FairMedium Air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMed0 = gGeoMan->GetMedium("Air");
    if (!pMed0)
        Fatal("Main", "Medium Air not found");

    FairGeoMedium* rpc_al = geoMedia->getMedium("aluminium");
    if (!rpc_al)
        Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(rpc_al);
    TGeoMedium* pMed1 = gGeoMan->GetMedium("aluminium");
    if (!pMed1)
        Fatal("Main", "Medium aluminium not found");

    FairGeoMedium* rpc_gas = geoMedia->getMedium("RPCgas");
    if (!rpc_gas)
        Fatal("Main", "FairMedium RPCgas not found");
    geoBuild->createMedium(rpc_gas);
    TGeoMedium* pMed2 = gGeoMan->GetMedium("RPCgas");
    if (!pMed2)
        Fatal("Main", "Medium RPCgas not found");

    FairGeoMedium* rpc_strip = geoMedia->getMedium("copper");
    if (!rpc_strip)
        Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(rpc_strip);
    TGeoMedium* pMed3 = gGeoMan->GetMedium("copper");
    if (!pMed3)
        Fatal("Main", "Medium copper not found");

    FairGeoMedium* rpc_glass = geoMedia->getMedium("RPC_glass");
    if (!rpc_glass)
        Fatal("Main", "FairMedium RPC_glass not found");
    geoBuild->createMedium(rpc_glass);
    TGeoMedium* pMed4 = gGeoMan->GetMedium("RPC_glass");
    if (!pMed4)
        Fatal("Main", "Medium RPC_glass not found");

    FairGeoMedium* rpc_pmma = geoMedia->getMedium("RPC_pmma");
    if (!rpc_pmma)
        Fatal("Main", "FairMedium RPC_pmma not found");
    geoBuild->createMedium(rpc_pmma);
    TGeoMedium* pMed5 = gGeoMan->GetMedium("RPC_pmma");
    if (!pMed5)
        Fatal("Main", "Medium RPC_pmma not found");

    FairGeoMedium* rpc_FR4 = geoMedia->getMedium("RPC_fr4");
    if (!rpc_FR4)
        Fatal("Main", "FairMedium RPC_fr4 not found");
    geoBuild->createMedium(rpc_FR4);
    TGeoMedium* pMed6 = gGeoMan->GetMedium("RPC_fr4");
    if (!pMed6)
        Fatal("Main", "Medium RPC_fr4 not found");

    FairGeoMedium* Vacuum = geoMedia->getMedium("vacuum");
    if (!Vacuum)
        Fatal("Main", "FairMedium vacuum not found");
    geoBuild->createMedium(Vacuum);
    TGeoMedium* pMed7 = gGeoMan->GetMedium("vacuum");
    if (!pMed7)
        Fatal("Main", "Medium vacuum not found");
    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName("RPC_TOFgeom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    gGeoMan->SetTopVolume(top);
    // --------------------------------------------------------------------------

    double xbox = 178.4;
    double ybox = 158.4;
    double zbox = 10.;

    /*double xbox=126;
    double ybox = 168.4;
    double zbox = 2.5103;  */

    // World definition
    TGeoVolume* Vacuum_box = gGeoManager->GetTopVolume();
    Vacuum_box->SetVisLeaves(kTRUE);

    // Definition of the mother volume where the RPC voulumes are going to be
    // mounted
    TGeoVolume* Rpc_module = gGeoManager->MakeBox("RPC", pMed0, xbox / 2.0, ybox / 2.0, zbox / 2.0);
    // Rpc_module->SetLineColor(kRed);

    // Global positioning
    Vacuum_box->AddNode(Rpc_module, 0, 0);

    // RPC volumes

    TGeoVolume* vol_al_F = gGeoManager->MakeBox("Al1", pMed1, det_xAlPos / 2.0, det_yAlPos / 2.0, det_zAlPos / 2.0);
    vol_al_F->SetLineColor(kGray);

    TGeoVolume* vol_pmmaLat1 =
        gGeoManager->MakeBox("pmmalat1", pMed7, det_latxPMMAPos / 2.0, det_latyPMMAPos / 2.0, det_latzPMMAPos / 2.0);
    vol_pmmaLat1->SetLineColor(kYellow);

    TGeoVolume* vol_pmmafront1 = gGeoManager->MakeBox(
        "pmmafront1", pMed5, (det_frontdxPMMAPos) / 2.0, det_frontdyPMMAPos / 2.0, det_frontdzPMMAPos / 2.0);
    vol_pmmafront1->SetLineColor(kYellow);

    TGeoVolume* vol_pmmatop1 = gGeoManager->MakeBox(
        "pmmatop1", pMed5, (det_topdxPMMAPos) / 2.0, det_topdyPMMAPos / 2.0, det_topdzPMMAPos / 2.0);
    vol_pmmatop1->SetLineColor(kYellow);

    TGeoVolume* vol_glass =
        gGeoManager->MakeBox("glass", pMed4, det_xGlassPos / 2.0, det_yGlassPos / 2.0, det_zGlassPos / 2.0);
    vol_glass->SetLineColor(kBlue);

    TGeoVolume* vol_FR4 = gGeoManager->MakeBox("FR4", pMed6, det_xFR4Pos / 2.0, det_yFR4Pos / 2.0, det_zFR4Pos / 2.0);
    vol_FR4->SetLineColor(kBlack);

    TGeoVolume* Cu_plane =
        gGeoManager->MakeBox("Cu_plane", pMed3, det_xGlassPos / 2.0, det_yGlassPos / 2.0, det_zStripPos / 2.0);
    Cu_plane->SetLineColor(kBlack);

    TGeoVolume* vol_FrSF6 =
        gGeoManager->MakeBox("gas1", pMed2, det_xFreonSF6Pos / 2.0, det_yFreonSF6Pos / 2.0, det_zFreonSF6Pos / 2.0);
    vol_FrSF6->SetLineColor(kGreen);

    TGeoVolume* vol_strip =
        gGeoManager->MakeBox("strip", pMed3, (det_xStripPos) / 2.0, (det_yStripPos) / 2.0, det_zStripPos / 2.0);
    vol_strip->SetLineColor(42);

    // join everything

    // add FR4
    double z = 0;

    Rpc_module->AddNode(vol_al_F, 1, new TGeoTranslation(0, 0, (det_zAlPos / 2.0)));
    Rpc_module->AddNode(vol_FR4, 2, new TGeoTranslation(0, 0, det_zAlPos + (det_zFR4Pos / 2.0)));
    Rpc_module->AddNode(Cu_plane, 3, new TGeoTranslation(0, 0, det_zAlPos + (det_zFR4Pos + det_zStripPos / 2.0)));

    z = det_zFR4Pos + det_zStripPos + det_zAlPos;

    // Add first 6 gap RPC
    int i = 4;
    int j = 0;
    Rpc_module->AddNode(vol_pmmafront1, i, new TGeoTranslation(0, 0, (z + det_frontdzPMMAPos / 2.0)));

    z += det_frontdzPMMAPos;

    for (j = 1; j < 14; j += 2)
    {

        i += j;

        Rpc_module->AddNode(vol_glass, i, new TGeoTranslation(0, 0, (det_zGlassPos / 2.0) + z));

        Rpc_module->AddNode(vol_FrSF6, i + 1, new TGeoTranslation(0, 0, (det_zFreonSF6Pos / 2.0) + det_zGlassPos + z));

        z += det_zGlassPos + det_zFreonSF6Pos;
    }

    i++;

    Rpc_module->AddNode(vol_pmmafront1, i, new TGeoTranslation(0, 0, (z + det_frontdzPMMAPos / 2.0)));

    z += det_frontdzPMMAPos;

    // Add 32 copper strips to the Rpc module

    Rpc_module->AddNode(vol_FR4, i, new TGeoTranslation(0, 0, (det_zFR4Pos / 2.0) + z));

    for (j = 1; j <= 40; j++)
    {
        i++;
        Rpc_module->AddNode(
            vol_strip,
            j,
            new TGeoTranslation(0,
                                -det_yGlassPos / 2 + det_yStripPos / 2 + (det_yStripPos + 0.1) * (j - 1),
                                (det_zStripPos / 2.0) + det_zFR4Pos + z));
    }

    z += det_zFR4Pos + det_zStripPos;

    // Add second 6 gap RPC

    i++;

    Rpc_module->AddNode(vol_pmmafront1, i, new TGeoTranslation(0, 0, (z + det_frontdzPMMAPos / 2.0)));

    z += det_frontdzPMMAPos;

    for (j = 1; j < 14; j += 2)
    {

        i += j;

        Rpc_module->AddNode(vol_glass, i, new TGeoTranslation(0, 0, (det_zGlassPos / 2.0) + z));

        Rpc_module->AddNode(vol_FrSF6, i + 1, new TGeoTranslation(0, 0, (det_zFreonSF6Pos / 2.0) + det_zGlassPos + z));

        z += det_zGlassPos + det_zFreonSF6Pos;
    }
    i++;

    Rpc_module->AddNode(vol_pmmafront1, i, new TGeoTranslation(0, 0, (z + det_frontdzPMMAPos / 2.0)));
    z += det_frontdzPMMAPos;
    i++;
    Rpc_module->AddNode(Cu_plane, i, new TGeoTranslation(0, 0, (det_zStripPos / 2.0) + z));
    i++;
    Rpc_module->AddNode(vol_FR4, i, new TGeoTranslation(0, 0, det_zStripPos + (det_zFR4Pos / 2.0) + z));
    i++;
    Rpc_module->AddNode(vol_al_F, i, new TGeoTranslation(0, 0, z + det_zStripPos + det_zFR4Pos + (det_zAlPos / 2.0)));

    // add pmma and aluminium outer parts
    Rpc_module->AddNode(vol_al_F, 1, new TGeoTranslation(0, 0, (det_zAlPos / 2.0)));

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    std::cout << "\033[34m Creating geometry:\033[0m "
              << "\033[33m" << geoFileName << " \033[0m" << std::endl;
    std::cout << "Macro finished successfully." << std::endl;
    gApplication->Terminate();
}
