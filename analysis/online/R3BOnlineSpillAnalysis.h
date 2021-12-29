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

#ifndef R3BONLINESPILLANALYSIS
#define R3BONLINESPILLANALYSIS

#include "FairTask.h"
#include "R3BGlobalAnalysis.h"
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "TClonesArray.h"
#include "TMath.h"
#include <cstdlib>

class TClonesArray;
class TF1;
class TH1F;
class TH2F;
class R3BEventHeader;

/**
 * This taks reads all detector data items and plots histograms
 * for online checks.
 */
class R3BOnlineSpillAnalysis : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BOnlineSpillAnalysis();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BOnlineSpillAnalysis(const char* name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BOnlineSpillAnalysis();

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
    inline void SetLosXYMCFD(Double_t offsetX1,
                             Double_t offsetY1,
                             Double_t veffX1,
                             Double_t veffY1,
                             Double_t offsetX2,
                             Double_t offsetY2,
                             Double_t veffX2,
                             Double_t veffY2)
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

    inline void SetLosXYTAMEX(Double_t offsetXT1,
                              Double_t offsetYT1,
                              Double_t veffXT1,
                              Double_t veffYT1,
                              Double_t offsetXT2,
                              Double_t offsetYT2,
                              Double_t veffXT2,
                              Double_t veffYT2)
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

    inline void SetLosXYToT(Double_t offsetXQ1,
                            Double_t offsetYQ1,
                            Double_t veffXQ1,
                            Double_t veffYQ1,
                            Double_t offsetXQ2,
                            Double_t offsetYQ2,
                            Double_t veffXQ2,
                            Double_t veffYQ2)
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
    /* Method for setting number of LOS detectors */
    inline void SetNofLosModules(Int_t nDets) { fNofLosDetectors = nDets; }
    /**
     * Methods for setting condition on filling xs from LOS
     * 0 = no condition, 1 = condition from fiber
     */
    inline void SetLosPosition(Int_t optcond) { foptcond = optcond; }
    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    inline void SetTpat(Int_t tpat) { fTpat = tpat; }

    /**
     * Methods for setting reset and readout times for Bmon
     */
    inline void SetBmon(Int_t time_range, Double_t time_step, Double_t SpillLength, Double_t DAQ_dead_time)
    {
        reset_time = time_range;
        read_time = time_step;
        fDAQ_dead_time = DAQ_dead_time; // dead time in micro seconds
        fSpillLength = SpillLength;
    }
    /**
     * Methods for setting number of planes and paddles
     */
    inline void SetNofModules(Int_t planes, Int_t ppp)
    {
        fNofPlanes = planes;
        fPaddlesPerPlane = ppp;
    }
    /* Put Custom functions here*/
    /** Function for exponential distribution*/
    double ran_expo(double lambda)
    {
        double u;

        u = rand() / (RAND_MAX + 1.0);

        return -log(1 - u) / lambda;
    }

    /** Sorting function, sorts an array and return the value of a. If a = 1 -> entries with -1.e9, these need to be
     * sorted out later.*/
    int sort_array(Int_t* entries, Double_t* array, Int_t* array2, Double_t* array3)
    {
        // array = new double[entries];
        bool sorted = false;
        // Bool_t Noise = false;
        Int_t a = 0;
        do
        {
            while (!sorted)
            { // Bubble-Sort
                sorted = true;
                for (int sorting = 0; sorting < *entries - 1; sorting++)
                {
                    if (array[sorting] == -1.e9)
                    {
                        a = 1;
                        // std::cout<<"a = "<<a<<" array[sorting]: "<<array[sorting]<<std::endl;
                    }
                    if (array[sorting] > array[sorting + 1])
                    {
                        Double_t temp = array[sorting];
                        array[sorting] = array[sorting + 1];
                        array[sorting + 1] = temp;
                        sorted = false;
                        /**Sort appending arrays */
                        Int_t temp2 = array2[sorting];
                        Double_t temp3 = array3[sorting];
                        array2[sorting] = array2[sorting + 1];
                        array3[sorting] = array3[sorting + 1];
                        array2[sorting + 1] = temp2;
                        array3[sorting + 1] = temp3;
                    }
                }
            }

        } while (!sorted);
        return a;
    }
    /**Function to merge multihits and give out mean HitTime */
    void merge_hits(Int_t* counter, Int_t maxEntries, Int_t* point, Double_t* array_read, Double_t* array_write)
    {
        Int_t merge_counter = 0;
        for (int i = *counter; i < maxEntries - 1; i++)
        {
            if (array_read[i + 1] - *(array_read + *counter) > 35.)
            {
                continue;
            }
            // std::cout<<"arr["<<*counter<<"]: "<<array_read[*counter]<<" arr["<<i+1<<"]:
            // "<<array_read[i+1]<<std::endl;
            merge_counter++;
        }
        if (merge_counter == 0)
            *(array_write + *point) = *(array_read + *counter);
        else
        {
            for (int j = 0; j < merge_counter; j++)
            {
                *(array_write + *point) += *(array_read + *counter + j) / merge_counter;
            }
        }
        *point += 1;
        *counter += merge_counter;
    }

    Int_t Fiber1_mapMatrix[16][16] = { // Fiber Mapping on MAPMT
                                       { 1, 17, 33, 49, 65, 81, 97, 113, 129, 145, 161, 177, 193, 209, 225, 241 },
                                       { 5, 21, 37, 53, 69, 85, 101, 117, 133, 149, 165, 181, 197, 213, 229, 245 },
                                       { 9, 25, 41, 57, 73, 89, 105, 121, 137, 153, 169, 185, 201, 217, 233, 249 },
                                       { 13, 29, 45, 61, 77, 93, 109, 125, 141, 157, 173, 189, 205, 221, 237, 253 },
                                       { 2, 18, 34, 50, 66, 82, 98, 114, 130, 146, 162, 178, 194, 210, 226, 242 },
                                       { 6, 22, 38, 54, 70, 86, 102, 118, 134, 150, 166, 182, 198, 214, 230, 246 },
                                       { 10, 26, 42, 58, 74, 90, 106, 122, 138, 154, 170, 186, 202, 218, 234, 250 },
                                       { 14, 30, 46, 62, 78, 94, 110, 126, 142, 158, 174, 190, 206, 222, 238, 254 },
                                       { 3, 19, 35, 51, 67, 83, 99, 115, 131, 147, 163, 179, 195, 211, 227, 243 },
                                       { 7, 23, 39, 55, 71, 87, 103, 119, 135, 151, 167, 183, 199, 215, 231, 247 },
                                       { 11, 27, 43, 59, 75, 91, 107, 123, 139, 155, 171, 187, 203, 219, 235, 251 },
                                       { 15, 31, 47, 63, 79, 95, 111, 127, 143, 159, 175, 191, 207, 223, 239, 255 },
                                       { 4, 20, 36, 52, 68, 84, 100, 116, 132, 148, 164, 180, 196, 212, 228, 244 },
                                       { 8, 24, 40, 56, 72, 88, 104, 120, 136, 152, 168, 184, 200, 216, 232, 248 },
                                       { 12, 28, 44, 60, 76, 92, 108, 124, 140, 156, 172, 188, 204, 220, 236, 252 },
                                       { 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256 }
    };

    Int_t FCTctr = 0;
    Int_t ACT_nctr = 0;
    Int_t ACT_nnctr = 0;
    Int_t CTctr = 0;
    bool check_anode_CT(int fibnum1, int fibnum2)
    {

        Int_t a = -1;
        Int_t b = -1;
        bool broken = false;
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                if (Fiber1_mapMatrix[i][j] == fibnum1)
                {
                    a = i;
                    b = j;
                    broken = true;
                    break;
                }
            }
            if (broken)
                break;
        }
        std::cout << "AnCT" << std::endl;
        std::cout << "Fib1: " << Fiber1_mapMatrix[a][b] << " Fib2: " << fibnum2 << std::endl;

        // check neighbouring anode
        if (Fiber1_mapMatrix[a + 1][b] == fibnum2 && a + 1 < 16)
        {
            return true;
            ACT_nctr++;
        }
        else if (Fiber1_mapMatrix[a - 1][b] == fibnum2 && a != 0)
        {
            return true;
            ACT_nctr++;
        }
        else if (Fiber1_mapMatrix[a][b + 1] == fibnum2 && b + 1 < 16)
        {
            return true;
            ACT_nctr++;
        }
        else if (Fiber1_mapMatrix[a][b - 1] == fibnum2 && b != 0)
        {
            return true;
            ACT_nctr++;
        }
        // check next neighbour
        else if (Fiber1_mapMatrix[a + 1][b + 1] == fibnum2 && a + 1 < 16 && b + 1 < 16)
        {
            return true;
            ACT_nnctr++;
        }
        else if (Fiber1_mapMatrix[a - 1][b + 1] == fibnum2 && a != 0 && b + 1 < 16)
        {
            return true;
            ACT_nnctr++;
        }
        else if (Fiber1_mapMatrix[a - 1][b - 1] == fibnum2 && a != 0 && b != 0)
        {
            return true;
            ACT_nnctr++;
        }
        else if (Fiber1_mapMatrix[a + 1][b - 1] == fibnum2 && a + 1 < 16 && b != 0)
        {
            return true;
            ACT_nnctr++;
        }

        else
            return false;
    }

    bool check_fiber_CT(int fibnum1, int fibnum2)
    {
        std::cout << "FiCT" << std::endl;
        std::cout << "Fib1: " << fibnum1 << " Fib2: " << fibnum2 << std::endl;
        if (fibnum1 - fibnum2 == 1 || fibnum2 - fibnum1 == 1)
            return true;
        else
            return false;
    }

    Double_t TotZeitEichung(double SpillLength, double pps)
    {
        /* Calibration parameters from known poisson distributions. Used 10,50,100,500 kHz laserpulse. */
        Double_t DT = 18.27 * TMath::Exp(0.00319 * pps / SpillLength / 1000.) + 30.47; //µs
        return DT;
    }

    void Reset_ROLU_Histo();
    void Reset_LOS_Histo();
    void Reset_SCI8_Histo();
    void Reset_BMON_Histo();
    void Reset_SPILLMON_Histo();
    void Reset_TOFD_Histo();
    void Reset_FIBERS_Histo();

  private:
    std::vector<TClonesArray*> fMappedItems;
    std::vector<TClonesArray*> fCalItems;
    std::vector<TClonesArray*> fHitItems;

    enum DetectorInstances
    {
        DET_AMS,
        DET_CALIFA,
        DET_BMON,
        DET_FI_FIRST,
        DET_FI1A = DET_FI_FIRST,
        DET_FI1B,
        DET_FI2A,
        DET_FI2B,
        DET_FI3A,
        DET_FI3B,
        DET_FI4,
        DET_FI5,
        DET_FI6,
        DET_FI7,
        DET_FI8,
        DET_FI9,
        DET_FI10,
        DET_FI11,
        DET_FI12,
        DET_FI13,
        DET_FI_LAST = DET_FI13,
        DET_L3T,
        DET_LOS,
        DET_MUSIC,
        DET_NEULAND,
        DET_PSPX,
        DET_PTOF,
        DET_ROLU,
        DET_SCI8,
        DET_STRAW,
        DET_TOFD,
        DET_SAMPLER,
        DET_MAX
    };

