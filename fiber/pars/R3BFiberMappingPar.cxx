/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------
// -----        R3BFiberMappingPar source file             -----
// -----    Created 18/03/22 by J.L. Rodriguez-Sanchez     -----
// -------------------------------------------------------------

#include "R3BFiberMappingPar.h"

#include "R3BLogger.h"
#include <FairParamList.h>

#include <TMath.h>
#include <TString.h>

// ---- Standard Constructor ---------------------------------------------------
R3BFiberMappingPar::R3BFiberMappingPar(const TString& name, const TString& title, const TString& context)
    : FairParGenericSet(name, title, context)
{
    fTrigmap.resize(fNbSides);
    for (size_t s = 0; s < fNbSides; s++)
    {
        fTrigmap[s] = new TArrayI(fNbChannels);
        for (size_t c = 0; c < fNbChannels; c++)
        {
            fTrigmap[s]->AddAt(0, c);
        }
    }
}

// ----  Method clear ----------------------------------------------------------
void R3BFiberMappingPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BFiberMappingPar::putParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "FairParamList not found");
        return;
    }
    list->add("fiberSidePar", fNbSides);
    list->add("fiberChannelPar", fNbChannels);

    R3BLOG(info, "Nb of channels: " << fNbChannels);
    R3BLOG(info, "Nb of sides: " << fNbSides);

    for (size_t s = 0; s < fNbSides; s++)
    {
        fTrigmap[s]->Set(fNbChannels);
        TString name = Form("fiberside%zuPar", s + 1);
        list->add(name.Data(), *fTrigmap[s]);
    }
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BFiberMappingPar::getParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "FairParamList not found");
        return kFALSE;
    }
    if (!list->fill("fiberSidePar", &fNbSides))
    {
        R3BLOG(info, "Could not initialize fiberSidePar");
        return kFALSE;
    }
    if (!list->fill("fiberChannelPar", &fNbChannels))
    {
        R3BLOG(info, "Could not initialize fiberChannelPar");
        return kFALSE;
    }

    for (size_t s = 0; s < fNbSides; s++)
    {
        fTrigmap[s]->Set(fNbChannels);
        TString name = Form("fiberside%zuPar", s + 1);
        if (!(list->fill(name.Data(), fTrigmap[s])))
        {
            R3BLOG(error, "Could not initialize " << name);
            return kFALSE;
        }
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BFiberMappingPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BFiberMappingPar::printParams()
{
    R3BLOG(info, "Mapping params for Fiber: Num of sides: " << fNbSides << " and channels: " << fNbChannels);
    for (size_t s = 0; s < fNbSides; s++)
        for (size_t c = 0; c < fNbChannels; c++)
        {
            R3BLOG(info, "Side: " << s + 1 << " , channel:" << c + 1 << ", value: " << fTrigmap[s]->GetAt(c));
        }
}

ClassImp(R3BFiberMappingPar)
