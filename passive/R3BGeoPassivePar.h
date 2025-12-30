/******************************************************************************
 *   Copyright (C) 2009 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2009-2026 Members of R3B Collaboration                     *
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

#include <FairParGenericSet.h>

class R3BGeoPassivePar : public FairParGenericSet
{
  public:
    R3BGeoPassivePar(const char* name = "R3BGeoPassivePar",
                     const char* title = "Passive Geometry Parameters",
                     const char* context = "TestDefaultContext");

    ~R3BGeoPassivePar(void) = default;

    void clear(void);

    void putParams(FairParamList*);

    Bool_t getParams(FairParamList*);

    TObjArray* fGeoSensNodes; // List of FairGeoNodes for sensitive volumes
    TObjArray* fGeoPassNodes; // List of FairGeoNodes for sensitive volumes

    TObjArray* GetGeoSensitiveNodes() { return fGeoSensNodes; }
    TObjArray* GetGeoPassiveNodes() { return fGeoPassNodes; }

    ClassDef(R3BGeoPassivePar, 1); // NOLINT
};
