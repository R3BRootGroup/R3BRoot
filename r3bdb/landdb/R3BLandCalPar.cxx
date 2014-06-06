#include "R3BLandCalPar.h"


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



void R3BLandCalPar::fill(UInt_t rid){
  // Fill the lists with correspondin TimeStamps (runID) 
  cout << "-I- R3BLandCalPar::fill() called with RID# " << rid << endl; 

  R3BLandTCalPar tpar;

  FairDbReader<R3BLandTCalPar>* r_tpar = tpar.GetParamReader();  
  
  // Define a Global Context
  ValTimeStamp ts(rid);
  time_t shift = ts.GetSec() + 60;
  ValTimeStamp    end(shift,0);
  cout << " DATE of RETRIEVAL " << end.AsString("s") << endl;
  ValCondition context(Detector::kLand,DataType::kData,end);

  // Activate reading for this Context
  r_tpar->Activate(context, GetVersion());


  Int_t numTCh = r_tpar->GetNumRows(); 
  cout << "-I- R3BlandCalPar numOfRow " << numTCh << endl;
  for (int i = 0; i <= numTCh; ++i) {
    R3BLandTCalPar* tcal_par = (R3BLandTCalPar*) r_tpar->GetRow(i);
    if (!tcal_par) { continue; }
    else 
	  fTCalParams->Add(tcal_par);    
  }
  cout << "-I- R3BlandCalPar filled with  " << fTCalParams->GetEntries()  << " Cal Objects " << endl;
}


void R3BLandCalPar::store(UInt_t rid){
  //<DB> store the lists
  Int_t nParams = fTCalParams->GetEntries();

  cout << "-I- R3BLandCalPar::store() called with RID# " << rid << " nParams: " << nParams << endl; 

  // Crystal nodes stored row-wized
  R3BLandTCalPar iPar;
  FairDbWriter<R3BLandTCalPar>* cW = iPar.ActivateWriter(rid); 

  if (cW) {
	// TCal Objects
	for(Int_t i=0;i<nParams;i++){
	  R3BLandTCalPar* t_par = (R3BLandTCalPar*) fTCalParams->At(i);
	  if (t_par) *cW << *t_par; 
	}
	// Reset the Writer
	if(!cW->Close()){
	  cout << " -W- R3BLandCalPar:: IO Error writing crystal data node ******** "<< endl;  
	}
  }//! (cWriter)
  else {
	cout << " -W- R3BLandCalPar:: Writer for R3BCaloCrystalPar not created "<< endl;  
  }


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



