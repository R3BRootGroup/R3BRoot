#include "R3BCaloCalPar.h"


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

ClassImp(R3BCaloCalPar);


R3BCaloCalPar::R3BCaloCalPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own)
{
  fDUCalParams = new TObjArray(500);
}


R3BCaloCalPar::~R3BCaloCalPar()
{
  if(fDUCalParams) {delete fDUCalParams; fDUCalParams=NULL;}
}


void R3BCaloCalPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloCalPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->addObject("CalifaDUCalParams", fDUCalParams);
}

Bool_t R3BCaloCalPar::getParams(FairParamList* list)
{
  if(!list) { return kFALSE;}
  if (!list->fillObject("CalifaDUCalParams", fDUCalParams)) {return kFALSE;}
  return kTRUE;
}

void R3BCaloCalPar::clear()
{
}



void R3BCaloCalPar::fill(UInt_t rid){
  // Fill the lists with correspondin TimeStamps (runID) 
  cout << "-I- R3BCaloCalPar::fill() called with RID# " << rid << endl; 

  R3BCaloDUCalPar tpar;

  FairDbReader<R3BCaloDUCalPar>* r_tpar = tpar.GetParamReader();  
  
  // Define a Global Context
  ValTimeStamp ts(rid);
  time_t shift = ts.GetSec() + 60;
  ValTimeStamp    end(shift,0);
  cout << " DATE of RETRIEVAL " << end.AsString("s") << endl;
  ValCondition context(Detector::kLand,DataType::kData,end);

  // Activate reading for this Context
  r_tpar->Activate(context, GetVersion());


  Int_t numTCh = r_tpar->GetNumRows(); 
  cout << "-I- R3BCaloCalPar numOfRow " << numTCh << endl;
  for (int i = 0; i <= numTCh; ++i) {
    R3BCaloDUCalPar* tcal_par = (R3BCaloDUCalPar*) r_tpar->GetRow(i);
    if (!tcal_par) { continue; }
    else 
	  fDUCalParams->Add(tcal_par);    
  }
  cout << "-I- R3BCaloCalPar filled with  " << fDUCalParams->GetEntries()  << " Cal Objects " << endl;
}


void R3BCaloCalPar::store(UInt_t rid){
  //<DB> store the lists
  Int_t nParams = fDUCalParams->GetEntries();

  cout << "-I- R3BCaloCalPar::store() called with RID# " << rid << " nParams: " << nParams << endl; 

  // Crystal nodes stored row-wized
  R3BCaloDUCalPar iPar;
  FairDbWriter<R3BCaloDUCalPar>* cW = iPar.ActivateWriter(rid); 

  if (cW) {
	// TCal Objects
	for(Int_t i=0;i<nParams;i++){
	  R3BCaloDUCalPar* t_par = (R3BCaloDUCalPar*) fDUCalParams->At(i);
	  if (t_par) *cW << *t_par; 
	}
	// Reset the Writer
	if(!cW->Close()){
	  cout << " -W- R3BCaloCalPar:: IO Error writing crystal data node ******** "<< endl;  
	}
  }//! (cWriter)
  else {
	cout << " -W- R3BCaloCalPar:: Writer for R3BCaloCrystalPar not created "<< endl;  
  }


}

void R3BCaloCalPar::Print(){

  std::cout<<" -----------  Califa Detection Unit Calib. Parameters -------------  "<<std::endl;

  std::cout<<" Number of DUCal Parameters "<< fDUCalParams->GetEntries() << std::endl;
  for(Int_t i=0;i<fDUCalParams->GetEntries(); i++){
	R3BCaloDUCalPar* du_par = (R3BCaloDUCalPar*) fDUCalParams->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (du_par) du_par->Print();
  }
  
}



