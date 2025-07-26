/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ----------------------------------------------------------------------------
//         Author: M. Feijoo
//
//         Last Update: 06/05/2022
//
//         Comments:
// ----------------------------------------------------------------------------

#include "TROOT.h"
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoVolume.h>
#include <TMath.h>
#include <fstream>
#include <iomanip>
#include <iostream>

// --------------------------------------------------------------------------
// Configurable geometry for the Tof.
// Use this macro to create root files with the different configurations
// and positions/angles of the detector.
//
// Execute macro:  root -l
//                 .L create_tofd_geo.C
//                 create_tofd_geo()
// --------------------------------------------------------------------------

void create_tofd_geo(const char* geoTag = "v2025.6")
{

    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    if (geoPath.IsNull())
    {
        std::cerr << "\033[35mERROR: VMCWORKDIR is not defined\033[0m" << std::endl;
        gApplication->Terminate();
    }

    auto fRefRot = new TGeoRotation();
    TGeoManager* gGeoMan = nullptr;

    // -------   Load media from media file   -----------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString medFile = geoPath + "/geometry/media_r3b.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    TString geoFileName = geoPath + "/geometry/tofd_";
    geoFileName = geoFileName + geoTag + ".geo.root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    auto mAir = geoMedia->getMedium("Air");
    if (!mAir)
        Fatal("Main", "FairMedium Air not found");
    geoBuild->createMedium(mAir);
    auto pAirMedium = gGeoMan->GetMedium("Air");
    if (!pAirMedium)
        Fatal("Main", "Medium Air not found");

    auto mTof = geoMedia->getMedium("plasticForTOF");
    if (!mTof)
        Fatal("Main", "FairMedium plasticForTOF not found");
    geoBuild->createMedium(mTof);
    auto pMedTof = gGeoMan->GetMedium("plasticForTOF");
    if (!pMedTof)
        Fatal("Main", "Medium plasticForTOF not found");

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = static_cast<TGeoManager*>(gROOT->FindObject("FAIRGeom"));
    gGeoMan->SetName("TOFDgeom");
    auto top = new TGeoVolumeAssembly("TOP");
    gGeoMan->SetTopVolume(top);

    // Top Volume
    auto pWorld = gGeoMan->GetTopVolume();
    pWorld->SetVisLeaves(kTRUE);

    // Detector specifications
    const size_t n_planes = 4;
    const size_t n_paddles = 44;

    // Paddle dimensions in cm
    const float dx = 2.7;
    const float dy = 100.0;
    const float dz = 0.5;

    // Positions (for each paddle)
    float x = 0.0, y = 0.0, z = 0.0;
    float shift = 0.0;
    const float air_gap_paddles = 0.04;
    const float air_gap_planes = 3.0;

    // Plane definition
    auto Plane = gGeoMan->MakeBox(
        "Plane", pAirMedium, dx / 2. * n_paddles + dx / 4. + (n_paddles - 1) * air_gap_paddles / 2., dy / 2., dz / 2.);
    Plane->SetLineColor(1);

    auto detector_width = n_paddles * dx + (n_paddles - 1) * air_gap_paddles;
    auto tofd = new TGeoVolumeAssembly("TOFD");

    auto Paddle = gGeoMan->MakeBox("Paddle", pMedTof, dx / 2., dy / 2., dz / 2.);
    Paddle->SetVisLeaves(kTRUE);
    Paddle->SetLineColor(kBlue);

    for (size_t n = 0; n < n_paddles; n++)
    {
        x = -detector_width / 2 + dx / 2 + n * (dx + air_gap_paddles);

        auto pMatrix = new TGeoCombiTrans("", x, y, z, fRefRot);
        pMatrix->RegisterYourself();
        Plane->AddNode(Paddle, n + 1, pMatrix);
    }

    for (size_t m = 0; m < n_planes; m++)
    {
        shift = (m % 2 == 0) ? -dx / 4. : dx / 4.;

        auto z_plane = -dz * n_planes / 2 - (n_planes - 1) * air_gap_planes / 2 + dz / 2 + m * (dz + air_gap_planes) +
                       (m > 1 ? 1.5 : -1.5);
        auto pMatrix_planes = new TGeoCombiTrans("", shift, y, z_plane, fRefRot);
        pMatrix_planes->RegisterYourself();
        tofd->AddNode(Plane, m + 1, pMatrix_planes);
    }

    auto t0 = new TGeoCombiTrans();
    t0->RegisterYourself();
    pWorld->AddNode(tofd, 1, t0);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile geoFile(geoFileName, "RECREATE");
    top->Write();
    // top->Draw("ogl");
    geoFile.Close();
    std::cout << "\033[34m Creating geometry:\033[0m "
              << "\033[33m" << geoFileName << " \033[0m" << std::endl;
    std::cout << "Macro finished successfully." << std::endl;
    gApplication->Terminate();
}
