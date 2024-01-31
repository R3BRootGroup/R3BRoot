/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
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
// -----             R3BAlpideOnlineSpectra               -----
// -----    Created 18/03/22 by J.L. Rodriguez-Sanchez    -----
// -----          Fill Alpide online histograms           -----
// ------------------------------------------------------------

#pragma once

#include <FairTask.h>
#include <TCanvas.h>
#include <vector>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;
class R3BAlpideMappingPar;

class R3BAlpideOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BAlpideOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BAlpideOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BAlpideOnlineSpectra() = default;

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init() override;

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    void Exec(Option_t* /*option*/) override;

    void SetParContainers() override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    void FinishEvent() override;

    /**
     * Methods to clean histograms.
     */
    virtual void Reset_Histo();

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

  private:
    void SetParameter();

    TClonesArray* fMappedItems = nullptr;
    TClonesArray* fCalItems = nullptr;
    TClonesArray* fHitItems = nullptr;

    R3BEventHeader* header = nullptr;        /**< Event header. */
    R3BAlpideMappingPar* fMap_Par = nullptr; /**< Parameter container. >*/
    int fTrigger = -1;                       /**< Trigger value. */
    int fTpat1 = 0, fTpat2 = 0;
    int fNEvents = 0;
    uint16_t fNbSensors = 24;

    std::vector<TH2F*> fh2_ColVsRow;
    std::vector<TH2F*> fh2_ColVsRowCal;
    std::vector<TH2F*> fh2_PosHit;
    std::vector<TH1F*> fh1_Calmult;
    std::vector<TH1F*> fh1_Clustermult;
    std::vector<TH1F*> fh1_Clustersize;

  public:
    ClassDefOverride(R3BAlpideOnlineSpectra, 1)
};
