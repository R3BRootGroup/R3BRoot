// ------------------------------------------------------------------
// -----         R3BAmsStripCalPar source file                  -----
// -----         Created 29/05/18  by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------------

#include "R3BAmsStripCalPar.h"

#include "FairLogger.h"
#include "FairParamList.h"

#include "TString.h"
#include "TMath.h"
#include "TArrayF.h"

#include <iostream>

using std::cout;
using std::endl;

// ---- Standard Constructor ---------------------------------------------------
R3BAmsStripCalPar::R3BAmsStripCalPar(const char* name,
					       const char* title,
					       const char* context)
  : FairParGenericSet(name,title,context) {
  fStripCalParams= new TArrayF(3072);//1024strips*3CalibrationParameters
  fNumDets=1;
  fNumStrips=1024;
  fNumStripsS=640;
  fNumStripsK=384;
  fNumParamsFit=3;//gaus
}

// ----  Destructor ------------------------------------------------------------
R3BAmsStripCalPar::~R3BAmsStripCalPar() {
  clear();
  delete fStripCalParams;
}

// ----  Method clear ----------------------------------------------------------
void R3BAmsStripCalPar::clear() {
  status=kFALSE;
  resetInputVersions();
}

// ----  Method putParams ------------------------------------------------------
void R3BAmsStripCalPar::putParams(FairParamList* list) {
  LOG(INFO) <<"R3BAmsStripCalPar::putParams() called" <<FairLogger::endl;
  if (!list){ return; }
  
  Int_t array_size = fNumDets*fNumStrips*fNumParamsFit;
  LOG(INFO) <<"Array Size: "<<array_size <<FairLogger::endl;
  
  fStripCalParams->Set(array_size);
  
  list->add("amsStripCalPar", *fStripCalParams);
  list->add("amsDetNumberPar", fNumDets);
  list->add("amsStripNumberPar", fNumStrips);
  list->add("amsStripSNumberPar", fNumStripsS);
  list->add("amsStripKNumberPar", fNumStripsK);
  list->add("amsStripParamsFitPar", fNumParamsFit);
}


// ----  Method getParams ------------------------------------------------------
Bool_t R3BAmsStripCalPar::getParams(FairParamList* list) {
  LOG(INFO) <<"R3BAmsStripCalPar::getParams() called" <<FairLogger::endl;
  if (!list){ return kFALSE;}

  if (!list->fill("amsDetNumberPar", &fNumDets) ) { 
    return kFALSE; 
  }  

  if (!list->fill("amsStripNumberPar", &fNumStrips) ) { 
    return kFALSE; 
  }

  if (!list->fill("amsStripSNumberPar", &fNumStripsS) ) { 
    return kFALSE; 
  }

  if (!list->fill("amsStripKNumberPar", &fNumStripsK) ) { 
    return kFALSE; 
  }
  
  if (!list->fill("amsStripParamsFitPar", &fNumParamsFit) ) { 
    return kFALSE; 
  }
  
  Int_t array_size = fNumDets*fNumStrips*fNumParamsFit;
  LOG(INFO) <<"Array Size: "<<array_size <<FairLogger::endl;
  fStripCalParams->Set(array_size);
  
  if (!(list->fill("amsStripCalPar",fStripCalParams))){
    LOG(INFO)<< "---Could not initialize amsStripCalPar"<<FairLogger::endl;
    return kFALSE;
  }
  
  return kTRUE;
}

// ----  Method printParams ----------------------------------------------------
void R3BAmsStripCalPar::printParams() {	
  LOG(INFO) << "R3BAmsStripCalPar: ams strip Parameters: "<<FairLogger::endl;
  Int_t array_size = fNumDets*fNumStrips*fNumParamsFit;
  
  for(Int_t d=0;d<fNumDets;d++) {
    cout << "AMS detector number: " << d << endl;
   for(Int_t i=0;i<fNumStrips;i++) {      
    cout << "AMS Strip number: " << i << endl;
    for(Int_t j=0;j<fNumParamsFit;j++) {
      cout << "FitParam("<<j<<") = "<<fStripCalParams->GetAt(d*fNumParamsFit*fNumStrips+i*fNumParamsFit+j) << endl;
    }
   }
  }
}


