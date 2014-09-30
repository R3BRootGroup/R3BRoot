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
  virtual void Exec(Option_t* option);
    
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
    
  TClonesArray *fRawHitCA;
  TClonesArray *fCrystalHitCA;
  R3BCaloCalPar* fCaloCalPar;

  TRandom* ran;

  //Int_t counter1;
  //Int_t counter2;

    
 public:
  //Class definition
  ClassDef(R3BCaloCal, 0)
};

#endif
