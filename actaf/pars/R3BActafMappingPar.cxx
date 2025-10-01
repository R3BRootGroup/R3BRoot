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
#include <TArrayD.h>
#include <TArrayI.h>
#include <algorithm>

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
    fSGCoeffs.resize(fNbSGcoefs);
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

void R3BActafMappingPar::SetNbSGCoeffs(Int_t num) { fSGCoeffs.resize(num); }

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

    list->add("NbBinsSamplePar", fNbBinsSample);
    R3BLOG(info, "Nb of bins of the sample: " << fNbBinsSample);

    list->add("NbPadsPar", fNbPads);
    R3BLOG(info, "Nb of pads: " << fNbPads);

    list->add("NbSGPar", fNbSGcoefs);
    R3BLOG(info, "Nb of pads: " << fNbSGcoefs);

    list->add("NbFADCModulesPar", fNbFADCModules);
    R3BLOG(info, "Nb of FADC-Modules: " << fNbFADCModules);

    TArrayI In_use(fNbPads);
    TArrayI Module(fNbPads);
    TArrayI Channel(fNbPads);
    TArrayI Pads(fNbPads);
    for (Int_t idx = 0; idx < fNbPads; idx++)
    {
        In_use[idx] = fIn_use[idx];
        Module[idx] = fModule[idx];
        Channel[idx] = fChannel[idx];
        Pads[idx] = fPad[idx];
    }

    TArrayD SGCoeffs(fNbSGcoefs);
    for (int idx = 0; idx < fNbSGcoefs; idx++)
    {
        SGCoeffs[idx] = fSGCoeffs[idx];
    }

    list->add("InUsePar", In_use);
    list->add("ModulePar", Module);
    list->add("ChannelPar", Channel);
    list->add("PadPar", Pads);
    list->add("SGCoeffs", SGCoeffs);
}

// Template to simplify the parameter getting
template <class TArrayT, class VecT>
void FillAndCopy(FairParamList* list, const char* name, TArrayT& arr, VecT& vec)
{
    if (!list->fill(name, &arr))
    {
        R3BLOG(warn, "---Could not initialize " << name);
        return;
    }
    const Int_t n = arr.GetSize();
    vec.resize(n);
    std::copy_n(arr.GetArray(), n, vec.begin());
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

    if (!list->fill("NbBinsSamplePar", &fNbBinsSample))
    {
        R3BLOG(error, "Could not initialize NbBinsSamplePar");
        return kFALSE;
    }
    else
    {
        R3BLOG(info, "Nb of bins of the sample: " << fNbBinsSample);
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

    if (!list->fill("NbSGcoefs", &fNbSGcoefs))
    {
        R3BLOG(error, "Could not initialize SGCoeffs");
        return kFALSE;
    }
    else
    {
        R3BLOG(info, "Nb of sg coeffs: " << fNbSGcoefs);
    }

    // Map names to arrays for cleaner loop
    TArrayI In_use(fNbPads);
    TArrayI Module(fNbPads);
    TArrayI Channel(fNbPads);
    TArrayI Pads(fNbPads);
    TArrayD SGCoeffs(fNbSGcoefs);

    // Ints
    FillAndCopy(list, "InUsePar", In_use, fIn_use);
    FillAndCopy(list, "ModulePar", Module, fModule);
    FillAndCopy(list, "ChannelPar", Channel, fChannel);
    FillAndCopy(list, "PadPar", Pads, fPad);

    // Doubles
    FillAndCopy(list, "SGCoeffs", SGCoeffs, fSGCoeffs);

    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BActafMappingPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BActafMappingPar::printParams()
{
    R3BLOG(info, "GeoVersion: " << fGeoVersion);
    R3BLOG(info, "Nb of Pads: " << fNbPads);
    R3BLOG(info, "Nb of Bins of the Sample: " << fNbBinsSample);
    R3BLOG(info, "Nb of FADC-Modules: " << fNbFADCModules);
    R3BLOG(info, "Nb of SG Coeffs: " << fNbSGcoefs);
    for (Int_t idx = 0; idx < fNbPads; idx++)
    {
        R3BLOG(info, "Pad: " << idx + 1 << ", module: " << fModule[idx] << ", channel: " << fChannel[idx]);
    }
}

ClassImp(R3BActafMappingPar)
