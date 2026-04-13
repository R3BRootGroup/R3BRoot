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
#include "TH1F.h"

class TClonesArray;
class R3BEventHeader;

class R3BTCalEngine;
class R3BTCalPar;

class R3BPWMapped2PreCalPar : public FairTask
{

  public:
    /** Default constructor **/
    R3BPWMapped2PreCalPar();

    /** Standard constructor **/
    R3BPWMapped2PreCalPar(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BPWMapped2PreCalPar() override;

    /** Virtual method Init **/
    InitStatus Init() override;

    /** Virtual method Exec **/
    void Exec(Option_t* opt) override;

    /** Virtual method FinishEvent **/
    void FinishEvent() override;

    /** Virtual method FinishTask **/
    void FinishTask() override;

    /** Virtual method Reset **/
    virtual void Reset();

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

  private:
    Int_t fNumChannels = 39;
    R3BTCalPar* fTCalPar = nullptr;        /**< Parameter container. */
    R3BTCalEngine* fEngine = nullptr;      /**< Instance of the TCAL engine. */
    TClonesArray* fMappedDataCA = nullptr; /**< Array with PW Mapped-input data. >*/

  public:
    ClassDefOverride(R3BPWMapped2PreCalPar, 1);
};
