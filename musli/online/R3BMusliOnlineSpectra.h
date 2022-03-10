/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
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
// -----                R3BMusliOnlineSpectra             -----
// -----    Created 06/10/19  by J.L. Rodriguez-Sanchez   -----
// -----           Fill MusIC online histograms           -----
// ------------------------------------------------------------

#ifndef R3BMusliOnlineSpectra_H
#define R3BMusliOnlineSpectra_H

#include "FairTask.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2F.h"
#include "TMath.h"
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

class TClonesArray;
class R3BEventHeader;

/**
 * This taks reads MusIC data and plots online histograms
 */
class R3BMusliOnlineSpectra : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BMusliOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BMusliOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BMusliOnlineSpectra();

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

    /**
     * Methods to clean histograms.
     */
    virtual void Reset_Histo();

  private:
    TClonesArray* fMapItemsMusli; /**< Array with mapped items. */

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header.      */
    Int_t fNEvents;         /**< Event counter.     */

    // Canvas
    TCanvas* cMusliMap_Mult;

    TCanvas* cMusliMap_E2;
    TCanvas* cMusliMap_E4;
    TCanvas* cMusliMap_E8;
    TCanvas* cMusliMap_E16;

    TCanvas* cMusliMap_EvsE2;
    TCanvas* cMusliMap_EvsE4;
    TCanvas* cMusliMap_EvsE8;

    TCanvas* cMusliMap_DT2;
    TCanvas* cMusliMap_DT4;
    TCanvas* cMusliMap_DT8;
    TCanvas* cMusliMap_DT16;

    TCanvas* cMusliMap_DTvsDT2;
    TCanvas* cMusliMap_DTvsDT4;
    TCanvas* cMusliMap_DTvsDT8;

    TCanvas* cMusliMap_EvsDT2;
    TCanvas* cMusliMap_EvsDT4;
    TCanvas* cMusliMap_EvsDT8;
    TCanvas* cMusliMap_EvsDT16;

    TCanvas* cMusliMap_Emean;

    TCanvas* cMusliMap_DeltaDT;

    // Histograms for Mapped data
    TH1I* fh1_Muslimap_mult;
    TH2I* fh2_Muslimap_mult;

    TH1I* fh1_Muslimap_E[15];
    TH2I* fh2_Muslimap_EvsE[14];
    TH1I* fh1_Muslimap_DT[15];
    TH2I* fh2_Muslimap_DTvsDT[14];
    TH2I* fh2_Muslimap_EvsDT[15];

    TH1F* fh1_Muslimap_Emean2;
    TH1F* fh1_Muslimap_Emean4;
    TH1F* fh1_Muslimap_Emean8;
    TH2F* fh2_Muslimap_Emean2vsEmean4;
    TH2F* fh2_Muslimap_Emean2vsEmean8;
    TH2F* fh2_Muslimap_Emean2vsE16;
    TH2F* fh2_Muslimap_Emean4vsEmean8;
    TH2F* fh2_Muslimap_Emean4vsE16;
    TH2F* fh2_Muslimap_Emean8vsE16;

    TH1I* fh1_Muslimap_DeltaDT;

  public:
    ClassDef(R3BMusliOnlineSpectra, 1)
};

#endif
