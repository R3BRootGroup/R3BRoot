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

//
//   ----- General macro for generating the MWPC3 geometry
//
//         Author: Jose Luis <joseluis.rodriguez.sanchez@usc.es>
//
//         Last Update: 07/10/19 (Jose Luis)
//
//         Comments:
//

#include "TGeoManager.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>

void create_mwpc3_geo(const char* geoTag = "3")
{
    // --------------------------------------------------------------------------
    // Configurable geometry for the MWPC3 detectors.
    // Use this macro to create root files with the different configurations
    // and positions of the MWPC3.
    //
    // Execute macro:  root -l
    //                 .L create_mwpc3_geo.C
    //                 create_mwpc3_geo()
    // --------------------------------------------------------------------------

    TGeoRotation* fRefRot = NULL;
    TGeoManager* gGeoMan = NULL;

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
    TString geoFileName = geoPath + "/geometry/mwpc_";
    geoFileName = geoFileName + geoTag + ".geo.root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("Air");
    if (!mAir)
        Fatal("Main", "FairMedium Air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("Air");
    if (!pMedAir)
        Fatal("Main", "Medium Air not found");

    FairGeoMedium* mVac = geoMedia->getMedium("vacuum");
    if (!mVac)
        Fatal("Main", "FairMedium vacuum not found");
    geoBuild->createMedium(mVac);
    TGeoMedium* pMedVac = gGeoMan->GetMedium("vacuum");
    if (!pMedVac)
        Fatal("Main", "Medium vacuum not found");

    FairGeoMedium* mSi = geoMedia->getMedium("silicon");
    if (!mSi)
        Fatal("Main", "FairMedium silicon not found");
    geoBuild->createMedium(mSi);
    TGeoMedium* pMedSi = gGeoMan->GetMedium("silicon");
    if (!pMedSi)
        Fatal("Main", "Medium silicon not found");

    FairGeoMedium* mCopper = geoMedia->getMedium("copper");
    if (!mCopper)
        Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    TGeoMedium* pMedCu = gGeoMan->GetMedium("copper");
    if (!pMedCu)
        Fatal("Main", "Medium copper not found");

    FairGeoMedium* mAl = geoMedia->getMedium("aluminium");
    if (!mAl)
        Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAl);
    TGeoMedium* pMedAl = gGeoMan->GetMedium("aluminium");
    if (!pMedAl)
        Fatal("Main", "Medium aluminium not found");

    FairGeoMedium* mFe = geoMedia->getMedium("iron");
    if (!mFe)
        Fatal("Main", "FairMedium iron not found");
    geoBuild->createMedium(mFe);
    TGeoMedium* pMedFe = gGeoMan->GetMedium("iron");
    if (!pMedFe)
        Fatal("Main", "Medium iron not found");

    FairGeoMedium* mAr = geoMedia->getMedium("ArCO2");
    if (!mAr)
        Fatal("Main", "FairMedium ArCO2 not found");
    geoBuild->createMedium(mAr);
    TGeoMedium* pMedAr = gGeoMan->GetMedium("ArCO2");
    if (!pMedAr)
        Fatal("Main", "Medium ArCO2 not found");

    FairGeoMedium* mMylar = geoMedia->getMedium("mylar");
    if (!mMylar)
        Fatal("Main", "FairMedium mylar not found");
    geoBuild->createMedium(mMylar);
    TGeoMedium* pMedMylar = gGeoMan->GetMedium("mylar");
    if (!pMedMylar)
        Fatal("Main", "Medium Mylar not found");
    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName("MWP3geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    gGeoMan->SetTopVolume(top);
    // --------------------------------------------------------------------------

    // out-of-file geometry definition
    Double_t dx, dy, dz;
    Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
    Double_t thx, phx, thy, phy, thz, phz;
    Double_t phi1, phi2;
    Double_t tx, ty, tz;

    TGeoRotation* zeroRot = new TGeoRotation; // zero rotation
    TGeoCombiTrans* tZero = new TGeoCombiTrans("tZero", 0., 0., 0., zeroRot);
    tZero->RegisterYourself();
    //-------------------------------------------------------------------

    // WORLD
    TGeoVolume* pAWorld = gGeoManager->GetTopVolume();

    TGeoRotation* rot_mwpc = new TGeoRotation("MWPCrot");

    // Defintion of the Mother Volume
    TGeoShape* pCBWorld = new TGeoBBox("MWPC_box", 120.0 / 2.0, 80.0 / 2.0, 9.0 / 2.0);

    TGeoVolume* pWorld = new TGeoVolume("MWPCWorld_3", pCBWorld, pMedAir);

    TGeoCombiTrans* t0 = new TGeoCombiTrans(0., 0., 0., rot_mwpc);

    // add as Mother Volume
    pAWorld->AddNode(pWorld, 0, t0);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 0.000;
    dy = 0.000;
    dz = 0.000;
    TGeoCombiTrans* pMatrix1 = new TGeoCombiTrans("", dx, dy, dz, 0);
    //
    dx = 119.9;
    dy = 79.9;
    dz = 5.0;
    TGeoShape* basea = new TGeoBBox("BaseAl", dx / 2., dy / 2., dz / 2.);
    dx = 90.;
    dy = 60.0;
    dz = 5.5;
    TGeoShape* baseb = new TGeoBBox("BaseAls", dx / 2., dy / 2., dz / 2.);
    TGeoSubtraction* basesub = new TGeoSubtraction(basea, baseb, pMatrix1, pMatrix1);
    TGeoShape* basesub_c = new TGeoCompositeShape("GE", basesub);
    TGeoVolume* basesub_log = new TGeoVolume("", basesub_c, pMedAl);
    basesub_log->SetVisLeaves(kTRUE);
    basesub_log->SetLineColor(29);

    // Position
    pWorld->AddNode(basesub_log, 0, pMatrix1);

    // Shape
    dx = 90.;
    dy = 60.;
    dz = 3.0;
    TGeoShape* Detector1 = new TGeoBBox("Detector_1", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* Detector_log1 = new TGeoVolume("MWPC3", Detector1, pMedAr);
    Detector_log1->SetVisLeaves(kTRUE);
    Detector_log1->SetLineColor(3);
    pWorld->AddNode(Detector_log1, 0, pMatrix1);

    dx = 0.000;
    dy = 0.000;
    dz = -1.5 - 0.002 / 2.;
    TGeoCombiTrans* pMatrix20 = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Shape
    dx = 90.;
    dy = 60.;
    dz = 0.0020;
    TGeoShape* basemylar = new TGeoBBox("basemylar", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* basemylar_log1 = new TGeoVolume("basemylar_log1", basemylar, pMedMylar); // first Kapton
    basemylar_log1->SetVisLeaves(kTRUE);
    basemylar_log1->SetLineColor(28);

    // Position
    pWorld->AddNode(basemylar_log1, 0, pMatrix20);

    // Volume:
    TGeoVolume* basemylar_log2 = new TGeoVolume("basemylar_log2", basemylar, pMedMylar); // Second Kapton
    basemylar_log2->SetVisLeaves(kTRUE);
    basemylar_log2->SetLineColor(28);

    dx = 0.000;
    dy = 0.000;
    dz = 1.5 + 0.002 / 2.;
    TGeoCombiTrans* pMatrix21 = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Position
    pWorld->AddNode(basemylar_log2, 0, pMatrix21);

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
