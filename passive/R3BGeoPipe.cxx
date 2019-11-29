/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// R3BGeoPipe
//
// Class for geometry of support structure
//
/////////////////////////////////////////////////////////////

#include "R3BGeoPipe.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"

ClassImp(R3BGeoPipe)

    R3BGeoPipe::R3BGeoPipe()
{
    // Constructor
    fName = "pipe";
    strcpy(modName, "p");
    strcpy(eleName, "p");
    maxSectors = 0;
    maxModules = 1;
}
Bool_t R3BGeoPipe::create(FairGeoBuilder* build)
{
    Bool_t rc = FairGeoSet::create(build);
    if (rc)
    {
        FairGeoLoader* loader = FairGeoLoader::Instance();
        FairGeoInterface* GeoInterface = loader->getGeoInterface();

        GeoInterface->getMasterNodes()->Add((TObject*)getVolume("pipeCentral"));
    }
    return rc;
}

R3BGeoPipe::~R3BGeoPipe() {}
