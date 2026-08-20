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

// -----------------------------------------------------------------------------

#pragma once

#include "FairTask.h"

#include "R3BDMCalData.h"
#include "R3BDMMapped2CalPar.h"
#include "R3BDMMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BTCalPar.h"

#include <TRandom.h>

class TClonesArray;
class R3BTCalModulePar;

class R3BDMMapped2Cal : public FairTask
{

  public:
    /** Default constructor **/
    R3BDMMapped2Cal();

    /** Destructor **/
    ~R3BDMMapped2Cal() override;

    /** Virtual method Exec **/
    void Exec(Option_t* option) override;

    /** Virtual method Reset **/
    virtual void Reset();

    // Fair specific Virtual method Init
    InitStatus Init() override;

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

    /** Accessor to select online mode **/
    void SetOnline(bool option = true) { fOnline = option; }

  private:
    Double_t fClockFreq = (1. / VFTX_CLOCK_MHZ * 1000.); /**< Clock cycle in [ns]. */
    Bool_t fOnline = false;
    R3BTCalPar* fTCalPar = nullptr;        /**< Parameter container. >*/
    TClonesArray* fMappedDataCA = nullptr; /**< Array with DM Mapped-input data. >*/
    TClonesArray* fDMCalDataCA = nullptr;  /**< Array with DM Cal- output data. >*/

    struct Entry
    {
        double time;
        R3BDMMappedData const* Mapped;
    };

  public:
    // Class definition
    ClassDefOverride(R3BDMMapped2Cal, 1)
};
