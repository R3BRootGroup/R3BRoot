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
  for (Int_t i=0;i<2;i++ ) fTChannels[i] = new TObjArray(500);
}


R3BLandCalPar::~R3BLandCalPar()
{
  for (Int_t i=0;i<2;i++ ) if(fTChannels[i]) delete fTChannels[i];
}


void R3BLandCalPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BLandCalPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->addObject("LandTChannelsR", fTChannels[0]);
  list->addObject("LandTChannelsL", fTChannels[1]);
}

Bool_t R3BLandCalPar::getParams(FairParamList* list)
{
  if(!list) { return kFALSE;}
  if (!list->fillObject("LandTChannelsR", fTChannels[0])) {return kFALSE;}
  if (!list->fillObject("LandTChannelsL", fTChannels[1])) {return kFALSE;}
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
  ValCondition context(Detector::kLand,DataType::kData,ts);

  // Activate reading for this Context
  r_tpar->Activate(context, GetVersion());


  Int_t numTCh = r_tpar->GetNumRows(); 
  for (int i = 0; i < numTCh; ++i) {
    R3BLandTCalPar* tcal_par = (R3BLandTCalPar*) r_tpar->GetRowByIndex(i);
    if (!tcal_par) { continue; }

    if (tcal_par->GetSide() == 0)
	  fTChannels[0]->Add(tcal_par);
    else 
	  fTChannels[1]->Add(tcal_par);

  }

}


void R3BLandCalPar::store(UInt_t rid){
  //<DB> store the lists
  Int_t n0 = fTChannels[0]->GetEntries();
  Int_t n1 = fTChannels[1]->GetEntries();

  cout << "-I- R3BLandCalPar::store() called with RID# " << rid << " n0: " << n0 << " n1: " << n1 << endl; 

  if ( n0 != n1 ) {
	cout << "-E- R3BLandCalPar::store() channels entries inconsistent " << endl; 
   exit(1);
  } 
  

  // Crystal nodes stored row-wized
  R3BLandTCalPar iPar;
  FairDbWriter<R3BLandTCalPar>* cW = iPar.ActivateWriter(rid); 

  if (cW) {
	// Crystals node
	for(Int_t i=0;i<n0;i++){
	  R3BLandTCalPar* t_par0 = (R3BLandTCalPar*) fTChannels[0]->At(i);
	  R3BLandTCalPar* t_par1 = (R3BLandTCalPar*) fTChannels[1]->At(i);
	  if (t_par0) *cW << *t_par0; 
	  if (t_par1) *cW << *t_par1; 
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

  std::cout<<" -----------  Land Channel Time Calib. Parameters -------------  "<<std::endl;

  std::cout<<" Side R:  "<< fTChannels[0]->GetEntries() << std::endl;
  for(Int_t i=0;i<fTChannels[0]->GetEntries(); i++){
	R3BLandTCalPar* t_par = (R3BLandTCalPar*) fTChannels[0]->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (t_par) t_par->Print();
  }

  std::cout<<" Side L:  "<< fTChannels[1]->GetEntries() << std::endl;
  for(Int_t i=0;i<fTChannels[1]->GetEntries(); i++){
	R3BLandTCalPar* t_par = (R3BLandTCalPar*) fTChannels[1]->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (t_par) t_par->Print();
  }
  
  
  
}



