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

constexpr int N_NUM = 72;

#include "FairParGenericSet.h"
#include "TObject.h"

#include "TArrayF.h"
#include "TObjArray.h"
#include <TObjString.h>

class FairParamList;

class R3BDMTotCalPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BDMTotCalPar(const char* name = "DMTotCalPar",
                   const char* title = "DM Cal Parameters",
                   const char* context = "R3BDMTotCalParContext");

    /** Destructor **/
    virtual ~R3BDMTotCalPar();

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
    [[nodiscard]] TArrayF* GetCalParams() const { return fCalParams; }

    void SetNumChannels(Int_t numberCha) { fNumChannels = numberCha; }
    void SetCalParams(Float_t cc, Int_t ii) { fCalParams->AddAt(cc, ii); }

    /** Create more Methods if you need them! **/

  private:
    Int_t fNumChannels;  /*< number of channels>*/
    TArrayF* fCalParams; /*< Calibration Parameters of Channels>*/

    const R3BDMTotCalPar& operator=(const R3BDMTotCalPar&);
    R3BDMTotCalPar(const R3BDMTotCalPar&);

  public:
    ClassDefOverride(R3BDMTotCalPar, 1);
};
