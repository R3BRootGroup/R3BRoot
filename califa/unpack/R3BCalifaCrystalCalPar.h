// ------------------------------------------------------------------
// -----         R3BCalifaCrystalCalPar header file             -----
// -----            Created 22/07/14   by H.Alvarez             -----
// -----            Modified 20/03/17  by P.Cabanelas           -----
// -----            Modified 11/12/17  by E.Galiana             -----
// ------------------------------------------------------------------


#ifndef R3BCALIFACRYSTALCALPAR_H
#define R3BCALIFACRYSTALCALPAR_H

#include "FairParGenericSet.h" // for FairParGenericSet

#include "TObject.h"
#include <TObjString.h>
#include "TObjArray.h"
#include "TArrayF.h"

using namespace std;

class FairParamList;

class R3BCalifaCrystalCalPar : public FairParGenericSet {
  
 public:
  
  /** Standard constructor **/
  R3BCalifaCrystalCalPar( const char* name    = "califaCrystalCalPar",
			  const char* title   = "Califa CrystalCal Parameters",
			  const char* context = "CalifaCalParContext");
  
  /** Destructor **/
  virtual ~R3BCalifaCrystalCalPar();
  
  /** Method to reset all parameters **/
  virtual void clear();
  
  /** Method to store all parameters using FairRuntimeDB **/
  virtual void putParams(FairParamList* list); 
  
  /** Method to retrieve all parameters using FairRuntimeDB**/
  Bool_t getParams(FairParamList* list);
  
  /** Method to print values of parameters to the standard output **/
  void printParams();
  
  /** Accessor functions **/
  const Double_t GetNumCrystals(){return fNumCrystals;}
  const Double_t GetNumParametersFit(){return fNumParamsFit;}
  TArrayF* GetCryCalParams(){return fCryCalParams;}
  
  void SetNumCrystals(Int_t numberCry){fNumCrystals=numberCry;}
  void SetNumParametersFit(Int_t numberParams){fNumParamsFit=numberParams;}
  void SetCryCalParams(Float_t cc, Int_t ii ){fCryCalParams->AddAt(cc, ii);}
  
  /** Create more Methods if you need them! **/
  
 private:
  
  TArrayF* fCryCalParams;/*< Calibration Parameters of Crystals>*/
  Int_t fNumCrystals;	/*< number of crystals>*/
  Int_t fNumParamsFit;	/*< number of cal parameters in the fit
			  pol1: A_fit & B_fit
			  pol2: A_fit, B_fit & C_fit>*/
  
  const R3BCalifaCrystalCalPar& operator=(const R3BCalifaCrystalCalPar&);/*< an assignment operator>*/
  
  R3BCalifaCrystalCalPar( const R3BCalifaCrystalCalPar&);/*< a copy constructor >*/
  
  ClassDef(R3BCalifaCrystalCalPar,1);
};

#endif
