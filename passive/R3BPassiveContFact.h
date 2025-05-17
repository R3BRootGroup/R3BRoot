/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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

#include <FairContFact.h>
#include <FairRuntimeDb.h>

class FairContainer;

class R3BPassiveContFact : public FairContFact
{
  public:
    R3BPassiveContFact();

    virtual ~R3BPassiveContFact() = default;

    FairParSet* createContainer(FairContainer*) override;

  private:
    void setAllContainers();

  public:
    // Class definition
    ClassDefOverride(R3BPassiveContFact, 0); // NOLINT
};
