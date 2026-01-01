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

#include "R3BCaveFloor.h"
#include "R3BLogger.h"

#include <FairGeoBuilder.h>
#include <FairGeoInterface.h>
#include <FairGeoLoader.h>
#include <FairGeoMedia.h>

#include <TGeoBBox.h>
#include <TGeoManager.h>
#include <TGeoVolume.h>

R3BCaveFloor::R3BCaveFloor()
    : R3BModule()
{
}

R3BCaveFloor::R3BCaveFloor(const TString& name)
    : R3BModule(name, "R3BCaveFloor", false)
{
}

R3BCaveFloor::R3BCaveFloor(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BCaveFloor(geoFile, { trans, rot })
{
}

R3BCaveFloor::R3BCaveFloor(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BModule("R3BCaveFloor", "Floor", false, geoFile, combi)
{
}

void R3BCaveFloor::ConstructGeometry()
{
    R3BLOG(info, "");

    auto* geoLoad = FairGeoLoader::Instance();
    auto* geoFace = geoLoad->getGeoInterface();
    auto* geoMedia = geoFace->getMedia();
    auto* geoBuild = geoLoad->getGeoBuilder();

    auto fairMedium = geoMedia->getMedium("concrete");
    R3BLOG_IF(fatal, !fairMedium, "FairGeoMedium concrete not found");
    geoBuild->createMedium(fairMedium);

    auto med = gGeoManager->GetMedium("concrete");
    R3BLOG_IF(fatal, !med, "TGeoMedium concrete not found");

    double dx = 1500.; // 15 m
    double dy = 10.;   // 10 cm
    double dz = 2500.; // 25 m

    auto* floor = gGeoManager->MakeBox("CaveFloor", med, dx / 2., dy / 2., dz / 2.);
    floor->SetLineColor(31);

    double yPos = -200. - dy / 2.;
    double zPos = 600.;

    auto* top = gGeoManager->GetTopVolume();
    top->AddNode(floor, 1, new TGeoTranslation(0., yPos, zPos));
}

ClassImp(R3BCaveFloor)
