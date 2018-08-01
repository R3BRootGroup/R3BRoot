// ----------------------------------------------------------------------
// -----                                                            -----
// -----                     R3BAmsMapped2StripCal                  -----
// -----             Created 29/05/18  by J.L. Rodriguez-Sanchez    -----
// ----------------------------------------------------------------------

#ifndef R3BAmsMapped2StripCal_H
#define R3BAmsMapped2StripCal_H

#include "FairTask.h"
#include "TH1F.h"
#include "R3BAmsMapped2StripCalPar.h"
#include "R3BAmsStripCalData.h"
#include "R3BAmsMappedData.h"
#include <TRandom.h>

class TClonesArray;
class R3BAmsStripCalPar;

class R3BAmsMapped2StripCal : public FairTask {
  
 public:
  /** Default constructor **/
  R3BAmsMapped2StripCal();

  /** Standard constructor **/
  R3BAmsMapped2StripCal(const char* name, Int_t iVerbose=1);
  
  /** Destructor **/
  virtual ~R3BAmsMapped2StripCal();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* option);
  
  /** Virtual method Reset **/
  virtual void Reset();
  
  virtual void SetParContainers();
  
  //Fair specific
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  /** Virtual method ReInit **/
  virtual InitStatus ReInit();
  
  /** Virtual method Finish **/
  virtual void Finish();

  void SetOnline(Bool_t option){fOnline=option;}
  
 private:
  
  void SetParameter();

  Int_t NumDets;  
  Int_t NumStrips;
  Int_t NumStripsS;
  Int_t NumStripsK;
  Int_t NumParams;
  Int_t MaxSigma;
  TArrayF* CalParams;

  Bool_t fOnline;//Don't store data for online
  
  R3BAmsStripCalPar* fCal_Par;       /**< Parameter container. >*/ 
  TClonesArray* fAmsMappedDataCA;    /**< Array with AMS Mapped- input data. >*/
  TClonesArray* fAmsStripCalDataCA;  /**< Array with AMS Cal- output data. >*/
  
  /** Private method AddCalData **/
  //** Adds a AmsStripCalData to the StripCalCollection
  R3BAmsStripCalData* AddCalData(Int_t detid, Int_t sideid, Int_t stripid, Double_t energy);

 public:
  //Class definition
  ClassDef(R3BAmsMapped2StripCal, 1)
    };

#endif
