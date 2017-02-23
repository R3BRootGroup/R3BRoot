#include "FairLogger.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_psp.h"
}

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BPspReader.h"
#include "R3BPspxMappedData.h"


R3BPspReader::R3BPspReader(EXT_STR_h101_PSP* data, UInt_t offset)
	: R3BReader("R3BPspReader")
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BPspxMappedData"))
{
}

R3BPspReader::~R3BPspReader()
{}

Bool_t R3BPspReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_PSP_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_PSP, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

    // Register output array in tree
    FairRootManager::Instance()->Register("PspxMapped", "Land", fArray, kTRUE);

	return kTRUE;
}

Bool_t R3BPspReader::Read()
{
	// Convert plain raw data to multi-dimensional array
    EXT_STR_h101_onion* data = (EXT_STR_h101_onion*)fData;

	// Display data
	// fLogger->Info(MESSAGE_ORIGIN, "  Event data:");
	
	
/* 
  // this is the data structure we have to read:
  struct {
    uint32_t M;                 // number of channels with data
    uint32_t MI[65 / * M * /];  // channel number
    uint32_t ME[65 / * M * /];  // offset in v array for that channel
    uint32_t _;                 // num items in v
    uint32_t v[650 / * _ * /];  // the energy data
  } PSPX[5];

* More info on the data format at:
* https://forum.gsi.de/index.php?t=msg&th=4798&start=0&
* 
* Each detector has 65 channels:
* 16 * 2 channels for vertical strips
* 16 * 2 channels for horizontal strips
* 1 channel for the cathode
* 
* However, one detector has either vertical OR horizontal strips, hence
* 32 channels are always empty.
* 
* Channel numbers: 
* 1) v1_bottom
* 2) v1_top
* 3) v2_bottom
* 4) v2_top
* ...
* 33) h1_right
* 34) h1_left
* 35) h2_right
* 36) h2_left
* 
* 65) cathode
* 
*/

	
	// loop over all detectors
	for (int d=0;d<5;d++)
	{
		if (d==2) continue; // skip PSP3 (via MADC32)
		
		uint32_t numChannels = data->PSPX[d].M; // not necessarly number of hits! (b/c multi hit)
		
		// loop over channels
		uint32_t curChannelStart=0;     // index in v for first item of current channel
		for (int i=0;i<numChannels;i++) 
		{
			uint32_t channel=data->PSPX[d].MI[i]; // or 1..65
			uint32_t nextChannelStart=data->PSPX[d].ME[i];  // index in v for first item of next channel
			
			// if we had multi hit data, we would need to read
			// j=curChannelStart; j < nextChannelStart; j++.
			// For the PSPs, however, we take the first hit only:

			new ((*fArray)[fArray->GetEntriesFast()])
				R3BPspxMappedData(d,channel,data->PSPX[d].v[curChannelStart]); // det,channel,energy
			
			curChannelStart=nextChannelStart;
		}
	}

    return kTRUE;
}

void R3BPspReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BPspReader)

