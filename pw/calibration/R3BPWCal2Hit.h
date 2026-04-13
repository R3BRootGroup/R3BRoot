/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
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
#include "R3BPWHitData.h"
#include "R3BPWHitPar.h"
#include "Rtypes.h"

class TClonesArray;
class R3BTGeoPar;

class R3BPWCal2Hit : public FairTask
{

  public:
    /** Default constructor **/
    R3BPWCal2Hit();

    /** Destructor **/
    ~R3BPWCal2Hit() override;

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
    TArrayF* fParCont = nullptr;
    TClonesArray* fPWCalDataCA = nullptr;
    TClonesArray* fPWHitDataCA = nullptr;
    Bool_t fOnline = false; // Selector for online data storage
    R3BPWHitPar* fHitPar;   // Parameter class

    R3BPWHitData* AddHitBar(UInt_t iDet, UInt_t bar, double time, double pos, double charge, double tof);

  public:
    ClassDefOverride(R3BPWCal2Hit, 1);
};
