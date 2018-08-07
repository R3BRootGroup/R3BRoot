#include "FairLogger.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BSci8Reader.h"
#include "R3BSci8MappedData.h"
extern "C" {
#include "ext_data_client.h"
#include "ext_h101_sci8.h"
}
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
#define NUM_SCI8_DETECTORS 1
#define NUM_SCI8_CHANNELS  2
#include <iostream>

using namespace std;

R3BSci8Reader::R3BSci8Reader(EXT_STR_h101_SCI8* data, UInt_t offset)
	: R3BReader("R3BSci8Reader")
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
	, fArray(new TClonesArray("R3BSci8MappedData"))
{	
}

R3BSci8Reader::~R3BSci8Reader()
{
}

Bool_t R3BSci8Reader::Init(ext_data_struct_info *a_struct_info)
{
    
	int ok;

	EXT_STR_h101_SCI8_ITEMS_INFO(ok, *a_struct_info, fOffset,
			EXT_STR_h101_SCI8, 0);
			
	
	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
				"Failed to setup structure information.");
		return kFALSE;
	}

	// Register output array in tree
	FairRootManager::Instance()->Register("Sci8Mapped", "Land", fArray, kTRUE);
    fArray->Clear();

	// clear struct_writer's output struct. Seems ucesb doesn't do that
	// for channels that are unknown to the current ucesb config.
	EXT_STR_h101_SCI8_onion* data = (EXT_STR_h101_SCI8_onion*)fData;
	
	for (int d=0;d<NUM_SCI8_DETECTORS;d++)
	{
		data->S8VTFM=0;
		data->S8VTCM=0;
		data->S8TTFLM=0;
		data->S8TTFTM=0;
		data->S8TTCLM=0;
		data->S8TTCTM=0;
	}
	return kTRUE;
}