#define NOF_FIB_DET (DET_FI_LAST - DET_FI_FIRST + 1)

    const char* fDetectorNames[DET_MAX + 1] = { "Ams",   "Califa",  "BeamMonitor", "Fi1a", "Fi1b", "Fi2a", "Fi2b",
                                                "Fi3a",  "Fi3b",    "Fi4",         "Fi5",  "Fi6",  "Fi7",  "Fi8",
                                                "Fi9",   "Fi10",    "Fi11",        "Fi12", "Fi13", "L3t",  "Los",
                                                "Music", "Neuland", "Pspx",        "Ptof", "Rolu", "Sci8", "Straw",
                                                "Tofd",  "Sampler", NULL };

    // If FiberI is present or not:
    Int_t ifibdet;
    // Number of fibers per detector
    Double_t n_fiber[NOF_FIB_DET] = { 256.,  256.,  256., 256,  512., 512.,  2048.,
                                      2048., 1024., 512., 512., 512., 1024., 1024. };
    Double_t tof_fiber[NOF_FIB_DET] = { 2580., 2580., 256., 256,  512., 512.,  2048.,
                                        2048., 1024., 512., 512., 512., 1024., 1024. };

    // check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header. */
    Int_t fTrigger;         /**< Trigger value. */
    Int_t fTpat;
    Int_t fSamp;
    Double_t fClockFreq; /**< Clock cycle in [ns]. */
    UInt_t fNofPlanes;
    UInt_t fPaddlesPerPlane; /**< Number of paddles per plane. */

    //   TClonesArray *fbmonMappedItems;

    Int_t fNofLosDetectors; /**< Number of LOS detectors. */

