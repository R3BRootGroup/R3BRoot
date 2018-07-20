// ------------------------------------------------------------
// -----                  R3BCalifaOnlineSpectra          -----
// -----          Created April 21th 2018 by E. Galiana   -----
// -----               Fill online histograms             -----
// ------------------------------------------------------------

#ifndef R3BCALIFAONLINESPECTRA
#define R3BCALIFAONLINESPECTRA
#define N_MAX_CRY 64
#define N_MAX_PETALS 8

#include "FairTask.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <array>
#include "TMath.h"
#include <cstdlib>

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;

/**
 * This taks reads CALIFA data and plots online histograms 
 */
class R3BCalifaOnlineSpectra : public FairTask {
  
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
  R3BCalifaOnlineSpectra(const char* name, Int_t iVerbose = 1);
  
  /**
   * Destructor.
   * Frees the memory used by the object.
   */
  virtual ~R3BCalifaOnlineSpectra();
  
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
   * Method for setting number of petals
   */
  inline void SetPetals(Int_t petals) {
    fCalifaNumPetals = petals;
  }
  
  /**
   * Method for setting number of crystals
   */
  //inline void SetOneCrystal(Int_t onecry) {
  //  fCalifaOneCrystal = onecry;
  //}
  
  /**
   * Method for setting the configuration parameters file
   */  
  inline void SetCalifaConfigFile(TString file){fCalifaFile=file;}
  
  /**
   * Method for setting the Data Level (Mapped or Cal)
   */ 
  inline void SetDisplayCalOn(Bool_t On){fCalON=On;}
  
  
 private:
  
  TClonesArray* fMappedItemsCalifa;     /**< Array with mapped items. */
  TClonesArray* fCalItemsCalifa;        /**< Array with cal items. */
  
  // check for trigger should be done globablly (somewhere else)
  R3BEventHeader* header;               /**< Event header. */
  Int_t fTrigger;                       /**< Trigger value. */
  Int_t fNEvents;        	   	/**< Event counter. */
  
  Int_t fCalifaNumPetals;              	/**< Number of Petals. */
  // Int_t fCalifaOneCrystal;      	/**< Crystal for plots. */
  
  TH1F* fh_Califa_energy_per_petal[N_MAX_PETALS];	
  TH1F* fh_Califa_crystals [N_MAX_PETALS][N_MAX_CRY];
  TH1F* fh_Califa_energy_oneCry;
  
  TH2F* fh_Califa_cryId_energy;
  TH2F* fh_Califa_cryId_petal;
  
  TString fCalifaFile;        	        /**< Config file name. */
  Bool_t  fCalON;                 	/**< Mapped or Cal selector. */
  
 public:
  ClassDef(R3BCalifaOnlineSpectra, 1)
};

#endif
