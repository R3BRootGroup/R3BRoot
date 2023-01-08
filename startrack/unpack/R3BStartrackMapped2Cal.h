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
// -----                R3BStartrackMapped2TCal           -----
// -----            Created July 2017 by M.Labiche        -----
// ----- Convert mapped data to time ordered data         -----
// ------------------------------------------------------------

#ifndef R3BSTARTRACKMAPPED2CAL_H
#define R3BSTARTRACKMAPPED2CAL_H

#include <map>

// ROOt headers:
#include "TGraph.h"
#include "TH2F.h"
#include <vector>

#include "FairTask.h"
#include "R3BStartrackMapped2CalPar.h"
//#include "R3BStartrackCalData.h"
#include "R3BStartrackMappedData.h"

class TClonesArray;
class TH2F;
// class R3BStartrackMapped2CalPar;
// class R3BEventHeader;

/**
 * An analysis task to order data in time for Startrack.
 * This class reads Stratrack mapped items and
 * produces time ordered items with time in [ns].
 */

class R3BStartrackMapped2Cal : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BStartrackMapped2Cal();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BStartrackMapped2Cal(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BStartrackMapped2Cal();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /**
     * Method for initialization of the parameter containers.
     * Called by the framework prior to Init() method.
     */
    virtual void SetParContainers();

    /**
     * Method for re-initialization of parameter containers
     * in case the Run ID has changed.
     */
    virtual InitStatus ReInit();

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
     * Method for setting the trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    // inline void SetTrigger(Int_t trigger)
    //{
    //    fTrigger = trigger;
    //}

    /**
     * Method for ordering data in time.
     */
    // virtual void InsertionSort(vector<long long> & v_ts, vector<long long> & v_index);
    // virtual void InsertionSort2(vector<long long> & v_ts, vector<long long> & v_block_index, vector<long long> &
    // v_hit_index);
    virtual void InsertionSort(std::vector<ULong64_t>& v_ts, std::vector<ULong64_t>& v_index);
    virtual void InsertionSort2(std::vector<long long>& v_ts,
                                std::vector<long long>& v_block_index,
                                std::vector<long long>& v_hit_index);

  private:
    // check for trigger should be done globablly (somewhere else)
    // R3BEventHeader* header;                     /**< Event header. */
    // Int_t fTrigger;                             /**< Trigger value. */
    //
    TClonesArray* fMappedItemsCA; /**< Array with mapped items - input data. as defined in
                                     r3bdata/StartrackData/R3BStartrackMappedData  */
    TClonesArray* fCalItemsCA;    /**< Array with cal items - output data.  as defined in
                                     r3bdata/StartrackData/R3BStartrackCalData */
    Int_t fNofCalItems;           /**< Number of produced time items per event. */

    R3BStartrackMapped2CalPar* fStartrackCalPar; /**< Parameter instance holding gain for energy correction */

    UInt_t fNofCalPars; /**< Number of modules in parameter file. */

    // UInt_t fNofPlanes;
    // UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */
    // UInt_t fNofEdges;      /**< Total number of edges. */
    // Double_t fClockFreq;     /**< Clock cycle in [ns]. */

    Int_t fN_first;
    ULong64_t fW3TSlast;
    Int_t fNbReject;

    // TH2F* fTsHit;
    // TGraph* grTsHit;

    // double gain[18][2][16][128];
    // double offset[18][2][16][128];

  public:
    ClassDef(R3BStartrackMapped2Cal, 1)
};

#endif
