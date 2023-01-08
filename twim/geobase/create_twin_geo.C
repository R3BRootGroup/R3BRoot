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

//   ----- General macro for generating the Twin-Music geometry
//
//         Author: Jose Luis <joseluis.rodriguez.sanchez@usc.es>
//
//         Last Update: 20/11/2022 (Jose Luis)
//
//         Comments:
//

#include "TGeoManager.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>

void create_twin_geo(const char* geoTag = "v22")
{
    // --------------------------------------------------------------------------
    // Configurable geometry for the TWIM-MUSIC.
    // Use this macro to create root files with the different configurations
    // and positions/angles of the detector.
    //
    // Execute macro:  root -l
    //                 .L create_twin_geo.C
    //                 create_twin_geo()
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
    TString geoFileName = geoPath + "/geometry/twinmusic_";
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
    TGeoMedium* pMed4 = gGeoMan->GetMedium("mylar");
    if (!pMed4)
        Fatal("Main", "Medium mylar not found");

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName("TWIMgeom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    gGeoMan->SetTopVolume(top);
    // --------------------------------------------------------------------------

    // out-of-file geometry definition
    Double_t dx = 0., dy = 0., dz = 0.;
    TGeoRotation* rotg = new TGeoRotation();
    TGeoCombiTrans* pMatrix0 = new TGeoCombiTrans("", dx, dy, dz, rotg);

    // Defintion of the Mother Volume
    TGeoShape* pbox1 = new TGeoBBox("twimbox1", 12. / 2.0, 23. / 2.0, 22.1);
    TGeoCombiTrans* t_box1 = new TGeoCombiTrans("t_box1", 6., 0., 0., fRefRot);
    t_box1->RegisterYourself();

    TGeoShape* pbox2 = new TGeoBBox("twimbox2", 12. / 2.0, 23. / 2.0, 22.1);
    TGeoCombiTrans* t_box2 = new TGeoCombiTrans("t_box2", -6., 0., 0., fRefRot);
    t_box2->RegisterYourself();

    TGeoCompositeShape* pMWorld = new TGeoCompositeShape("TWIMbox", "twimbox1:t_box1 + twimbox2:t_box2");

    TGeoVolume* pWorld = new TGeoVolume("TWIMWorld", pMWorld, pMedAr);

    TGeoCombiTrans* t0 = new TGeoCombiTrans();

    // Adding the Mother Volume
    top->AddNode(pWorld, 0, t0);

    // --------------- Detector --------------------------------------

    TGeoVolumeAssembly* twim = new TGeoVolumeAssembly("TWIMDet");

    // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
    // Shape: Anode type: TGeoBBox
    dx = 11.000;
    dy = 11.000;
    dz = 2.5000;
    Float_t xcathode = 0.05; // mm
    TGeoShape* AnodeBox = new TGeoBBox("TWIMAnode", dx / 2., dy / 2., dz / 2.);
    // Volume: TOFLog
    TGeoVolume* AnodeLog = new TGeoVolume("TwinLog", AnodeBox, pMedAr);
    AnodeLog->SetVisLeaves(kTRUE);
    AnodeLog->SetLineColor(2);

    Int_t nbSections = 4;
    Int_t nbAnodes = 16;
    TGeoCombiTrans* pMatrixAnode[nbSections * nbAnodes];

    Int_t nbanode = 0;
    Float_t offsetx = 11. / 2. + xcathode / 2.;
    Float_t secposx[4] = { offsetx, offsetx, -offsetx, -offsetx };
    Float_t secposy[4] = { -11. / 2., 11. / 2., 11. / 2., -11. / 2. };
    for (Int_t i = 0; i < nbSections; i++)
    { // Sections
        dx = secposx[i];
        dy = secposy[i];
        for (Int_t j = 0; j < nbAnodes; j++)
        { // Anodes per section

            nbanode = i * nbAnodes + j;
            dz = 2.5 * (-7.5 + j);

            pMatrixAnode[nbanode] = new TGeoCombiTrans("", dx, dy, dz, rotg);
            twim->AddNode(AnodeLog, nbanode + 1, pMatrixAnode[nbanode]);
        }
    }

    dx = xcathode;
    dy = 22.0;
    dz = nbAnodes * 2.5 + 2. * 2.;
    TGeoShape* twin1 = new TGeoBBox("", dx / 2., dy / 2., dz / 2.);
    // Volume:
    TGeoVolume* twin_log = new TGeoVolume("cathode", twin1, pMedCu);
    twin_log->SetVisLeaves(kTRUE);
    twin_log->SetLineColor(3);

    // Position Mother Volume
    TGeoCombiTrans* pMatrix1 = new TGeoCombiTrans("", 0., 0., 0., rotg);
    twim->AddNode(twin_log, 0, pMatrix1);
    pWorld->AddNode(twim, 0, pMatrix0);

    // Screening anodes, Shape & volume: TGeoBBox
    dx = 11.0;
    dy = 22.0;
    dz = 2.00;
    TGeoVolume* SAnode = gGeoManager->MakeBox("TwimScreening", pMedAr, dx / 2., dy / 2., dz / 2.);
    SAnode->SetVisLeaves(kTRUE);
    SAnode->SetLineColor(5);

    dx = secposx[0];
    dy = 0.000;
    dz = -21.0;
    TGeoRotation* arot1 = new TGeoRotation();
    TGeoCombiTrans* pMatrixd1 = new TGeoCombiTrans("", dx, dy, dz, arot1);
    pWorld->AddNode(SAnode, 0, pMatrixd1);

    dx = secposx[2];
    dy = 0.000;
    dz = -21.0;
    TGeoCombiTrans* pMatrixd1b = new TGeoCombiTrans("", dx, dy, dz, arot1);
    pWorld->AddNode(SAnode, 1, pMatrixd1b);

    dx = secposx[0];
    dz = 21.0;
    TGeoRotation* arot2 = new TGeoRotation();
    TGeoCombiTrans* pMatrixd2 = new TGeoCombiTrans("", dx, dy, dz, arot2);
    pWorld->AddNode(SAnode, 2, pMatrixd2);

    dx = secposx[2];
    dz = 21.0;
    TGeoCombiTrans* pMatrixd2b = new TGeoCombiTrans("", dx, dy, dz, arot2);
    pWorld->AddNode(SAnode, 3, pMatrixd2b);

    // Windows, Shape & volume: TGeoBBox
    dx = 22.0;
    dy = 22.0;
    dz = 0.0025;
    TGeoVolume* Wind = gGeoManager->MakeBox("TwimWindow", pMed4, dx / 2., dy / 2., dz / 2.);
    Wind->SetVisLeaves(kTRUE);
    Wind->SetLineColor(13);

    dx = 0.000;
    dy = 0.000;
    dz = -22.0 - 0.0025 / 2.;
    TGeoRotation* arot3 = new TGeoRotation();
    TGeoCombiTrans* pMatrixd3 = new TGeoCombiTrans("", dx, dy, dz, arot3);
    pWorld->AddNode(Wind, 0, pMatrixd3);

    dz = 22.0 + 0.0025 / 2.;
    TGeoRotation* arot4 = new TGeoRotation();
    TGeoCombiTrans* pMatrixd4 = new TGeoCombiTrans("", dx, dy, dz, arot4);
    pWorld->AddNode(Wind, 1, pMatrixd4);

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
}
