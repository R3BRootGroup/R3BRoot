#include "R3BDBLmdUnpack.h"


#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "FairLmdSource.h"

#include <iostream>

using namespace std;





R3BDBLmdUnpack::R3BDBLmdUnpack()
    : FairUnpack(-4, -4, -4, -4, -4)
	,fCount(0)
{
}


R3BDBLmdUnpack::~R3BDBLmdUnpack()
{
  fStartTimes.clear();
  fStopTimes.clear();
  fHeaderInfo.clear();
}


Bool_t R3BDBLmdUnpack::Init()
{
    return kTRUE;
}


void R3BDBLmdUnpack::Register()
{
}


Bool_t R3BDBLmdUnpack::DoUnpack(Int_t* data, Int_t size)
{
  //cout << " -I- R3BDBLmdUnpack::DoUnpack  data:" <<  data << " size:"  << size << endl; 

  if (sizeof(s_filhe) == size) { 
	cout << " Do Unpack file header ************* " << size << endl;
    s_filhe* fxInfoHeader = (s_filhe*) data; 
    LmdHeaderInfo* header = new LmdHeaderInfo();
    header->label = fxInfoHeader->filhe_label;
    header->file  = fxInfoHeader->filhe_file;
    header->time = fxInfoHeader->filhe_time;
    header->exp = fxInfoHeader->filhe_exp_l;
    header->comment = fxInfoHeader->s_strings->string_l;
    header->length = fxInfoHeader->filhe_dlen;
    header->type = fxInfoHeader->filhe_type;
    header->subtype = fxInfoHeader->filhe_subtype;
    header->frag = fxInfoHeader->filhe_frag;
    header->used = fxInfoHeader->filhe_used;    
    header->buf = fxInfoHeader->filhe_buf;
    header->evt = fxInfoHeader->filhe_evt;
    header->stime = fxInfoHeader->filhe_stime[0];
    header->runid = fxInfoHeader->filhe_run_l;
    header->swap = fxInfoHeader->filhe_free[0];
    
	fHeaderInfo.push_back(header);
  
    fCount=0;
  } 
  else if (sizeof(s_bufhe) == size) {
    s_bufhe* fxBuffer = (s_bufhe*) data; 
	fCount++; 
    if (fCount==1){  
	fStartTimes.push_back(new ValTimeStamp(fxBuffer->l_time[0] ,  fxBuffer->l_time[1]));
	cout << " DoUnpack Start Time " << fCount << " size : " <<  size << endl;
      
    } else if (fCount==2){
	fStopTimes.push_back(new ValTimeStamp(fxBuffer->l_time[0] ,  fxBuffer->l_time[1]));
  	cout << " DoUnpack Stop Time " << fCount << " size : " <<  size << endl;
    }      
  } 
 
  return kTRUE;
}


void R3BDBLmdUnpack::Reset()
{
}


LmdHeaderInfo::LmdHeaderInfo(){
	label=file=time=exp=comment=""; 
    length=type=subtype=frag=used=buf=evt=stime=runid=swap=-1; 
  };


void LmdHeaderInfo::Dump(){
       
  cout	   << " label#  " << label
           << " Time#   " << time
           << " File#   " << file << endl;

  cout << "-I- LMDHeaderInfo: additional file info: " 
       << " len# " << length
       << " subtype# "<< type
	   << " type# " << subtype
       << " frag# " << frag
       << " used# " << used 
       << " buf# " << buf
	   << " evt# " << evt
       << " stime# " << stime
       << " swap# " << swap
       << " run_id# " << runid
       << " exp# " <<  exp
       << " comment# " << comment
       << endl;	
  }
  


ClassImp(LmdHeaderInfo)
ClassImp(R3BDBLmdUnpack)
