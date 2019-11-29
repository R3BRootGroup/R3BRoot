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

#ifndef R3BROOT_R3BGEOGDML_H
#define R3BROOT_R3BGEOGDML_H

#include "FairModule.h"
#include "TGeoMatrix.h"

class R3BGeoGDML : public FairModule
{
  public:
    R3BGeoGDML() = default;
    R3BGeoGDML(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());
    explicit R3BGeoGDML(const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    void ConstructGeometry() override;

  protected:
    TGeoCombiTrans fCombiTrans;
    ClassDefOverride(R3BGeoGDML, 1)
};

#endif // R3BROOT_R3BGEOGDML_H
