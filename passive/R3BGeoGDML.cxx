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

#include "R3BGeoGDML.h"

R3BGeoGDML::R3BGeoGDML(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BGeoGDML(geoFile, { trans, rot })
{
}

R3BGeoGDML::R3BGeoGDML(const TString& geoFile, const TGeoCombiTrans& combi)
    : fCombiTrans(combi)
{
    SetGeometryFileName(geoFile);
}

void R3BGeoGDML::ConstructGeometry() { ConstructGDMLGeometry(dynamic_cast<TGeoMatrix*>(fCombiTrans.Clone())); }
