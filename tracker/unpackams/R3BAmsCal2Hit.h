// ----------------------------------------------------------------------
// -----                                                            -----
// -----                     R3BAmsCal2Hit                          -----
// -----             Created 01/06/18  by J.L. Rodriguez-Sanchez    -----
// ----------------------------------------------------------------------

#ifndef R3BAmsCal2Hit_H
#define R3BAmsCal2Hit_H

#include "FairTask.h"
#include "R3BAmsCal2Hit.h"
#include "R3BAmsStripCalData.h"
#include "R3BAmsHitData.h"
#include <TRandom.h>

class TClonesArray;

class R3BAmsCal2Hit : public FairTask {
  
 public:
  /** Default constructor **/
  R3BAmsCal2Hit();

  /** Standard constructor **/
  R3BAmsCal2Hit(const char* name, Int_t iVerbose=1);
  
  /** Destructor **/
  virtual ~R3BAmsCal2Hit();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* option);
  
  /** Virtual method Reset **/
  virtual void Reset();
  
  //Fair specific
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  /** Virtual method ReInit **/
  virtual InitStatus ReInit();
  
  /** Virtual method Finish **/
  virtual void Finish();
  
  
 private:

  Double_t fPitchK, fPitchS;

  TClonesArray* fAmsStripCalDataCA;  /**< Array with AMS Cal- input data. >*/
  TClonesArray* fAmsHitDataCA;       /**< Array with AMS Hit- output data. >*/
  
  /** Private method AddHitData **/
  //** Adds a AmsHitData to the HitCollection
  R3BAmsHitData* AddHitData(Int_t detid, Int_t numhit, Double_t x, Double_t y, Double_t z, Double_t energy);

 public:
  //Class definition
  ClassDef(R3BAmsCal2Hit, 1)
    };

#endif