#define NOF_LOS_DET 2

    Double_t flosVeffXV[2];
    Double_t flosVeffYV[2];
    Double_t flosOffsetXV[2];
    Double_t flosOffsetYV[2];
    Double_t flosVeffXT[2];
    Double_t flosVeffYT[2];
    Double_t flosOffsetXT[2];
    Double_t flosOffsetYT[2];
    Double_t flosVeffXQ[2];
    Double_t flosVeffYQ[2];
    Double_t flosOffsetXQ[2];
    Double_t flosOffsetYQ[2];
    Int_t foptcond;

    unsigned long long time_V_mem = 0, time_start = 0, time_mem = 0, time = 0, time_begin = 0;
    unsigned long long time_prev[NOF_LOS_DET] = { 0 };

    unsigned long long time_prev_read = 0, time_to_read = 0;
    unsigned long ic_mem = 0, see_mem = 0, tofdor_mem = 0;
    unsigned long ic_start = 0, see_start = 0, tofdor_start = 0;
    unsigned long long time_spill_start = 0, time_spill_end = 0;
    unsigned long long time_previous_event = 0;
    long samplerPrev = 0, samplerCurr = 0, samplerSpill = 0, samplerBegin = 0;
    UInt_t spillCounter = 0;

    Bool_t spill_on = false;
    Bool_t spill_off = false;
    Bool_t spill_on_sampler = false;
    Double_t time_clear = -1.;
    Double_t tdiff = 0.;
    Double_t fNorm = 1.;
    Int_t iclear_count = 1;
    UInt_t reset_time;         // time after which bmon spectra are reseted
    Double_t read_time;        // step in which scalers are read, in sec
    Double_t calib_SEE = 1.;   // SEETRAM calibration factor
    Double_t see_offset = 7.1; // SEETRAM offset in kHz
    Double_t fSpillLength;
    Double_t fDAQ_dead_time;
    unsigned long fNEvents = 0, fNEvents_start = 0; /**< Event counter. */

    // spill monitor
    Double_t fLastWindowTime[NOF_FIB_DET];
    Double_t LastWindowTime = 0.;
    Double_t tLastInWindow = 0.;
    Double_t time_global = 0.;
    Int_t FFT_Counter;
    Int_t xHits = 20;
    Double_t Schwelle = 90.;
    Double_t xmax;
    Double_t TIME_Fib_length = 0.;
    Double_t TIME_previous_FL = 0.;
    Double_t dtime_min = 80.;
    Double_t dtime_max = 95.;
    Int_t nHits_prev = 0;
    Double_t time_tmp = 0.;
    Int_t iFib_tmp = -1;
    Int_t nHit_ctr = 0;
    Int_t count_dead = 0;
    Double_t tLastPrev = 0.;
    Double_t spill_times = 0.;
    Double_t spill_times_first = 0.;
    Int_t spill_ctr = 0;
    Int_t spill_ctr_LOS = 0;
    Int_t spill_ctr_FIB = 0;
    Double_t dt_LOS = 0.;
    Double_t dt_LOS_first = 0.;
    Double_t dt_FIB_first = 0.;
    Double_t FIB_time_spill = 0.;
    Int_t hit_counter = 0;
    Int_t SPILLS = 0;
    int a_ctr = 0;
    Int_t packagectr = 0;
    Int_t pHits = 0;
    Double_t F_duty = 0.;
    Double_t F_duty_fib = 0.;
    Double_t Hans_mean = 0.;
    Bool_t MissedSpillEnd = false;
    Bool_t spill_off_calc = false;
    Int_t hans_ctr = 0;
    Double_t Hans_check = 0.;
    Int_t short_ctr = 0;
    Double_t dt_prev = 0.;
    Double_t dt_dead_mean = 0.;
    Int_t dead_ctr = 0;
    Double_t deadTime = fDAQ_dead_time;
    Double_t tt_prev = 0.;
    Bool_t skip_spill = false;
    Double_t testcounter = 0.;
    Int_t numHits1a = 0;
    Int_t numHits1b = 0;
    Bool_t first_in_spill = true;
    Double_t CTAN = 0.;
    Double_t CTANN = 0.;
    Double_t CTF = 0.;
    Int_t PPSF = 0;
    Bool_t spillon_fib = true;
    Double_t time_spill_on_fib = 0.;

    TH1F* fh_spill_times_fib_fine;
    TH1F* fh_spill_times_fib_Coarse;
    TH1F* fh_spill_times_fib_daq_first;
    TH1F* fh_spill_times_FIB_pois;
    TH1F* fh_spill_times_FIB_pois_Highstat;
    TH1F* fh_spill_hans_fib;
    TH1F* fh_spill_Q_E_real_fib;
    TH1F* fh_spill_Q_E_pois_fib;
    TH1F* fh_spill_frac_fib;
    TH1F* fh_DutyFactor_fib;
    TH1F* fh_DutyFactor_MtA_fib;
    TH1F* fh_DutyFactor_fib_pois;
    TH1F* fh_DutyFactor_MaxToAvg_fib;
    TH1F* fh_DutyFactor_MaxRun_fib;
    TH1F* fh_DutyFactor_AvgRun_fib;
    TH1F* fh_rate_sum_fib;
    TH1F* fh_rate_sum_SAMPFIB;
    TH1F* fh_rate_sum_DTmean_fib;
    TH1F* fh_rate_sum_DTmean_SAMPFIB;
    TH1F* fh_spillQuality_ppt_fib;
    TH1F* fh_spillQuality_ppt_fib_stat;

    TH1F* fh_rate_fib;
    TH1F* fh_rate_fib_adj;
    TH1F* fh_rate_fib_set;
    TH1F* fh_particle_fib;
    TH1F* fh_particle_fib_pois;

    TH1F* fh_FIB_count_t;
    TH1F* fh_FIB_TrgW_dt;
    TH1F* fh_LOS_TrgW_dt;
    TH1F* fh_SAMP_TrgW_dt;
    TH1F* fh_FIB_TrgW_dt_coarse;
    TH1F* fh_LOS_TrgW_dt_coarse;
    TH1F* fh_SAMP_TrgW_dt_coarse;
    TH1F* fh_FIB_Freq;

    TH1F* fh_FIB_tDiff_pois;

    TH1F* fh_spill_hits_vs_time;
    TH1F* fh_spill_hits_vs_time_fine;
    TH1F* fh_spill_times_fib;
    TH1F* fh_spill_times_LOS;
    TH1F* fh_spill_times_LOS_Fine;
    TH1F* fh_spill_times_LOSFIB_Fine;
    TH1F* fh_spill_times_LOSFIB_Fine_adj;
    TH1F* fh_spill_times_LOS_Fine_adj;
    TH1F* fh_spill_times_LOS_Coarse;
    TH1F* fh_spill_times_LOS_pois;
    TF1* Pois;
    TH1F* fh_r_pois_fib;
    TH1F* fh_r_pois_LOS;
    TH1F* fh_LOS_tDiff;
    TH1F* fh_SAMPFIB_tDiff;
    TH1F* fh_LOS_freq;
    TH1F* fh_LOS_tDiff_pois;
    TH1F* fh_DutyFactor_LOS;
    TH1F* fh_DutyFactor_SAMPFIB;
    TH1F* fh_DutyFactor_SAMPFIB_clean;
    TH1F* fh_DutyFactor_SAMP_clean;
    TH1F* fh_DutyFactor_MtA;
    TH1F* fh_DutyFactor_LOS_pois;
    TH1F* fh_DutyFactor_SAMPFIB_pois;
    TH1F* fh_DutyFactor_MaxToAvg;
    TH1F* fh_DutyFactor_Max;
    TH1F* fh_DutyFactor_Max_clean;
    TH1F* fh_DutyFactor_Max_SAMPFIB;
    TH1F* fh_DutyFactor_Max_SAMPFIB_clean;
    TH1F* fh_DutyFactor_Avg;
    TH1F* fh_DutyFactor_Avg_clean;
    TH1F* fh_DutyFactor_Avg_SAMPFIB;
    TH1F* fh_DutyFactor_Avg_SAMPFIB_clean;
    TH1F* fh_DutyFactor_PLD;
    TH1F* fh_rate;
    TH1F* fh_rate_SAMPFIB;
    TH1F* fh_rate_adj;
    TH1F* fh_rate_adj_SAMPFIB;
    TH1F* fh_rate_set;
    TH1F* fh_rate_set_SAMPFIB;
    TH1F* fh_spill_times_daq;
    TH1F* fh_spill_times_daq_first;
    TH1F* fh_particle;
    TH1F* fh_particle_pois;
    TH1F* fh_spill_hans;
    TH1F* fh_spill_hans_byMax;
    TH1F* fh_spill_hans_sum_byMax;
    TH1F* fh_spill_hans_SAMPFIB;
    TH1F* fh_spill_hans_SAMPFIB_byMean;
    TH1F* fh_spill_Q_E_real;
    TH1F* fh_spill_Q_E_real_SAMPFIB;
    TH1F* fh_spill_Q_E_pois;
    TH1F* fh_spill_Q_E_pois_SAMPFIB;
    TH1F* fh_spill_frac;
    TH1F* fh_spill_frac_SAMPFIB;
    TH1* fh_FFT;
    TH1* fh_FFT_adj;
    TH1* fh_FFT_adj_SAMPFIB;
    TH1* fh_FFT_add;
    TH1* fh_FFT_add_SAMPFIB;
    TH1F* fh_hans_sum;
    TH1F* fh_hans_sum_byMax;
    TH1F* fh_hans_SAMPFIB_sum;
    TH1F* fh_hans_SAMPFIB_sum_byMean;
    TH1F* fh_EL_sum;
    TH1F* fh_EL_SAMPFIB_sum;
    TH1F* fh_MtA_sum;
    TH1F* fh_rate_sum;
    TH1F* fh_rate_sum_DTmean;
    TH1F* fh_deadTime_adj;
    TH1F* fh_deadTime_mean;
    TH1F* fh_deadTime_sum;
    TH1F* fh_spillQuality_ppt;
    TH1F* fh_spillQuality_ppt_SAMPFIB;
    TH1F* fh_spillQuality_ppt_stat;
    TH1F* fh_spillQuality_ppt_stat_SAMPFIB;
    TH1F* fh_DutyFactor_MaxRun;
    TH1F* fh_DutyFactor_AvgRun;

    TH2F* fh_spill_times_Fib1;

    //------------------------------
    TH1F* fhTpat;
    TH1F* fh_spill_length;
    TH1F* fhTrigger;
    // TH1F* fh_particle;
    // TH1F* fh_particle_pois;
    TH1F* fh_TOFDOR_spill;
    TH1F* fh_spill_times;
    TH1F* fh_spill_times_zoom;
    TH1F* fh_spill_times_pois;
    // TH1F* fh_spill_times_daq;
    // TH1F* fh_spill_times_daq_first;
    TH1F* fh_frequencies;
    TH1F* fh_TOFDOR;
    // TH1F* fh_rate;
    // TH1F *h3;
    TH1F* fh_TimePreviousEvent;

    TH1F* fh_sci8_channels;
    TH1F* fh_sci8_tres_MCFD;
    TH1F* fh_sci8_tres_TAMEX;
    TH1F* fh_sci8_tot_mean;
    TH1F* fh_tof_sci8;
    TH2F* fh_sci8_tot;
    TH1F* fh_sci8_dt_hits;
    TH1F* fh_sci8_dt_hits_l;
    TH1F* fh_sci8_dt_hits_t;
    TH1F* fh_sci8_multihit;
    TH2F* fh_sci8_multihitVFTX;
    TH2F* fh_sci8_multihitLEAD;
    TH2F* fh_sci8_multihitTRAI;

    TH2F* fh_rolu_tot;
    TH1F* fh_rolu_channels;

    TH1F* fh_los_channels[NOF_LOS_DET];
    TH1F* fh_los_tres_MCFD[NOF_LOS_DET];
    TH1F* fh_los_tres_TAMEX[NOF_LOS_DET];
    TH1F* fh_los_tres_MTDC;
    TH1F* fh_los_tot_mean[NOF_LOS_DET];
    TH2F* fh_los_tot[NOF_LOS_DET];
    TH1F* fh_los_multihit[NOF_LOS_DET];
    TH2F* fh_los_multihitVFTX[NOF_LOS_DET];
    TH2F* fh_los_multihitLEAD[NOF_LOS_DET];
    TH2F* fh_los_multihitTRAI[NOF_LOS_DET];
    TH2F* fh_los_pos_MCFD[NOF_LOS_DET];
    TH2F* fh_los_pos_TAMEX[NOF_LOS_DET];
    TH2F* fh_los_pos_ToT[NOF_LOS_DET];
    TH2F* fh_los_ihit_ToT[NOF_LOS_DET];
    TH2F* fh_los_dt_hits_ToT[NOF_LOS_DET];
    TH2F* fh_los_ToT_vs_ToT;

    TH1F* fh_los_TOF[NOF_LOS_DET];
    TH1F* fh_los_dt_hits[NOF_LOS_DET];
    TH2F* fh_los_dt_first_ToT[NOF_LOS_DET];
    TH1F* fh_dt_hits[NOF_LOS_DET];

    TH1F* fh_channels_Fib[NOF_FIB_DET];
    TH1F* fh_channels_single_Fib[NOF_FIB_DET];
    TH1F* fh_fibers_Fib[NOF_FIB_DET];
    TH1F* fh_mult_Fib[NOF_FIB_DET];
    TH2F* fh_Fib_ToF[NOF_FIB_DET];
    TH2F* fh_Fib_pos_xy[NOF_FIB_DET];
    TH2F* fh_time_Fib[NOF_FIB_DET];
    TH2F* fh_multihit_m_Fib[NOF_FIB_DET];
    TH2F* fh_multihit_s_Fib[NOF_FIB_DET];
    TH2F* fh_ToT_m_Fib[NOF_FIB_DET];
    TH2F* fh_ToT_s_Fib[NOF_FIB_DET];
    TH2F* fh_Fib_vs_Events[NOF_FIB_DET];
    TH2F* fh_ToT_single_Fib[NOF_FIB_DET];

    TH1F* fh_tofd_channels[N_PLANE_MAX_TOFD];
    TH2F* fh_tofd_multihit[N_PLANE_MAX_TOFD];
    TH2F* fh_tofd_ToF[N_PLANE_MAX_TOFD];
    TH2F* fh_tofd_TotPm[N_PLANE_MAX_TOFD];
    TH2F* fh_tofd_dt[3];

    TH1F* fh_ptof_channels;
    TH1F* fh_ptof_channels_cut;
    TH1F* fh_ptof_test1;
    TH1F* fh_ptof_test2;
    TH1F* fh_ptof_TotPm1[N_PADDLE_MAX_PTOF];
    TH1F* fh_ptof_TotPm2[N_PADDLE_MAX_PTOF];

    TH1F* fh_pspx_channel_x[N_PSPX];      /**< PSPX x channel profile on mapped level */
    TH1F* fh_pspx_channel_y[N_PSPX];      /**< PSPX y channel profile on mapped level */
    TH1F* fh_pspx_multiplicity_x[N_PSPX]; /**< PSPX x multiplicity on mapped level */
    TH1F* fh_pspx_multiplicity_y[N_PSPX]; /**< PSPX y multiplicity on mapped level */

    TH2F* fh_pspx_strips_position[N_PSPX];        /**< PSPX 2D position on mapped level */
    TH2F* fh_pspx_hit_position[(N_PSPX + 1) / 2]; /**< PSPX 2D position on hit level */

    TH1F* fh_pspx_hit_energy[(N_PSPX + 1) / 2]; /**< PSPX energy on hit level */
    TH2F* fh_pspx_cal_energy_frontback[N_PSPX]; /**< PSPX energy front vs back on cal level */

  public:
    ClassDef(R3BOnlineSpillAnalysis, 2)
};

#endif
