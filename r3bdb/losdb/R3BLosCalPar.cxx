#include "R3BLosCalPar.h"


#include "FairParamList.h"              // for FairParamList

#include "Riosfwd.h"                    // for ostream
#include "TString.h"                    // for TString

#include <stdlib.h>                     // for exit
#include <memory>                       // for auto_ptr, etc
#include <vector>                       // for vector, vector<>::iterator



using namespace std;

ClassImp(R3BLosCalPar);


R3BLosCalPar::R3BLosCalPar(const char* name, const char* title, const char* context, Bool_t own)
: FairParGenericSet(name,title,context, own)
, fTCalParams(new TObjArray(500))
{
}


R3BLosCalPar::~R3BLosCalPar()
{
  if(fTCalParams) {delete fTCalParams; fTCalParams=NULL;}
}


void R3BLosCalPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BLosCalPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->addObject("LosTCalParams", fTCalParams);
}

Bool_t R3BLosCalPar::getParams(FairParamList* list)
{
  if(!list) { return kFALSE;}
  if (!list->fillObject("LosTCalParams", fTCalParams)) {return kFALSE;}
  return kTRUE;
}

void R3BLosCalPar::clear()
{
}


void R3BLosCalPar::Print(){

  std::cout<<" -----------  Land Time Calib. Parameters -------------  "<<std::endl;

  std::cout<<" Number of TCal Parameters "<< fTCalParams->GetEntries() << std::endl;
  for(Int_t i=0;i<fTCalParams->GetEntries(); i++){
	R3BLosTCalPar* t_par = (R3BLosTCalPar*) fTCalParams->At(i);
    cout << "----------------------------------------------------------------------" << endl;
	if (t_par) t_par->Print();
  }
  
}



