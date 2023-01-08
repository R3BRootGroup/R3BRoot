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

#ifndef R3BTOFDCONTFACT_H
#define R3BTOFDCONTFACT_H 1

#include "FairContFact.h"

class FairContainer;

class R3BTofDContFact : public FairContFact
{
  private:
    void setAllContainers();

  public:
    /**
     * Default constructor.
     */
    R3BTofDContFact();

    /**
     * Destructor.
     */
    ~R3BTofDContFact() {}

    FairParSet* createContainer(FairContainer*);
    ClassDef(R3BTofDContFact, 0) // Factory for all TofD parameter containers
};

#endif /* !R3BTOFDCONTFACT_H */
