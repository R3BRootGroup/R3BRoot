#include "R3BLosTCalPar.h"

#include "FairParamList.h"              // for FairParamList

#include "Riosfwd.h"                    // for ostream
#include "TString.h"                    // for TString

#include <stdlib.h>                     // for exit
#include <memory>                       // for auto_ptr, etc
#include <vector>                       // for vector, vector<>::iterator


using namespace std;

ClassImp(R3BLosTCalPar);



R3BLosTCalPar::R3BLosTCalPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
	fCompId(0),
	fBarId(0),
    fSide(0),
    fNofChannels(0)
{
  // Reset all parameters
  clear();
}


R3BLosTCalPar::~R3BLosTCalPar()
{
}


void R3BLosTCalPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BLosTCalPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("comp_id",  fCompId);
  list->add("bar_id", fBarId);
  list->add("side", fSide);
  list->add("nofchannels", fNofChannels);
  list->add("bin_low", fBinLow, NCHMAX);
  list->add("bin_up", fBinUp, NCHMAX);
  list->add("time", fTime, NCHMAX);
  
}

Bool_t R3BLosTCalPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("comp_id", &fCompId)) { return kFALSE; }
  if (!list->fill("bar_id", &fBarId)) { return kFALSE; }
  if (!list->fill("side", &fSide)) { return kFALSE; }
  if (!list->fill("nofchannels", &fNofChannels)) { return kFALSE; }
  if (!list->fill("bin_low", fBinLow, NCHMAX)) { return kFALSE; }
  if (!list->fill("bin_up", fBinUp, NCHMAX)) { return kFALSE; }
  if (!list->fill("time", fTime, NCHMAX)) { return kFALSE; }
 
  return kTRUE;
}

void R3BLosTCalPar::clear()
{
  fCompId=fBarId=fSide=fNofChannels=0;
  // <DB> Not so much overhead here.
  for(Int_t i=0; i<NCHMAX; i++) { fBinLow[i]=fBinUp[i]=0;fTime[i]=0.;}
}



void R3BLosTCalPar::Print()
{
  std::cout<<"   R3BLosTCalPar: Time Calibration Parameters: "<<std::endl;
  std::cout<<"   fCompId: "<<  fCompId <<  std::endl;
  std::cout<<"   fBarId: "<<  fBarId <<  std::endl;
  std::cout<<"   fSide: "<<  fSide <<  std::endl;
  std::cout<<"   fNofChannels: "<<  fNofChannels <<  std::endl;
 for (Int_t i=0;i<fNofChannels;i++) {
   if ( (fBinLow[i] != 0) && (fBinUp[i] != 0) &&  (fTime[i] != 0) )
   std::cout<<"   BinLow: " <<  fBinLow[i]  <<  " BinUp "  << fBinUp[i] << " Time:" << fTime[i] <<  std::endl;
   }
}


Bool_t R3BLosTCalPar::Compare(const R3BLosTCalPar& that ) const {
  //  
  Bool_t test_h =  (fCompId   == that.fCompId)
 	               &&  (fBarId   == that.fBarId)
 	               &&  (fSide   == that.fSide)
                   &&  (fNofChannels == that.fNofChannels);
  
 
  Bool_t test_d=kTRUE;
  for(Int_t i=0; i<fNofChannels;i++){

    Int_t  a =  GetBinLowAt(i);
    Int_t  b =  that.GetBinLowAt(i);
    Int_t  c =  GetBinUpAt(i);
    Int_t  d =  that.GetBinUpAt(i);

    Double_t  t1 =  GetTimeAt(i);
    Double_t  t2 =  that.GetTimeAt(i);
	if ( ( a != b ) || ( c != d )  || (t1 != t2 ) ){
	  test_d = kFALSE;
      break;
    }      
  }
  
  return (test_h && test_d); 
}
