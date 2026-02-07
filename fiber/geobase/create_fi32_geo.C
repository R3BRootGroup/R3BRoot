/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

//   ----- General Macro to create the geometry of Fiber-32 detector
//
//         Author: Jose Luis <j.l.rodriguez.sanchez@udc.es>
//
//         Last Update: 10/05/22 (Jose Luis)
//
//         Comments:
//

#include "TGeoManager.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>

void create_fi32_geo(std::string geoTag = "v2022.1")
{
    TGeoManager* gGeoMan = nullptr;

    int number_fibers = 512;
    float detector_width = 50.00;
    float detector_height = 50.00;
    float fiber_thickness = 0.1034;

    // -------   Load media from media file   -----------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    if (geoPath.IsNull())
    {
        std::cerr << "\033[35mERROR: VMCWORKDIR is not defined\033[0m" << std::endl;
        gApplication->Terminate();
    }
    TString medFile = geoPath + "/geometry/media_r3b.geo";
    medFile.ReplaceAll("./", "/");
    medFile.ReplaceAll("//", "/");
    geoFace->setMediaFile(medFile.Data());
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    TString geoFileName = geoPath + "/geometry/fi32_";
    geoFileName = geoFileName + geoTag + ".geo.root";
    geoFileName.ReplaceAll("//", "/");
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("Air");
    if (!mAir)
        Fatal("Main", "FairMedium Air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMed2 = gGeoMan->GetMedium("Air");
    if (!pMed2)
        Fatal("Main", "Medium Air not found");

    FairGeoMedium* mVac = geoMedia->getMedium("vacuum");
    if (!mVac)
        Fatal("Main", "FairMedium vacuum not found");
    geoBuild->createMedium(mVac);
    TGeoMedium* pMed1 = gGeoMan->GetMedium("vacuum");
    if (!pMed1)
        Fatal("Main", "Medium vacuum not found");

    FairGeoMedium* mGfi = geoMedia->getMedium("plasticForGFI");
    if (!mGfi)
        Fatal("Main", "FairMedium plasticForGFI not found");
    geoBuild->createMedium(mGfi);
    TGeoMedium* pMed35 = gGeoMan->GetMedium("plasticForGFI");
    if (!pMed35)
        Fatal("Main", "Medium plasticForGFI not found");

    FairGeoMedium* mAl = geoMedia->getMedium("aluminium");
    if (!mAl)
        Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAl);
    TGeoMedium* pMed21 = gGeoMan->GetMedium("aluminium");
    if (!pMed21)
        Fatal("Main", "Medium aluminium not found");

    FairGeoMedium* mCH2 = geoMedia->getMedium("CH2");
    if (!mCH2)
        Fatal("Main", "FairMedium CH2 not found");
    geoBuild->createMedium(mCH2);
    TGeoMedium* pMed38 = gGeoMan->GetMedium("CH2");
    if (!pMed38)
        Fatal("Main", "Medium CH2 not found");
    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName("FIBER32geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    gGeoMan->SetTopVolume(top);
    // --------------------------------------------------------------------------

    Float_t dx = 0.0;
    Float_t dy = 0.0;
    Float_t dz = 0.0;

    TGeoRotation* pMatrix3 = new TGeoRotation();
    TGeoCombiTrans* pMatrix2 = new TGeoCombiTrans("", dx, dy, dz, pMatrix3);
    TGeoCombiTrans* pMatrix21 = new TGeoCombiTrans("", dx, dy, dz + fiber_thickness, pMatrix3);
    TGeoCombiTrans* pMatrix22 = new TGeoCombiTrans("", dx, dy, dz - fiber_thickness, pMatrix3);

    // World definition
    TGeoVolume* pWorld = gGeoManager->GetTopVolume();
    pWorld->SetVisLeaves(kTRUE);

    TGeoVolume* airbox = gGeoManager->MakeBox(
        "Fiber32World", pMed2, detector_width / 2. + 1, detector_height / 2., fiber_thickness * 2.);
    airbox->SetLineColor(2);
    airbox->SetVisLeaves(kTRUE);

    auto fFiber = new TGeoTube("FiberTube", 0, fiber_thickness / 2, detector_height / 2.);
    auto fFiberLog = new TGeoVolume("Fi32", fFiber, pMed35);
    fFiberLog->SetLineColor(4);
    fFiberLog->SetVisLeaves(kTRUE);

    auto shielding = gGeoManager->MakeBox("Shielding", pMed38, detector_width / 2., detector_height / 2., 0.00025);
    shielding->SetLineColor(12);
    shielding->SetVisLeaves(kTRUE);

    airbox->AddNode(shielding, 1, pMatrix21);
    airbox->AddNode(shielding, 2, pMatrix22);

    TGeoRotation* pMatrixTube = new TGeoRotation();
    pMatrixTube->RotateX(90);

    Int_t real_fiber_id = 1;
    for (int fiber_id = 0; fiber_id < number_fibers; fiber_id++)
    {
        auto shiftx = (fiber_thickness * TMath::Cos(19. / 180 * TMath::Pi()));
        auto shiftz = (fiber_thickness * TMath::Sin(19. / 180 * TMath::Pi())) * ((fiber_id % 2 == 0) ? -1. : 1.) / 2.;
        airbox->AddNode(
            fFiberLog,
            real_fiber_id++,
            new TGeoCombiTrans("", -detector_width / 2 + (fiber_id + 0.5) * shiftx, 0.0, shiftz, pMatrixTube));
    }
    pWorld->AddNode(airbox, 1, pMatrix2);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile geoFile(geoFileName, "RECREATE");
    top->Write();
    // top->Draw();
    geoFile.Close();

    std::cout << "\033[34m Creating geometry:\033[0m "
              << "\033[33m" << geoFileName << " \033[0m" << std::endl;
    std::cout << "Macro finished successfully." << std::endl;
    gApplication->Terminate();
}
