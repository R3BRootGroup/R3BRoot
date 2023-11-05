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
// -----                  R3BLosOnlineSpectra             -----
// -----            Created 13-04-2016 by M.Heil          -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------

#ifndef R3BLosOnlineSpectra_H
#define R3BLosOnlineSpectra_H 1

#include "FairTask.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "TClonesArray.h"
#include "TFolder.h"
#include "TMath.h"
#include <cstdlib>

class TClonesArray;
class R3BTDCCyclicCorrector;
class TH1F;
class TH2F;
class R3BEventHeader;

/**
 * This taks reads LOS data items and plots histograms
 * for online checks.
 */
class R3BLosOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BLosOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BLosOnlineSpectra(const char* name, int iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BLosOnlineSpectra();

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
     * Methods for setting position offset and effective velocity of light
     */
    inline void SetLosXYMCFD(double offsetX1,
                             double offsetY1,
                             double veffX1,
                             double veffY1,
                             double offsetX2,
                             double offsetY2,
                             double veffX2,
                             double veffY2)
    {
        flosOffsetXV[0] = offsetX1;
        flosOffsetYV[0] = offsetY1;
        flosVeffXV[0] = veffX1;
        flosVeffYV[0] = veffY1;
        flosOffsetXV[1] = offsetX2;
        flosOffsetYV[1] = offsetY2;
        flosVeffXV[1] = veffX2;
        flosVeffYV[1] = veffY2;
    }

    inline void SetLosXYTAMEX(double offsetXT1,
                              double offsetYT1,
                              double veffXT1,
                              double veffYT1,
                              double offsetXT2,
                              double offsetYT2,
                              double veffXT2,
                              double veffYT2)
    {
        flosOffsetXT[0] = offsetXT1;
        flosOffsetYT[0] = offsetYT1;
        flosVeffXT[0] = veffXT1;
        flosVeffYT[0] = veffYT1;
        flosOffsetXT[1] = offsetXT2;
        flosOffsetYT[1] = offsetYT2;
        flosVeffXT[1] = veffXT2;
        flosVeffYT[1] = veffYT2;
    }

    inline void SetLosXYToT(double offsetXQ1,
                            double offsetYQ1,
                            double veffXQ1,
                            double veffYQ1,
                            double offsetXQ2,
                            double offsetYQ2,
                            double veffXQ2,
                            double veffYQ2)
    {
        flosOffsetXQ[0] = offsetXQ1;
        flosOffsetYQ[0] = offsetYQ1;
        flosVeffXQ[0] = veffXQ1;
        flosVeffYQ[0] = veffYQ1;
        flosOffsetXQ[1] = offsetXQ2;
        flosOffsetYQ[1] = offsetYQ2;
        flosVeffXQ[1] = veffXQ2;
        flosVeffYQ[1] = veffYQ2;
    }
    /* Method for pile-up */
    inline void SetEpileup(double Epileup) { fEpileup = Epileup; }

    /* Method for setting number of LOS detectors */
    inline void SetNofLosModules(int nDets)
    {
        fNofLosDetectors = nDets;

        time_prev.resize(nDets);

        fh_los_channels.resize(nDets);
        fh_los_tres_MCFD.resize(nDets);
        fh_los_tres_TAMEX.resize(nDets);
        fh_los_tot_mean.resize(nDets);
        fh_los_tot.resize(nDets);
        fh_los_multihit.resize(nDets);
        fh_los_multihitVFTX.resize(nDets);
        fh_los_multihitLEAD.resize(nDets);
        fh_los_multihitTRAI.resize(nDets);
        fh_los_pos_MCFD.resize(nDets);
        fh_los_pos_TAMEX.resize(nDets);
        fh_los_pos_ToT.resize(nDets);
        fh_los_ihit_ToT.resize(nDets);
        fh_los_dt_hits_ToT.resize(nDets);
        fh_losToT_vs_Events.resize(nDets);
        fh_losTAMEX_vs_Events.resize(nDets);
        fh_losMCFD_vs_Events.resize(nDets);
        fh_los_vftx_tamex.resize(nDets);
    }

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(int trigger) { fTrigger = trigger; }
    inline void SetTpat(int tpat) { fTpat = tpat; }

    void Reset_LOS_Histo();

  private:
    std::vector<TClonesArray*> fMappedItems;
    std::vector<TClonesArray*> fCalItems;

    R3BTDCCyclicCorrector* fCyclicCorrector;
    TClonesArray* fMappedItemsTwim; /**< Array with mapped items. */
    TClonesArray* fCalItemsTwim;    /**< Array with cal items. */

    enum DetectorInstances
    {
        DET_LOS,
        DET_MAX
    };

    const char* fDetectorNames[DET_MAX + 1] = { "Los", NULL };

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    int fTrigger;           /**< Trigger value. */
    int fTpat;
    int fSamp;
    double fClockFreq; /**< Clock cycle in [ns]. */
    int nLosEvents = 0;
    //   TClonesArray *fbmonMappedItems;
    int fNofLosDetectors; /**< Number of LOS detectors. */

    double flosVeffXV[2];
    double flosVeffYV[2];
    double flosOffsetXV[2];
    double flosOffsetYV[2];
    double flosVeffXT[2];
    double flosVeffYT[2];
    double flosOffsetXT[2];
    double flosOffsetYT[2];
    double flosVeffXQ[2];
    double flosVeffYQ[2];
    double flosOffsetXQ[2];
    double flosOffsetYQ[2];
    int foptcond;

    unsigned long long time_V_mem = 0, time_start = 0, time = 0, time_mem = 0;
    std::vector<unsigned long long> time_prev;
    unsigned long long time_prev_read = 0, time_to_read = 0;
    unsigned long ic_mem = 0, see_mem = 0, tofdor_mem = 0;
    unsigned long ic_start = 0, see_start = 0, tofdor_start = 0;
    unsigned long long time_spill_start = 0, time_spill_end = 0;
    unsigned long long time_previous_event = 0;

    double time_clear = -1.;
    double tdiff = 0.;
    double fNorm = 1.;
    double fEpileup;

    unsigned long fNEvents = 0, fNEvents_start = 0; /**< Event counter. */

    TH1F* fhTpat;
    TH1F* fh_spill_length;
    TH1F* fhTrigger;

    std::vector<TH1F*> fh_los_channels;
    std::vector<TH1F*> fh_los_tres_MCFD;
    std::vector<TH1F*> fh_los_tres_TAMEX;
    std::vector<TH1F*> fh_los_tot_mean;
    std::vector<TH2F*> fh_los_tot;
    std::vector<TH1F*> fh_los_multihit;
    std::vector<TH2F*> fh_los_multihitVFTX;
    std::vector<TH2F*> fh_los_multihitLEAD;
    std::vector<TH2F*> fh_los_multihitTRAI;
    std::vector<TH2F*> fh_los_pos_MCFD;
    std::vector<TH2F*> fh_los_pos_TAMEX;
    std::vector<TH2F*> fh_los_pos_ToT;
    std::vector<TH2F*> fh_los_ihit_ToT;
    std::vector<TH2F*> fh_los_dt_hits_ToT;
    std::vector<TH2F*> fh_losToT_vs_Events;
    std::vector<TH2F*> fh_losTAMEX_vs_Events;
    std::vector<TH2F*> fh_losMCFD_vs_Events;
    std::vector<TH1F*> fh_los_vftx_tamex;

  public:
    ClassDef(R3BLosOnlineSpectra, 2)
};

#endif
