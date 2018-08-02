// ------------------------------------------------------------
// -----                  R3BAmsOnlineSpectra             -----
// -----    Created 22/07/18  by J.L. Rodriguez-Sanchez   -----
// -----           Fill AMS online histograms             -----
// ------------------------------------------------------------

#ifndef R3BAmsOnlineSpectra_H
#define R3BAmsOnlineSpectra_H

#include "FairTask.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <array>
#include "TMath.h"
#include <cstdlib>
#include "TH2F.h"
#include "TCanvas.h"

class TClonesArray;
class R3BEventHeader;

/**
 * This taks reads AMS data and plots online histograms 
 */
class R3BAmsOnlineSpectra : public FairTask {
  
 public:
  /**
   * Default constructor.
   * Creates an instance of the task with default parameters.
   */
  R3BAmsOnlineSpectra();
  
  /**
   * Standard constructor.
   * Creates an instance of the task.
   * @param name a name of the task.
   * @param iVerbose a verbosity level.
   */
  R3BAmsOnlineSpectra(const char* name, Int_t iVerbose = 1);
  
  /**
   * Destructor.
   * Frees the memory used by the object.
   */
  virtual ~R3BAmsOnlineSpectra();
  
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
   * Method for setting the Data Level (Mapped or Cal)
   */ 
  inline void SetDisplayCalOn(Bool_t On){fCalON=On;}


  void Reset_AMS_Histo();
  
 private:
  
  TClonesArray* fMappedItemsAms;     /**< Array with mapped items. */
  TClonesArray* fCalItemsAms;        /**< Array with cal items. */
  TClonesArray* fHitItemsAms;        /**< Array with hit items. */
  
  // check for trigger should be done globablly (somewhere else)
  R3BEventHeader* header;               /**< Event header. */
  Int_t fTrigger;                       /**< Trigger value. */
  Int_t fNEvents;        	   	/**< Event counter. */
  
  TCanvas* cMap;
  TCanvas* cCal;
  TCanvas* cHit;

  TH2F* fh_Ams_energy_allStrips[4];
  TH2F* fh_Ams_energy_allCalStrips[8];
  TH2F* fh_Ams_hit[4];
  
  //TString fAmsFile;        	        /**< Config file name. */
  Bool_t  fCalON;                 	/**< Mapped or Cal selector. */
  
 public:
  ClassDef(R3BAmsOnlineSpectra, 1)
};

#endif
