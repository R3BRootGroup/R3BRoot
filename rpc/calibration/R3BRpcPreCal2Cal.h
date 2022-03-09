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
// -----                     R3BRpcPreCal2Cal                              -----
// -----                Created 17/01/2021 by H. Alvarez-Pol               -----
// -----------------------------------------------------------------------------

#ifndef R3BRPCPRECAL2CAL_H
#define R3BRPCPRECAL2CAL_H

#include "FairTask.h"


#include "R3BRpcPreCal2CalPar.h"

#include "R3BRpcStripPreCalData.h"
#include "R3BRpcPmtPreCalData.h"

#include "R3BRpcStripCalData.h"
#include "R3BRpcPmtCalData.h"

#include <TRandom.h>

class TClonesArray;
class R3BRpcTotCalPar;

class R3BRpcPreCal2Cal : public FairTask
{

  public:
    /** Default constructor **/
    R3BRpcPreCal2Cal();

    /** Destructor **/
    virtual ~R3BRpcPreCal2Cal();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option);

    /** Virtual method Reset **/
    virtual void Reset();

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
    
    Int_t fNumChannels;
    R3BRpcTotCalPar* fTotCalPar;
    TArrayF* fParCont;
    UInt_t fNEvent;
    // Don't store data for online
    Bool_t fOnline;
    
    TClonesArray* fStripPreCalDataCA; /**< Array with RPC PreCal-input data. >*/
    TClonesArray* fPmtPreCalDataCA; /**< Array with RPC PreCal-input data. >*/

    TClonesArray* fRpcStripCalDataCA;    /**< Array with Strip Cal- output data. >*/
    TClonesArray* fRpcPmtCalDataCA;    /**< Array with Pmt Cal- output data. >*/
    Int_t event =0;
  public: 
    // Class definition
    ClassDef(R3BRpcPreCal2Cal, 1)
};  

#endif
