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

// ----------------------------------------------------------------
// -----		 R3BMwpc2CalPar 		        -----
// -----     Created 10/10/19 by J.L. Rodriguez-Sanchez       -----
// ----------------------------------------------------------------

#ifndef R3BMwpc2CalPar_H
#define R3BMwpc2CalPar_H 1

#include "FairParGenericSet.h"
#include "TArrayF.h"
#include "TObjArray.h"
#include "TObject.h"
#include <TObjString.h>

class FairParamList;

class R3BMwpc2CalPar : public FairParGenericSet
{
  public:
    /** Standard constructor **/
    R3BMwpc2CalPar(const char* name = "mwpc2CalPar",
                   const char* title = "Mwpc2 Cal Parameters",
                   const char* context = "Mwpc2CalParContext");

    /** Destructor **/
    virtual ~R3BMwpc2CalPar();

    /** Method to reset all parameters **/
    virtual void clear();

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Method to print values of parameters to the standard output **/
    void printParams();

    /** Accessor functions **/
    const Int_t GetNumPadsX() { return fNumPadsX; }
    const Int_t GetNumPadsY() { return fNumPadsY; }
    const Int_t GetNumParametersFit() { return fNumParamsFit; }

    TArrayF* GetPadCalParams() { return fPadCalParams; }

    void SetNumPadsX(Int_t numberPadsX) { fNumPadsX = numberPadsX; }
    void SetNumPadsY(Int_t numberPadsY) { fNumPadsY = numberPadsY; }
    void SetNumParametersFit(Int_t numberParams) { fNumParamsFit = numberParams; }
    void SetPadCalParams(Float_t cc, Int_t ii) { fPadCalParams->AddAt(cc, ii); }

    /** Create more Methods if you need them! **/

  private:
    TArrayF* fPadCalParams;
    Int_t fNumPadsX;
    Int_t fNumPadsY;
    Int_t fNumParamsFit;

    const R3BMwpc2CalPar& operator=(const R3BMwpc2CalPar&); // An assignment operator

    R3BMwpc2CalPar(const R3BMwpc2CalPar&); // A copy constructor

    ClassDef(R3BMwpc2CalPar, 0);
};

#endif
