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

#ifndef R3BRPCCALPAR_H
#define R3BRPCCALPAR_H

#include "FairParGenericSet.h"
#include "TObject.h"

#include "TArrayF.h"
#include "TObjArray.h"
#include <TObjString.h>

class FairParamList;

class R3BRpcCalPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BRpcCalPar(const char* name = "RPCCalPar",
                 const char* title = "RPC Cal Parameters",
                 const char* context = "RPCCalParContext");

    /** Destructor **/
    virtual ~R3BRpcCalPar();

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
    const Double_t GetNumChannels() { return fNumChannels; }
    TArrayF* GetCalParams() { return fCalParams; }

    void SetNumChannels(Int_t numberCha) { fNumChannels = numberCha; }
    void SetCalParams(Float_t cc, Int_t ii) { fCalParams->AddAt(cc, ii); }

    /** Create more Methods if you need them! **/

  private:
    TArrayF* fCalParams; /*< Calibration Parameters of Channels>*/
    Int_t fNumChannels;  /*< number of channels>*/

    const R3BRpcCalPar& operator=(const R3BRpcCalPar&);
    R3BRpcCalPar(const R3BRpcCalPar&);

    ClassDef(R3BRpcCalPar, 1);
};

#endif /* R3BRPCCALPAR_H */
