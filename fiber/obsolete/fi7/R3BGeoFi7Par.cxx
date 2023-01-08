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

//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

#include "R3BGeoFi7Par.h"

#include "FairParamList.h"

#include "TObjArray.h"

#include <iomanip>
#include <iostream>

ClassImp(R3BGeoFi7Par)

    R3BGeoFi7Par::R3BGeoFi7Par(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{

    fGeoSensNodes = new TObjArray();
    fGeoPassNodes = new TObjArray();
}

R3BGeoFi7Par::~R3BGeoFi7Par(void) {}

void R3BGeoFi7Par::clear(void)
{
    if (fGeoSensNodes)
        delete fGeoSensNodes;
    if (fGeoPassNodes)
        delete fGeoPassNodes;
}

void R3BGeoFi7Par::putParams(FairParamList* l)
{
    if (!l)
        return;
    l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
    l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
}

Bool_t R3BGeoFi7Par::getParams(FairParamList* l)
{
    if (!l)
        return kFALSE;
    if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes))
        return kFALSE;
    if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes))
        return kFALSE;

    return kTRUE;
}
