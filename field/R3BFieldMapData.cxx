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

// -------------------------------------------------------------------------
// -----                    R3BFieldMapData source file                -----
// -----                V. Friese                14/02/06              -----
// -------------------------------------------------------------------------
#include "R3BFieldMapData.h"

#include "R3BFieldMap.h"

#include "TArrayF.h"

// -------------   Default constructor  ----------------------------------
R3BFieldMapData::R3BFieldMapData()
{
    fType = 1;
    fXmin = fYmin = fZmin = 0.;
    fXmax = fYmax = fZmax = 0.;
    fNx = fNy = fNz = 0;
    fBx = fBy = fBz = NULL;
}
// ------------------------------------------------------------------------

// -------------   Standard constructor   ---------------------------------
R3BFieldMapData::R3BFieldMapData(const char* mapName)
    : TNamed(mapName, "CBM Field Map Data")
{
    fType = 1;
    fXmin = fYmin = fZmin = 0.;
    fXmax = fYmax = fZmax = 0.;
    fNx = fNy = fNz = 0;
    fBx = fBy = fBz = NULL;
}
// ------------------------------------------------------------------------

// -----   Constructor from R3BFieldMap   ------------------------------
R3BFieldMapData::R3BFieldMapData(const char* name, const R3BFieldMap& map)
    : TNamed(name, "CBM Field Map Data")
{

    fType = map.GetType();
    fXmin = map.GetXmin();
    fYmin = map.GetYmin();
    fZmin = map.GetZmin();
    fXmax = map.GetXmax();
    fYmax = map.GetYmax();
    fZmax = map.GetZmax();
    fNx = map.GetNx();
    fNy = map.GetNy();
    fNz = map.GetNz();
    fBx = new TArrayF(*(map.GetBx()));
    fBy = new TArrayF(*(map.GetBy()));
    fBz = new TArrayF(*(map.GetBz()));

    // Take out scaling factor and convert from kG to T
    if (map.GetScale() == 0.)
        LOG(fatal) << "R3BFieldMap: Zero scaling factor";
    Double_t factor = map.GetScale() * 10.;
    Int_t index = 0;
    for (Int_t ix = 0; ix < fNx; ix++)
    {
        for (Int_t iy = 0; iy < fNy; iy++)
        {
            for (Int_t iz = 0; iz < fNz; iz++)
            {
                index = ix * fNy * fNz + iy * fNz + iz;
                if (fBx)
                    (*fBx)[index] = (*fBx)[index] / factor;
                if (fBy)
                    (*fBy)[index] = (*fBy)[index] / factor;
                if (fBz)
                    (*fBz)[index] = (*fBz)[index] / factor;
            } // z loop
        }     // y loop
    }         // x loop
}
// ------------------------------------------------------------------------

// ------------   Destructor   --------------------------------------------
R3BFieldMapData::~R3BFieldMapData()
{
    if (fBx)
        delete fBx;
    if (fBy)
        delete fBy;
    if (fBz)
        delete fBz;
}
// ------------------------------------------------------------------------

ClassImp(R3BFieldMapData)
