/******************************************************************************
 *   Copyright (C) 2020 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2020-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BNeutronWindowAndSomeAir.h"

#include <FairGeoBuilder.h>
#include <FairGeoInterface.h>
#include <FairGeoLoader.h>
#include <FairGeoMedia.h>
#include <FairLogger.h>

#include <TGeoManager.h>
#include <TGeoMedium.h>

R3BNeutronWindowAndSomeAir::R3BNeutronWindowAndSomeAir(double xstart, double xstop)
    : FairModule("NeutronWindowAndSomeAir", "NeutronWindowAndSomeAir", false)
    , fStart(xstart)
    , fStop(xstop)
{
}

void R3BNeutronWindowAndSomeAir::ConstructRootGeometry(TGeoMatrix*)
{
    if (gGeoManager == nullptr)
    {
        LOG(fatal) << __FUNCTION__ << ": No gGeoManager";
    }

    auto volSomeAir = gGeoManager->MakeBox("SomeAir", FindMaterial("Air"), 125, 125, (fStop - fStart) / 2.);
    gGeoManager->GetTopVolume()->AddNode(volSomeAir, 1, new TGeoTranslation(0, 0, fStart + (fStop - fStart) / 2.));
    auto volWindow = gGeoManager->MakeTube("NeutronWindow", FindMaterial("Steel"), 0, 70, 0.2);
    gGeoManager->GetTopVolume()->AddNode(volWindow, 1, new TGeoTranslation(0, 0, fStart - 0.2));
}

// TODO: Simulation crashes without this. It should not be necessary to do this?!
TGeoMedium* R3BNeutronWindowAndSomeAir::FindMaterial(const std::string& mat) const
{
    auto geoLoad = FairGeoLoader::Instance();
    auto geoFace = geoLoad->getGeoInterface();
    auto geoBuild = geoLoad->getGeoBuilder();
    auto geoMedia = geoFace->getMedia();

    auto fairMedium = geoMedia->getMedium(mat.c_str());
    if (!fairMedium)
    {
        LOG(fatal) << __FUNCTION__ << ": FairGeoMedium " << mat << " not found";
    }
    geoBuild->createMedium(fairMedium);

    auto med = gGeoManager->GetMedium(mat.c_str());
    if (!med)
    {
        LOG(fatal) << __FUNCTION__ << ": TGeoMedium " << mat << " not found";
    }
    return med;
}

ClassImp(R3BNeutronWindowAndSomeAir)
