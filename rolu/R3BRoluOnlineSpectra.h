/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------
// -----                  R3BRoluOnlineSpectra            -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------

#pragma once

#include <FairTask.h>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <TClonesArray.h>
#include <TFolder.h>
#include <TMath.h>
#include <cstdlib>

#define VFTX_CLOCK_MHZ 200

constexpr size_t fNofRolu = 2;

class TClonesArray;
class R3BCoarseTimeStitch;
class TH1F;
class TH2F;
class R3BEventHeader;

/**
 * This tasks reads ROLU data items and plots histograms
 * for online checks.
 */
class R3BRoluOnlineSpectra : public FairTask
{
  public:
    // Default constructor.
    R3BRoluOnlineSpectra();

    // Standard constructor.
    explicit R3BRoluOnlineSpectra(const char* name, int iVerbose = 1);

    // Destructor.
    virtual ~R3BRoluOnlineSpectra() = default;

    // Method for task initialization.
    InitStatus Init() override;

    // Method for event loop implementation.
    void Exec(Option_t*) override;

    // A method for finish of processing of an event.
    void FinishEvent() override;

    // Method for finish of the task execution.
    void FinishTask() override;

    // Method for setting the trigger value.
    //  @param trigger 1 - physics, 2 - offspill, -1 - all events.
    inline void SetTrigger(int trigger) { fTrigger = trigger; }
    inline void SetTpat(int tpat) { fTpat = tpat; }

    void Reset_ROLU_Histo();

    // Method for setting number of ROLU detectors
    inline void SetNofRoluModules(int nDets) { fNofRoluDetectors = nDets; }

  private:
    TClonesArray* fMappedItems = nullptr;
    TClonesArray* fCalItems = nullptr;

    R3BEventHeader* header{};
    int fTrigger = -1;
    int fTpat = -1;
    double fClockFreq = 1. / VFTX_CLOCK_MHZ * 1000.; // Clock cycle in [ns]
    int nRoluEvents = 0;
    int fNofRoluDetectors = 1; // Number of ROLU detectors

    const char* fDetectorNames[fNofRolu] = { "Rolu" };

    std::vector<TH1F*> fh1_rolu_channels;
    std::vector<TH1F*> fh1_rolu_multiplicity;
    std::vector<TH2F*> fh2_rolu_tot;
    std::vector<TH2F*> fh2_rolu_hit1vshit2;
    std::vector<std::vector<TH1F*>> fh1_rolu_tot;
    std::vector<std::vector<TH1F*>> fh1_rolu_LE_raw;

  public:
    // Class definition
    ClassDefOverride(R3BRoluOnlineSpectra, 0); // NOLINT
};
