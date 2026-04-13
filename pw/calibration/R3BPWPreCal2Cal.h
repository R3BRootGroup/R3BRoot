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

#include "R3BPWCalData.h"
#include "R3BPWPreCal2CalPar.h"
#include "R3BPWPreCalData.h"

#include <TRandom.h>

class TClonesArray;
class R3BPWTotCalPar;

class R3BPWPreCal2Cal : public FairTask
{

  public:
    /** Default constructor **/
    R3BPWPreCal2Cal();

    /** Destructor **/
    ~R3BPWPreCal2Cal() override;

    /** Virtual method Exec **/
    void Exec(Option_t* option) override;

    /** Virtual method Reset **/
    virtual void Reset();

    // Fair specific
    /** Virtual method Init **/
    InitStatus Init() override;

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

    /** Accessor to select online mode **/
    void SetOnline(bool option = true) { fOnline = option; }

  private:
    Int_t fNumChannels = 32;
    R3BPWTotCalPar* fTotCalPar = nullptr;
    TArrayF* fParCont = nullptr;
    Bool_t fOnline = false;                // Don't store data for online
    TClonesArray* fPreCalDataCA = nullptr; /**< Array with PW PreCal-input data. >*/
    TClonesArray* fPWCalDataCA = nullptr;  /**< Array with Strip Cal- output data. >*/

  public:
    // Class definition
    ClassDefOverride(R3BPWPreCal2Cal, 1)
};