Bool_t R3BSci8Reader::Read()
{
	// Convert plain raw data to multi-dimensional array
	EXT_STR_h101_SCI8_onion* data = (EXT_STR_h101_SCI8_onion*)fData;

	/*
	 * For variable definition, see structure EXT_STR_h101_SCI8_onion_t 
	 * in ext_str_h101_sci8_tamex.h
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
	
	Bool_t fprint = false;
	
	for (int d=0;d<NUM_SCI8_DETECTORS;d++)
	{		    
     
     Int_t Sum = data->S8VTF+data->S8TTFT+data->S8TTFL;
     //if(data->S8TTFT != data->S8TTFL) fprint = true;
    // if(fNEvents == 9698 || fNEvents == 9701 || fNEvents == 9704) fprint = true;
		// First, we prepare time arrays for VFTX

  // VFTX first:
		uint32_t numChannels = data->S8VTFM; // not necessarly number of hits! (b/c multi hit)				
		// loop over channels
		uint32_t curChannelStart=0;     // index in v for first item of current channel
        Double_t mean_coarse_vftx = 0.;
		int sum_coarse_vftx = 0;
 // First get the average coarse time to shift all coarse counters in the same cycle (by calculateing, in the second step, deviations from the mean value.
 // If the coarse time is smaller than mean value by more than 200, then coarse counter was reseted, and thus, to its value 8192 (in case of VFTX) will be added.		
		for (int i=0;i<numChannels;i++) 
		{
			uint32_t channel = data->S8VTFMI[i]; // = 1..8
			uint32_t nextChannelStart = data->S8VTFME[i];  // index in v for first item of next channel
				
			for (int j = curChannelStart; j < nextChannelStart; j++){		
				
			int coarse_vftx = data->S8VTCv[j];
			
            mean_coarse_vftx = mean_coarse_vftx + coarse_vftx;
            sum_coarse_vftx = sum_coarse_vftx + 1;	
						
			}
			curChannelStart = nextChannelStart;
		}
            mean_coarse_vftx = mean_coarse_vftx / float(sum_coarse_vftx);


        curChannelStart=0; 
		for (int i=0;i<numChannels;i++) // VFTX, now do the mapping
		{
			uint32_t channel = data->S8VTFMI[i]; // = 1..8
			uint32_t nextChannelStart = data->S8VTFME[i];  // index in v for first item of next channel
				
			for (int j = curChannelStart; j < nextChannelStart; j++){		
				
				int coarse_vftx = data->S8VTCv[j];
			    if((mean_coarse_vftx - float(coarse_vftx)) > 200.) coarse_vftx = coarse_vftx + 8192;
			
				if(fprint) cout<<"SCI8 READER VFTX: "<<fNEvents<<", "<<Sum<<", "<<channel<<", "<<data->S8VTFv[j]<<", "<<data->S8VTCv[j]
				               <<", "<<coarse_vftx<<", "<<mean_coarse_vftx<<endl;					
				
				new ((*fArray)[fArray->GetEntriesFast()])				
				R3BSci8MappedData(
						d+1,                  // detector number
						channel,              // channel number: 1-8
						0,                    // VFTX (0),TAMEX leading (1), TAMEX trailing (2)
						data->S8VTFv[j], // VFTX fine time
						coarse_vftx  // VFTX coarse time
						);
						
			}
			curChannelStart = nextChannelStart;
		}

// Next, TAMEX leading; first, first get the average coarse time to shift all coarse counters in the same cycle (the same as for VFTX, only here on adds 2048).     
//   if(data->S8TTFT == data->S8TTFL && data->S8TTCT == data->S8TTCL && data->S8TTFL == data->S8TTCL)
    if( 1 == 1)
	{
		numChannels = data->S8TTFLM;
		curChannelStart=0;
		Double_t mean_coarse_leading = 0.;
		int sum_coarse_leading = 0;
		for (int i=0;i<numChannels;i++)
		{
			uint32_t channel = data->S8TTFLMI[i];
			uint32_t nextChannelStart = data->S8TTFLME[i];
            
			for (int j = curChannelStart; j < nextChannelStart; j++){
				
			int coarse_leading = data->S8TTCLv[j];
			
            mean_coarse_leading = mean_coarse_leading + coarse_leading;
            sum_coarse_leading = sum_coarse_leading + 1;
            		
			}
			 
			curChannelStart = nextChannelStart;
		}  

		
		mean_coarse_leading = mean_coarse_leading / float(sum_coarse_leading);
		
		
				// Next, TAMEX leading into mapped array     
		curChannelStart=0;
		Double_t mean_coarse_lead = 0.;
		int sum_coarse_lead = 0;		
		for (int i=0;i<numChannels;i++)
		{
			uint32_t channel = data->S8TTFLMI[i];
			uint32_t nextChannelStart = data->S8TTFLME[i];
            
			for (int j = curChannelStart; j < nextChannelStart; j++){
				
			int coarse_leading = data->S8TTCLv[j];
			if((mean_coarse_leading - float(coarse_leading)) > 200.) coarse_leading = coarse_leading + 2048;
		// We now calculate again meanv alue of the "shifted" coarse leading times; this will be needed at the next step in order to
		// shift coarse trailing times in the same clock cycle as coarse leading	
            mean_coarse_lead = mean_coarse_lead + coarse_leading;
            sum_coarse_lead = sum_coarse_lead + 1;
                        	
			if(fprint) cout<<"SCI8 READER leading edges: "<<fNEvents<<", "<<Sum<<", "<<channel<<", "<<data->S8TTFLv[j]<<", "<<data->S8TTCLv[j]<<
			                 ", "<<coarse_leading<<"; "<<mean_coarse_leading<<endl;
				
				new ((*fArray)[fArray->GetEntriesFast()])				
				R3BSci8MappedData(
						d+1,
						channel,
						1,
						data->S8TTFLv[j],
						coarse_leading
						);
		
			}
			 
			curChannelStart = nextChannelStart;
		}  
		mean_coarse_lead = mean_coarse_lead / float(sum_coarse_lead);

		// At last, TAMEX trailing: for each mapped leading edge, look for correspondiing trailing edge
	
		numChannels = data->S8TTFTM;
				 
		curChannelStart=0;
		
		for (int i=0;i<data->S8TTFTM;i++) 
		{
			
			uint32_t channel = data->S8TTFTMI[i];
			uint32_t nextChannelStart = data->S8TTFTME[i];
		
			for (int j = curChannelStart; j < nextChannelStart; j++){
			
			int coarse=data->S8TTCTv[j];
			
			if(coarse <= 25 && mean_coarse_lead >= 2023) coarse = coarse + 2048;		
			 	
			if(fprint) cout<<"SCI8 Reader trailing before sorting: "<<fNEvents<<", "<<Sum<<", "<<channel<<", "<<data->S8TTFTv[j]<<"; "<<
			                 coarse<<", "<<mean_coarse_lead <<endl;	
						 
			 R3BSci8MappedData* mapped=NULL;
			 		 
			 Int_t n=fArray->GetEntriesFast();
			 for (int k=0;k<n;k++)
			 {
		  // Get the leading coarse time from mapped array:	
			 R3BSci8MappedData* hit = (R3BSci8MappedData*)fArray->At(k);
			
			 UInt_t iTypeL = hit->GetType();
			 UInt_t iCha  = hit->GetChannel();
			 
			 
			 if(iTypeL == 1) //(iCha == channel)
			 {
			 int coarse_leading=0;
			 if(iTypeL == 1) coarse_leading = hit->fTimeCoarse;	
			 
			 int tot=coarse - coarse_leading;
			 
			  			
			 if ((tot<=25) && (tot>=0))    // there is a corresponding leading edge
			  {
				
				new ((*fArray)[fArray->GetEntriesFast()])				
				R3BSci8MappedData(
						d+1,
						channel,
						2,
						data->S8TTFTv[j],
						coarse 
						);
						
						if(fprint) cout<<"SCI8 Reader trailing edges: "<<fNEvents<<", "<<Sum<<", "<<channel<<", "<<data->S8TTFTv[j]<<"; "<<
						                  coarse<<", "<<tot<<endl;
								
					break;
			  }
		  }
		     }
							
			}
			curChannelStart = nextChannelStart;
		}     
	}
	else
	{
		
	}

    }
	fNEvents += 1;
	     
	return kTRUE;
}

void R3BSci8Reader::FinishTask()
{        	
}

void R3BSci8Reader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BSci8Reader)

