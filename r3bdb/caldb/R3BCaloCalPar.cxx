#include "R3BCaloCalPar.h"


#include "FairDbLogFormat.h"
#include "FairDbLogService.h"
#include "FairDbOutTableBuffer.h"         // for FairDbOutRowStream
#include "FairDbStatement.h"            // for FairDbStatement

#include "FairParamList.h"              // for FairParamList

#include "Riosfwd.h"                    // for ostream
#include "TString.h"                    // for TString
#include "TArrayD.h"                    // for TString

#include <sstream>
#include <stdio.h>                     // for exit
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
  list->add("NrOfDUnits", fDUCalParams->GetEntries());
  
  for (Int_t i=0;i<fDUCalParams->GetEntries();i++){
    stringstream ss;
    ss << i;
	TString du_id(ss.str());
    TArrayD values(5);
    R3BCaloDUCalPar* dupar = (R3BCaloDUCalPar*) fDUCalParams->At(i);
    values[0] = dupar->GetGammaCal_offset(); 
    values[1] = dupar->GetGammaCal_gain(); 
    values[2] = dupar->GetToTCal_offset();
    values[3] = dupar->GetToTCal_gain();
    values[4] = dupar->GetConversionFactor();

    list->add(du_id.Data(), values); 
        
  } 

}

Bool_t R3BCaloCalPar::getParams(FairParamList* list)
{
  if(!list) { return kFALSE;}
  Int_t ndus;
  if (!list->fill("NrOfDUnits", &ndus)) {return kFALSE;}

  cout << " NrOFUNits " << ndus << endl;
 
  TArrayD* values = new TArrayD(5);
 
  for(Int_t i=0;i<ndus;i++){
    stringstream ss;
    ss << i;
	TString du_id(ss.str());
    if ( !list->fill(du_id.Data(), values) ) return kFALSE;
     
	R3BCaloDUCalPar* dupar = new R3BCaloDUCalPar();
	dupar->SetDetectionUnit(i);
	dupar->SetGammaCal_offset(values->At(0));
	dupar->SetGammaCal_gain(values->At(1));
	dupar->SetToTCal_offset(values->At(2));
	dupar->SetToTCal_gain(values->At(3));
	dupar->SetConversionFactor(values->At(4));
	fDUCalParams->Add( dupar );
    cout << " Entries in Array " <<  fDUCalParams->GetEntries()  << endl;
    for(Int_t j=0;j<5;j++) cout << " idx: " << j << "values# " << values->At(j) << endl;
  }
  
  if (values) delete values;

  return kTRUE;
}

void R3BCaloCalPar::ReadFile(string file) {
  vector<string> data;
  string datasegments, line;

  ifstream infile(file.c_str());
  while (getline(infile,line)) {
   stringstream dataline;
   dataline<<line;
   while (getline(dataline,datasegments,'\t')) {
    if(datasegments=="null")
     data.push_back("");
    else
     data.push_back(datasegments);
   }
   dataline.clear();
   R3BCaloDUCalPar dupar;
   dupar.SetDetectionUnit(atoi(data[0].c_str()));
   dupar.SetGammaCal_offset(atof(data[1].c_str()));
   dupar.SetGammaCal_gain(atof(data[2].c_str()));
   dupar.SetToTCal_offset(atof(data[3].c_str()));
   dupar.SetToTCal_gain(atof(data[4].c_str()));
   dupar.SetConversionFactor(atof(data[5].c_str()));
   fDUCalParams->Add(&dupar);
  }
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




