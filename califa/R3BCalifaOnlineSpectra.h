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

#ifndef R3BCALIFAONLINESPECTRA
#define R3BCALIFAONLINESPECTRA

#include "FairTask.h"
#include "TCanvas.h"
#include "TMath.h"
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#define Nb_Sides 2
#define Nb_Rings 5
#define Nb_Preamps 16
#define Nb_PreampCh 16

class TClonesArray;
class R3BCalifaMappingPar;
class TH1F;
class TH2F;
class R3BEventHeader;

/**
 * This taks reads CALIFA data and plots online histograms
 */
class R3BCalifaOnlineSpectra : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BCalifaOnlineSpectra();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BCalifaOnlineSpectra(const TString& name, Int_t iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BCalifaOnlineSpectra();

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
     * Method for setting number of rings
     */
    inline void SetRings(Int_t rings) { fNumRings = rings; }

    /**
     * Method for setting max. energy per crystal in MeV for Barrel histograms at CAL level
     */
    inline void SetMaxEnergyperCrystalBarrel(Int_t maxenergy) { fMaxEnergyBarrel = maxenergy; }

    /**
     * Method for setting max. energy per crystal in MeV for Iphos histograms at CAL level
     */
    inline void SetMaxEnergyperCrystalIphos(Int_t maxenergy) { fMaxEnergyIphos = maxenergy; }

    /**
     * Method for setting the configuration parameters file
     */
    inline void SetCalifaConfigFile(TString file) { fCalifaFile = file; }

    /**
     * Method to select binning and max range
     */
    inline void SetRange_bins(Int_t Histos_bins) { fMapHistos_bins = Histos_bins; }
    inline void SetRange_max(Int_t Histos_max) { fMapHistos_max = Histos_max; }

    /**
     * Method for setting the number of bins of Febex histograms
     */
    inline void SetBinChannelFebex(Int_t bin) { fBinsChannelFebex = bin; }

    /**
     * Method for setting max range of Febex histograms
     */
    inline void SetMaxBinFebex(Int_t max) { fMaxBinChannelFebex = max; }

    /**
     * Method to reset histograms
     */
    void Reset_CALIFA_Histo();

    /**
     * Method to change histogram scales
     */
    void Log_CALIFA_Histo();

    /**
     * Method for setting histogram sequence (Febex or Preamp. channels)
     */
    void Febex2Preamp_CALIFA_Histo();

  private:
    void SetParameter();

    Int_t fMapHistos_max;
    Int_t fMapHistos_bins;

    R3BCalifaMappingPar* fMap_Par;    /**< Container with mapping parameters. >*/
    TClonesArray* fMappedItemsCalifa; /**< Array with mapped items.    */
    TClonesArray* fCalItemsCalifa;    /**< Array with cal items.       */
    TClonesArray* fHitItemsCalifa;    /**< Array with hit items.       */
    TClonesArray* fWRItemsCalifa;     /**< Array with WR-Califa items. */
    TClonesArray* fWRItemsMaster;     /**< Array with WR-Master items. */

    // Check for trigger should be done globablly (somewhere else)
    R3BEventHeader* header; /**< Event header.  */
    Int_t fNEvents;         /**< Event counter. */

    Int_t fNbCalifaCrystals;              /**< Number of Crystals in Califa. */
    Int_t fNumSides;                      /**< Number of Sides, left and right.   */
    Int_t fNumRings;                      /**< Number of Rings.   */
    Int_t fNumPreamps;                    /**< Number of Preamps per ring.   */
    Int_t fNumCrystalPreamp;              /**< Number of Crystals/Channels per Preamp. */
    Int_t fBinsChannelFebex;              /**< Number of Bins per Febex channel. */
    Int_t fMaxBinChannelFebex;            /**< Maximum bin for Febex histograms. */
    Int_t fOrderFebexPreamp[Nb_PreampCh]; /**< Selector for febex or preamp sequence. */

    TString fCalifaFile;    /**< Config file name. */
    Int_t fMaxEnergyBarrel; /**< Max. energy for Barrel histograms at CAL level. */
    Int_t fMaxEnergyIphos;  /**< Max. energy for Iphos histograms at CAL level. */
    Bool_t fLogScale;       /**< Selecting scale. */
    Bool_t fRaw2Cal;        /**< Mapped or Cal selector. */
    Bool_t fFebex2Preamp;   /**< Febex or Preamp selector. */
    Int_t fFebexInfo[Nb_Sides][Nb_Rings][Nb_Preamps]
                    [4]; /**< Febex slot and module info: 0 slot and 1 module, (PR) 2 slot and 3 module. */

    // Canvas
    TCanvas* cCalifaMult;
    TCanvas* cCalifa_cry_energy;
    TCanvas* cCalifa_cry_energy_cal;
    TCanvas* cMap_RingR[Nb_Rings];
    TCanvas* cMap_RingL[Nb_Rings];
    TCanvas* cMapCry[Nb_Sides][Nb_Rings][Nb_Preamps];
    TCanvas* cMapCryTot[Nb_Sides][Nb_Rings][Nb_Preamps];
    TCanvas* cMapCryCal[Nb_Sides][Nb_Rings][Nb_Preamps];
    TCanvas* cMapCryP[Nb_Sides][Nb_Rings][Nb_Preamps];
    TCanvas* cMapCryPTot[Nb_Sides][Nb_Rings][Nb_Preamps];
    TCanvas* cMapCryPCal[Nb_Sides][Nb_Rings][Nb_Preamps];
    TCanvas* cCalifaCoinE;
    TCanvas* cCalifaCoinPhi;
    TCanvas* cCalifaCoinTheta;
    TCanvas* cCalifa_angles;
    TCanvas* cCalifa_theta_energy;
    TCanvas* cCalifa_hitenergy;

    // WR data
    TCanvas* cCalifa_wr;
    TH1F* fh1_Califa_wr;
    TCanvas* cWrs;
    TH1F* fh1_wrs[2];

    // Histograms
    TH1F* fh1_Califa_Mult;
    TH1F* fh1_Califa_MultHit;
    TH2F* fh2_Califa_cryId_energy;
    TH2F* fh2_Preamp_vs_ch_R[Nb_Rings];
    TH2F* fh2_Preamp_vs_ch_L[Nb_Rings];
    TH1F* fh1_crystals[Nb_Sides][Nb_Rings][Nb_Preamps][Nb_PreampCh];
    TH2F* fh2_crystalsETot[Nb_Sides][Nb_Rings][Nb_Preamps][Nb_PreampCh];
    TH1F* fh1_crystals_p[Nb_Sides][Nb_Rings][Nb_Preamps][Nb_PreampCh];
    TH2F* fh2_crystalsETot_p[Nb_Sides][Nb_Rings][Nb_Preamps][Nb_PreampCh];
    TH2F* fh2_Califa_cryId_energy_cal;
    TH1F* fh1_crystals_cal[Nb_Sides][Nb_Rings][Nb_Preamps][Nb_PreampCh];
    TH1F* fh1_crystals_p_cal[Nb_Sides][Nb_Rings][Nb_Preamps][Nb_PreampCh];
    TH2F* fh2_Califa_coinE;
    TH2F* fh2_Califa_coinTheta;
    TH2F* fh2_Califa_coinPhi;
    TH2F* fh2_Califa_theta_phi;
    TH2F* fh2_Califa_theta_energy;
    TH1F* fh1_Califa_total_energy;

  public:
    ClassDef(R3BCalifaOnlineSpectra, 1)
};

#endif
