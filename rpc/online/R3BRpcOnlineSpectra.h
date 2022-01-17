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

<<<<<<< HEAD
=======
<<<<<<< HEAD:ssd/online/R3BAmsOnlineSpectra.h
// ------------------------------------------------------------
// -----                  R3BAmsOnlineSpectra             -----
// -----    Created 22/07/18  by J.L. Rodriguez-Sanchez   -----
// -----           Fill AMS online histograms             -----
// ------------------------------------------------------------

#ifndef R3BAmsOnlineSpectra_H
#define R3BAmsOnlineSpectra_H
=======
>>>>>>> changes to the mapped2Cal
// -----------------------------------------------------------
// -----             R3BRpcOnlineSpectra                 -----
// -----   Created 23/01/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill RPC online histograms              -----
// -----------------------------------------------------------

#ifndef R3BRpcOnlineSpectra_H
#define R3BRpcOnlineSpectra_H
<<<<<<< HEAD
=======
>>>>>>> changes to the mapped2Cal:rpc/online/R3BRpcOnlineSpectra.h
>>>>>>> changes to the mapped2Cal

#include "FairTask.h"

#include "TCanvas.h"
<<<<<<< HEAD
=======
#include "TH2F.h"
>>>>>>> changes to the mapped2Cal
#include "TMath.h"
#include <Rtypes.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
<<<<<<< HEAD
=======
<<<<<<< HEAD:ssd/online/R3BAmsOnlineSpectra.h

class TClonesArray;
class R3BEventHeader;
class R3BAmsMappingPar;
=======
>>>>>>> changes to the mapped2Cal
#include <vector>

class TClonesArray;
class R3BEventHeader;
class TH1F;
class TH2F;
<<<<<<< HEAD

/**
 * This taks reads RPC data and plots online histograms
 */
class R3BRpcOnlineSpectra : public FairTask
=======
>>>>>>> changes to the mapped2Cal:rpc/online/R3BRpcOnlineSpectra.h

/**
 * This taks reads AMS data and plots online histograms
 */
class R3BAmsOnlineSpectra : public FairTask
>>>>>>> changes to the mapped2Cal
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
<<<<<<< HEAD
    R3BRpcOnlineSpectra();
=======
    R3BAmsOnlineSpectra();
>>>>>>> changes to the mapped2Cal

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
<<<<<<< HEAD
    R3BRpcOnlineSpectra(const TString& name, Int_t iVerbose = 1);
=======
    R3BAmsOnlineSpectra(const TString& name, Int_t iVerbose = 1);
>>>>>>> changes to the mapped2Cal

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
<<<<<<< HEAD
    virtual ~R3BRpcOnlineSpectra();
=======
<<<<<<< HEAD:ssd/online/R3BAmsOnlineSpectra.h
    virtual ~R3BAmsOnlineSpectra();
=======
    virtual ~R3BRpcOnlineSpectra();
>>>>>>> changes to the mapped2Cal:rpc/online/R3BRpcOnlineSpectra.h
>>>>>>> changes to the mapped2Cal

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
<<<<<<< HEAD
    virtual InitStatus Init() override;
=======
<<<<<<< HEAD:ssd/online/R3BAmsOnlineSpectra.h
    virtual InitStatus Init();
=======
    virtual InitStatus Init() override;
>>>>>>> changes to the mapped2Cal:rpc/online/R3BRpcOnlineSpectra.h
>>>>>>> changes to the mapped2Cal

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option) override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent() override;

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
<<<<<<< HEAD
    virtual void FinishTask() override;
=======
<<<<<<< HEAD:ssd/online/R3BAmsOnlineSpectra.h
    virtual void FinishTask();

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();
=======
    virtual void FinishTask() override;
>>>>>>> changes to the mapped2Cal:rpc/online/R3BRpcOnlineSpectra.h
>>>>>>> changes to the mapped2Cal

    /**
     * Method to reset histograms
     */
<<<<<<< HEAD
=======
<<<<<<< HEAD:ssd/online/R3BAmsOnlineSpectra.h
    void Reset_AMS_Histo();

  private:
    void SetParameter();

    TClonesArray* fMappedItemsAms; /**< Array with mapped items. */
    TClonesArray* fCalItemsAms;    /**< Array with cal items. */
    TClonesArray* fHitItemsAms;    /**< Array with hit items. */

    R3BAmsMappingPar* fMap_Par; /**< Container with mapping parameters. >*/

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fNEvents;         /**< Event counter. */

    TCanvas* cMap;
    TCanvas *cCalL, *cCalR;
    TCanvas* cHit[6];
    TCanvas* cHitAngles;
    TCanvas* cHitEnergyCor;
    TCanvas* cHitThetaCor;
    TCanvas* cHitPhiCor;

    TH2F* fh_Ams_energy_allStrips[6];
    TH2F* fh_Ams_energy_allCalStrips[12];
    TH2F* fh_Ams_hit_Pos[6];
    TH2F* fh_Ams_hit_E[6];
    TH2F* fh_Ams_hit_E_theta[6];
    TH1F* fh_Ams_hit_Mul[6][2];
    TH2F* fh2_ams_theta_phi[2];
    TH2F* fh2_ams_e1_e2[2];
    TH2F* fh2_ams_theta1_theta2[4];
    TH2F* fh2_ams_phi1_phi2[2];

    // TString fAmsFile;        	      /**< Config file name. */
    Int_t fNbDet; /**< Number of AMS detectors. */

  public:
    ClassDef(R3BAmsOnlineSpectra, 1)
=======
>>>>>>> changes to the mapped2Cal
    void Reset_RPC_Histo();

    /**
     * Method to set the trigger
     */
    void SetTrigger(Int_t trigg) { fTrigger = trigg; }

  private:
    R3BEventHeader* fEventHeader; // // Pointer to the R3BEventHeader structure
    TClonesArray* fRefMappedItems;   // Array with mapped items.
    TClonesArray* fPmtMappedItems;   // Array with mapped items.
    TClonesArray* fStripMappedItems;   // Array with mapped items.


    TClonesArray* fCalItems;      // Array with cal items.
    TClonesArray* fHitItems;      // Array with hit items.

    R3BEventHeader* header; // Event header.
    Int_t fTrigger;         // Trigger value.
    Int_t fNEvents;         // Event counter.

    TH1F **stripCoarseRightHisto;
    TH1F **stripFineRightHisto;

    TH1F **stripCoarseLeftHisto;
    TH1F **stripFineLeftHisto;

    TH1F **pmtCoarseHisto;
    TH1F **pmtFineHisto;

    TH1F **refCoarseHisto;
    TH1F **refFineHisto;

    TH2F *stripCoarseLeftCorr;
    TH2F *stripCoarseRightCorr;

    TH2F *stripFineLeftCorr;
    TH2F *stripFineRightCorr;

    TCanvas *leftStripCanvasCoarse;
    TCanvas *rightStripCanvasCoarse;

    TCanvas *leftStripCanvasFine;
    TCanvas *rightStripCanvasFine;

    TCanvas *stripCoarseLeftCorrCanvas;
    TCanvas *stripCoarseRightCorrCanvas;

    TCanvas *stripFineLeftCorrCanvas;
    TCanvas *stripFineRightCorrCanvas;

  public:
    ClassDefOverride(R3BRpcOnlineSpectra, 1)
<<<<<<< HEAD
=======
>>>>>>> changes to the mapped2Cal:rpc/online/R3BRpcOnlineSpectra.h
>>>>>>> changes to the mapped2Cal
};

#endif /* R3BRpcOnlineSpectra_H */
