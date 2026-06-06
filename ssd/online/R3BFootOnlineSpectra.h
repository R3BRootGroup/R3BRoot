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
// -----             R3BFootOnlineSpectra                 -----
// -----    Created 16/07/21 by J.L. Rodriguez-Sanchez    -----
// -----       Modified 05/2025 by Pablo Gonzalez Rusell  -----
// -----          Fill FOOT online histograms             -----
// ------------------------------------------------------------

#pragma once

#include <FairTask.h>

#include <Rtypes.h>
#include <TCanvas.h>
#include <TMath.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

class TClonesArray;
class R3BEventHeader;
class R3BFootMappingPar;
class R3BFootHitPar;
class TH1F;
class TH2F;

/**
 * This tasks reads FOOT data and plots online histograms
 */
class R3BFootOnlineSpectra : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BFootOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BFootOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BFootOnlineSpectra() = default;

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    InitStatus Init() override;

    /** Virtual method SetParContainers **/
    void SetParContainers() override;

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
     * Method to reset histograms
     */
    void Reset_FOOT_Histo();

    /*
     * Method to set the refresh rate of the sigmas
     */

    void SetSigmaRefreshRate(int rate) { fSigmaRefreshRate = rate; }

    void SetBinParams(double minE, double maxE, double binsE, double minZ, double maxZ)
    {
        fMinE = minE;
        fMaxE = maxE;
        fBinsE = binsE;
        fMinZ = minZ;
        fMaxZ = maxZ;
    }

    void SetMaxSize(double max) { fMaxSize = max; }

    /**
     * Method to set the number of detectors
     */
    inline void SetNbDet(int ndet) { fNbDet = ndet; }

    /**
     * Method to set the trigger
     */
    inline void SetTrigger(int trigg) { fTrigger = trigg; }

    inline void SetTpat(int tpat) { fTpat = tpat; }

  private:
    void SetParameter();

    R3BEventHeader* fEventHeader = nullptr; // // Pointer to the R3BEventHeader structure
    TClonesArray* fMappedItems = nullptr;   // Array with mapped items.
    TClonesArray* fCalItems = nullptr;      // Array with cal items.
    TClonesArray* fHitItems = nullptr;      // Array with hit items.

    R3BFootMappingPar* fMap_Par = nullptr; // Parameter container with mapping
    R3BFootHitPar* fHit_Par = nullptr;     // Parameter container with mapping

    int fTrigger = -1; // Trigger value.
    int fTpat = 0;
    int fNEvents = 0; // Event counter.
    int fNbDet = 8;   // Number of Foot detectors.
    int eventNumber = 0;
    int fSigmaRefreshRate = 5000;

    // Number of bins and limits for energy
    double fMinE = -100.;
    double fMaxE = 5000.;
    double fBinsE = 1000;

    double fMinZ = 0;
    double fMaxZ = 15;

    // Maximum size of the cluster (-1 for no maximum)
    double fMaxSize = -1;

    // Number of possible combinations of correlations
    int dim = 6;

    // Variables measured by each foot (example of G-249 experiment)
    std::vector<int> fXNdx = { 0, 2, 5, 7 };
    std::vector<int> fYNdx = { 1, 3, 4, 6 };

    // Different combinations for correlations
    std::vector<int> fCorrNdxX;
    std::vector<int> fCorrNdxY;

    // Histograms for map data
    std::vector<TH2F*> fh2_EnergyVsStrip;
    // Histograms for cal data
    std::vector<TH2F*> fh2_EnergyVsStrip_cal;
    std::vector<TH2F*> fh2_SigmaVsStrip;
    // Histograms for hit data
    std::vector<TH1F*> fh1_pos;
    std::vector<TH1F*> fh1_ene;
    std::vector<TH2F*> fh2_eta;

    std::vector<TH1F*> fh1_posMax;
    std::vector<TH1F*> fh1_eneMax;
    std::vector<TH2F*> fh2_etaMax;

    std::vector<TH2F*> fh2_foot_corr;
    std::vector<TH1F*> fh1_mult;
    std::vector<TH1F*> fh1_size;
    std::vector<TH1F*> fh1_charge;
    std::vector<TH2F*> fh2_energy_corr;
    std::vector<TH2F*> fh2_pos_charge;
    std::vector<TH2F*> fh2_energy_corr_max;

    std::vector<TH2F*> fh2_XY_max_corr;
    std::vector<TH2F*> fh2_XX_max_corr;
    std::vector<TH2F*> fh2_YY_max_corr;

  public:
    ClassDefOverride(R3BFootOnlineSpectra, 1)
};
