#include "R3BDBDaqInfo.h"

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

ClassImp(R3BDBDaqInfo);


R3BDBDaqInfo::R3BDBDaqInfo(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own)
{
}


R3BDBDaqInfo::~R3BDBDaqInfo()
{
}


void R3BDBDaqInfo::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BDBDaqInfo::putParams() called"<<std::endl;
  if(!list) { return; }
  //  list->addObject("Califa Materia", fMateria);
}

Bool_t R3BDBDaqInfo::getParams(FairParamList* list)
{
  if(!list) { return kFALSE;}
  //  if (!list->fillObject("Califa Materia", fMateria)) {return kFALSE;}
  return kTRUE;
}

void R3BDBDaqInfo::clear()
{
}



void R3BDBDaqInfo::fill(UInt_t rid){
  // Fill the lists with correspondin TimeStamps (runID) 
  cout << "-I- R3BDBDaqInfo::fill() called with RID# " << rid << endl; 

  /*
  R3BCaloMediaPar mpar;
  FairDbReader<R3BCaloMediaPar>* r_mpar = mpar.GetParamReader();    
  // Define a Global Context
  ValTimeStamp ts(rid);
  ValCondition context(Detector::kCal,DataType::kData,ts);
  // Activate reading for this Context
  r_mpar->Activate(context, GetVersion());

  Int_t numMedia = r_mpar->GetNumRows(); 
  for (int i = 0; i < numMedia; ++i) {
    R3BCaloMediaPar* media_par = (R3BCaloMediaPar*) r_mpar->GetRowByIndex(i);
    if (!media_par) { continue; }
    fMateria->Add(media_par);
  }
  */
}


void R3BDBDaqInfo::store(UInt_t rid){
  //<DB> store the lists
  cout << "-I- R3BDBDaqInfo::store() called with RID# " << rid << endl; 

  /*
  // Crystal Geometries
  for(Int_t i=0;i<fGeomCrystals->GetEntries();i++){
	R3BCaloCrystalGeoPar* cgeo_par = (R3BCaloCrystalGeoPar*) fGeomCrystals->At(i);
    if (cgeo_par) cgeo_par->store(rid); 
  }

 
  // Crystal nodes stored row-wized
  R3BCaloCrystalPar iPar;
  FairDbWriter<R3BCaloCrystalPar>* cW = iPar.ActivateWriter(rid); 

  if (cW) {
	// Crystals node
	for(Int_t i=0;i<fNodeCrystals->GetEntries();i++){
	  R3BCaloCrystalPar* c_par = (R3BCaloCrystalPar*) fNodeCrystals->At(i);
	  if (c_par) *cW << *c_par; 
	}
	// Reset the Writer
	if(!cW->Close()){
	  cout << " -W- R3BDBDaqInfo:: IO Error writing crystal data node ******** "<< endl;  
	}
  }//! (cWriter)
  else {
	cout << " -W- R3BDBDaqInfo:: Writer for R3BCaloCrystalPar not created "<< endl;  
  }

  */

}

void R3BDBDaqInfo::Print(){

  std::cout<<" -----------  Global Daq Parameters -------------  "<<std::endl;

  /*
  std::cout<<" Califa Media : "<< fMateria->GetEntries() << std::endl;
  for(Int_t i=0;i<fMateria->GetEntries(); i++){
	R3BCaloMediaPar* media_par = (R3BCaloMediaPar*) fMateria->At(i);  
    cout << "----------------------------------------------------------------------" << endl;
	if (media_par) media_par->Print();
  }
  */

}



