/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ---------------------------------------------------------------
// -----  R3BFiberMAPMTCorrelationOnlineSpectra source file  -----
// -----    Created 27/04/22 by J.L. Rodriguez-Sanchez       -----
// ---------------------------------------------------------------

#pragma once

#include <FairTask.h>

class TClonesArray;
class TH2F;
class R3BEventHeader;
class R3BFiberMappingPar;

class R3BFiberMAPMTCorrelationOnlineSpectra : public FairTask
{
  public:
    R3BFiberMAPMTCorrelationOnlineSpectra();

    explicit R3BFiberMAPMTCorrelationOnlineSpectra(const TString, const TString, int iVerbose = 1);

    virtual ~R3BFiberMAPMTCorrelationOnlineSpectra() = default;

    InitStatus Init() override;

    InitStatus ReInit() override;

    void Exec(Option_t*) override;

    void FinishEvent() override;

    void FinishTask() override;

    void SetParContainers() override;

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(int trigger) { fTrigger = trigger; }
    inline void SetTpat(int tpat1, int tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }

    void Reset_Histo();

  private:
    TString fName1, fName2;
    UInt_t fNbfibers1 = 512, fNbfibers2 = 512;
    R3BFiberMappingPar* fMapPar1 = nullptr;
    R3BFiberMappingPar* fMapPar2 = nullptr;
    TClonesArray* fHitItems1 = nullptr;
    TClonesArray* fHitItems2 = nullptr;

    R3BEventHeader* header = nullptr;
    int fTrigger = -1;
    int fTpat1 = -1, fTpat2 = -1;
    unsigned long long fNEvents = 0;

    TH2F* fh_Fib_posX;
    TH2F* fh_Fib_posY;
    TH2F* fh_Fib_tot;

  public:
    // Class definition
    ClassDefOverride(R3BFiberMAPMTCorrelationOnlineSpectra, 1); // NOLINT
};
