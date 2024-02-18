/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2024 Members of R3B Collaboration                          *
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
// -----            R3BCalifavsSpiderOnlineSpectra        -----
// -----    Created 26/01/24 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------

#pragma once

#include <FairTask.h>
#include <TCanvas.h>
#include <vector>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;

class R3BCalifavsSpiderOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BCalifavsSpiderOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    explicit R3BCalifavsSpiderOnlineSpectra(const TString& name, int iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BCalifavsSpiderOnlineSpectra() = default;

    /** Virtual method SetParContainers **/
    void SetParContainers() override;

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

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    void FinishEvent() override;

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    void FinishTask() override;

    /**
     * Method for setting min proton energy (in keV) for opening angle histogram
     */
    inline void SetMinProtonEnergyForOpening(double min) { fMinProtonE = min; }

    /**
     * Method for setting min phi (in deg) for Califa&Spider correlations
     */
    inline void SetMinPhi(double min) { fMinPhi_Dif = min; }

    /**
     * Method for setting the fTpat
     */
    inline void SetTpat(int tpat) { fTpat = tpat; }

    /**
     * Method to reset histograms
     */
    void Reset_Histo();

  private:
    TClonesArray* fHitItemsCalifa = nullptr;
    TClonesArray* fHitItemsSpider = nullptr;
    R3BEventHeader* fHeader = nullptr;

    int fNEvents = 0;
    int fTpat = 0;
    double fMinProtonE = 0.;
    double fMinPhi_Dif = 4.; // deg

    std::vector<TH2F*> fh2_Califa_theta_phi; // 0: all, 1: with Spider
    TH2F* fh2_Califa_coinPhi;
    TH2F* fh2_Califa_coinTheta;
    TCanvas* cCalifa_angles;

  public:
    ClassDefOverride(R3BCalifavsSpiderOnlineSpectra, 1)
};
