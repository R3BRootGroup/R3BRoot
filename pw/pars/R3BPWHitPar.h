/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
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

#define N_BAR 16

#include <TObjString.h>
#include <TVector3.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "TArrayF.h"

class R3BPWHitPar : public FairParGenericSet
{
  public:
    /** Standard constructor **/
    R3BPWHitPar(const char* name = "PWHitPar",
                const char* title = "PW Hit Finder Parameters",
                const char* context = "R3BPWHitParContext");
    /** Destructor **/
    virtual ~R3BPWHitPar() override;

    /** Method to reset all parameters **/
    void clear() override;

    /** Method to store all parameters using FairRuntimeDB **/
    void putParams(FairParamList* list) override;

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list) override;

    /** Method to print values of parameters to the standard output **/
    void print() override;
    void printParams() override;

    /** Accessor functions **/
    [[nodiscard]] Double_t GetNumChannels() const { return fNumChannels; }
    [[nodiscard]] TArrayF* GetCalParams() { return fHitCalParams; }

    void SetNumChannels(Int_t numberCha) { fNumChannels = numberCha; }
    void SetCalParams(Float_t cc, Int_t ii) { fHitCalParams->AddAt(cc, ii); }

  private:
    Int_t fNumChannels; /*< number of channels>*/
    TArrayF* fHitCalParams;

    const R3BPWHitPar& operator=(const R3BPWHitPar&);
    R3BPWHitPar(const R3BPWHitPar&);

  public:
    ClassDefOverride(R3BPWHitPar, 2); //
};
