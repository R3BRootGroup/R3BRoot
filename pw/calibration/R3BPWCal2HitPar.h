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
#include "R3BPWHitPar.h"
#include "TGraph.h"
#include "TH1F.h"

class TClonesArray;
class R3BEventHeader;

class R3BPWCal2HitPar : public FairTask
{

  public:
    /** Default constructor **/
    R3BPWCal2HitPar();

    /** Standard constructor **/
    R3BPWCal2HitPar(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BPWCal2HitPar() override;

    /** Virtual method Init **/
    InitStatus Init() override;

    /** Virtual method Exec **/
    void Exec(Option_t* opt) override;

    /** Virtual method FinishTask **/
    void FinishTask() override;

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

    /** Virtual method SetParContainers **/
    void SetParContainers() override;

    void SetNBCalib() { fPWCalib = false; }

  private:
    void CalculateParsBar();

    Bool_t fPWCalib = true;
    Int_t fNumChannels = 16;
    R3BPWHitPar* fHitPar = nullptr;     /**< Container for Hit parameters. >*/
    TClonesArray* fCalDataCA = nullptr; /**< Array with Cal PW Strip - input data. >*/
    TH1F* fhTime[N_BAR - 1];

  public:
    ClassDefOverride(R3BPWCal2HitPar, 1);
};
