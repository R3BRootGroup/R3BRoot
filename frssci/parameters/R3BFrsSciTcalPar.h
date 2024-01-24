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

#ifndef R3BFRSSCITCALPAR_H
#define R3BFRSSCITCALPAR_H

#include "FairParGenericSet.h"
#include "TArrayD.h"
#include "TObjArray.h"
#include "TObject.h"

#include <TObjString.h>

class FairParamList;

class R3BFrsSciTcalPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BFrsSciTcalPar(const char* name = "FrsSciTcalPar",
                     const char* title = "FrsSciTcal Parameters",
                     const char* context = "FrsSciTcalParContext");

    /** Destructor **/
    virtual ~R3BFrsSciTcalPar();

    /** Method to reset all parameters **/
    virtual void clear();

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Method to print values of parameters to the standard output **/
    void printParams();

    /** Accessor functions **/

    void SetNumDets(Int_t nDets) { fNumDets = nDets; }
    void SetNumPmts(Int_t nPmts) { fNumPmts = nPmts; }
    void SetNumPmts() { fNumPmts = 3; }
    void SetNumPars(Int_t n) { fNumPars = n; }
    void SetNumPars() { fNumPars = 1000; }
    void SetMinStat(Int_t stat) { fMinStat = stat; }
    void SetOneTcalParam(Double_t ft_ns, UInt_t rank) { fAllTcalParams->AddAt(ft_ns, rank); }

    const Int_t GetNumDets() { return fNumDets; }
    const Int_t GetNumPmts() { return fNumPmts; }
    const Int_t GetNumPars() { return fNumPars; }
    const Int_t GetMinStat() { return fMinStat; }

    TArrayD* GetAllTcalParams() { return fAllTcalParams; }
    Double_t GetOneTcalParam(UInt_t rank) { return (Double_t)fAllTcalParams->GetAt(rank); }

  private:
    Int_t fNumDets; // number of FrsSci detectors
    Int_t fNumPmts; // number of Pmts  (=3)
    Int_t fNumPars; // 1000 parameters per signal for VFTX calibration
    Int_t fMinStat;
    TArrayD* fAllTcalParams; // Calibration Parameters for all signals of all detectors

    const R3BFrsSciTcalPar& operator=(const R3BFrsSciTcalPar&);
    R3BFrsSciTcalPar(const R3BFrsSciTcalPar&);

    ClassDef(R3BFrsSciTcalPar, 1);
};

#endif // R3BFRSSCITCALPAR_H
