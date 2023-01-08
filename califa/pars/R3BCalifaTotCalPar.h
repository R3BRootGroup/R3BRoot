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

#ifndef R3BCalifaTotCalPar_H
#define R3BCalifaTotCalPar_H

#include "FairParGenericSet.h"
#include "TObject.h"

#include "TArrayF.h"
#include "TObjArray.h"
#include <TObjString.h>

class FairParamList;

class R3BCalifaTotCalPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BCalifaTotCalPar(const char* name = "califaTotCalPar",
                       const char* title = "Califa_TotCal_Parameters",
                       const char* context = "CalifaTotCalParContext");

    /** Destructor **/
    virtual ~R3BCalifaTotCalPar();

    /** Method to reset all parameters **/
    virtual void clear();

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Method to print values of parameters to the standard output **/
    virtual void print();
    void printParams();

    /** Accessor functions **/
    const Double_t GetNumCrystals() { return fNumCrystals; }
    const Double_t GetNumParametersFit() { return fNumParamsFit; }
    TArrayF* GetCryCalParams() { return fCryCalParams; }

    void SetNumCrystals(Int_t numberCry) { fNumCrystals = numberCry; }
    void SetNumParametersFit(Int_t numberParams) { fNumParamsFit = numberParams; }
    void SetCryCalParams(Float_t cc, Int_t ii) { fCryCalParams->AddAt(cc, ii); }

    /** Create more Methods if you need them! **/

  private:
    TArrayF* fCryCalParams; /*< Calibration Parameters of Crystals>*/
    Int_t fNumCrystals;     /*< number of crystals>*/
    Int_t fNumParamsFit;    /*< number of cal parameters in the fit
                  pol1: A_fit & B_fit
                  pol2: A_fit, B_fit & C_fit>*/

    const R3BCalifaTotCalPar& operator=(const R3BCalifaTotCalPar&);
    R3BCalifaTotCalPar(const R3BCalifaTotCalPar&);

    ClassDef(R3BCalifaTotCalPar, 1);
};

#endif /* R3BCalifaTotCalPar_H */
