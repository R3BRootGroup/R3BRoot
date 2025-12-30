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

#include <FairDetector.h>
#include <FairModule.h>

#include <Rtypes.h>

class R3BCave : public FairModule
{
  public:
    R3BCave();
    explicit R3BCave(const char* name, const char* Title = "Exp Cave");

    virtual ~R3BCave() = default;

    virtual void ConstructGeometry();

    virtual FairModule* CloneModule() const;

  private:
    double world[3]{};

    R3BCave(const R3BCave&);
    R3BCave& operator=(const R3BCave&);

  public:
    ClassDef(R3BCave, 2); // NOLINT
};
