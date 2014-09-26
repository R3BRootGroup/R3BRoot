// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStarTrackUnpack                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2014 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------


//ROOT headers
#include "TClonesArray.h"
#include "TH1F.h"
#include "TMath.h"

//Fair headers
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

//STaRTracker headers
#include "R3BStarTrackRawHit.h"
#include "R3BStarTrackUnpack.h"
#include "R3BSTaRTraHit.h"

//#define NUMCHANNELS 1500
//#define NUMMODULES 1500

#include <iostream>
using namespace std;


//R3BStarTrackUnpack: Constructor
R3BStarTrackUnpack::R3BStarTrackUnpack(char *strTraDir,
                             Short_t type, Short_t subType,
                             Short_t procId,
                             Short_t subCrate, Short_t control)
: FairUnpack(type, subType, procId, subCrate, control),
fRawData(new TClonesArray("R3BStarTrackRawHit")),
fNHits(0)
{
}



//Virtual R3BStarTrackUnpack: Public method
R3BStarTrackUnpack::~R3BStarTrackUnpack()
{
  LOG(INFO) << "R3BStarTrackUnpack: Delete instance" << FairLogger::endl;
  delete fRawData;
}



//Init: Public method
Bool_t R3BStarTrackUnpack::Init()
{
  Register();
  return kTRUE;
}



//Register: Protected method
void R3BStarTrackUnpack::Register()
{
  LOG(INFO) << "Registration of StarTrack Unpacker" << FairLogger::endl;
 FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) {
    return;
  }
  fMan->Register("StarTrackRawHit", "Raw data from R3B Si Tracker", fRawData, kTRUE);
}



