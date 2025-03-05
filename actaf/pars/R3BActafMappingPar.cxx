/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// --------------------------------------------------------------
// -----             R3BActafMappingPar source file         -----
// -----      Created 15/03/25 by J.L. Rodriguez-Sanchez    -----
// --------------------------------------------------------------

#include <FairParamList.h>
#include <TArrayI.h>

#include "R3BActafMappingPar.h"
#include "R3BLogger.h"

// ---- Standard Constructor ---------------------------------------------------
R3BActafMappingPar::R3BActafMappingPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    fIn_use.resize(fNbPads);
    fModule.resize(fNbPads);
    fChannel.resize(fNbPads);
    fPad.resize(fNbPads);
    for (Int_t idx = 0; idx < fNbPads; idx++)
    {
        fIn_use[idx] = 1;
        fModule[idx] = (idx / 16) + 1;
        fChannel[idx] = (idx % 16) + 1;
        fPad[idx] = idx + 1;
    }
}

// ----  Destructor ------------------------------------------------------------
R3BActafMappingPar::~R3BActafMappingPar()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method SetNbSensors ---------------------------------------------------
void R3BActafMappingPar::SetNbPads(Int_t pads)
{
    fNbPads = pads;
    fIn_use.resize(pads);
    fModule.resize(pads);
    fChannel.resize(pads);
    fPad.resize(pads);
}

// ----  Method clear ----------------------------------------------------------
void R3BActafMappingPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BActafMappingPar::putParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "Could not initialize FairParamList");
        return;
    }

    list->add("GeoVersionPar", fGeoVersion);
    R3BLOG(info, "Geometry version: " << fGeoVersion);

    list->add("NbPadsPar", fNbPads);
    R3BLOG(info, "Nb of pads: " << fNbPads);

    list->add("NbFADCModulesPar", fNbFADCModules);
    R3BLOG(info, "Nb of FADC-Modules: " << fNbFADCModules);

    auto* In_use = new TArrayI(fNbPads);
    auto* Module = new TArrayI(fNbPads);
    auto* Channel = new TArrayI(fNbPads);
    auto* Pads = new TArrayI(fNbPads);

    for (Int_t idx = 0; idx < fNbPads; idx++)
    {
        In_use->AddAt(fIn_use[idx], idx);
        Module->AddAt(fModule[idx], idx);
        Channel->AddAt(fChannel[idx], idx);
        Pads->AddAt(fPad[idx], idx);
    }
    list->add("InUsePar", *In_use);
    list->add("ModulePar", *Module);
    list->add("ChannelPar", *Channel);
    list->add("PadPar", *Pads);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BActafMappingPar::getParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "Could not initialize FairParamList");
        return kFALSE;
    }

    if (!list->fill("GeoVersionPar", &fGeoVersion))
    {
        R3BLOG(error, "Could not initialize GeoVersionPar");
        return kFALSE;
    }
    else
    {
        R3BLOG(info, "Geometry version: " << fGeoVersion);
    }

    if (!list->fill("NbPadsPar", &fNbPads))
    {
        R3BLOG(error, "Could not initialize NbPadsPar");
        return kFALSE;
    }
    else
    {
        R3BLOG(info, "Nb of pads: " << fNbPads);
    }

    if (!list->fill("NbFADCModulesPar", &fNbFADCModules))
    {
        R3BLOG(error, "Could not initialize NbFADCModulesPar");
        return kFALSE;
    }
    else
    {
        R3BLOG(info, "Nb of FADC-Modules: " << fNbFADCModules);
    }

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BActafMappingPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BActafMappingPar::printParams()
{
    R3BLOG(info, "GeoVersion: " << fGeoVersion);
    R3BLOG(info, "Nb of Pads: " << fNbPads);
    R3BLOG(info, "Nb of FADC-Modules: " << fNbFADCModules);
    for (Int_t idx = 0; idx < fNbPads; idx++)
    {
        R3BLOG(info, "Pad: " << idx + 1 << ", module: " << fModule[idx] << ", channel: " << fChannel[idx]);
    }
}

ClassImp(R3BActafMappingPar)
