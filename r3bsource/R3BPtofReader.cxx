#include "FairLogger.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BPtofReader.h"
#include "R3BPaddleTamexMappedData.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_ptof.h"
}

#define MAX_PTOF_PLANES 1

R3BPtofReader::R3BPtofReader(EXT_STR_h101_PTOF* data, UInt_t offset)
	: R3BReader("R3BPtofReader")
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BPaddleTamexMappedData"))
{
}

R3BPtofReader::~R3BPtofReader()
{}

Bool_t R3BPtofReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_PTOF_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_PTOF, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

    // Register output array in tree
    FairRootManager::Instance()->Register("PtofMapped", "Land", fArray, kTRUE);


	// initial clear (set number of hits to 0)
	EXT_STR_h101_PTOF_onion* data = (EXT_STR_h101_PTOF_onion*)fData;
	for (int d=0;d<MAX_PTOF_PLANES;d++)
		for (int t=0;t<2;t++) // tube
		{
			data->PTOF_P[d].T[t].TFLM=0;
			data->PTOF_P[d].T[t].TFTM=0;
		}

	return kTRUE;
}

Bool_t R3BPtofReader::Read()
{
	// Convert plain raw data to multi-dimensional array
    EXT_STR_h101_PTOF_onion* data = (EXT_STR_h101_PTOF_onion*)fData;

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
  } PTOF_P[1];


#endif



	
	for (int d=0;d<MAX_PTOF_PLANES;d++)  // loop over all planes
		for (int t=0;t<2;t++)            // loop over tube 1 and 2
		{
			// ******** leading edges ********
			
			// # of channels with data. not necessarly number of hits! (b/c multi hit)
			uint32_t numChannels = data->PTOF_P[d].T[t].TFLM; 
				
			// loop over channels
			uint32_t curChannelStart=0;     // index in v for first item of current channel
			for (int i=0;i<numChannels;i++) 
			{
				uint32_t channel=data->PTOF_P[d].T[t].TFLMI[i]; // or 1..65
				uint32_t nextChannelStart=data->PTOF_P[d].T[t].TFLME[i];  // index in v for first item of next channel
				
				for (int j=curChannelStart;j<nextChannelStart;j++) 
				{
					
					R3BPaddleTamexMappedData* mapped=NULL;

					// see if we can find a mappedData with matching PM1 leading coarse time
					if (t==1)
					{ 
						int n=fArray->GetEntriesFast();
						int coarse=data->PTOF_P[d].T[t].TCLv[j];
						for (int k=0;k<n;k++)
						{
							R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fArray->At(k);
							// if leading time1 within 100ns window and time2 not yet set
							if ((hit->fCoarseTime2LE==0) && (abs(hit->fCoarseTime1LE - coarse)<=20))
							{
								mapped=hit;
								break;
							}
						}
					}
					
					if (!mapped) mapped=new ((*fArray)[fArray->GetEntriesFast()])
						R3BPaddleTamexMappedData(d+1,    // 1..n   (plane)
							channel);				     // 1..n   (bar)

					if (t==0) // PM1
					{
						mapped->fCoarseTime1LE= data->PTOF_P[d].T[t].TCLv[j];  // coarse time leading edge
						mapped->fFineTime1LE  = data->PTOF_P[d].T[t].TFLv[j];  // fine time leading edge
					}
					else // PM2
					{
						mapped->fCoarseTime2LE= data->PTOF_P[d].T[t].TCLv[j];  // coarse time leading edge
						mapped->fFineTime2LE  = data->PTOF_P[d].T[t].TFLv[j];  // fine time leading edge
					}
				}
				
				curChannelStart=nextChannelStart;
			} // for leading edges
			
			


			// ******** trailing edges ********
			
			// # of channels with data. not necessarly number of hits! (b/c multi hit)
			numChannels = data->PTOF_P[d].T[t].TFTM; 
				
			// loop over channels
			curChannelStart=0;     // index in v for first item of current channel
			for (int i=0;i<numChannels;i++) 
			{
				uint32_t channel=data->PTOF_P[d].T[t].TFTMI[i]; // or 1..65
				uint32_t nextChannelStart=data->PTOF_P[d].T[t].TFTME[i];  // index in v for first item of next channel
				
				for (int j=curChannelStart;j<nextChannelStart;j++) {
					
					R3BPaddleTamexMappedData* mapped=NULL;

					// see if we can find a mappedData with matching PM1 data
					int n=fArray->GetEntriesFast();
					int coarse=data->PTOF_P[d].T[t].TCTv[j];
					// matching a trailing time is only useful if the corresponding
					// leading time is present. Meaning: no need to check trailing1
					// to leading2 and vice-versa. This reduces the combinations we have to 
					// check to:
					// match trailing2 to leading2
					// match trailing1 to leading1
					
					if (t==0) // do the if outside to most inner loop for performance reasons
						 // PM1
						for (int k=0;k<n;k++)
						{
							R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fArray->At(k);
							int tot=coarse - hit->fCoarseTime1LE;
							if ((tot<=1000) && (tot>=0) &&   // time-over-treshold reasonable
								(hit->fCoarseTime1TE==0) &&  // trailing not yet set
								(hit->fCoarseTime1LE!=0))    // leading IS set (else tot is crap)
							{
								mapped=hit;
								break;
							}
						}
					else // PM2
						for (int k=0;k<n;k++)
						{
							R3BPaddleTamexMappedData* hit = (R3BPaddleTamexMappedData*)fArray->At(k);
							// if leading time within 5000ns window and trailing2 not yet set
							int tot=coarse - hit->fCoarseTime2LE;
							if ((tot<=1000) && (tot>=0) &&   // time-over-treshold reasonable
								(hit->fCoarseTime2TE==0) &&  // trailing not yet set
								(hit->fCoarseTime2LE!=0))    // leading IS set
							{
								mapped=hit;
								break;
							}
						}
					
					if (!mapped) mapped=new ((*fArray)[fArray->GetEntriesFast()])
						R3BPaddleTamexMappedData(d+1,    // 1..n   (plane)
							channel);				     // 1..n   (bar)

					if (t==0) // PM1
					{
						mapped->fCoarseTime1TE= data->PTOF_P[d].T[t].TCTv[j];  // coarse time leading edge
						mapped->fFineTime1TE  = data->PTOF_P[d].T[t].TFTv[j];  // fine time leading edge
					}
					else      // PM2
					{
						mapped->fCoarseTime2TE= data->PTOF_P[d].T[t].TCTv[j];  // coarse time leading edge
						mapped->fFineTime2TE  = data->PTOF_P[d].T[t].TFTv[j];  // fine time leading edge
					}
				}
				
				curChannelStart=nextChannelStart;
			} // for trailing edges

		} // for tubes


}

void R3BPtofReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BPtofReader)

