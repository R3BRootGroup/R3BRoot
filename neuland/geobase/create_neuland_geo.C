/******************************************************************************
 *   Copyright (C) 2023 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
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

#include "./helper_neuland_geometry.C"
#include "TGeoManager.h"
#include "TMath.h"
#include <iomanip>
#include <iostream>

void create_neuland_geo(const Int_t nPlanes = 26, const TString geoTag = "v2022.5")
{
    new FairGeoLoader("TGeo", "FairGeoLoader");
    gGeoManager->SetName("NEULANDgeom");

    TGeoVolume* volPaddle = BuildPaddleVolume();
    TGeoRotation* rot0 = nullptr;
    TGeoRotation* rot90 = new TGeoRotation();
    rot90->RotateZ(90.);
    TGeoRotation* rot45 = new TGeoRotation();
    rot45->RotateY(45.);

    TGeoVolume* volNeuland = new TGeoVolumeAssembly("volNeuland");
    Int_t nindex = 0, nPlane = 0;
    for (Double_t b = -nPlanes * gPaddleDistance + gPaddleDistance; b < nPlanes * gPaddleDistance;
         b += gPaddleDistance * 2)
    {
        nPlane++;
        for (Double_t a = -gPaddlesPerPlane * gPaddleDistance + gPaddleDistance; a < gPaddleDistance * gPaddlesPerPlane;
             a += gPaddleDistance * 2)
        {
            nindex++;
            if (nPlane % 2 == 1)
            {
                volNeuland->AddNode(volPaddle, nindex, new TGeoTranslation(0, a, b));
            }
            else
            {
                volNeuland->AddNode(volPaddle, nindex, new TGeoCombiTrans(a, 0, b, rot90));
            }
        }
    }

    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    gGeoManager->SetTopVolume(top);
    top->AddNode(volNeuland, 1, new TGeoCombiTrans(0., 0., 0., rot0));

    // ---------------   Finish   -----------------------------------------------
    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();
    gGeoManager->Test();

    // -------   Geometry file name (output)   ----------------------------------
    TString geoFileName = TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/neuland_" + geoTag + ".geo.root";
    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    std::cout << std::endl;
    std::cout << "Done. " << nindex << " Paddles in " << nPlane << " Planes" << std::endl;
    std::cout << std::endl;
    std::cout << "\033[34m Creating geometry:\033[0m "
              << "\033[33m" << geoFileName << " \033[0m" << std::endl;
    std::cout << "Macro finished successfully." << std::endl;
}
