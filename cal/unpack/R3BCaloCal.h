// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloCal                            -----
// -----                Created 18/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALOCAL_H
#define R3BCALOCAL_H

#include "FairTask.h"
#include "R3BCaloCalPar.h"
#include "R3BCaloCrystalHit.h"
#include "R3BCaloRawHit.h"
#include <TRandom.h>

class TClonesArray;

class R3BCaloCal : public FairTask {
    
 public:
  /** Default constructor **/
  R3BCaloCal();
  
  /** Destructor **/
  virtual ~R3BCaloCal();
  
  /** Virtual method Exec **/
  virtual void Exec(char c);
    
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
  Double32_t CalibrateEnergy(R3BCaloRawHit* chit,char c);
  Double32_t CalibrateFastComponent(R3BCaloRawHit* chit);
  Double32_t CalibrateSlowComponent(R3BCaloRawHit* chit);
  Double32_t CalibrateTime(R3BCaloRawHit* chit);

    
  TClonesArray *fRawHitCA;
  TClonesArray *fCrystalHitCA;
  R3BCaloCalPar* fCaloCalPar;
  float rando;
    
 public:
  //Class definition
  ClassDef(R3BCaloCal, 0)
};

#endif
