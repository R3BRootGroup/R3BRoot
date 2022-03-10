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

#ifndef R3BRPCCAL2HIT_H
#define R3BRPCCAL2HIT_H

#include "FairTask.h"
#include "R3BRpcStripHitData.h"
#include "R3BRpcPmtHitData.h"
#include "R3BRpcHitPar.h"
#include "Rtypes.h"

#include <TVector3.h>

class TClonesArray;
class R3BTGeoPar;

class R3BRpcCal2Hit : public FairTask
{

  public:
    /** Default constructor
     **/
    R3BRpcCal2Hit();

    /** Destructor **/
    virtual ~R3BRpcCal2Hit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();

    /** Virtual method Finish **/
     virtual void Finish();

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

  private:
    TClonesArray* fCalDataCA;
    TClonesArray* fRpcHitStripDataCA;
    TClonesArray* fRpcHitPmtDataCA;
    TClonesArray* fRpcCalStripDataCA;
    TClonesArray* fRpcCalPmtDataCA;

    TArrayF* fParCont1;
    TArrayF* fParCont2;

    Bool_t fOnline; // Selector for online data storage
    // Parameter class
    R3BRpcHitPar* fHitPar;


    R3BRpcStripHitData* AddHit(UInt_t channel, double time, double pos, double charge);


    /** Private method AddHit
     ** Adds a RpcHit to the HitCollection
     **/

    ClassDef(R3BRpcCal2Hit, 1);
};

#endif /* R3BRPCCAL2HIT_H */
