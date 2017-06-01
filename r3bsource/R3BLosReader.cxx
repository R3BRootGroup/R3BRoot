#include "FairLogger.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BLosReader.h"
#include "R3BLosMappedData.h"
extern "C" {
#include "ext_data_client.h"
#include "ext_h101_los_tamex.h"
}
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
#define NUM_LOS_DETECTORS 1
#define NUM_LOS_CHANNELS  16
#include <iostream>

using namespace std;

R3BLosReader::R3BLosReader(EXT_STR_h101_LOS_TAMEX* data, UInt_t offset)
	: R3BReader("R3BLosReader")
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BLosMappedData"))
{	
}

R3BLosReader::~R3BLosReader()
{
}

Bool_t R3BLosReader::Init(ext_data_struct_info *a_struct_info)
{
	
	for(UInt_t i = 0; i < 16; i++)
    {
     for(UInt_t k = 0; k < 3; k++)
     {
		Icount[i][k] = 0; 
	 }
	} 
	
	Ievents = 0;
	
	int ok;

	EXT_STR_h101_LOS_TAMEX_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_LOS_TAMEX, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

    // Register output array in tree
    FairRootManager::Instance()->Register("LosMapped", "Land", fArray, kTRUE);

	// clear struct_writer's output struct. Seems ucesb doesn't do that
	// for channels that are unknown to the current ucesb config.
	EXT_STR_h101_LOS_TAMEX_onion* data = (EXT_STR_h101_LOS_TAMEX_onion*)fData;
	for (int d=0;d<NUM_LOS_DETECTORS;d++)
	{
	    data->LOS[d].VTFM=0;
	    data->LOS[d].VTCM=0;
	    data->LOS[d].TTFLM=0;
	    data->LOS[d].TTFTM=0;
	    data->LOS[d].TTCLM=0;
	    data->LOS[d].TTCTM=0;
    }
	return kTRUE;
}

Bool_t R3BLosReader::Read()
{
	// Convert plain raw data to multi-dimensional array
    EXT_STR_h101_LOS_TAMEX_onion* data = (EXT_STR_h101_LOS_TAMEX_onion*)fData;
    
/*
 * For variable definition, see structure EXT_STR_h101_LOS_TAMEX_onion_t 
 * in ext_str_h101_los_tamex.h
 *** VFTX DATA ***
 * VTF = Size of the array TFv contaning fine VFTX times
 * VTFv = Array containing the actual data on the fine  times
 * VTFM = No of channels having time
 * VTFMI = Array of TFM size containing the channel numbers of each channel with data
 * VTFME = Array of TFM size containing the index of the first element of the next channel in data array TFv
 *
 * VTC = Size of the array TCv contaning coarse VFTX times
 * VTCv = Array containing the actual data on the coarse times
 * VTCM = No of channels having coarse time
 * VTCMI = Array of TCM size containing the channel numbers of each channel with data
 * VTCME = Array of TCM size containing the index of the first element of the next channel in data array TCv 
 * 
 *** TAMEX DATA *** Added by Aleksandra, Oct. 2016
 * TTFL = Size of the array TFLv contaning fine leading times
 * TTFLv = Array containing the actual data on the fine leading times
 * TTFLM = No of channels having fine leading time
 * TTFLMI = Array of TFLM size containing the channel numbers of each channel with data
 * TTFLME = Array of TFLM size containing the index of the first element of the next channel in data array TFLv
 *
 * TTCL = Size of the array TCLv contaning coarse leading times
 * TTCLv = Array containing the actual data on the coarse leading times
 * TTCLM = No of channels having coarse leading time
 * TTCLMI = Array of TCLM size containing the channel numbers of each channel with data
 * TTCLME = Array of TCLM size containing the index of the first element of the next channel in data array TCLv
 *  
 * The same logic is for trailing times: TTFT, TTFTv, ..., TTCTMI, TTCTME
*/

      
	// loop over all detectors
	for (int d=0;d<NUM_LOS_DETECTORS;d++)
	{		    
		uint32_t numChannels  = data->LOS[d].VTFM; // not necessarly number of hits! (b/c multi hit)
		uint32_t numChannelsL = data->LOS[d].TTFLM;
        uint32_t numChannelsT = data->LOS[d].TTFTM;
        
	
            
		// loop over channels
	  uint32_t curChannelStart=0;     // index in v for first item of current channel

		for (int i=0;i<numChannels;i++)  // i=0,1,2,3...NUM_LOS_CHANNELS-1
		{

			uint32_t channel=data->LOS[d].VTFMI[i]; // or 1..65
			uint32_t channelT=data->LOS[d].TTFLMI[i]; // or 1..65
			
			
			uint32_t nextChannelStart=data->LOS[d].VTFME[i];  // index in v for first item of next channel

  			for (int j=curChannelStart;j<nextChannelStart;j++){
                           
             uint32_t fine_times[3]={data->LOS[d].VTFv[j],data->LOS[d].TTFLv[j],data->LOS[d].TTFTv[j]}; // Fine time: VFTX, TAMEX leading, TAMEX trailing  
             uint32_t coarse_times[3]={data->LOS[d].VTCv[j],data->LOS[d].TTCLv[j],data->LOS[d].TTCTv[j]}; // Coarse time: VFTX, TAMEX leading, TAMEX trailing  
     
  //   cout<<"Reader Time input "<<channel<<", "<<channelT<<", "<<data->LOS[d].VTFv[j]<<" "<<data->LOS[d].TTFLv[j]<<" "<<data->LOS[d].TTFTv[j]<<endl;
  //  cout<<"Reader Time arrays "<<fine_times[0]<<" "<<fine_times[1]<<" "<<fine_times[2]<<endl;
     
// cout<<"R3BReader: det. "<<d+1<<", chann. "<<channel<<", "<< fine_times[0]<<", "<<fine_times[1]<<", "<<fine_times[2]<<endl;          
             Int_t itstart = 0;
             Int_t itstop = 3;
             for (Int_t itype=itstart ; itype<itstop; itype++){	                
								
				new ((*fArray)[fArray->GetEntriesFast()])				
					R3BLosMappedData(
						d+1,
						channel,   // 1-16
						itype,
						fine_times[itype],
						coarse_times[itype]								
						); // det,channel,type,fine_time,coarse_time; type = 0:VFTX, 1:TAMEX leading, 2: Tamex trailing
				
		Ievents++;

//	if(channel == 15)	cout<<"R3BReader fMapped array: "<<d<<", "<<channel<<", "<<itype<<", "<<fine_times[itype]<<", "<<coarse_times[itype]<<endl;
/*
	for(UInt_t ii = 0; ii < 8; ii++)
    {
     for(UInt_t ik = 0; ik < 3; ik++)
     {
		if(ii == i && ik == itype) Icount[i][itype]++;
	 }
	} 	
*/	
}						
																		   		
		}
					
			curChannelStart=nextChannelStart;
		}
		
		   
	}
/*   
   for(UInt_t ii = 0; ii < 8; ii++)
    {
     for(UInt_t ik = 0; ik < 3; ik++)
     {
		cout<<"R3BLosReader::FinishTask  Channel: "<<ii<<", Type: "<<ik<<", Count: "<<Icount[ii][ik]<<endl; 
	 }
	} 	
*/	
//	cout<<"LOS Reader, total number of events: "<<Ievents<<endl;
    return kTRUE;
}
void R3BLosReader::FinishTask()
{
 	
}
void R3BLosReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BLosReader)

