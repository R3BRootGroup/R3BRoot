/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2023 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

//    General macro for generating the R3B-Music geometry
//
//    Author: Jose Luis <j.l.rodriguez.sanchez@udc.es>
//
//    Last Update: 14/02/2023
//
//    Comments:
//

#include "TGeoManager.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>

void create_music_geo(const char* geoTag = "v2023.2")
{
    // --------------------------------------------------------------------------
    //
    // Execute macro:  root -l
    //                 .L create_music_geo.C
    //                 create_music_geo()
    // --------------------------------------------------------------------------

    // Create a null rotation
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

    // -------   Geometry file name (output)   ----------------------------------
    TString geoFileName = geoPath + "/geometry/music_";
    geoFileName = geoFileName + geoTag + ".geo.root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mCu = geoMedia->getMedium("copper");
    if (!mCu)
        Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCu);
    TGeoMedium* pMed1 = gGeoMan->GetMedium("copper");
    if (!pMed1)
        Fatal("Main", "Medium copper not found");

    FairGeoMedium* mAl = geoMedia->getMedium("aluminium");
    if (!mAl)
        Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAl);
    TGeoMedium* pMed2 = gGeoMan->GetMedium("aluminium");
    if (!pMed2)
        Fatal("Main", "Medium aluminium not found");

    FairGeoMedium* mSteel = geoMedia->getMedium("Steel");
    if (!mSteel)
        Fatal("Main", "FairMedium Steel not found");
    geoBuild->createMedium(mSteel);
    TGeoMedium* pMed3 = gGeoMan->GetMedium("Steel");
    if (!pMed3)
        Fatal("Main", "Medium Steel not found");

    FairGeoMedium* mMylar = geoMedia->getMedium("mylar");
    if (!mMylar)
        Fatal("Main", "FairMedium mylar not found");
    geoBuild->createMedium(mMylar);
    TGeoMedium* pMed4 = gGeoMan->GetMedium("mylar");
    if (!pMed4)
        Fatal("Main", "Medium mylar not found");

    FairGeoMedium* mAr = geoMedia->getMedium("ArCO2");
    if (!mAr)
        Fatal("Main", "FairMedium ArCO2 not found");
    geoBuild->createMedium(mAr);
    TGeoMedium* pMed5 = gGeoMan->GetMedium("ArCO2");
    if (!pMed5)
        Fatal("Main", "Medium ArCO2 not found");
    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName("MUSICgeom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    gGeoMan->SetTopVolume(top);
    // --------------------------------------------------------------------------

    TGeoCombiTrans* t0 = new TGeoCombiTrans();
    Double_t dx, dy, dz;
    Double_t thx, thy, thz;
    Double_t phx, phy, phz;
    Int_t nbanodes = 8;

    TGeoVolumeAssembly* MusicDet = new TGeoVolumeAssembly("MUSICWorld");
    top->AddNode(MusicDet, 1, t0);

    // Anode Shape & volume: TGeoBBox
    dx = 20.0;
    dy = 20.0;
    dz = 5.00;
    TGeoVolume* Anode = gGeoManager->MakeBox("Anode", pMed5, dx / 2., dy / 2., dz / 2.);
    Anode->SetVisLeaves(kTRUE);
    Anode->SetLineColor(2);

    for (Int_t i = 0; i < nbanodes; i++)
    {
        dz = -20.0 + 2.5 + 5. * i;
        dy = 0.000;
        dx = 0.000;
        TGeoCombiTrans* pMatrix = new TGeoCombiTrans("", dx, dy, dz, fRefRot);
        MusicDet->AddNode(Anode, i + 1, pMatrix);
    }

    // Screening anodes, Shape & volume: TGeoBBox
    dx = 20.0;
    dy = 20.0;
    dz = 2.00;
    TGeoVolume* SAnode = gGeoManager->MakeBox("Screening", pMed5, dx / 2., dy / 2., dz / 2.);
    SAnode->SetVisLeaves(kTRUE);
    SAnode->SetLineColor(5);

    dx = 0.000;
    dy = 0.000;
    dz = -21.0;
    TGeoRotation* arot1 = new TGeoRotation();
    TGeoCombiTrans* pMatrixd1 = new TGeoCombiTrans("", dx, dy, dz, arot1);
    MusicDet->AddNode(SAnode, 0, pMatrixd1);

    dz = 21.0;
    TGeoRotation* arot2 = new TGeoRotation();
    TGeoCombiTrans* pMatrixd2 = new TGeoCombiTrans("", dx, dy, dz, arot2);
    MusicDet->AddNode(SAnode, 1, pMatrixd2);

    // Windows, Shape & volume: TGeoBBox
    dx = 20.0;
    dy = 20.0;
    dz = 0.0025;
    TGeoVolume* Wind = gGeoManager->MakeBox("Window", pMed4, dx / 2., dy / 2., dz / 2.);
    Wind->SetVisLeaves(kTRUE);

    dx = 0.000;
    dy = 0.000;
    dz = -22.0 - 0.0025 / 2.;
    TGeoRotation* arot3 = new TGeoRotation();
    TGeoCombiTrans* pMatrixd3 = new TGeoCombiTrans("", dx, dy, dz, arot3);
    MusicDet->AddNode(Wind, 0, pMatrixd3);

    dz = 22.0 + 0.0025 / 2.;
    TGeoRotation* arot4 = new TGeoRotation();
    TGeoCombiTrans* pMatrixd4 = new TGeoCombiTrans("", dx, dy, dz, arot4);
    MusicDet->AddNode(Wind, 1, pMatrixd4);

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
