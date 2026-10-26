/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include "FairTask.h"
#include "R3BHFIBCal2HitPar.h"
#include "R3BHFIBCalData.h"
#include "R3BHFIBHitData.h"
#include "Rtypes.h"
#include <TRandom3.h>
#include <TVector3.h>

class TClonesArray;
class R3BHFIBTotCalPar;

class R3BHFIBCal2Hit : public FairTask
{

  public:
    // Default constructor
    R3BHFIBCal2Hit();

    /** Destructor **/
    ~R3BHFIBCal2Hit() override;

    /** Virtual method Exec **/
    void Exec(Option_t* opt) override;

    /** Virtual method Reset **/
    virtual void Reset();

    /** Accessor to select online mode **/
    void SetOnline(bool option = true) { fOnline = option; }

    /** Virtual method Init **/
    InitStatus Init() override;

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

  private:
    Int_t fNumChannels = 256;
    R3BHFIBTotCalPar* fTotCalPar = nullptr;
    TArrayF* fParCont = nullptr;
    Bool_t fOnline = false;
    TClonesArray* fHFIBCalDataCA = nullptr;
    TClonesArray* fHFIBHitDataCA = nullptr;
    TRandom3* rnd;

    R3BHFIBHitData* AddHitFiber(UInt_t iDet, UInt_t fiber, double pos, double time, double charge);

  public:
    ClassDefOverride(R3BHFIBCal2Hit, 1);
};
