#include "R3BLandCalPar.h"


#include "FairParamList.h"              // for FairParamList

#include "Riosfwd.h"                    // for ostream
#include "TString.h"                    // for TString

#include <stdlib.h>                     // for exit
#include <memory>                       // for auto_ptr, etc
#include <vector>                       // for vector, vector<>::iterator



using namespace std;

ClassImp(R3BLandCalPar);


R3BLandCalPar::R3BLandCalPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own)
{
  fTCalParams = new TObjArray(500);
}


R3BLandCalPar::~R3BLandCalPar()
{
  if(fTCalParams) {delete fTCalParams; fTCalParams=NULL;}
}


void R3BLandCalPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BLandCalPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->addObject("LandTCalParams", fTCalParams);
}

Bool_t R3BLandCalPar::getParams(FairParamList* list)
{
  if(!list) { return kFALSE;}
  if (!list->fillObject("LandTCalParams", fTCalParams)) {return kFALSE;}
  return kTRUE;
}

void R3BLandCalPar::clear()
{
}


void R3BLandCalPar::Print(){

  std::cout<<" -----------  Land Time Calib. Parameters -------------  "<<std::endl;

  std::cout<<" Number of TCal Parameters "<< fTCalParams->GetEntries() << std::endl;
  for(Int_t i=0;i<fTCalParams->GetEntries(); i++){
	R3BLandTCalPar* t_par = (R3BLandTCalPar*) fTCalParams->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (t_par) t_par->Print();
  }
  
}



