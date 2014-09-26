// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloCalib                            -----
// -----                Created 18/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKCALIB_H
#define R3BSTARTRACKCALIB_H

#include "FairTask.h"
#include "R3BSTaRTrackerDigitHit.h"
#include "R3BStarTrackRawHit.h"
#include "R3BStarTrackCalibPar.h"

class TClonesArray;

class R3BStarTrackCalib : public FairTask {
    
 public:
  /** Default constructor **/
  R3BStarTrackCalib();
  
  /** Destructor **/
  virtual ~R3BStarTrackCalib();
  
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
  Int_t MapModuleID(R3BStarTrackRawHit* chit);
  Int_t MapSide(R3BStarTrackRawHit* chit);
  Int_t MapAsicID(R3BStarTrackRawHit* chit);
  Int_t MapStripID(R3BStarTrackRawHit* chit);
  Double32_t CalibrateEnergy(R3BStarTrackRawHit* chit);
  Double32_t CalibrateTime(R3BStarTrackRawHit* chit);

    
  TClonesArray *fRawHitCA;
  TClonesArray *fSiDetHitCA;

  R3BStarTrackCalibPar* fStarTrackCalibPar;
    
 public:
  //Class definition
  ClassDef(R3BStarTrackCalib, 0)
};

#endif
