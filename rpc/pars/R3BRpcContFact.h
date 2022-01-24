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

#ifndef R3BRPCCONTFACT_H
#define R3BRPCCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class R3BRpcContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    R3BRpcContFact();
    ~R3BRpcContFact() {}
    FairParSet* createContainer(FairContainer*);
    ClassDef(R3BRpcContFact, 0) // Factory for all RPC parameter containers
};

#endif /* !R3BRPCCONTFACT_H */
