/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
// -----             R3BActafOnlineSpectra                -----
// -----    Created 27/02/25 by J.L. Rodriguez-Sanchez    -----
// -----          Fill Actaf online histograms            -----
// ------------------------------------------------------------

#pragma once

#include <FairTask.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <vector>

class TClonesArray;
class TH1F;
class TH2F;
class TH2Poly;
class R3BEventHeader;
class R3BActafMappingPar;
class R3BActafGeometry;

namespace r3b::util
{
    template <class T>
    inline void ClearIfNotNull(T* ptr) noexcept
    {
        if (ptr)
            ptr->Clear();
    }
} // namespace r3b::util

class R3BActafOnlineSpectra : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BActafOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BActafOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BActafOnlineSpectra() = default;

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

    void SetParContainers() override;

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    void FinishEvent() override;

    void FinishTask() override;

    /**
     * Methods to clean histograms.
     */
    virtual void Reset_Histo();

    /*
     * Methods to control de event viewer
     */

    virtual void Next_event();
    virtual void Prev_event();
    virtual void Reset_event();
    virtual void plotSingleEventCanvas();

    /**
     * Method for setting the trigger value.
     */
    void SetTrigger(int trigger) { fTrigger = trigger; }
    void SetTpat(int tpat1, int tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }

    // Setters for the histogram setting
    void setHistPars(int nBins, int minBin, int maxBin, int& nBinsRef, int& minRef, int& maxRef)
    {
        nBinsRef = nBins;
        minRef = minBin;
        maxRef = maxBin;
    }

    void SetGeoVersion(int geo) { fGeoversion = geo; }

    void SetEcalHistPars(int n, int minB, int maxB) { setHistPars(n, minB, maxB, nBinsEcal, nEcalMin, nEcalMax); }

    void SetTraceHistPars(int minB, int maxB)
    {
        setHistPars(std::abs(maxB - minB) / 4, minB, maxB, nBinsTrace, nTraceMin, nTraceMax);
    }

    void SetZcalHistPars(int n, int minB, int maxB) { setHistPars(n, minB, maxB, nBinsZcal, nZcalMin, nZcalMax); }
    void SetTLeadingHistPars(int n, int minB, int maxB)
    {
        setHistPars(n, minB, maxB, nBinstLeading, ntLeadingMin, ntLeadingMax);
    }
    void SetMaxAmpHistPars(int n, int minB, int maxB)
    {
        setHistPars(n, minB, maxB, nBinsMaxAmp, nMaxAmpMin, nMaxAmpMax);
    }
    void SetTSyncHistPars(int n, int minB, int maxB) { setHistPars(n, minB, maxB, nBinsTSync, nTSyncMin, nTSyncMax); }

    void SetNBinsSample(int n) { nBinsSample = n; }

    void SetMaxTimeForRate(int max) { max_second_for_rate = max; }

    void SetUpdateRate(int num) { updateRate = num; }

  private:
    void SetParameter();

    int fGeoversion = 2025;
    R3BActafGeometry* fActafGeo;

    TClonesArray* fMappedItems = nullptr;
    TClonesArray* fCalItems = nullptr;
    TClonesArray* fHitItems = nullptr;
    TClonesArray* fWrItems = nullptr;

    R3BEventHeader* header = nullptr;       /** Event header */
    R3BActafMappingPar* fMap_Par = nullptr; /** Parameter container */
    int fTrigger = -1;                      /** Trigger value */
    int fTpat1 = 0, fTpat2 = 0;
    unsigned long fNEvents = 0;
    static constexpr int fChn = 16;
    static constexpr int fFadcs = 8;
    static constexpr int fPads = fChn * fFadcs;
    static constexpr int fRings = 8;
    bool fDisplaytraces = true;

    std::vector<TH2F*> fh2_RawTraces;
    std::vector<TH2F*> fh2_CorrectedTraces;
    std::vector<TH1F*> fh1_RawE;
    std::vector<TH1F*> fh1_Baseline;

    // Map histograms
    TH2F* fh2_ERaw_map = nullptr;
    TH2F* fh2_Baseline_map = nullptr;
    TH2F* fh2_MaxPos_map = nullptr;
    TH2F* fh2_Risetime_map = nullptr;
    TH2F* fh2_ModVsCh_map = nullptr;
    TH1F* fh1_sigmaInit = nullptr;
    TH1F* fh1_sigmaFilt = nullptr;
    TH2F* fh2_sigmaInitVsPad = nullptr;
    TH2F* fh2_sigmaFiltVsPad = nullptr;
    TH2F* fh2_meanInitVsPad = nullptr;
    TH2F* fh2_meanFiltVsPad = nullptr;

    // Cal histograms
    TH2F* fh2_Ecal_cal = nullptr;
    TH2F* fh2_zPos_cal = nullptr;
    TH2F* fh2_tLeading_cal = nullptr;
    TH2F* fh2_maxAmp_cal = nullptr;
    TH2F* fh2_tSync_cal = nullptr;
    std::vector<TH2F*> fh2_FilteredTraces;

    int nBinsEcal = 100;
    int nEcalMin = 0;
    int nEcalMax = 500000;

    int nBinsTrace = 500;
    int nTraceMin = -100;
    int nTraceMax = 2000;

    int nBinsZcal = 100;
    int nZcalMin = 0;
    int nZcalMax = 1000;

    int nBinstLeading = 100;
    int ntLeadingMin = 0;
    int ntLeadingMax = 5000;

    int nBinsMaxAmp = 100;
    int nMaxAmpMin = 0;
    int nMaxAmpMax = 1500;

    int nBinsTSync = 100;
    int nTSyncMin = 0;
    int nTSyncMax = 5000;

    int nBinsSample = 2692;

    size_t nbWrs = 9;

    uint64_t pre_timetag = 0;
    uint64_t first_timestamp = 0;
    uint64_t overall_rate = 0;
    uint64_t fsec_rate = 0; // upstream section rates
    uint64_t ssec_rate = 0; // downstream section rates
    int last_second = 0;
    int max_second_for_rate = 600;
    int max_rate = 1000;

    int updateRate = 1;

    std::vector<uint64_t> pre_timestamp{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    // Hit histograms
    std::vector<TH1F*> fh1_RingCounts;
    std::vector<TH2Poly*> fh2_XYPos;
    std::vector<TH2Poly*> fh2_XYPos_Evts;
    std::vector<TH1F*> fh1_PhiCounts;
    std::vector<TH1F*> fh1_Sync;
    std::vector<TH1F*> fh1_WrSync;
    std::vector<TGraph*> fgraph_rates;

    TH1F* fh1_CountsPerSide = nullptr;
    TH1F* fh1_DetMask = nullptr;
    TH2F* fh2_Phi1VsPhi2 = nullptr;
    TH2F* fh2_timetag_signal = nullptr;
    TCanvas* cRates = nullptr;

    // Params for the event viewer
    int eventViewerNb = 0;
    int firstBufferEvent = 0;
    static constexpr int maxEventViewerBatch = 100;
    int selectEvent = 0;
    std::array<std::array<double, fPads>, maxEventViewerBatch> eventCountsX;
    std::array<std::array<double, fPads>, maxEventViewerBatch> eventCountsY;
    std::array<std::array<double, fPads>, maxEventViewerBatch> eventCountsE;

  public:
    ClassDefOverride(R3BActafOnlineSpectra, 1);
};
