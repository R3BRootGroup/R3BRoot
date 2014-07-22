// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloCalib                            -----
// -----                Created 18/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALOCALIB_H
#define R3BCALOCALIB_H

#include "FairTask.h"
#include "R3BCaloCrystalHit.h"
#include "R3BCaloRawHit.h"
#include "R3BCaloCalibPar.h"

class TClonesArray;

class R3BCaloCalib : public FairTask {
    
 public:
  /** Default constructor **/
  R3BCaloCalib();
  
  /** Destructor **/
  virtual ~R3BCaloCalib();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
    
  /** Virtual method Reset **/
  virtual void Reset();

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
  //private functions making most of the calibration work
  Int_t MapCrystalID(R3BCaloRawHit* chit);
  Double32_t CalibrateEnergy(R3BCaloRawHit* chit);
  Double32_t CalibrateFastComponent(R3BCaloRawHit* chit);
  Double32_t CalibrateSlowComponent(R3BCaloRawHit* chit);
  Double32_t CalibrateTime(R3BCaloRawHit* chit);

    
  TClonesArray *fRawHitCA;
  TClonesArray *fCrystalHitCA;

  R3BCaloCalibPar* fCaloCalibPar;
    
 public:
  //Class definition
  ClassDef(R3BCaloCalib, 0)
};

#endif
