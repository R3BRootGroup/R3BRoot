#include "FairLogger.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BTofdReader.h"
#include "R3BPaddleTamexMappedData.h" 

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_tofd.h"
#include EXP_SPECIFIC_H101_FILE
}

#define MAX_TOFD_PLANES   4

R3BTofdReader::R3BTofdReader(EXT_STR_h101* data)
	: R3BReader("R3BTofdReader")
	, fData(data)
	, fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BPaddleTamexMappedData"))
{
}

R3BTofdReader::~R3BTofdReader()
{}

Bool_t R3BTofdReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_TOFD_ITEMS_INFO(ok, *a_struct_info, EXT_STR_h101, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

    // Register output array in tree
    FairRootManager::Instance()->Register("TofdMapped", "Land", fArray, kTRUE);


	// initial clear (set number of hits to 0)
	EXT_STR_h101_onion* data = (EXT_STR_h101_onion*)fData;
	for (int d=0;d<MAX_TOFD_PLANES;d++)
		for (int t=0;t<2;t++) // tube
		{		
			data->TOFD_P[d].T[t].TFLM=0;
			data->TOFD_P[d].T[t].TFTM=0;
		}

	return kTRUE;
}

Bool_t R3BTofdReader::Read()
{
	// Convert plain raw data to multi-dimensional array
    EXT_STR_h101_onion* data = (EXT_STR_h101_onion*)fData;

#if 0

  struct {
    struct {
      uint32_t TFLM;
      uint32_t TFLMI[4 /* TFLM */];
      uint32_t TFLME[4 /* TFLM */];
      uint32_t TFL;
      uint32_t TFLv[40 /* TFL */];
      uint32_t TFTM;
      uint32_t TFTMI[4 /* TFTM */];
      uint32_t TFTME[4 /* TFTM */];
      uint32_t TFT;
      uint32_t TFTv[40 /* TFT */];
      uint32_t TCLM;
      uint32_t TCLMI[4 /* TCLM */];
      uint32_t TCLME[4 /* TCLM */];
      uint32_t TCL;
      uint32_t TCLv[40 /* TCL */];
      uint32_t TCTM;
      uint32_t TCTMI[4 /* TCTM */];
      uint32_t TCTME[4 /* TCTM */];
      uint32_t TCT;
      uint32_t TCTv[40 /* TCT */];
    } T[2];
  } TOFD_P[1];


#endif

// ToDo: Here we need to implement the reconstruction of zero suppressed
// multi hit data.
// Problem: We have no possibility to know which edges belong together
// because times are not calibrated yet. So we cannot reconstruct events
// based on physics data (times).
// In principle, each signal/hit should ALWAYS produce a leading and a
// trailing edge, with a fine and coarse time each. So we use this info
// to reconstruct multi-hit data, but we check for same number of
// multi-hits in leading and trailing edges and skip the event on mismatch


	// loop over all planes
	for (int d=0;d<MAX_TOFD_PLANES;d++)
		for (int t=0;t<2;t++) // tube
		{		
			uint32_t numChannels = data->TOFD_P[d].T[t].TFLM; // not necessarly number of hits! (b/c multi hit)
			
			// check for same number of leading and trailing edges.
			// we trust that we have same number of fine and coarse
			// times because they come encoded together in the same 32bit word.
			if (numChannels != data->TOFD_P[d].T[t].TFTM)
				fLogger->Error(MESSAGE_ORIGIN,"TOFD: Different number of leading and trailing edges (plane=%d,side=%d).", d+1, t+1);
	
			// loop over channels
			uint32_t curChannelStart=0;     // index in v for first item of current channel
			for (int i=0;i<numChannels;i++) 
			{
				uint32_t channel=data->TOFD_P[d].T[t].TFLMI[i]; // or 1..65
				uint32_t nextChannelStart=data->TOFD_P[d].T[t].TFLME[i];  // index in v for first item of next channel
				
				for (int j=curChannelStart;j<nextChannelStart;j++)
					new ((*fArray)[fArray->GetEntriesFast()])
						R3BPaddleTamexMappedData(d+1,    // 1..n
							channel,				     // 1..n (bar)
							t+1,					     // 1 or 2
							data->TOFD_P[d].T[t].TCLv[j],  // coarse time leading edge
							data->TOFD_P[d].T[t].TFLv[j],  // fine time leading edge
							data->TOFD_P[d].T[t].TCTv[j],  // coarse time trailing edge
							data->TOFD_P[d].T[t].TFTv[j],  // fine time trailing edge
							false);
				
				curChannelStart=nextChannelStart;
			}
		}





/* Note: This reads VFTX data and stores it into a Tamex structure.
 * VFTX delivers only one coarse and fine time per channel while
 * Tamex delivers data for two edges.
 */
/* 
	Int_t dummy=0;
	for (int plane=0;plane<1;plane++)
		for (int bar=0;bar<4;bar++)
			for (int pm=0;pm<2;pm++)
			{
				if (data->TOFD_P[plane].B[bar].T[pm].tfl==0) continue; // no time converted
	
				new ((*fArray)[fArray->GetEntriesFast()])
					R3BPaddleTamexMappedItem(plane+1,// 1..n
						bar+1,					     // 1..n
						pm+1,					     // 1 or 2
						data->TOFD_P[plane].B[bar].T[pm].tcl,  // coarse time leading edge
						data->TOFD_P[plane].B[bar].T[pm].tfl,  // fine time leading edge
						data->TOFD_P[plane].B[bar].T[pm].tct,  // coarse time trailing edge
						data->TOFD_P[plane].B[bar].T[pm].tft,  // fine time trailing edge
						false);
	
			}	
*/			
}

void R3BTofdReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BTofdReader)

