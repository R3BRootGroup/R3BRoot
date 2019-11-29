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

#ifndef R3BNEULANDHITPAR_H
#define R3BNEULANDHITPAR_H

#include "FairParGenericSet.h" // for FairParGenericSet
#include "R3BNeulandHitModulePar.h"
#include "TObjArray.h"

class FairParamList;

#define NMODULEMAX 6000

class R3BNeulandHitPar : public FairParGenericSet
{

  public:
    /**
     * Standard constructor. Creates instance of this class.
     * @param name a name of container.
     * @param title a title of container.
     * @param context context/purpose for parameters and conditions.
     * @param own class ownership, if flag is kTRUE FairDB has the par.
     */
    R3BNeulandHitPar(const char* name = "NeulandHitPar",
                     const char* title = "Calibration",
                     const char* context = "TestDefaultContext",
                     Bool_t own = kTRUE);
    /**
     * Destructor. Cleares the memory used by the object.
     */
    virtual ~R3BNeulandHitPar(void);

    /**
     * Method to reset the values of parameters.
     */
    void clear(void);

    /**
     * Method to store parameters using FairRuntimeDB.
     * @param list a list of parameters.
     */
    void putParams(FairParamList* list);

    /**
     * Method to retrieve parameters using FairRuntimeDB.
     * @param list a list of parameters.
     * @return kTRUE if successful, else kFALSE.
     */
    Bool_t getParams(FairParamList* list);

    /**
     * Method to print value of parameters to the standard output.
     * Calls printParams() for every module container.
     */
    void printParams();

    /**
     * Method to add parameter container for a module.
     * Extends the array.
     * @param tch a parameter container for a detector module.
     */
    void AddModulePar(R3BNeulandHitModulePar* tch) { fParams->Add(tch); }

    /**
     * Method to retrieve the arrray with module containers.
     * @return an array with parameter containers of type R3BNeulandHitModulePar.
     */
    TObjArray* GetListOfModulePar() { return fParams; }

    /**
     * Method to get number of modules storred in array.
     * @return size of array.
     */
    Int_t GetNumModulePar() { return fParams->GetEntriesFast(); }

    /**
     * Method to get single parameter container for a specific module.
     * @param idx an index of a module.
     * @return parameter container of this module.
     */
    R3BNeulandHitModulePar* GetModuleParAt(Int_t idx) { return (R3BNeulandHitModulePar*)fParams->At(idx); }

    // Global time offset in ns
    inline Double_t GetGlobalTimeOffset() { return fGlobalTimeOffset; }
    inline void SetGlobalTimeOffset(Double_t t0) { fGlobalTimeOffset = t0; }

  private:
    TObjArray* fParams; /**< an array with parameter containers of all modules */

    Double_t fGlobalTimeOffset;

    ClassDef(R3BNeulandHitPar, 2);
};

#endif /* !R3BNEULANDHITPAR_H*/