//DoUnpack: Public method
Bool_t R3BStarTrackUnpack::DoUnpack(Int_t *data, Int_t size)  // used for Mbs format datafile 
{
 
  // TODO: adapt it for Tracker when  data format is known for tracker
  
  LOG(INFO) << "R3BSTaRTrackUnpack : Unpacking with DoUnpack() ... size = " << size << FairLogger::endl;
  //LOG(INFO) << "R3BSTaRTrackUnpack : Unpacking... size = " << size << FairLogger::endl;

  UInt_t l_s = 0;
  UInt_t* pl_data = (UInt_t*)(data + l_s); 
 

  //while(l_s < size) {
  while(l_s < 2) {    // 2 words of 2x32 bits 
 
      //UInt_t* p1 = (UInt_t*)(data + l_i);
      //UInt_t l_sam_id = (p1[0] & 0xf0000000) >> 28; // identifies the sam
      //UInt_t l_gtb_id = (p1[0] & 0x0f000000) >> 24; // 0 or 1, identifies which of the 2 cables of the sam
      //UInt_t l_lec = (p1[0] & 0x00f00000) >> 20;

      //UInt_t l_da_siz = (pl_data[0] & 0x000001ff);  // To be changed for tracker maybe ?
      UInt_t l_da_siz = 1;  // = size ; To be changed for tracker maybe ?


	   UInt_t module_id;  // module id
	   UInt_t side;
	   UInt_t asic_id;    // Chip id
	   UInt_t strip_id;   // strip id 
	   UInt_t adcData;  // adc value for energy loss in Si
	   UShort_t ts1=0x00000000;
	   UShort_t ts2;	   
	   
	   ULong_t lclock;     // local timestamp set = to white rabbit


      // LOG(INFO) << "R3BSTaRTrackUnpack :   size:" << l_da_siz << FairLogger::endl;

	   //l_s += 1;
       pl_data = (UInt_t*)(data + l_s);
       
       //for (Int_t i = 0; i < l_da_siz; i += 2)  // !! to be checked:l_da_siz shoulb be 32 bits and one needs to read 2 words of 32bits so incrementation of 2
       for (Int_t i = 0; i < l_da_siz; i += 1)  // !! to be checked:l_da_siz shoulb be 32 bits and one needs to read 2 words of 32bits so incrementation of 2
	 {

	   UShort_t hit;
       	   hit= pl_data[l_s] & 0xE0000000 >> 29;
 
           LOG(INFO) << "R3BSTaRTrackUnpack :   hit :" << hit << FairLogger::endl;

	
	   //ULong_t evsize; 
	   //ULong_t WRclock;     // global timestamp (white rabbit)



	   // data starting with the 2 first high bit at 10:  (l_s=0)
      	   if( (pl_data[l_s] & 0x80000000 >> 28)== 0x8)  // 0x8=1000
	     {
                LOG(INFO) << "R3BSTaRTrackUnpack :   wordA :" <<  (pl_data[l_s] & 0x80000000 >> 28) << FairLogger::endl;

	       //read timestamp High bit
               //LOG(INFO) << "R3BSTaRTrackUnpack :   ts1 :" <<  ts1 << FairLogger::endl;
	       //ts1= (pl_data[l_s+1] & 0x000FFFFF);
               //LOG(INFO) << "R3BSTaRTrackUnpack :   ts1 :" <<  ts1 << FairLogger::endl;
	     }


	   if(l_s==1)
	     {
 
	       
	     }

	  
	   // data word starting with the 2 first high bit at 11:
       	   if( (pl_data[l_s] & 0xC0000000 >> 28) == 0xC)  // 0xC=1100
	     { 
             LOG(INFO) << "R3BSTaRTrackUnpack :   wordB :" <<  (pl_data[l_s] & 0xC0000000 >> 28) << FairLogger::endl;
	       // first 32 bit word:
	       //module_id = (pl_data[i] & 0x1F000000) >> 24; // 0-3
	       module_id = (pl_data[l_s] & 0x1F800000) >> 23; // 0-3
                 LOG(INFO) << "R3BSTaRTrackUnpack :   module id:" <<  module_id << FairLogger::endl;
		 //side=  (pl_data[l_s] & 0x00800000) >> 23; // 0-1
                 //LOG(INFO) << "R3BSTaRTrackUnpack :   side:" <<  side << FairLogger::endl;
	       asic_id =  (pl_data[l_s] & 0x00780000) >> 19;  // 0-15
                 LOG(INFO) << "R3BSTaRTrackUnpack :   asic_id:" <<  asic_id << FairLogger::endl;
	       strip_id =  (pl_data[l_s] & 0x0007F000) >> 12;   // 0-127
                 LOG(INFO) << "R3BSTaRTrackUnpack :   strip_id:" <<   strip_id << FairLogger::endl;
	       adcData = (pl_data[l_s] & 0x00000FFF);  
                 LOG(INFO) << "R3BSTaRTrackUnpack :   adcData:" << adcData << FairLogger::endl;


	       //read timestamp low bit
	       ts2= (pl_data[(l_s+1)] & 0x0FFFFFFF);
                LOG(INFO) << "R3BSTaRTrackUnpack :   ts2 :" <<  ts2 << FairLogger::endl;
	     } 


	 }

       l_s += 1;


       if(ts1!=0x00000000)
	 { lclock = (unsigned long long int)(ts1 & 0x000FFFFF) <<28 | (ts2 & 0x0FFFFFFF);
	 }


       
       LOG(DEBUG) << "R3BStartrackerUnpack : Strip_ID IS " << strip_id << ",  Chip ID IS " << asic_id << " , Ladder ID IS " << module_id << " , ADC Data IS " << adcData << FairLogger::endl;
       new ((*fRawData)[fNHits]) R3BStarTrackRawHit(module_id, side, asic_id, strip_id, adcData, lclock);
       fNHits++;
 

  }

  //LOG(INFO) << "R3BSTaRTrackUnpack : Number of hits in STarTracker: " << fNHits << FairLogger::endl;
    return kTRUE;

}

  

