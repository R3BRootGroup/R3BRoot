// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                     R3BCalifaMapped2CrystalCal                    -----
// -----                Created 18/07/2014 by H. Alvarez-Pol               -----
// -----                 Modified 20/03/2017 by P. Cabanelas               -----
// -----                 Modified 15/12/2017 by E. Galiana                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALIFAMAPPED2CRYSTALCAL_H
#define R3BCALIFAMAPPED2CRYSTALCAL_H

#include "FairTask.h"
#include "TH1F.h"
#include "R3BCalifa.h"
#include "R3BCalifaMapped2CrystalCalPar.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappedData.h"
//#include "R3BCalifaCrystalCalPar.h"
#include <TRandom.h>

class TClonesArray;
class R3BCalifaCrystalCalPar;

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
  
  virtual void SetParContainers();
  
  //Fair specific
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  /** Virtual method ReInit **/
  virtual InitStatus ReInit();
  
  /** Virtual method Finish **/
  virtual void Finish();
  
  
 private:
  
  void SetParameter();
  
  Int_t NumCrystals=0;
  Int_t NumParams=0;
  TArrayF* CalParams;
  
  R3BCalifaCrystalCalPar* fCal_Par;  /**< Parameter container. >*/ 
  TClonesArray* fCalifaMappedDataCA; /**< Array with CALIFA Mapped- input data. >*/
  TClonesArray* fCalifaCryCalDataCA; /**< Array with CALIFA Cal- output data. >*/
  
  /** Private method AddCalData **/
  //** Adds a CalifaCryCalData to the CryCalCollection
  
  R3BCalifaCrystalCalData* AddCalData(Int_t id, Double_t energy, Double_t Nf, Double_t Ns, ULong64_t time, Double_t tot_energy);
  
  
  
 public:
  //Class definition
  ClassDef(R3BCalifaMapped2CrystalCal, 1)
    };

#endif
