#include "R3BDBLmdAnalyzer.h"


#include "TList.h"
#include "TObjString.h" 

#include "ValTimeStamp.h"

#include <iostream>
using namespace std;




R3BDBLmdAnalyzer::R3BDBLmdAnalyzer()
  : FairLmdSource(),
    fNEvent(0),
    fCurrentEvent(0)
{
}


R3BDBLmdAnalyzer::R3BDBLmdAnalyzer(const R3BDBLmdAnalyzer& source)
  : FairLmdSource(source),
    fNEvent(0), 
	fCurrentEvent(0)
{
}


R3BDBLmdAnalyzer::~R3BDBLmdAnalyzer()
{
  fStartTimes.clear();
  fStopTimes.clear();
}



Bool_t R3BDBLmdAnalyzer::Init()
{
  if(! FairLmdSource::Init()) {
    return kFALSE;
  }

 
  // Init Counters
  fNEvent=fCurrentEvent=0;
  return kTRUE;
}


Int_t R3BDBLmdAnalyzer::ReadEvent()
{
 
  void* evtptr = &fxEvent;
  void* buffptr = &fxBuffer;
 
  Int_t status = f_evt_get_event(fxInputChannel, (INTS4**)evtptr,(INTS4**) buffptr);


  if(GETEVT__SUCCESS != status) {

    if(GETEVT__NOMORE == status ) {
      // Store Stop Time
      fStopTimes.push_back(new ValTimeStamp(fxBuffer->l_time[0] ,  fxBuffer->l_time[1]));
      PrintHeaderInfo();
      Close();
    }


    if(fCurrentFile >= fFileNames->GetSize()) {
      return 1;
    }
	
	
    TString name = ((TObjString*)fFileNames->At(fCurrentFile))->GetString();
    if(! OpenNextFile(name)) {
      return 1;
    } else {
      fCurrentFile += 1;
      return ReadEvent();
    }
  }

  //Store Start Times
  if (fCurrentEvent==0 ) 
	fStartTimes.push_back(new ValTimeStamp(fxBuffer->l_time[0] ,  fxBuffer->l_time[1]));

  /* 
  cout << "-I- LMDANA: buffer header " << endl;
  cout << "                   b_dlen#  "<<  fxBuffer->l_dlen << endl;
  cout << "                   l_evt#   "<<  fxBuffer->l_evt << endl;
  cout << "                   h_begin# " <<  fxBuffer->h_begin << endl;
  cout << "                   h_end#   " <<  fxBuffer->h_end << endl;

  ValTimeStamp evt_time( fxBuffer->l_time[0] ,  fxBuffer->l_time[1]);
  cout << "                   l_time0#   " <<  evt_time.Format("iso") << endl;
  cout << "                   l_time1#   " <<  fxBuffer->l_time[1] << endl; 
  */

 
  Int_t nrSubEvts = f_evt_get_subevent(fxEvent, 0, NULL, NULL, NULL);
 
  Int_t sebuflength;
  Short_t setype;
  Short_t sesubtype;
  Short_t seprocid;
  Short_t sesubcrate;
  Short_t secontrol;
  
  if (fCurrentEvent%10000==0)
  cout << " -I- LMD_ANA:  evt# " <<  fCurrentEvent << "  n_subevt# " << nrSubEvts << " evt processed# " << fNEvent <<  endl;

  for(Int_t i = 1; i <= nrSubEvts; i++) {
    void* SubEvtptr = &fxSubEvent;
    void* EvtDataptr = &fxEventData;
    Int_t nrlongwords;
    status = f_evt_get_subevent(fxEvent, i, (Int_t**)SubEvtptr, (Int_t**)EvtDataptr, &nrlongwords);
 
   if(status) {
      return 1;
    }

    sebuflength = nrlongwords;
    setype = fxSubEvent->i_type;
    sesubtype = fxSubEvent->i_subtype;
    seprocid = fxSubEvent->i_procid;
    sesubcrate = fxSubEvent->h_subcrate;
    secontrol = fxSubEvent->h_control;
      
	// cout << " -I- LMD_ANA:  subevent# " << i <<  " length: " << sebuflength <<  " type: " <<   setype << endl;
	//  
    //if(! Unpack(fxEventData, sebuflength,
    //            setype, sesubtype,
	//             seprocid, sesubcrate, secontrol)) {
    //  return 2;
	// }
  }
 
  // Increment evt counters.
  fNEvent++;
  fCurrentEvent++;
  return 0;
}




void R3BDBLmdAnalyzer::Close()
{
  FairLmdSource::Close();
  fCurrentEvent=0;
}

void R3BDBLmdAnalyzer::PrintHeaderInfo(){
 // Info of the current opened file 
  TString fName = ((TObjString*)fFileNames->At(fCurrentFile-1))->GetString();
  cout << "-I- LMDANA: open file: " <<  fName.Data()  << " Length# "<<fxInfoHeader->filhe_dlen 
	   << " label#  " << fxInfoHeader->filhe_label   
       << " Time#   " << fxInfoHeader->filhe_time 
       << " File#   " << fxInfoHeader->filhe_file << endl;

  cout << "-I- LMDANA: additional file info: " 
       << " len# " << fxInfoHeader->filhe_dlen
       << " subtype# "<< fxInfoHeader->filhe_subtype
	   << " type# " << fxInfoHeader->filhe_type
       << " frag# " << fxInfoHeader->filhe_frag
       << " used# " << fxInfoHeader->filhe_used
       << " buf# " << fxInfoHeader->filhe_buf
	   << " evt# " << fxInfoHeader->filhe_evt
       << " stime# " << fxInfoHeader->filhe_stime[0]
       << " swap# " << fxInfoHeader->filhe_free[0]
       << " run_id# " << fxInfoHeader->filhe_run_l
       << " exp# " << fxInfoHeader->filhe_exp_l
       << " comment# " << fxInfoHeader->s_strings->string_l  
       << endl;

}



ClassImp(R3BDBLmdAnalyzer)


