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
#include "R3BPWTotCalPar.h"
#include "TH1F.h"

class TClonesArray;
class R3BPWTotCalPar;

class R3BPWPreCal2CalPar : public FairTask
{

  public:
    /** Default constructor **/
    R3BPWPreCal2CalPar();

    /** Standard constructor **/
    R3BPWPreCal2CalPar(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BPWPreCal2CalPar() override;

    /** Virtual method Init **/
    InitStatus Init() override;

    /** Virtual method Exec **/
    void Exec(Option_t* opt) override;

    /** Virtual method FinishTask **/
    void FinishTask() override;

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

  private:
    Int_t fNumChannels = 32;
    R3BPWTotCalPar* fTotCalPar = nullptr;
    TClonesArray* fPreCalDataCA = nullptr; /**< Array with PW PreCal-input data. >*/
    TH1F* fhTot[N_NUM];

  public:
    ClassDefOverride(R3BPWPreCal2CalPar, 1);
};
