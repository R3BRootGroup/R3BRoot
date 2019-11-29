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

// --------------------------------------------------------------
// -----          R3BMusicHitPar source file                -----
// -----     Created 24/11/19  by J.L. Rodriguez-Sanchez    -----
// --------------------------------------------------------------

#ifndef R3BMusicHitPar_H
#define R3BMusicHitPar_H

#include "FairParGenericSet.h" // for FairParGenericSet

#include "TArrayF.h"
#include "TObjArray.h"
#include "TObject.h"
#include <TObjString.h>

class FairParamList;

class R3BMusicHitPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BMusicHitPar(const char* name = "musicHitPar",
                   const char* title = "MUSIC Hit Parameters",
                   const char* context = "MusicHitParContext");

    /** Destructor **/
    virtual ~R3BMusicHitPar();

    /** Method to reset all parameters **/
    virtual void clear();

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Method to print values of parameters to the standard output **/
    void printParams();

    /** Accessor functions **/
    const Double_t GetNumParametersFit() { return fNumParamsFit; }
    TArrayF* GetDetectorHitParams() { return fDetHitParams; }

    void SetNumParametersFit(Int_t numberParams) { fNumParamsFit = numberParams; }
    void SetDetectorHitParams(Double_t cc, Int_t ii) { fDetHitParams->AddAt(cc, ii); }

    // Create more Methods if you need them!

  private:
    TArrayF* fDetHitParams; // Calibration Parameters for detector
    Int_t fNumParamsFit;    /* number of cal parameters in the fit
                               gaus: A_fit & B_fit & C_fit*/

    const R3BMusicHitPar& operator=(const R3BMusicHitPar&); /*< an assignment operator>*/

    R3BMusicHitPar(const R3BMusicHitPar&); /*< a copy constructor >*/

    ClassDef(R3BMusicHitPar, 1);
};

#endif
