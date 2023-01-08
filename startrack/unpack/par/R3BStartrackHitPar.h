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

#ifndef R3BSTARTRACKHITPAR_H
#define R3BSTARTRACKHITPAR_H 1

#include <TObjString.h>
#include <TVector3.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BStartrackHitPar : public FairParGenericSet
{
  public:
    R3BStartrackHitPar(const char* name = "R3BStartrackHitPar",
                       const char* title = "The Hit  parameter",
                       const char* context = "TestDefaultContext");
    ~R3BStartrackHitPar(void){};
    void clear(void){};
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    virtual void Print(Option_t* option = "") const;
    /** Accessor functions **/
    const Double_t GetThreshold() { return fThreshold; }
    const Double_t GetEStripResolution() { return fEStripResolution; }

    void SetThreshold(Double_t value) { fThreshold = value; }
    void SetEStripResolution(Double_t value) { fEStripResolution = value; }

  private:
    // Hit. Parameters

    // Minimum energy requested in a Strip to be considered in a startrack Hit
    Double_t fThreshold;
    // Experimental resolution @ 1 MeV
    Double_t fEStripResolution;

    ClassDef(R3BStartrackHitPar, 1); //
};

#endif /* !R3BSTARTRACKHITPAR_H*/
