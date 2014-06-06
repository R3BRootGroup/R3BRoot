#include "R3BLosCalPar.h"


#include "FairDbLogFormat.h"
#include "FairDbLogService.h"
#include "FairDbOutTableBuffer.h"         // for FairDbOutRowStream
#include "FairDbStatement.h"            // for FairDbStatement

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
{
  fTCalParams = new TObjArray(500);
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



void R3BLosCalPar::fill(UInt_t rid){
  // Fill the lists with correspondin TimeStamps (runID) 
  cout << "-I- R3BLosCalPar::fill() called with RID# " << rid << endl; 

  R3BLosTCalPar tpar;

  FairDbReader<R3BLosTCalPar>* r_tpar = tpar.GetParamReader();
  
  // Define a Global Context
  ValTimeStamp ts(rid);
  time_t shift = ts.GetSec() + 60;
  ValTimeStamp    end(shift,0);
  cout << " DATE of RETRIEVAL " << end.AsString("s") << endl;
  ValCondition context(Detector::kLos,DataType::kData,end);

  // Activate reading for this Context
  r_tpar->Activate(context, GetVersion());


  Int_t numTCh = r_tpar->GetNumRows(); 
  cout << "-I- R3BLosCalPar numOfRow " << numTCh << endl;
  for (int i = 0; i <= numTCh; ++i) {
    R3BLosTCalPar* tcal_par = (R3BLosTCalPar*) r_tpar->GetRow(i);
    if (!tcal_par) { continue; }
    else 
	  fTCalParams->Add(tcal_par);    
  }
  cout << "-I- R3BLosCalPar filled with  " << fTCalParams->GetEntries()  << " Cal Objects " << endl;
}


void R3BLosCalPar::store(UInt_t rid){
  //<DB> store the lists
  Int_t nParams = fTCalParams->GetEntries();

  cout << "-I- R3BLosCalPar::store() called with RID# " << rid << " nParams: " << nParams << endl; 

  // Crystal nodes stored row-wized
  R3BLosTCalPar iPar;
  FairDbWriter<R3BLosTCalPar>* cW = iPar.ActivateWriter(rid);

  if (cW) {
	// TCal Objects
	for(Int_t i=0;i<nParams;i++){
	  R3BLosTCalPar* t_par = (R3BLosTCalPar*) fTCalParams->At(i);
	  if (t_par) *cW << *t_par; 
	}
	// Reset the Writer
	if(!cW->Close()){
	  cout << " -W- R3BLosCalPar:: IO Error writing crystal data node ******** "<< endl;  
	}
  }//! (cWriter)
  else {
	cout << " -W- R3BLosCalPar:: Writer for R3BCaloCrystalPar not created "<< endl;  
  }


}

void R3BLosCalPar::Print(){

  std::cout<<" -----------  LOS Time Calib. Parameters --------------  "<<std::endl;

  std::cout<<" Number of TCal Parameters "<< fTCalParams->GetEntries() << std::endl;
  for(Int_t i=0;i<fTCalParams->GetEntries(); i++){
	R3BLosTCalPar* t_par = (R3BLosTCalPar*) fTCalParams->At(i);
    cout << "----------------------------------------------------------------------" << endl;
	if (t_par) t_par->Print();
  }
  
}



