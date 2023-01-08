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
//   ----- General macro for generating the MWPC 1&2  geometries
//
//         Author: Jose Luis <joseluis.rodriguez.sanchez@usc.es>
//
//         Last Update: 06/10/19 (Jose Luis)
//
//         Comments:
//

#include "TGeoManager.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>

void create_mwpc1and2_geo(int geoId = 1)
{
    // --------------------------------------------------------------------------
    // Configurable geometry for the MWPC1 and MWPC2 detectors.
    // Use this macro to create root files with the different configurations
    // and positions of the MWPCs.
    //
    // Execute macro:  root -l
    //                 .L create_mwpc1and2_geo.C
    //                 create_mwpc1and2_geo()
    // --------------------------------------------------------------------------

    TGeoRotation* fRefRot = NULL;
    TGeoManager* gGeoMan = NULL;

    if (geoId == 0)
    {
        std::cout << "Introduce geoTag (1 or 2):" << std::endl;
        std::cin >> geoId;
    }

    TString geoTag = Form("%01d", geoId);

    TString detName = "MWPC" + geoTag;
    TString WorldName = "MWPCWorld" + geoTag;

    if (detName != "MWPC1" && detName != "MWPC2")
    {
        std::cout << "error: Detector name not valid, should be MWPC1 or MWPC2" << std::endl;
        return;
    }

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
    gGeoMan->SetName(detName + "geom");
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

    // WORLD -------------------------------------------------------------
    TGeoVolume* pAWorld = gGeoManager->GetTopVolume();

    TGeoRotation* rot_mwpc = new TGeoRotation("MWPCrot");
    rot_mwpc->RotateY(0.0);

    // Defintion of the Mother Volume
    TGeoShape* pCBWorld = new TGeoBBox("MWPC_box", 35.0 / 2.0, 35.0 / 2.0, 6.0 / 2.0);

    TGeoVolume* pWorld = new TGeoVolume(WorldName, pCBWorld, pMedAr);
    TGeoCombiTrans* t0 = new TGeoCombiTrans(0.0, 0.0, 0.0, rot_mwpc);

    // Add the sphere as Mother Volume
    pAWorld->AddNode(pWorld, 0, t0);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 0.000;
    dy = 0.000;
    dz = 0.000;
    TGeoCombiTrans* pMatrix1 = new TGeoCombiTrans("", dx, dy, dz, 0);
    // Shape: sofia1 type: TGeoTubeSeg
    dx = 53.800;
    dy = 57.000;
    dz = 1.2000;
    TGeoShape* base1 = new TGeoBBox("base1", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log1 = new TGeoVolume("base_log1", base1, pMedAl);
    base_log1->SetVisLeaves(kTRUE);
    base_log1->SetLineColor(3);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 21.500;
    dy = 21.500;
    dz = 2.2000;
    TGeoShape* base2 = new TGeoBBox("base2", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log2 = new TGeoVolume("base_log2", base2, pMedAl);
    base_log2->SetVisLeaves(kTRUE);
    base_log2->SetLineColor(2);

    TGeoSubtraction* basesub1 = new TGeoSubtraction(base1, base2, pMatrix1, pMatrix1);
    TGeoShape* basesub_1 = new TGeoCompositeShape("GE", basesub1);
    TGeoVolume* basesub_log1 = new TGeoVolume("", basesub_1, pMedAl);
    basesub_log1->SetVisLeaves(kTRUE);
    basesub_log1->SetLineColor(4);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 25.60 + 10.0 / 2.0 - 6.80;
    dy = 0.0;
    dz = 0.000;
    TGeoCombiTrans* pMatrix3 = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 10.000;
    dy = 25.000;
    dz = 2.2000;
    TGeoShape* base3 = new TGeoBBox("base3", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log3 = new TGeoVolume("base_log3", base3, pMedAl);
    base_log3->SetVisLeaves(kTRUE);
    base_log3->SetLineColor(2);

    TGeoSubtraction* basesub2 = new TGeoSubtraction(basesub_1, base3, pMatrix1, pMatrix3);
    TGeoShape* basesub_2 = new TGeoCompositeShape("GE", basesub2);
    TGeoVolume* basesub_log2 = new TGeoVolume("", basesub_2, pMedAl);
    basesub_log2->SetVisLeaves(kTRUE);
    basesub_log2->SetLineColor(4);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = -25.60 - 10.0 / 2.0 + 8.00;
    dy = 0.0;
    dz = 0.000;
    TGeoCombiTrans* pMatrix3b = new TGeoCombiTrans("", dx, dy, dz, 0);

    TGeoSubtraction* basesub3 = new TGeoSubtraction(basesub_2, base3, pMatrix1, pMatrix3b);
    TGeoShape* basesub_3 = new TGeoCompositeShape("GE", basesub3);
    TGeoVolume* basesub_log3 = new TGeoVolume("", basesub_3, pMedAl);
    basesub_log3->SetVisLeaves(kTRUE);
    basesub_log3->SetLineColor(4);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 0.00;
    dy = 27.0 + 18.0 / 2.0 - 8.0;
    dz = 0.000;
    TGeoCombiTrans* pMatrix4 = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 17.000;
    dy = 18.000;
    dz = 2.2000;
    TGeoShape* base4 = new TGeoBBox("base4", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log4 = new TGeoVolume("base_log4", base4, pMedAl);
    base_log4->SetVisLeaves(kTRUE);
    base_log4->SetLineColor(2);

    TGeoSubtraction* basesub4 = new TGeoSubtraction(basesub_3, base4, pMatrix1, pMatrix4);
    TGeoShape* basesub_4 = new TGeoCompositeShape("GE", basesub4);
    TGeoVolume* basesub_log4 = new TGeoVolume("", basesub_4, pMedAl);
    basesub_log4->SetVisLeaves(kTRUE);
    basesub_log4->SetLineColor(4);

    // Combi transformation:
    dx = 0.00;
    dy = -30.0 - 18.0 / 2.0 + 11.0;
    dz = 0.000;
    TGeoCombiTrans* pMatrix4b = new TGeoCombiTrans("", dx, dy, dz, 0);

    TGeoSubtraction* basesub5 = new TGeoSubtraction(basesub_4, base4, pMatrix1, pMatrix4b);
    TGeoShape* basesub_5 = new TGeoCompositeShape("GE", basesub5);
    TGeoVolume* basesub_log5 = new TGeoVolume("plane1", basesub_5, pMedAl);
    basesub_log5->SetVisLeaves(kTRUE);
    basesub_log5->SetLineColor(18);

    pWorld->AddNode(basesub_log5, 0, pMatrix1);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 0.000;
    dy = 0.000;
    dz = 0.00;
    TGeoCombiTrans* pMatrix6 = new TGeoCombiTrans("", dx, dy, dz, 0);
    // Shape: sofia1 type: TGeoTubeSeg
    dx = 39.000;
    dy = 38.000;
    dz = 1.2000;
    TGeoShape* base6 = new TGeoBBox("base6", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log6 = new TGeoVolume("base_log6", base6, pMedAl);
    base_log6->SetVisLeaves(kTRUE);
    base_log6->SetLineColor(2);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 21.000;
    dy = 21.000;
    dz = 2.2000;
    TGeoShape* base7 = new TGeoBBox("base7", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log7 = new TGeoVolume("base_log7", base7, pMedAl);
    base_log7->SetVisLeaves(kTRUE);
    base_log7->SetLineColor(7);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal2 = GetGlobalPosition(pMatrix6);
    // pWorld->AddNode(base_log7, 0, pGlobal2);

    TGeoSubtraction* basesub6 = new TGeoSubtraction(base6, base7, pMatrix1, pMatrix1);
    TGeoShape* basesub_6 = new TGeoCompositeShape("GE", basesub6);
    TGeoVolume* basesub_log6 = new TGeoVolume("", basesub_6, pMedAl);
    basesub_log6->SetVisLeaves(kTRUE);
    basesub_log6->SetLineColor(2);

    // pWorld->AddNode(basesub_log6, 0, pGlobal2);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 39.0 / 2.0 + 2.5;
    dy = 17.0 / 2.0 + 1.75;
    dz = 0.000;
    TGeoCombiTrans* pMatrix7 = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 5.000;
    dy = 3.500;
    dz = 1.2000;
    TGeoShape* base8 = new TGeoBBox("base8", dx / 2., dy / 2., dz / 2.);

    TGeoUnion* basesub7 = new TGeoUnion(basesub_6, base8, pMatrix1, pMatrix7);
    TGeoShape* basesub_7 = new TGeoCompositeShape("GE", basesub7);
    TGeoVolume* basesub_log7 = new TGeoVolume("", basesub_7, pMedAl);
    basesub_log7->SetVisLeaves(kTRUE);
    basesub_log7->SetLineColor(2);

    // pWorld->AddNode(basesub_log7, 0, pGlobal2);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 39.0 / 2.0 + 2.5;
    dy = -17.0 / 2.0 - 1.75;
    dz = 0.000;
    TGeoCombiTrans* pMatrix7b = new TGeoCombiTrans("", dx, dy, dz, 0);

    TGeoUnion* basesub8 = new TGeoUnion(basesub_7, base8, pMatrix1, pMatrix7b);
    TGeoShape* basesub_8 = new TGeoCompositeShape("GE", basesub8);
    TGeoVolume* basesub_log8 = new TGeoVolume("plane2", basesub_8, pMedAl);
    basesub_log8->SetVisLeaves(kTRUE);
    basesub_log8->SetLineColor(29);

    pWorld->AddNode(basesub_log8, 0, pMatrix6);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 20.000;
    dy = 20.000;
    dz = 1.000;

    TGeoShape* Detector1 = new TGeoBBox("Detector_1", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* Detector_log1 = new TGeoVolume(detName, Detector1, pMedAr);
    Detector_log1->SetVisLeaves(kTRUE);
    Detector_log1->SetLineColor(3);

    pWorld->AddNode(Detector_log1, 0, pMatrix6);

    // AddSensitiveVolume(Detector_log1);
    // fNbOfSensitiveVol+=1;

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 0.000;
    dy = 0.000;
    dz = -1.800 - 0.8 / 2.0;
    TGeoCombiTrans* pMatrix9 = new TGeoCombiTrans("", dx, dy, dz, 0);
    // Shape: sofia1 type: TGeoTubeSeg
    dx = 39.000;
    dy = 38.000;
    dz = 0.8000;
    TGeoShape* base9 = new TGeoBBox("base9", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log9 = new TGeoVolume("base_log9", base9, pMedAl);
    base_log9->SetVisLeaves(kTRUE);
    base_log9->SetLineColor(2);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal3 = GetGlobalPosition(pMatrix9);
    // pWorld->AddNode(base_log9, 0, pGlobal3);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 30.350;
    dy = 26.500;
    dz = 1.8000;
    TGeoShape* base10 = new TGeoBBox("base10", dx / 2., dy / 2., dz / 2.);

    TGeoSubtraction* basesub9 = new TGeoSubtraction(base9, base10, pMatrix1, pMatrix1);
    TGeoShape* basesub_9 = new TGeoCompositeShape("GE", basesub9);
    TGeoVolume* basesub_log9 = new TGeoVolume("plane3", basesub_9, pMedAl);
    basesub_log9->SetVisLeaves(kTRUE);
    basesub_log9->SetLineColor(29);

    pWorld->AddNode(basesub_log9, 0, pMatrix9);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 0.000;
    dy = 0.000;
    dz = -1.800 - 0.8 - 0.5 / 2.0;
    TGeoCombiTrans* pMatrix11 = new TGeoCombiTrans("", dx, dy, dz, 0);
    // Shape: sofia1 type: TGeoTubeSeg
    dx = 39.000;
    dy = 38.000;
    dz = 0.5000;
    TGeoShape* base11 = new TGeoBBox("base11", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log11 = new TGeoVolume("base_log11", base11, pMedAl);
    base_log11->SetVisLeaves(kTRUE);
    base_log11->SetLineColor(28);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal4 = GetGlobalPosition(pMatrix11);
    // pWorld->AddNode(base_log11, 1, pGlobal4);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 25.250;
    dy = 26.500;
    dz = 2.2000;
    TGeoShape* base12 = new TGeoBBox("base12", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log12 = new TGeoVolume("base_log12", base12, pMedAl);
    base_log12->SetVisLeaves(kTRUE);
    base_log12->SetLineColor(28);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal2 = GetGlobalPosition(pMatrix6);
    // pWorld->AddNode(base_log7, 0, pGlobal2);

    TGeoSubtraction* basesub13 = new TGeoSubtraction(base11, base12, pMatrix1, pMatrix1);
    TGeoShape* basesub_13 = new TGeoCompositeShape("GE", basesub13);
    TGeoVolume* basesub_log13 = new TGeoVolume("", basesub_13, pMedAl);
    basesub_log13->SetVisLeaves(kTRUE);
    basesub_log13->SetLineColor(28);

    // pWorld->AddNode(basesub_log13, 0, pGlobal4);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = -39.0 / 2.0 - 2.5;
    dy = -24.5 / 2.0 + 1.50;
    dz = 0.000;
    TGeoCombiTrans* pMatrix12 = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 5.000;
    dy = 3.000;
    dz = 0.500;
    TGeoShape* base13 = new TGeoBBox("base13", dx / 2., dy / 2., dz / 2.);

    TGeoUnion* basesub14 = new TGeoUnion(basesub_13, base13, pMatrix1, pMatrix12);
    TGeoShape* basesub_14 = new TGeoCompositeShape("GE", basesub14);
    TGeoVolume* basesub_log14 = new TGeoVolume("", basesub_14, pMedAl);
    basesub_log14->SetVisLeaves(kTRUE);
    basesub_log14->SetLineColor(28);
    // pWorld->AddNode(basesub_log14, 0, pGlobal4);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = -39.0 / 2.0 - 2.5;
    dy = +24.5 / 2.0 - 1.50;
    dz = 0.000;
    TGeoCombiTrans* pMatrix12b = new TGeoCombiTrans("", dx, dy, dz, 0);
    TGeoUnion* basesub15 = new TGeoUnion(basesub_14, base13, pMatrix1, pMatrix12b);
    TGeoShape* basesub_15 = new TGeoCompositeShape("GE", basesub15);
    TGeoVolume* basesub_log15 = new TGeoVolume("plane4", basesub_15, pMedAl);
    basesub_log15->SetVisLeaves(kTRUE);
    basesub_log15->SetLineColor(29);
    pWorld->AddNode(basesub_log15, 0, pMatrix11);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 0.000;
    dy = 0.000;
    dz = -1.800 - 0.8 - 0.5 - 0.5;
    TGeoCombiTrans* pMatrix14 = new TGeoCombiTrans("", dx, dy, dz, 0);
    // Shape: sofia1 type: TGeoTubeSeg
    dx = 39.000;
    dy = 38.000;
    dz = 1.0000;
    TGeoShape* base14 = new TGeoBBox("base14", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log14 = new TGeoVolume("base_log14", base14, pMedAl);
    base_log14->SetVisLeaves(kTRUE);
    base_log14->SetLineColor(10);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal5 = GetGlobalPosition(pMatrix14);
    // pWorld->AddNode(base_log14, 0, pGlobal5);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 21.000;
    dy = 21.000;
    dz = 2.2000;
    TGeoShape* base15 = new TGeoBBox("base15", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log15 = new TGeoVolume("base_log15", base15, pMedAl);
    base_log15->SetVisLeaves(kTRUE);
    base_log15->SetLineColor(10);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal5 = GetGlobalPosition(pMatrix14);
    // pWorld->AddNode(base_log15, 0, pGlobal5);

    TGeoSubtraction* basesub16 = new TGeoSubtraction(base14, base15, pMatrix1, pMatrix1);
    TGeoShape* basesub_16 = new TGeoCompositeShape("GE", basesub16);
    TGeoVolume* basesub_log16 = new TGeoVolume("", basesub_16, pMedAl);
    basesub_log16->SetVisLeaves(kTRUE);
    basesub_log16->SetLineColor(10);

    // pWorld->AddNode(basesub_log16, 0, pGlobal5);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 39.0 / 2.0 - 8.2 - 1.75;
    dy = 38.0 / 2.0 + 2.5;
    dz = 0.000;
    TGeoCombiTrans* pMatrix15 = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 3.500;
    dy = 5.000;
    dz = 1.000;
    TGeoShape* base16 = new TGeoBBox("base16", dx / 2., dy / 2., dz / 2.);

    TGeoUnion* basesub17 = new TGeoUnion(basesub_16, base16, pMatrix1, pMatrix15);
    TGeoShape* basesub_17 = new TGeoCompositeShape("GE", basesub17);
    TGeoVolume* basesub_log17 = new TGeoVolume("", basesub_17, pMedAl);
    basesub_log17->SetVisLeaves(kTRUE);
    basesub_log17->SetLineColor(10);

    // pWorld->AddNode(basesub_log17, 0, pGlobal5);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 39.0 / 2.0 - 8.2 - 1.75;
    dy = -38.0 / 2.0 - 2.5;
    dz = 0.000;
    TGeoCombiTrans* pMatrix15b = new TGeoCombiTrans("", dx, dy, dz, 0);

    TGeoUnion* basesub18 = new TGeoUnion(basesub_17, base16, pMatrix1, pMatrix15b);
    TGeoShape* basesub_18 = new TGeoCompositeShape("GE", basesub18);
    TGeoVolume* basesub_log18 = new TGeoVolume("", basesub_18, pMedAl);
    basesub_log18->SetVisLeaves(kTRUE);
    basesub_log18->SetLineColor(10);

    // pWorld->AddNode(basesub_log18, 0, pGlobal5);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = -39.0 / 2.0 + 6.8 + 1.75;
    dy = 38.0 / 2.0 + 2.5;
    dz = 0.000;
    TGeoCombiTrans* pMatrix15c = new TGeoCombiTrans("", dx, dy, dz, 0);

    TGeoUnion* basesub19 = new TGeoUnion(basesub_18, base16, pMatrix1, pMatrix15c);
    TGeoShape* basesub_19 = new TGeoCompositeShape("GE", basesub19);
    TGeoVolume* basesub_log19 = new TGeoVolume("", basesub_19, pMedAl);
    basesub_log19->SetVisLeaves(kTRUE);
    basesub_log19->SetLineColor(10);

    // pWorld->AddNode(basesub_log19, 0, pGlobal5);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = -39.0 / 2.0 + 6.8 + 1.75;
    dy = -38.0 / 2.0 - 2.5;
    dz = 0.000;
    TGeoCombiTrans* pMatrix15d = new TGeoCombiTrans("", dx, dy, dz, 0);

    TGeoUnion* basesub20 = new TGeoUnion(basesub_19, base16, pMatrix1, pMatrix15d);
    TGeoShape* basesub_20 = new TGeoCompositeShape("GE", basesub20);
    TGeoVolume* basesub_log20 = new TGeoVolume("plane5", basesub_20, pMedAl);
    basesub_log20->SetVisLeaves(kTRUE);
    basesub_log20->SetLineColor(29);
    pWorld->AddNode(basesub_log20, 0, pMatrix14);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 21.000;
    dy = 21.000;
    dz = 1.0000;

    TGeoShape* Detector2 = new TGeoBBox("Detector_2", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* Detector_log2 = new TGeoVolume("MWPC12", Detector2, pMedAr);
    Detector_log2->SetVisLeaves(kTRUE);
    Detector_log2->SetLineColor(3);

    // pWorld->AddNode(Detector_log2, 0, pGlobal5);

    // AddSensitiveVolume(Detector_log2);
    // fNbOfSensitiveVol+=1;

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 0.000;
    dy = 0.000;
    dz = -1.800 - 0.8 - 0.5 - 1.0 - 0.6;
    TGeoCombiTrans* pMatrix17 = new TGeoCombiTrans("", dx, dy, dz, 0);
    // Shape: sofia1 type: TGeoTubeSeg
    dx = 39.000;
    dy = 38.000;
    dz = 1.2000;
    TGeoShape* base17 = new TGeoBBox("base17", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log17 = new TGeoVolume("base_log17", base17, pMedAl);
    base_log17->SetVisLeaves(kTRUE);
    base_log17->SetLineColor(7);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal6 = GetGlobalPosition(pMatrix17);
    // pWorld->AddNode(base_log14, 0, pGlobal5);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 21.000;
    dy = 21.000;
    dz = 2.2000;
    TGeoShape* base18 = new TGeoBBox("base18", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log18 = new TGeoVolume("base_log18", base18, pMedAl);
    base_log18->SetVisLeaves(kTRUE);
    base_log18->SetLineColor(7);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal5 = GetGlobalPosition(pMatrix14);
    // pWorld->AddNode(base_log15, 0, pGlobal5);

    TGeoSubtraction* basesub21 = new TGeoSubtraction(base17, base18, pMatrix1, pMatrix1);
    TGeoShape* basesub_21 = new TGeoCompositeShape("GE", basesub21);
    TGeoVolume* basesub_log21 = new TGeoVolume("", basesub_21, pMedAl);
    basesub_log21->SetVisLeaves(kTRUE);
    basesub_log21->SetLineColor(7);

    // pWorld->AddNode(basesub_log21, 0, pGlobal6);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 4.000;
    dy = 6.600;
    dz = 4.000;
    TGeoShape* base19 = new TGeoBBox("base19", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log19 = new TGeoVolume("base_log19", base19, pMedAl);
    base_log19->SetVisLeaves(kTRUE);
    base_log19->SetLineColor(18);

    dx = 39.0 / 2.0 - 2.0;
    dy = 38.0 / 2.0 + 3.3;
    dz = -2.000 - 0.6;
    TGeoCombiTrans* pMatrix17b = new TGeoCombiTrans("gaz", dx, dy, dz, 0);
    // TGeoCombiTrans* pGlobal6b = GetGlobalPosition(pMatrix17b);
    pWorld->AddNode(base_log19, 0, pMatrix17b);

    dx = -39.0 / 2.0 - 2.0;
    dy = -38.0 / 2.0 + 3.3;
    dz = 2.000 - 0.6;
    TGeoCombiTrans* pMatrix18 = new TGeoCombiTrans("", dx, dy, dz, 0);

    TGeoUnion* basesub22 = new TGeoUnion(basesub_21, base19, pMatrix1, pMatrix18);
    TGeoShape* basesub_22 = new TGeoCompositeShape("GE", basesub22);
    TGeoVolume* basesub_log22 = new TGeoVolume("Plane6", basesub_22, pMedAl);
    basesub_log22->SetVisLeaves(kTRUE);
    basesub_log22->SetLineColor(17);

    pWorld->AddNode(basesub_log22, 0, pMatrix17);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 0.000;
    dy = 0.000;
    dz = -1.800 - 0.8 - 0.5 - 1.0 - 1.2 - 0.2 - 0.0020;
    TGeoCombiTrans* pMatrix19 = new TGeoCombiTrans("", dx, dy, dz, 0);
    // Shape: sofia1 type: TGeoTubeSeg
    dx = 22.950;
    dy = 22.950;
    dz = 0.4000;
    TGeoShape* base20 = new TGeoBBox("base20", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log20 = new TGeoVolume("base_log20", base20, pMedAl);
    base_log20->SetVisLeaves(kTRUE);
    base_log20->SetLineColor(17);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal7 = GetGlobalPosition(pMatrix19);
    // pWorld->AddNode(base_log14, 0, pGlobal5);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 21.000;
    dy = 21.000;
    dz = 1.0000;
    TGeoShape* base21 = new TGeoBBox("base21", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log21 = new TGeoVolume("base_log21", base21, pMedAl);
    base_log21->SetVisLeaves(kTRUE);
    base_log21->SetLineColor(7);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal7 = GetGlobalPosition(pMatrix19);
    // pWorld->AddNode(base_log21, 0, pGlobal7);

    TGeoSubtraction* basesub23 = new TGeoSubtraction(base20, base21, pMatrix1, pMatrix1);
    TGeoShape* basesub_23 = new TGeoCompositeShape("GE", basesub23);
    TGeoVolume* basesub_log23 = new TGeoVolume("", basesub_23, pMedAl);
    basesub_log23->SetVisLeaves(kTRUE);
    basesub_log23->SetLineColor(17);
    pWorld->AddNode(basesub_log23, 0, pMatrix19);

    dx = 0.000;
    dy = 0.000;
    dz = 0.60 + 0.0010 + 0.2;
    TGeoCombiTrans* pMatrix19b = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal7b = GetGlobalPosition(pMatrix19b);
    pWorld->AddNode(basesub_log23, 1, pMatrix19b);

    dx = 0.000;
    dy = 0.000;
    dz = -1.800 - 0.8 - 0.5 - 1.0 - 1.2 - 0.0010;
    TGeoCombiTrans* pMatrix20 = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 22.90;
    dy = 22.90;
    dz = 0.0020;
    TGeoShape* base22 = new TGeoBBox("base22", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log22 = new TGeoVolume("base_log22", base22, pMedMylar); // first Kapton
    base_log22->SetVisLeaves(kTRUE);
    base_log22->SetLineColor(28);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal8 = GetGlobalPosition(pMatrix20);
    pWorld->AddNode(base_log22, 1, pMatrix20);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 21.00;
    dy = 21.00;
    dz = 0.0020;
    TGeoShape* base22b = new TGeoBBox("base22b", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log22b = new TGeoVolume("base_log22b", base22b, pMedMylar); // Second Kapton
    base_log22b->SetVisLeaves(kTRUE);
    base_log22b->SetLineColor(28);

    dx = 0.000;
    dy = 0.000;
    dz = -1.800 - 0.8 - 0.5 + 0.0010;
    TGeoCombiTrans* pMatrix20b = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal8b = GetGlobalPosition(pMatrix20b);
    pWorld->AddNode(base_log22b, 1, pMatrix20b);

    dx = 0.000;
    dy = 0.000;
    dz = -1.800 - 0.0010;
    TGeoCombiTrans* pMatrix20c = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Position Mother Volume
    // TGeoCombiTrans* pGlobal8c = GetGlobalPosition(pMatrix20c);
    pWorld->AddNode(base_log22b, 2, pMatrix20c);

    dx = 0.000;
    dy = 0.000;
    dz = 0.60 + 0.0010;
    TGeoCombiTrans* pMatrix20d = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Position Mother Volume
    pWorld->AddNode(base_log22, 2, pMatrix20d);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 39.0 / 2.0 + 7.0 / 2.0;
    dy = 0.000;
    dz = -1.200;
    TGeoCombiTrans* pMatrix21 = new TGeoCombiTrans("", dx, dy, dz, 0);
    // Shape: sofia1 type: TGeoTubeSeg
    dx = 7.0000;
    dy = 16.950;
    dz = 0.4000;
    TGeoShape* base23 = new TGeoBBox("base23", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log23 = new TGeoVolume("base_log23", base23, pMedMylar);
    base_log23->SetVisLeaves(kTRUE);
    base_log23->SetLineColor(3);

    // Position Mother Volume
    pWorld->AddNode(base_log23, 0, pMatrix21);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = -39.0 / 2.0 - 7.0 / 2.0;
    dy = 0.000;
    dz = -1.800 - 0.8 - 0.5 / 2.0;
    TGeoCombiTrans* pMatrix21b = new TGeoCombiTrans("", dx, dy, dz, 0);

    // Position Mother Volume
    pWorld->AddNode(base_log23, 1, pMatrix21b);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = -0.7000;
    dy = 38.0 / 2.0 + 3.5;
    dz = -1.800 - 0.8 - 0.5 - 0.5;

    TGeoRotation* rot_1 = new TGeoRotation("rot_1");
    rot_1->RotateZ(90.0);

    TGeoCombiTrans* pMatrix21c = new TGeoCombiTrans("", dx, dy, dz, rot_1);
    TGeoCombiTrans* pMatrix21d = new TGeoCombiTrans("", dx, -1.0 * dy, dz, rot_1);

    // Position Mother Volume
    pWorld->AddNode(base_log23, 2, pMatrix21c);
    pWorld->AddNode(base_log23, 3, pMatrix21d);

    // TRANSFORMATION MATRICES
    // Combi transformation:
    dx = 53.80 / 2.0 + 2.15;
    dy = 0.000;
    dz = -0.60;
    TGeoCombiTrans* pMatrix22 = new TGeoCombiTrans("", dx, dy, dz, 0);
    // Shape: sofia1 type: TGeoTubeSeg
    dx = 16.7;
    dy = 31.0;
    dz = 0.70;
    TGeoShape* base24 = new TGeoBBox("base24", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* base_log24 = new TGeoVolume("base_log24", base24, pMedAl);
    base_log24->SetVisLeaves(kTRUE);
    base_log24->SetLineColor(25);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 6.50;
    dy = 25.0;
    dz = 1.70;
    TGeoShape* base25 = new TGeoBBox("base25", dx / 2., dy / 2., dz / 2.);

    dx = -8.35 + 3.10;
    dy = 0.000;
    dz = 0.000;
    TGeoCombiTrans* pMatrix23 = new TGeoCombiTrans("", dx, dy, dz, 0);

    TGeoSubtraction* basesub24 = new TGeoSubtraction(base24, base25, pMatrix1, pMatrix23);
    TGeoShape* basesub_24 = new TGeoCompositeShape("GE", basesub24);
    TGeoVolume* basesub_log24 = new TGeoVolume("", basesub_24, pMedAl);
    basesub_log24->SetVisLeaves(kTRUE);
    basesub_log24->SetLineColor(25);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 4.10;
    dy = 9.0;
    dz = 1.70;
    TGeoShape* base26 = new TGeoBBox("base25", dx / 2., dy / 2., dz / 2.);

    dx = -8.35 + 6.20 + 2.0;
    dy = 0.000;
    dz = 0.000;
    TGeoCombiTrans* pMatrix24 = new TGeoCombiTrans("", dx, dy, dz, 0);

    TGeoSubtraction* basesub25 = new TGeoSubtraction(basesub_24, base26, pMatrix1, pMatrix24);
    TGeoShape* basesub_25 = new TGeoCompositeShape("GE", basesub25);
    TGeoVolume* basesub_log25 = new TGeoVolume("", basesub_25, pMedAl);
    basesub_log25->SetVisLeaves(kTRUE);
    basesub_log25->SetLineColor(25);

    TGeoRotation* rot_2 = new TGeoRotation("rot_2");
    rot_2->RotateZ(60.0);

    dx = 8.35;
    dy = 15.50;
    dz = 0.000;
    TGeoCombiTrans* pMatrix25 = new TGeoCombiTrans("", dx, dy, dz, rot_2);

    // Shape: sofia1 type: TGeoTubeSeg
    dx = 10.72;
    dy = 19.72;
    dz = 1.70;
    TGeoShape* base27 = new TGeoBBox("base27", dx / 2., dy / 2., dz / 2.);

    TGeoSubtraction* basesub26 = new TGeoSubtraction(basesub_25, base27, pMatrix1, pMatrix25);
    TGeoShape* basesub_26 = new TGeoCompositeShape("GE", basesub26);
    TGeoVolume* basesub_log26 = new TGeoVolume("", basesub_26, pMedAl);
    basesub_log26->SetVisLeaves(kTRUE);
    basesub_log26->SetLineColor(25);

    TGeoRotation* rot_3 = new TGeoRotation("rot_3");
    rot_3->RotateZ(-60.0);

    dx = 8.35;
    dy = -15.50;
    dz = 0.000;
    TGeoCombiTrans* pMatrix26 = new TGeoCombiTrans("", dx, dy, dz, rot_3);

    TGeoSubtraction* basesub27 = new TGeoSubtraction(basesub_26, base27, pMatrix1, pMatrix26);
    TGeoShape* basesub_27 = new TGeoCompositeShape("GE", basesub27);
    TGeoVolume* basesub_log27 = new TGeoVolume("", basesub_27, pMedAl);
    basesub_log27->SetVisLeaves(kTRUE);
    basesub_log27->SetLineColor(24);

    pWorld->AddNode(basesub_log27, 0, pMatrix22);

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
