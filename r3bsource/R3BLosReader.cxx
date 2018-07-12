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
#define NUM_LOS_CHANNELS  8
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
	 * VTFME = Array of TFM size containing the index of the first element of the next channel in data array TFv;
	 *         TFME[i]-FTME[i-1] = number of data for channel i;
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

		// First, we prepare time arrays for VFTX

		uint32_t numChannels = data->LOS[d].VTFM; // not necessarly number of hits! (b/c multi hit)
				
	//	if(data->LOS[d].VTF > 8) cout<<"LOS VFTX multihit: "<<data->LOS[d].VTF <<endl;
	//	if(data->LOS[d].TTFL != data->LOS[d].TTFT) cout<<"LOS Leading fine!= coarse: "<<data->LOS[d].TTFL<<", "<< data->LOS[d].TTCL<<endl;
	//	if(data->LOS[d].TTCT != data->LOS[d].TTCT) cout<<"LOS Trailing fine != coarse: "<<data->LOS[d].TTFT<<", "<< data->LOS[d].TTCT<<endl;
	
 //if(data->LOS[d].VTF == 8 && data->LOS[d].TTFL == 8 && data->LOS[d].VTF == 8){
		//cout<<"LOS Reader, numChannels: "<<numChannels<<", "<<numChannelsL<<", "<<numChannelsT<<endl;

		// loop over channels
		uint32_t curChannelStart=0;     // index in v for first item of current channel
		for (int i=0;i<numChannels;i++)
		{
			uint32_t channel = data->LOS[d].VTFMI[i]; // = 1..8
			uint32_t nextChannelStart = data->LOS[d].VTFME[i];  // index in v for first item of next channel
				
//  if(fNEvents == 1968 || fNEvents == 2138 || fNEvents == 13773) cout<<"LOSReader 1: "<<data->LOS[d].VTF<<", "<<data->LOS[d].VTFMI[i]<<", "<<data->LOS[d].VTFME[i]<<", "<<curChannelStart<<", "<<data->LOS[d].VTC<<endl;			

			for (int j = curChannelStart; j < nextChannelStart; j++){							
				new ((*fArray)[fArray->GetEntriesFast()])				
				R3BLosMappedData(
						d+1,                  // detector number
						channel,              // channel number: 1-8
						0,                    // VFTX (0),TAMEX leading (1), TAMEX trailing (2)
						data->LOS[d].VTFv[j], // VFTX fine time
						data->LOS[d].VTCv[j]  // VFTX coarse time
						);
						
		//	     if(fNEvents == 1968 || fNEvents == 2138 || fNEvents == 13773) 
		//	     cout<<"Reader VFTX "<<channel<<", "<<data->LOS[d].VTFv[j]<<", "<<data->LOS[d].VTCv[j] <<endl;
			}
			curChannelStart = nextChannelStart;
		}

		// Next, TAMEX leading      
		numChannels = data->LOS[d].TTFLM;
		curChannelStart=0;
		for (int i=0;i<numChannels;i++)
		{
			uint32_t channel = data->LOS[d].TTFLMI[i];
			uint32_t nextChannelStart = data->LOS[d].TTFLME[i];
			
						
//	if(fNEvents == 1968 || fNEvents == 2138 || fNEvents == 13773) cout<<"LOSReader 2: "<<data->LOS[d].TTFL<<", "<<data->LOS[d].TTFLMI[i]<<", "<<data->LOS[d].TTFLME[i]<<", "<<curChannelStart<<", "<<data->LOS[d].TTCL<<endl;
			

			for (int j = curChannelStart; j < nextChannelStart; j++){
				new ((*fArray)[fArray->GetEntriesFast()])				
				R3BLosMappedData(
						d+1,
						channel,
						1,
						data->LOS[d].TTFLv[j],
						data->LOS[d].TTCLv[j]
						);
		
			//    if(fNEvents == 1968 || fNEvents == 2138 || fNEvents == 13773) 
			//    cout<<"Reader Leading "<<channel<<", "<<data->LOS[d].TTFLv[j]<<", "<<data->LOS[d].TTCLv[j] <<endl;
			}
			 
			curChannelStart = nextChannelStart;
		}  

		// At last, TAMEX trailing

		numChannels = data->LOS[d].TTFTM;
		curChannelStart=0;
		for (int i=0;i<numChannels;i++)
		{
			uint32_t channel = data->LOS[d].TTFTMI[i];
			uint32_t nextChannelStart = data->LOS[d].TTFTME[i];
			
			
 //if(fNEvents == 1968 || fNEvents == 2138 || fNEvents == 13773) cout<<"LOSReader 3: "<<data->LOS[d].TTFT<<", "<<data->LOS[d].TTFTMI[i]<<", "<<data->LOS[d].TTFTME[i]<<", "<<curChannelStart<<", "<<data->LOS[d].TTCT<<endl;
		
			for (int j = curChannelStart; j < nextChannelStart; j++){
				new ((*fArray)[fArray->GetEntriesFast()])				
				R3BLosMappedData(
						d+1,
						channel,
						2,
						data->LOS[d].TTFTv[j],
						data->LOS[d].TTCTv[j]
						);
			
		//	   if(fNEvents == 1968 || fNEvents == 2138 || fNEvents == 13773) 
		//	   cout<<"Reader Trailing "<<channel<<", "<<	data->LOS[d].TTFTv[j]<<", "<<data->LOS[d].TTCTv[j] <<endl;
			
		   }
			curChannelStart = nextChannelStart;
		}
		
	
//	}	 
	}     
	
	fNEvents += 1;
	     
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

