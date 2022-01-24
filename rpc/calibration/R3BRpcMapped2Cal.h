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

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                     R3BRpcMapped2Cal                              -----
// -----                Created 17/01/2021 by H. Alvarez-Pol               -----
// -----------------------------------------------------------------------------

#ifndef R3BRPCMAPPED2CAL_H
#define R3BRPCMAPPED2CAL_H

#include "FairTask.h"
#include "R3BRpc.h"
#include "R3BRpcCalData.h"
#include "R3BRpcMapped2CalPar.h"
#include "R3BRpcMappedData.h"
#include <TRandom.h>

class TClonesArray;
class R3BRpcCalPar;

class R3BRpcMapped2Cal : public FairTask
{

  public:
    /** Default constructor **/
    R3BRpcMapped2Cal();

    /** Destructor **/
    virtual ~R3BRpcMapped2Cal();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option);

    /** Virtual method Reset **/
    virtual void Reset();

    virtual void SetParContainers();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Finish **/
    virtual void Finish();

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    void SetParameter();

    Int_t fNumChannels;
    TArrayF* fCalParams;
    // Don't store data for online
    Bool_t fOnline;

    R3BRpcCalPar* fCal_Par;         /**< Parameter container. >*/
    TClonesArray* fRpcMappedDataCA; /**< Array with RPC Mapped- input data. >*/
    TClonesArray* fRpcCalDataCA;    /**< Array with RPC Cal- output data. >*/

    /** Private method AddCalData **/
    //** Adds a RpcCalData to the RpcCalCollection
    R3BRpcCalData* AddCalData(Int_t id, Double_t energy, uint64_t wrts, Double_t tot_energy);

  public:
    // Class definition
    ClassDef(R3BRpcMapped2Cal, 1)
};

#endif
