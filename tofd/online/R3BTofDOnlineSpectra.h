/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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
// -----               R3BTofDOnlineSpectra               -----
// -----    Created 18/03/22 by J.L. Rodriguez-Sanchez    -----
// -----           Fill Tofd online histograms            -----
// ------------------------------------------------------------

#ifndef R3BTofDOnlineSpectra_H
#define R3BTofDOnlineSpectra_H 1

#include "FairTask.h"
#include "TCanvas.h"
#include <vector>

class TClonesArray;
class TH1F;
class TH2F;
class R3BTimeStitch;
class R3BEventHeader;
class R3BTofDMappingPar;

class R3BTofDOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BTofDOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BTofDOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTofDOnlineSpectra();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option);

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent();

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();

    virtual void SetParContainers();

    /**
     * Methods to clean histograms.
     */
    virtual void Reset_Histo();

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    void SetTpat(Int_t tpat1, Int_t tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }

    /**
     * Methods for setting number of planes and paddles
     */
    void SetNofModules(Int_t planes, Int_t p)
    {
        fNofPlanes = planes;
        fPaddlesPerPlane = p;
    }

    void SetMaxMult(Int_t m) { fMaxmul = m; }

    void SetClockRange(Double_t r) { fC_range_ns = r; }

    void SetClockCoincidenceRange(Double_t r) { fC_bar_coincidence_ns = r; }

  private:
    void SetParameter();
    R3BTimeStitch* fTimeStitch;
    TClonesArray* fMappedItems;
    TClonesArray* fCalItems;
    TClonesArray* fHitItems;
    TClonesArray* fCalTriggerItems; /**< Array with trigger Cal items - input data. */

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    R3BTofDMappingPar* fMapPar;
    Int_t fTrigger; /**< Trigger value. */
    Int_t fTpat1, fTpat2;
    UInt_t fNofPlanes;
    Int_t fMaxmul;
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */
    Double_t fC_range_ns;
    Double_t fC_bar_coincidence_ns;
    unsigned long fNEvents; /**< Event counter. */

    std::vector<TH1F*> fh_tofd_channels;
    std::vector<TH2F*> fh_tofd_multihit;
    std::vector<TH2F*> fh_tofd_TotPm;
    std::vector<TH2F*> fh_tofd_dt;
    std::vector<TH2F*> fh_tofd_multihit_coinc;
    std::vector<TH2F*> fh_tofd_TotPm_coinc;
    std::vector<TH2F*> fh_num_side;
    std::vector<TH1F*> fh_tofd_bars;
    std::vector<TH2F*> fh_tofd_Tot_hit;
    std::vector<TH2F*> fh_tofd_time_hit;
    std::vector<TH1F*> fh_tofd_multihit_hit;
    std::vector<TH2F*> fh_tofd_dt_hit;

    std::vector<TH2F*> fh_tofd_time_los_h2;
    std::vector<TH1F*> fh_tofd_time_los[44];

  public:
    ClassDef(R3BTofDOnlineSpectra, 1)
};

#endif /* R3BTofDOnlineSpectra_H */