//DoUnpack2: Public method  (use for R3B Si tracker and data file from DL)
// Each block_main should contain a pair of words: 
/**/
Bool_t R3BStarTrackUnpack::DoUnpack2(Int_t *data_word0, Int_t *data_word1, Int_t size)   
{
  //LOG(INFO) << "R3BSTaRTrackUnpack : Unpacking with DoUnpack2() ... size = " << size << FairLogger::endl;
  
  UInt_t l_s = 0;

  UInt_t *pl_data_word0 = (UInt_t*) data_word0;   
  UInt_t *pl_data_word1 = (UInt_t*) data_word1;   
 
  
  //cout << "size=" << size << endl; 
  //cout << "data_word0=" << *data_word0 << endl; 
  //cout << "data_word1=" << *data_word1 << endl; 
  //cout << "data_word0=" << *pl_data_word0 << endl; 
  //cout << "data_word1=" << *pl_data_word1 << endl; 
  

  //LOG(INFO) << "Unpacking Startracker data" << FairLogger::endl;
  LOG(DEBUG) << "Unpacking Startracker data" << FairLogger::endl;
 

  // Check the trailer: reject or keep the block.
  if( (*data_word0 & 0xFFFFFFFF) == 0xFFFFFFFF &&
      (*data_word1 & 0xFFFFFFFF) == 0xFFFFFFFF) {
   // this marks the point after which there is no more good data in the block_main
   //log_file << "End of block " << itr_1 << std::endl; 
    LOG(INFO) << "End of block " <<  FairLogger::endl; 
    //flag_terminator=true;
    //break;
  }


  // Check if word_0 begins with:
  // - 10 then is type A word. 
  // - 11 then is type B word.
  if ( (*data_word0 & 0xC0000000)==0x80000000 ){
    //cout << "Words type A (msb=10)." << std::endl;
    LOG(DEBUG) << "Words type A (msb=10)." << FairLogger::endl;
    word_0A=*pl_data_word0;
    word_1A=*pl_data_word1;
  }
  if ( (*data_word0 & 0xC0000000)==0xC0000000 ){
    //cout << "Words type B (msb=11)."<< std::endl;
    LOG(DEBUG) << "Words type B (msb=11)."<< std::endl;
    word_0B=*pl_data_word0;
    word_1B=*pl_data_word1;
  }

  
  while(l_s < size) {
   
      l_s++;
   
    
    LOG(DEBUG) << "At GOSIP memory" << FairLogger::endl;
    
       
    // Real R3B Si Tracker data channel

    //ULong_t evsize; 
    ULong_t timestamp; 
    UShort_t energy;  // energy loss in Si
    UInt_t side=0;  // module side
    UInt_t channel_id;  // Channel id
    UInt_t asic_id;  // Chip id
    UInt_t module_id;  // module id
    //UShort_t reserved;


	    // Extract time stamp.
	    // Msb from word_0A bits 0-19.
	    // Lsb from word_0B bits 0-27.
	    timestamp =  (unsigned long long int) (word_0A & 0x000FFFFF) << 28 | (word_1B & 0x0FFFFFFF);
	    
	    // Extract Energy (ADC counts).
	    // From word_2 bits 0-11.
	    energy = (word_0B & 0x00000FFF); 
	    
	    // Extract channel id.
	    // From word_2 bits 12-18. 
	    channel_id = (word_0B & 0x0007F000) >> 12;
	    
	    // Extract ASIC id.
	    // From word_2 bits 19-22.
	    asic_id = (word_0B & 0x00780000) >> 19;
	    
	    // Extract module id.
	    // From word_2 bits 23-28.
	    module_id = (word_0B & 0x1F800000) >> 23;

    
	    //LOG(INFO) << " --------- event " << FairLogger::endl
    LOG(DEBUG) << " --------- event " << FairLogger::endl
    << "        Channel_id " << channel_id << FairLogger::endl
    << "        ASIC_id " << asic_id << FairLogger::endl
    << "        Module_id " << module_id << FairLogger::endl
    << "          energy " << energy << FairLogger::endl
    << "       timestamp " << timestamp << FairLogger::endl
    << "=================================" << FairLogger::endl;
    
    //new ((*fRawData)[fNHits]) R3BStarTrackRawHit(channel_id, asic_id, module_id, energy, timestamp);
    new ((*fRawData)[fNHits]) R3BStarTrackRawHit(module_id, side, asic_id, channel_id, energy, timestamp);
    fNHits++;
  }
  
  
  LOG(DEBUG) << "End of memory" << FairLogger::endl;
  LOG(DEBUG) << "R3BStarTrackUnpack: Number of Si Tracker raw hits: " << fNHits << FairLogger::endl;
  //LOG(INFO) << "R3BStarTrackUnpack: Number of Si Tracker raw hits: " << fNHits << FairLogger::endl;
  
  
  return kTRUE;
}
/**/


//Reset: Public method
void R3BStarTrackUnpack::Reset()
{
  LOG(DEBUG) << "Clearing Data Structure" << FairLogger::endl;
  fRawData->Clear();
  fNHits = 0;
}



ClassImp(R3BStarTrackUnpack)
