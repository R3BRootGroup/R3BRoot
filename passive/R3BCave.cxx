/******************************************************************************
 *   Copyright (C) 2009 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2009-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BCave.h"
#include "R3BGeoCave.h"
#include "R3BGeoPassivePar.h"

#include <FairGeoInterface.h>
#include <FairGeoLoader.h>
#include <FairGeoNode.h>
#include <FairGeoRootBuilder.h>
#include <FairGeoVolume.h>
#include <FairRun.h>
#include <FairRuntimeDb.h>

#include <TList.h>
#include <TObjArray.h>
#include <algorithm>
#include <stddef.h>

R3BCave::R3BCave()
    : FairModule()
{
}

R3BCave::R3BCave(const char* name, const char* Title)
    : FairModule(name, Title)
{
}

R3BCave::R3BCave(const R3BCave& right)
    : FairModule(right)
{
    std::copy(std::begin(right.world), std::end(right.world), world);
}

void R3BCave::ConstructGeometry()
{
    auto* loader = FairGeoLoader::Instance();
    auto* GeoInterface = loader->getGeoInterface();
    auto* MGeo = new R3BGeoCave();
    MGeo->setGeomFile(GetGeometryFileName());
    GeoInterface->addGeoModule(MGeo);
    auto rc = GeoInterface->readSet(MGeo);
    if (rc)
    {
        MGeo->create(loader->getGeoBuilder());
    }

    auto* volList = MGeo->getListOfVolumes();
    // store geo parameter

    auto* fRun = FairRun::Instance();
    auto* rtdb = FairRun::Instance()->GetRuntimeDb();

    auto* par = dynamic_cast<R3BGeoPassivePar*>(rtdb->getContainer("R3BGeoPassivePar"));
    auto* fSensNodes = par->GetGeoSensitiveNodes();
    auto* fPassNodes = par->GetGeoPassiveNodes();

    TListIter iter(volList);
    FairGeoNode* node = nullptr;
    FairGeoVolume* aVol = nullptr;

    while ((node = dynamic_cast<FairGeoNode*>(iter.Next())))
    {
        aVol = dynamic_cast<FairGeoVolume*>(node);
        if (node->isSensitive())
        {
            fSensNodes->AddLast(aVol);
        }
        else
        {
            fPassNodes->AddLast(aVol);
        }
    }
    par->setChanged();
    par->setInputVersion(fRun->GetRunId(), 1);
}

FairModule* R3BCave::CloneModule() const { return new R3BCave(*this); }

ClassImp(R3BCave)
