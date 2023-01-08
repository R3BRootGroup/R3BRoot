/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef ELILUMONCONTFACT_H
#define ELILUMONCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class ELILuMonContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    ELILuMonContFact();
    ~ELILuMonContFact() {}
    FairParSet* createContainer(FairContainer*);
    void activateParIo(FairParIo* io);
    ClassDef(ELILuMonContFact, 0) // Factory for all TOF parameter containers
};

#endif /* !ELILUMONCONTFACT_H */
