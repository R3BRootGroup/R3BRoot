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
// -----            R3BCalifavsTofDOnlineSpectra          -----
// -----    Created 21/05/22 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------

#ifndef R3BCalifavsTofDOnlineSpectra_H
#define R3BCalifavsTofDOnlineSpectra_H 1

#include "FairTask.h"
#include "TCanvas.h"

class TClonesArray;
class TH2F;
class R3BEventHeader;

class R3BCalifavsTofDOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BCalifavsTofDOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BCalifavsTofDOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BCalifavsTofDOnlineSpectra();

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /** Virtual method ReInit **/
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
     * Method for setting min proton energy (in keV) for opening angle histogram
     */
    inline void SetMinProtonEnergyForOpening(Float_t min) { fMinProtonE = min; }

    /**
     * Method to reset histograms
     */
    void Reset_Histo();

    /**
     * Method for setting the fTpat
     */
    void SetTpat(Int_t tpat) { fTpat = tpat; }

    /**
     * Method for setting the charge of the nuclear residue
     */
    void SetZCharge(Float_t z) { fZselection = z; }

  private:
    TClonesArray* fHitItemsCalifa;
    TClonesArray* fHitItemsTofd;

    R3BEventHeader* header;
    Int_t fNEvents;
    Int_t fTpat;
    Float_t fZselection;
    Float_t fMinProtonE; /* Min proton energy (in keV) to calculate the opening angle */

    TH2F* fh2_Califa_theta_phi[2]; // 0: all, 1: with TofD
    TH2F* fh2_Califa_coinPhi;
    TH2F* fh2_Califa_coinTheta;
    TCanvas* cCalifa_angles;

  public:
    ClassDef(R3BCalifavsTofDOnlineSpectra, 1)
};

#endif /* R3BCalifavsTofDOnlineSpectra_H */
