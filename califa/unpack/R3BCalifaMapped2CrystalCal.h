// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                     R3BCalifaMapped2CrystalCal                    -----
// -----                Created 18/07/2014 by H. Alvarez-Pol               -----
// -----                 Modified 20/03/2017 by P. Cabanelas               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALIFAMAPPED2CRYSTALCAL_H
#define R3BCALIFAMAPPED2CRYSTALCAL_H

#include "FairTask.h"
#include "R3BCalifaMapped2CrystalCalPar.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappedData.h"
#include <TRandom.h>

class TClonesArray;

class R3BCalifaMapped2CrystalCal : public FairTask {
    
 public:
  /** Default constructor **/
  R3BCalifaMapped2CrystalCal();
  
  /** Destructor **/
  virtual ~R3BCalifaMapped2CrystalCal();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* option);
    
  /** Virtual method Reset **/
  virtual void Reset();

  void UseHighRange(bool highRange);
  void SetCalibratePID(bool cal);

protected:
  //Fair specific
  /** Virtual method Init **/
  virtual InitStatus Init();
    
  /** Virtual method ReInit **/
  virtual InitStatus ReInit();
    
  /** Virtual method SetParContainers **/
  virtual void SetParContainers();
    
  /** Virtual method Finish **/
  virtual void Finish();

  virtual void Register();
 
 private:
    
  TClonesArray *fMappedDataCA;
  TClonesArray *fCrystalCalDataCA;
  R3BCalifaMapped2CrystalCalPar* fCalifaCalPar;

  TRandom* ran;

// Use high range calibration (instead of gamma calibraton)
  bool calHighRange;
// Calibrate PID
  bool calPID;

  //Int_t counter1;
  //Int_t counter2;

  UInt_t nEvents;

 public:
  //Class definition
  ClassDef(R3BCalifaMapped2CrystalCal, 1)
};

#endif
