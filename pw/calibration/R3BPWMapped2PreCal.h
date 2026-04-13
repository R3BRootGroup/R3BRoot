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

// -----------------------------------------------------------------------------

#pragma once

#include "FairTask.h"

#include "R3BPWMapped2PreCalPar.h"
#include "R3BPWMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include "R3BPWPreCalData.h"

#include <TRandom.h>

class TClonesArray;
class R3BTCalModulePar;

class R3BPWMapped2PreCal : public FairTask
{

  public:
    /** Default constructor **/
    R3BPWMapped2PreCal();

    /** Destructor **/
    ~R3BPWMapped2PreCal() override;

    /** Method Exec **/
    void Exec(Option_t* option) override;

    /** Virtual method Reset **/
    virtual void Reset();

    /** Method Init **/
    InitStatus Init() override;

    /** Method ReInit **/
    InitStatus ReInit() override;

    /** Accessor to select online mode **/
    void SetOnline(bool option = true) { fOnline = option; }

  private:
    Double_t fClockFreq = (1. / VFTX_CLOCK_MHZ * 1000.); /**< Clock cycle in [ns]. */
    Bool_t fOnline = false;                              // Don't store data for online
    R3BTCalPar* fTCalPar = nullptr;                      /**< Parameter container. >*/
    TClonesArray* fMappedDataCA = nullptr;               /**< Array with PW Mapped-input data. >*/
    TClonesArray* fPWPreCalDataCA = nullptr;             /**< Array with PW Cal- output data. >*/

    struct Entry
    {
        double time;
        R3BPWMappedData const* Mapped;
    };

  public:
    // Class definition
    ClassDefOverride(R3BPWMapped2PreCal, 1)
};
