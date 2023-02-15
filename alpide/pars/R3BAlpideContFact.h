/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3BAlpideContFact_H
#define R3BAlpideContFact_H 1

#include "FairContFact.h"

class FairContainer;

class R3BAlpideContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BAlpideContFact();
    virtual ~R3BAlpideContFact() {}

    FairParSet* createContainer(FairContainer*);
    ClassDef(R3BAlpideContFact, 0)
};

#endif /* R3BAlpideContFact_H */
