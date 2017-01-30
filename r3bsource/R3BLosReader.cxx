#include "FairLogger.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BLosReader.h"
#include "R3BLosMappedData.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_los.h"
}

#define NUM_LOS_DETECTORS 2
#define NUM_LOS_CHANNELS  4
#include <iostream>

using namespace std;

R3BLosReader::R3BLosReader(EXT_STR_h101_LOS* data, UInt_t offset)
	: R3BReader("R3BLosReader")
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BLosMappedData"))
{
}

R3BLosReader::~R3BLosReader()
{}

Bool_t R3BLosReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_LOS_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_LOS, 0);

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
	EXT_STR_h101_LOS_onion* data = (EXT_STR_h101_LOS_onion*)fData;
	for (int d=0;d<NUM_LOS_DETECTORS;d++)
	    data->LOS[d].TFM=0;

	return kTRUE;
}

Bool_t R3BLosReader::Read()
{
	// Convert plain raw data to multi-dimensional array
    EXT_STR_h101_LOS_onion* data = (EXT_STR_h101_LOS_onion*)fData;

/*

  struct {
    uint32_t TFM;
    uint32_t TFMI[5 / * TFM * /];
    uint32_t TFME[5 / * TFM * /];
    uint32_t TF;
    uint32_t TFv[50 / * TF * /];
    uint32_t TCM;
    uint32_t TCMI[5 / * TCM * /];
    uint32_t TCME[5 / * TCM * /];
    uint32_t TC;
    uint32_t TCv[50 / * TC * /];
  } LOS[2];
 
*/

	// loop over all detectors
	for (int d=0;d<NUM_LOS_DETECTORS;d++)
	{		
		uint32_t numChannels = data->LOS[d].TFM; // not necessarly number of hits! (b/c multi hit)
		
		// loop over channels
		uint32_t curChannelStart=0;     // index in v for first item of current channel
		for (int i=0;i<numChannels;i++) 
		{
			uint32_t channel=data->LOS[d].TFMI[i]; // or 1..65
			uint32_t nextChannelStart=data->LOS[d].TFME[i];  // index in v for first item of next channel
			for (int j=curChannelStart;j<nextChannelStart;j++){
				new ((*fArray)[fArray->GetEntriesFast()])
					R3BLosMappedData(
						d+1,
						channel,
						data->LOS[d].TCv[j],
						data->LOS[d].TFv[j]
						); // det,channel,energy
		}
			
			curChannelStart=nextChannelStart;
		}
	}
    return kTRUE;
}

void R3BLosReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BLosReader)

