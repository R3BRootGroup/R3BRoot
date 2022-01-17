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

#ifndef R3BRPCMAPPED2CALPAR_H
#define R3BRPCMAPPED2CALPAR_H

#include "FairTask.h"
#include "TH1F.h"

class TClonesArray;
class R3BRpcCalPar;
class R3BEventHeader;

class R3BRpcMapped2CalPar : public FairTask
{

  public:
    /** Default constructor **/
    R3BRpcMapped2CalPar();

    /** Standard constructor **/
    R3BRpcMapped2CalPar(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BRpcMapped2CalPar();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method FinishEvent **/
    virtual void FinishEvent();

    /** Virtual method FinishTask **/
    virtual void FinishTask();

    /** Virtual method Reset **/
    virtual void Reset();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();

    /** Accessor functions **/
    const Int_t GetNumChannels() { return fNumChannels; }

    void SetNumChannels(Int_t numberCha) { fNumChannels = numberCha; }

    void SetDebugMode(Bool_t debug) { fDebugMode = debug; }

  private:
    void SetParameter();
    Bool_t fDebugMode;
    Int_t fNumChannels;

    R3BRpcCalPar* fCalPar;       /**< Container for Cal parameters. >*/
    TClonesArray* fMappedDataCA; /**< Array with RPC Mapped-input data. >*/

  public:
    ClassDef(R3BRpcMapped2CalPar, 1);
};

#endif
