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
// -----                      R3BFieldPar source file                  -----
// -------------------------------------------------------------------------
#include "R3BFieldPar.h"
#include "R3BAladinFieldMap.h"
#include "R3BFieldConst.h"
#include "R3BFieldMap.h"
#include "R3BGladFieldMap.h"

#include "FairParamList.h"

#include "TDirectory.h"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

// const int kMaxLen = 2048;

// ------   Constructor   --------------------------------------------------
R3BFieldPar::R3BFieldPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
{
    fType = -1;
    fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;
    fBx = fBy = fBz = 0.;
    fMapName = "";
    fPosX = fPosY = fPosZ = 0.;
    fScale = 0.;
}
// -------------------------------------------------------------------------

R3BFieldPar::R3BFieldPar()
{
    fType = -1;
    fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;
    fBx = fBy = fBz = 0.;
    fMapName = "";
    fPosX = fPosY = fPosZ = 0.;
    fScale = 0.;
}
// -------------------------------------------------------------------------

// ------   Destructor   ---------------------------------------------------
R3BFieldPar::~R3BFieldPar() {}
// -------------------------------------------------------------------------

// ------   Put parameters   -----------------------------------------------
void R3BFieldPar::putParams(FairParamList* list)
{

    if (!list)
        return;

    list->add("Field Type", fType);

    if (fType == 0)
    { // constant field
        list->add("Field min x", fXmin);
        list->add("Field max x", fXmax);
        list->add("Field min y", fYmin);
        list->add("Field max y", fYmax);
        list->add("Field min z", fZmin);
        list->add("Field max z", fZmax);
        list->add("Field Bx", fBx);
        list->add("Field By", fBy);
        list->add("Field Bz", fBz);
    }

    else if (fType >= 1 && fType <= kMaxFieldMapType)
    { // field map
        list->add("Field map name", fMapName);
        list->add("Field x position", fPosX);
        list->add("Field y position", fPosY);
        list->add("Field z position", fPosZ);
        list->add("Field scaling factor", fScale);
        list->add("Current", fCurrent);
    }
}
// -------------------------------------------------------------------------

// --------   Get parameters   ---------------------------------------------
Bool_t R3BFieldPar::getParams(FairParamList* list)
{

    if (!list)
        return kFALSE;

    if (!list->fill("Field Type", &fType))
        return kFALSE;

    if (fType == 0)
    { // constant field
        if (!list->fill("Field min x", &fXmin))
            return kFALSE;
        if (!list->fill("Field max x", &fXmax))
            return kFALSE;
        if (!list->fill("Field min y", &fYmin))
            return kFALSE;
        if (!list->fill("Field max y", &fYmax))
            return kFALSE;
        if (!list->fill("Field min z", &fZmin))
            return kFALSE;
        if (!list->fill("Field max z", &fZmax))
            return kFALSE;
        if (!list->fill("Field Bx", &fBx))
            return kFALSE;
        if (!list->fill("Field By", &fBy))
            return kFALSE;
        if (!list->fill("Field Bz", &fBz))
            return kFALSE;
    }

    else if (fType >= 1 && fType <= kMaxFieldMapType)
    { // field map
        Text_t mapName[80];
        if (!list->fill("Field map name", mapName, 80))
            return kFALSE;
        fMapName = mapName;
        if (!list->fill("Field x position", &fPosX))
            return kFALSE;
        if (!list->fill("Field y position", &fPosY))
            return kFALSE;
        if (!list->fill("Field z position", &fPosZ))
            return kFALSE;
        if (!list->fill("Field scaling factor", &fScale))
            return kFALSE;
        if (!list->fill("Current", &fCurrent))
            return kFALSE;
    }

    return kTRUE;
}
// -------------------------------------------------------------------------

// ---------   Set parameters from R3BField   ------------------------------
void R3BFieldPar::SetParameters(FairField* field)
{

    if (!field)
    {
        cerr << "-W- R3BFieldPar::SetParameters: Empty field pointer!" << endl;
        return;
    }

    fType = field->GetType();
    if (TString(field->GetName()).Contains("Glad"))
    {
        fType = 2;
    }

    if (fType == 0)
    { // constant field
        R3BFieldConst* fieldConst = (R3BFieldConst*)field;
        fBx = fieldConst->GetBx();
        fBy = fieldConst->GetBy();
        fBz = fieldConst->GetBz();
        fXmin = fieldConst->GetXmin();
        fXmax = fieldConst->GetXmax();
        fYmin = fieldConst->GetYmin();
        fYmax = fieldConst->GetYmax();
        fZmin = fieldConst->GetZmin();
        fZmax = fieldConst->GetZmax();
        fMapName = "";
        fPosX = fPosY = fPosZ = fScale = 0.;
    }

    else if (fType >= 1 && fType <= kMaxFieldMapType)
    { // field map
        // to be implemented for the  case of R3B
        R3BFieldMap* fieldMap = (R3BFieldMap*)field;
        fBx = fBy = fBz = 0.;
        fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;

        fMapName = field->GetName();
        // fPosX   = fieldMap->GetPositionX();
        // fPosY   = fieldMap->GetPositionY();
        // fPosZ   = fieldMap->GetPositionZ();
        fScale = fieldMap->GetScale();

        if (1 == fType)
        {
            R3BAladinFieldMap* aladinFieldMap = (R3BAladinFieldMap*)field;
            fCurrent = aladinFieldMap->GetCurrent();
        }
        else if (2 == fType)
        {
            R3BGladFieldMap* gladFieldMap = (R3BGladFieldMap*)field;
            fPosX = gladFieldMap->GetPositionX();
            fPosY = gladFieldMap->GetPositionY();
            fPosZ = gladFieldMap->GetPositionZ();
            fXmin = -gladFieldMap->GetXmax();
            fXmax = gladFieldMap->GetXmax();
            fYmin = -gladFieldMap->GetYmax();
            fYmax = gladFieldMap->GetYmax();
            fZmin = gladFieldMap->GetZmin();
            fZmax = gladFieldMap->GetZmax();
        }
    }

    else
    {
        cerr << "-W- R3BFieldPar::SetParameters: Unknown field type " << fType << "!" << endl;
        fBx = fBy = fBz = 0.;
        fXmin = fXmax = fYmin = fYmax = fZmin = fZmax = 0.;
        fMapName = "";
        fPosX = fPosY = fPosZ = fScale = 0.;
    }

    return;
}
// -------------------------------------------------------------------------

ClassImp(R3BFieldPar)
