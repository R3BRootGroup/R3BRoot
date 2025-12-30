/******************************************************************************
 *   Copyright (C) 2018 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2018-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include "R3BModule.h"

class R3BTGeoPar;

class R3BCollimator : public R3BModule
{
  public:
    R3BCollimator();

    R3BCollimator(const TString& name);

    R3BCollimator(const TString& name,
                  const TString& geoFile,
                  const TGeoTranslation& trans,
                  const TGeoRotation& rot = TGeoRotation());

    R3BCollimator(const TString& name, const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    virtual void ConstructGeometry();

    void SetParContainers();

    ClassDef(R3BCollimator, 1);

  protected:
    R3BTGeoPar* fTGeoPar; //!
};
