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
// -----             R3BAlpideOnlineSpectra               -----
// -----    Created 18/03/22 by J.L. Rodriguez-Sanchez    -----
// -----          Fill Alpide online histograms           -----
// ------------------------------------------------------------

#ifndef R3BAlpideOnlineSpectra_H
#define R3BAlpideOnlineSpectra_H 1

#include "FairTask.h"
#include "TCanvas.h"
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
    virtual ~R3BAlpideOnlineSpectra();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init() override;

    /** Virtual method ReInit **/
    virtual InitStatus ReInit() override;

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option) override;

    virtual void SetParContainers() override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent() override;

    /**
     * Methods to clean histograms.
     */
    virtual void Reset_Histo();

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat1, Int_t tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }

  private:
    void SetParameter();

    TClonesArray* fMappedItems;
    TClonesArray* fCalItems;
    TClonesArray* fHitItems;

    R3BEventHeader* header;        /**< Event header. */
    R3BAlpideMappingPar* fMap_Par; /**< Parameter container. >*/
    Int_t fTrigger;                /**< Trigger value. */
    Int_t fTpat1, fTpat2;
    Int_t fNEvents;
    UInt_t fNbSensors;

    std::vector<TH2F*> fh2_ColVsRow;
    std::vector<TH2F*> fh2_ColVsRowCal;
    std::vector<TH2F*> fh2_PosHit;
    std::vector<TH1F*> fh1_Calmult;
    std::vector<TH1F*> fh1_Clustermult;
    std::vector<TH1F*> fh1_Clustersize;

  public:
    ClassDefOverride(R3BAlpideOnlineSpectra, 1)
};

#endif
