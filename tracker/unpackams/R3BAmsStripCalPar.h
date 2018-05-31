// ------------------------------------------------------------------
// -----         R3BAmsStripCalPar source file                  -----
// -----         Created 29/05/18  by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------------


#ifndef R3BAmsStripCalPar_H
#define R3BAmsStripCalPar_H

#include "FairParGenericSet.h" // for FairParGenericSet

#include "TObject.h"
#include <TObjString.h>
#include "TObjArray.h"
#include "TArrayF.h"

using namespace std;

class FairParamList;

class R3BAmsStripCalPar : public FairParGenericSet {
  
 public:
  
  /** Standard constructor **/
  R3BAmsStripCalPar( const char* name    = "amsStripCalPar",
			  const char* title   = "AMS Strip Parameters",
			  const char* context = "AMSCalParContext");
  
  /** Destructor **/
  virtual ~R3BAmsStripCalPar();
  
  /** Method to reset all parameters **/
  virtual void clear();
  
  /** Method to store all parameters using FairRuntimeDB **/
  virtual void putParams(FairParamList* list); 
  
  /** Method to retrieve all parameters using FairRuntimeDB**/
  Bool_t getParams(FairParamList* list);
  
  /** Method to print values of parameters to the standard output **/
  void printParams();
  
  /** Accessor functions **/
  const Double_t GetNumDets(){return fNumDets;}
  const Double_t GetNumStrips(){return fNumStrips;}
  const Double_t GetNumStripsS(){return fNumStripsS;}
  const Double_t GetNumStripsK(){return fNumStripsK;}
  const Double_t GetNumParametersFit(){return fNumParamsFit;}
  TArrayF* GetStripCalParams(){return fStripCalParams;}

  void SetNumDets(Int_t numberDets){fNumDets=numberDets;}  
  void SetNumStrips(Int_t numberStrip){fNumStrips=numberStrip;}
  void SetNumStripsS(Int_t numberStrip){fNumStripsS=numberStrip;}
  void SetNumStripsK(Int_t numberStrip){fNumStripsK=numberStrip;}
  void SetNumParametersFit(Int_t numberParams){fNumParamsFit=numberParams;}
  void SetStripCalParams(Double_t cc, Int_t ii ){fStripCalParams->AddAt(cc, ii);}
  
  /** Create more Methods if you need them! **/
  
 private:
  
  TArrayF* fStripCalParams;// Calibration Parameters of strip
  Int_t fNumDets;       // number of detectors
  Int_t fNumStrips;	// number of strips
  Int_t fNumStripsS;	// number of strips S-side
  Int_t fNumStripsK;	// number of strips K-side
  Int_t fNumParamsFit;	/* number of cal parameters in the fit
			  gaus: A_fit & B_fit & C_fit*/
  
  const R3BAmsStripCalPar& operator=(const R3BAmsStripCalPar&);/*< an assignment operator>*/
  
  R3BAmsStripCalPar( const R3BAmsStripCalPar&);/*< a copy constructor >*/
  
  ClassDef(R3BAmsStripCalPar,1);
};

#endif
