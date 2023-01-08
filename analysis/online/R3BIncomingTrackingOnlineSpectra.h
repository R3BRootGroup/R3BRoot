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
// -----          R3BIncomingTrackingOnlineSpectra        -----
// -----    Created 25/02/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill tracking online histograms          -----
// ------------------------------------------------------------

#ifndef R3BIncomingTrackingOnlineSpectra_H
#define R3BIncomingTrackingOnlineSpectra_H 1

#include "FairTask.h"
#include "TCanvas.h"
#include "TCutG.h"
#include "TH1.h"
#include "TH2F.h"

class TClonesArray;
class R3BEventHeader;
class R3BTGeoPar;

class R3BIncomingTrackingOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BIncomingTrackingOnlineSpectra();

    R3BIncomingTrackingOnlineSpectra(const TString& name, float, float, float, float);

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BIncomingTrackingOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BIncomingTrackingOnlineSpectra();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    virtual void SetParContainers();

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
    TClonesArray* fMwpc0HitDataCA; /**< Array with Mwpc0 Hit-input data. >*/
    TClonesArray* fMwpc1HitDataCA; /**< Array with Mwpc1 Hit-input data. >*/
    TClonesArray* fFrsHitDataCA;   /**< Array with FRS Hit-input data. >*/

    // Parameters
    R3BTGeoPar* fMw0GeoPar;
    R3BTGeoPar* fTargetGeoPar;
    R3BTGeoPar* fMw1GeoPar;

    R3BEventHeader* header;
    Int_t fNEvents;
    Float_t fPosTarget;
    Float_t fWidthTarget;
    Float_t fDist_acelerator_glad;
    TString fNameCut;

    // Canvas
    TCanvas* cTrackingXZ;
    TCanvas* cTrackingYZ;
    TCanvas* cBeamProfileTarget;
    TCanvas* cZvsBeta;

    // Histograms for Hit data
    TH2F* fh2_tracking_planeXZ;
    TH2F* fh2_tracking_planeYZ;
    TH2F* fh2_target_PosXY;
    TH2F* fh2_ZvsBeta;
    TH2F* fh2_angvsposx;
    TH2F* fh2_angvsposy;
    TCutG* fCutIncoming;

  public:
    ClassDef(R3BIncomingTrackingOnlineSpectra, 1)
};

#endif /* R3BIncomingTrackingOnlineSpectra_H */
