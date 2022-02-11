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

// --------------------------------------------------------------
// -----             R3BAlpideMappingPar source file        -----
// -----      Created 11/02/22 by J.L. Rodriguez-Sanchez    -----
// --------------------------------------------------------------

#include "FairLogger.h"
#include "FairParamList.h"

#include "R3BAlpideMappingPar.h"
#include "R3BLogger.h"

#include "TString.h"

// ---- Standard Constructor ---------------------------------------------------
R3BAlpideMappingPar::R3BAlpideMappingPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNbSensors(1)
{
    fIn_use[AlpideCols][AlpideRows].resize(fNbSensors);
    for (Int_t c = 0; c < AlpideCols; c++)
        for (Int_t r = 0; r < AlpideRows; r++)
            for (Int_t s = 0; s < fNbSensors; s++)
                fIn_use[c][r][s] = 1;
}

// ----  Destructor ------------------------------------------------------------
R3BAlpideMappingPar::~R3BAlpideMappingPar() { clear(); }

// ----  Method SetNbSensors ---------------------------------------------------
void R3BAlpideMappingPar::SetNbSensors(UInt_t n)
{
    fNbSensors = n;
    fIn_use[AlpideCols][AlpideRows].resize(fNbSensors);
    for (Int_t c = 0; c < AlpideCols; c++)
        for (Int_t r = 0; r < AlpideRows; r++)
            for (Int_t s = 0; s < fNbSensors; s++)
                fIn_use[c][r][s] = 1;
}

// ----  Method clear ----------------------------------------------------------
void R3BAlpideMappingPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BAlpideMappingPar::putParams(FairParamList* list)
{
    R3BLOG(INFO, "called");
    if (!list)
    {
        R3BLOG(ERROR, "Could not initialize FairParamList");
        return;
    }

    list->add("alpideNbSensorsPar", fNbSensors);

    Int_t defzero = 0;
    for (int s = 1; s <= fNbSensors; s++)
        for (int c = 1; c <= AlpideCols; c++)
            for (int r = 1; r <= AlpideRows; r++)
                if (fIn_use[c - 1][r - 1][s - 1] == 0)
                {
                    TString ns = "Sensor" + s;
                    TString nc = "_Col" + c;
                    TString nr = "_Row" + r;
                    TString name = ns + nc + nr;
                    list->add(name, defzero);
                }
}

// ----  Method getParams ------------------------------------------------------
Bool_t R3BAlpideMappingPar::getParams(FairParamList* list)
{
    R3BLOG(INFO, "called");
    if (!list)
    {
        R3BLOG(ERROR, "Could not initialize FairParamList");
        return kFALSE;
    }

    if (!list->fill("alpideNbSensorsPar", &fNbSensors))
    {
        LOG(ERROR) << "Could not initialize alpideNbSensorsPar";
        return kFALSE;
    }
    else
        R3BLOG(INFO, "Nb Sensors: " << fNbSensors);

    for (int s = 1; s <= fNbSensors; s++)
        for (Int_t c = 1; c <= AlpideCols; c++)
            for (Int_t r = 1; r <= AlpideRows; r++)
            {
                TString ns = "Sensor" + s;
                TString nc = "_Col" + c;
                TString nr = "_Row" + r;
                TString name = ns + nc + nr;
                Int_t value = 1;
                auto check = fillParams(name, &value, list);
                if (check)
                    fIn_use[c - 1][r - 1][s - 1] = value;
            }
    return kTRUE;
}

Bool_t R3BAlpideMappingPar::fillParams(const Text_t* name, Int_t* values, FairParamList* paramList, const Int_t nValues)
{
    // Copies the data from the list object into the parameter array of type Int_t.
    // The function returns an error, if the array size of the list object is not equal
    // to nValues.
    if (values == 0)
    {
        return kFALSE;
    }
    FairParamObj* o = static_cast<FairParamObj*>(paramList->FindObject(name));
    if (o != 0 && strcmp(o->getParamType(), "Int_t") == 0)
    {
        Int_t l = o->getLength();
        Int_t n = o->getNumParams();
        if (n == nValues)
        {
            memcpy(values, o->getParamValue(), l);
            return kTRUE;
        }
        else
        {
            R3BLOG(ERROR, "Different array sizes for parameter " << name);
            return kFALSE;
        }
    }
    return kFALSE;
}

// ----  Method printParams ----------------------------------------------------
void R3BAlpideMappingPar::printParams()
{
    R3BLOG(INFO, "Nb Sensors: " << fNbSensors);
    for (Int_t s = 0; s < fNbSensors; s++)
    {
        LOG(INFO) << "Alpide Sensor: " << s + 1;
        for (Int_t c = 0; c < AlpideCols; c++)
            for (Int_t r = 0; r < AlpideRows; r++)
            {
                if (fIn_use[c][r][s] == 0)
                    LOG(INFO) << "Pixel column: " << c + 1 << ", row: " << r + 1 << " is skipped";
            }
    }
}

ClassImp(R3BAlpideMappingPar);
