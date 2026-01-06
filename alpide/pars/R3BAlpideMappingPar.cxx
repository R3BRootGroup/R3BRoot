/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2026 Members of R3B Collaboration                     *
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
// -----             R3BAlpideMappingPar source file        -----
// -----      Created 11/02/22 by J.L. Rodriguez-Sanchez    -----
// --------------------------------------------------------------

#include "R3BAlpideMappingPar.h"
#include "R3BLogger.h"

#include <FairLogger.h>
#include <FairParamList.h>

#include <TArrayD.h>
#include <sstream>
#include <string>

// ---- Standard Constructor ---------------------------------------------------
R3BAlpideMappingPar::R3BAlpideMappingPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
}

// ----  Destructor ------------------------------------------------------------
R3BAlpideMappingPar::~R3BAlpideMappingPar() { clear(); }

// ----  Method clear ----------------------------------------------------------
void R3BAlpideMappingPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BAlpideMappingPar::putParams(FairParamList* list)
{
    R3BLOG(info, "called");
    if (!list)
    {
        R3BLOG(fatal, "Could not initialize FairParamList");
        return;
    }

    list->add("GeoVersionPar", fGeoVersion);
    R3BLOG(info, "Geometry version: " << fGeoVersion);

    list->add("NbSensorsPar", fNbSensors);
    R3BLOG(info, "Nb of ALPIDE sensors: " << fNbSensors);

    list->add("NbMaskPixelsPar", fNbMaskPixels);
    R3BLOG(info, "Nb of masking pixels: " << fNbMaskPixels);

    TArrayD fMask_pixels(fNbMaskPixels);
    for (auto pixel = 0; pixel < fNbMaskPixels; pixel++)
    {
        fMask_pixels[pixel] = fMask_sensors[pixel];
    }
    list->add("MaskPixelPar", fMask_pixels);
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BAlpideMappingPar::getParams(FairParamList* list)
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

    if (!list->fill("NbSensorsPar", &fNbSensors))
    {
        R3BLOG(error, "Could not initialize NbSensorsPar");
        return kFALSE;
    }
    else
    {
        R3BLOG(info, "Nb of ALPIDE sensors: " << fNbSensors);
    }

    if (!list->fill("NbMaskPixelsPar", &fNbMaskPixels))
    {
        R3BLOG(error, "Could not initialize NbMaskPixelsPar");
        return kFALSE;
    }
    else
    {
        R3BLOG(info, "Nb of masking pixels: " << fNbMaskPixels);
    }

    if (fNbMaskPixels > 0)
    {
        TArrayD fMask_pixels(fNbMaskPixels);
        if (!(list->fill("MaskPixelPar", &fMask_pixels)))
        {
            R3BLOG(error, "Could not initialize MaskPixelPar");
            return kFALSE;
        }

        for (auto pixel = 0; pixel < fNbMaskPixels; pixel++)
        {
            fMask_sensors.push_back(fMask_pixels[pixel]);

            auto sen = this->GetSensorId(fMask_sensors[pixel]);
            auto col = this->GetCol(fMask_sensors[pixel]);
            auto row = this->GetRow(fMask_sensors[pixel]);
            LOG(info) << "Skipping Sensor: " << sen << " Col: " << col << " Row: " << row;
        }
    }
    return kTRUE;
}

// ----  Method print ----------------------------------------------------------
void R3BAlpideMappingPar::print() { printParams(); }

// ----  Method printParams ----------------------------------------------------
void R3BAlpideMappingPar::printParams()
{
    R3BLOG(info, "Nb Sensors: " << fNbSensors);
    R3BLOG(info, "Geo Version: " << fGeoVersion);
    R3BLOG(info, "Nb masked pixels: " << fNbMaskPixels);

    for (auto pixel = 0; pixel < fNbMaskPixels; pixel++)
    {
        auto sen = this->GetSensorId(fMask_sensors[pixel]);
        auto col = this->GetCol(fMask_sensors[pixel]);
        auto row = this->GetRow(fMask_sensors[pixel]);
        LOG(info) << "Skipping Sensor: " << sen << " Col: " << col << " Row: " << row;
    }
}

ClassImp(R3BAlpideMappingPar)
