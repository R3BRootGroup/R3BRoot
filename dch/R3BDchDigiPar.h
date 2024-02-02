/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3BDCHDIGIPAR_H
#define R3BDCHDIGIPAR_H 1

#include <TObjString.h>
#include <TVector3.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BDchDigiPar : public FairParGenericSet
{
  public:
    R3BDchDigiPar(const char* name = "R3BDchDigiParTest",
                  const char* title = "Tutorial  parameter",
                  const char* context = "TestDefaultContext");
    ~R3BDchDigiPar(void){};
    void clear(void){};
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    virtual void Print(Option_t* option) const;
    /** Accessor functions **/

  private:
    // Digi. Parameters

    ClassDef(R3BDchDigiPar, 1); //
};

#endif /* !R3BDCHDIGIPAR_H*/
