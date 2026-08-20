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
#include "R3BDMCal2HitPar.h"
#include "R3BDMCalData.h"
#include "R3BDMHitData.h"
#include "Rtypes.h"
#include <TRandom3.h>
#include <TVector3.h>

class TClonesArray;
class R3BDMTotCalPar;

class R3BDMCal2Hit : public FairTask
{

  public:
    // Default constructor
    R3BDMCal2Hit();

    /** Destructor **/
    ~R3BDMCal2Hit() override;

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
    Int_t fNumChannels = 72;
    R3BDMTotCalPar* fTotCalPar = nullptr;
    TArrayF* fParCont = nullptr;
    Bool_t fOnline = false;
    TClonesArray* fDMCalDataCA = nullptr;
    TClonesArray* fDMHitDataCA = nullptr;
    TRandom3* rnd;

    R3BDMHitData* AddHitDiamond(UInt_t iDet, UInt_t pad, double pos_x, double pos_y, double time, double charge);

  public:
    ClassDefOverride(R3BDMCal2Hit, 1);
};
