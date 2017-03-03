#include "FairLogger.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BTofdReader.h"
#include "R3BPaddleTamexMappedData.h" 

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_tofd.h"
}

#define MAX_TOFD_PLANES   6

R3BTofdReader::R3BTofdReader(EXT_STR_h101_TOFD* data, UInt_t offset)
	: R3BReader("R3BTofdReader")
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BPaddleTamexMappedData"))
{
}

R3BTofdReader::~R3BTofdReader()
{}

Bool_t R3BTofdReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_TOFD_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_TOFD, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

    // Register output array in tree
    FairRootManager::Instance()->Register("TofdMapped", "Land", fArray, kTRUE);


	// initial clear (set number of hits to 0)
	EXT_STR_h101_TOFD_onion* data = (EXT_STR_h101_TOFD_onion*)fData;
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
    EXT_STR_h101_TOFD_onion* data = (EXT_STR_h101_TOFD_onion*)fData;

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

/* 
 * We actually CAN do the full reconstruction of a paddle (PMs +
 * leading + trailing edges) here in the reader since we can rely on the
 * coarse time for the reconstruction. Sudden time offsets due to resetting
 * of clock counters should not occur within the data of one paddle.
 * 
 * So we would do:
 * 
 * for (planes)
 *   for (tube)
 *     for (leading times)
 * 		 add to existing entry or else create a new one
 *     for (trailing times)
 * 		 add to existing entry or create a new one
 * 
 */


	
	for (int d=0;d<MAX_TOFD_PLANES;d++)  // loop over all planes
		for (int t=0;t<2;t++)            // loop over tube 1 and 2
		{
			// ******** leading edges ********
			
			// # of channels with data. not necessarly number of hits! (b/c multi hit)
			uint32_t numChannels = data->TOFD_P[d].T[t].TFLM; 
				
			// loop over channels
			uint32_t curChannelStart=0;     // index in v for first item of current channel
			for (int i=0;i<numChannels;i++) 
			{
				uint32_t channel=data->TOFD_P[d].T[t].TFLMI[i]; // or 1..65
				uint32_t nextChannelStart=data->TOFD_P[d].T[t].TFLME[i];  // index in v for first item of next channel
				
				for (int j=curChannelStart;j<nextChannelStart;j++) 
				{
					
					R3BPaddleTamexMappedData* mapped=NULL;

					// see if we can find a mappedData with matching PM1 leading coarse time
					if (t==1)
					{ 
						int n=fArray->GetEntriesFast();
						int coarse=data->TOFD_P[d].T[t].TCLv[j];
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
						mapped->fCoarseTime1LE= data->TOFD_P[d].T[t].TCLv[j];  // coarse time leading edge
						mapped->fFineTime1LE  = data->TOFD_P[d].T[t].TFLv[j];  // fine time leading edge
					}
					else // PM2
					{
						mapped->fCoarseTime2LE= data->TOFD_P[d].T[t].TCLv[j];  // coarse time leading edge
						mapped->fFineTime2LE  = data->TOFD_P[d].T[t].TFLv[j];  // fine time leading edge
					}
				}
				
				curChannelStart=nextChannelStart;
			} // for leading edges
			
			


			// ******** trailing edges ********
			
			// # of channels with data. not necessarly number of hits! (b/c multi hit)
			numChannels = data->TOFD_P[d].T[t].TFTM; 
				
			// loop over channels
			curChannelStart=0;     // index in v for first item of current channel
			for (int i=0;i<numChannels;i++) 
			{
				uint32_t channel=data->TOFD_P[d].T[t].TFTMI[i]; // or 1..65
				uint32_t nextChannelStart=data->TOFD_P[d].T[t].TFTME[i];  // index in v for first item of next channel
				
				for (int j=curChannelStart;j<nextChannelStart;j++) {
					
					R3BPaddleTamexMappedData* mapped=NULL;

					// see if we can find a mappedData with matching PM1 data
					int n=fArray->GetEntriesFast();
					int coarse=data->TOFD_P[d].T[t].TCTv[j];
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
						mapped->fCoarseTime1TE= data->TOFD_P[d].T[t].TCTv[j];  // coarse time leading edge
						mapped->fFineTime1TE  = data->TOFD_P[d].T[t].TFTv[j];  // fine time leading edge
					}
					else      // PM2
					{
						mapped->fCoarseTime2TE= data->TOFD_P[d].T[t].TCTv[j];  // coarse time leading edge
						mapped->fFineTime2TE  = data->TOFD_P[d].T[t].TFTv[j];  // fine time leading edge
					}
				}
				
				curChannelStart=nextChannelStart;
			} // for trailing edges

		} // for tubes


#ifdef OLDTOFDREADER
// Beginning of old stuff:

	// HTT: edges book keeping.
	int edges[MAX_TOFD_PLANES][6][2][2];
	memset(edges, 0, sizeof edges);

	// loop over all planes
	for (int d=0;d<MAX_TOFD_PLANES;d++)
		for (int t=0;t<2;t++) // tube
		{		
			uint32_t numChannels = data->TOFD_P[d].T[t].TFLM; // not necessarly number of hits! (b/c multi hit)
			
			// check for same number of leading and trailing edges.
			// we will have a mismatch if the signal crosses the border
			// of the gate.
			// we trust that we have same number of fine and coarse
			// times because they come encoded together in the same 32bit word.
            if (numChannels != data->TOFD_P[d].T[t].TFTM)
            {
//MH: we have to find out how we deal with theses events. For now, I comment out the error message
//                 fLogger->Error(MESSAGE_ORIGIN,"TOFD: Different number of channels for leading and trailing edge data (plane=%d,side=%d).", d+1, t+1);
               // continue; // all following hits are shifted and bad
               return false;
            }
	
			// loop over channels
			uint32_t curChannelStart=0;     // index in v for first item of current channel
			for (int i=0;i<numChannels;i++) 
			{
				uint32_t channel=data->TOFD_P[d].T[t].TFLMI[i]; // or 1..65
				if (channel!=data->TOFD_P[d].T[t].TFTMI[i])
				{
					// different channel number in leading and trailing 
					//printf("Leading Ch %d Trailing Ch %d\n",channel,data->TOFD_P[d].T[t].TFTMI[i]);
					return false;
				}
				  
				uint32_t nextChannelStart=data->TOFD_P[d].T[t].TFLME[i];  // index in v for first item of next channel
				if (nextChannelStart!=data->TOFD_P[d].T[t].TFTME[i])
				{
					// different number of (multi-)hits for leading and trailing
					// in the current channel
					
					//printf("Different number of leading and trailing hits for P:%d B:%d S: %d\n\
next Lead Ch %d next Trail Ch %d\n",d+1,channel,t+1,nextChannelStart,data->TOFD_P[d].T[t].TFTME[i]);
					return false;
				}

				
				for (int j=curChannelStart;j<nextChannelStart;j++) {
					
/*					
					// RPL: DEBUG energies outside expected channel range
					if (data->TOFD_P[d].T[t].TFTv[j]>800)
					{
					  printf("P%d B:%d E=%u 0x%x i=%d\n",d,channel,data->TOFD_P[d].T[t].TFTv[j],data->TOFD_P[d].T[t].TFTv[j],j);
					  printf("Num Channels: %d\n", data->TOFD_P[d].T[t].TFTM );
					  for (int k=0;k<data->TOFD_P[d].T[t].TFTM;k++)
						  printf("Channel: %d first-of-next: %d\n",data->TOFD_P[d].T[t].TFTMI[k],data->TOFD_P[d].T[t].TFTME[k]);
					  for (int k=0;k<nextChannelStart;k++)	
						  printf("%d: E=%u\n",k,data->TOFD_P[d].T[t].TFTv[k]);
					}
*/					
					new ((*fArray)[fArray->GetEntriesFast()])
						R3BPaddleTamexMappedData(d+1,    // 1..n   (plane)
							channel,				     // 1..n   (bar)
							t+1,					     // 1 or 2 (tube)
							data->TOFD_P[d].T[t].TCLv[j],  // coarse time leading edge
							data->TOFD_P[d].T[t].TFLv[j],  // fine time leading edge
							data->TOFD_P[d].T[t].TCTv[j],  // coarse time trailing edge
							data->TOFD_P[d].T[t].TFTv[j],  // fine time trailing edge
							false);
				}
				
				curChannelStart=nextChannelStart;
			}

			/*
			// HTT: Fill edge book.
			int ofs = 0;
			for (int i=0;i<data->TOFD_P[d].T[t].TFLM;i++) {
				uint32_t channel = data->TOFD_P[d].T[t].TFLMI[i];
				int next = data->TOFD_P[d].T[t].TFLME[i];
				if (0 <= channel && 6 > channel) {
					edges[d][channel][t][0] += next - ofs;
				}
				ofs = next;
			}
			ofs = 0;
			for (int i=0;i<data->TOFD_P[d].T[t].TFTM;i++) {
				uint32_t channel = data->TOFD_P[d].T[t].TFTMI[i];
				int next = data->TOFD_P[d].T[t].TFTME[i];
				if (0 <= channel && 6 > channel) {
					edges[d][channel][t][1] += next - ofs;
				}
				ofs = next;
			}
			*/
		}

/*
	// HTT: Print mismatches.
	for (int p = 0; MAX_TOFD_PLANES > p; ++p) {
		for (int b = 0; 6 > b; ++b) {
			if (edges[p][b][0][0] != edges[p][b][0][1] ||
			    edges[p][b][0][0] != edges[p][b][1][0] ||
			    edges[p][b][0][0] != edges[p][b][1][1]) {
				fLogger->Error(MESSAGE_ORIGIN, "TOFD: Plane=%d Bar=%d: Edge mismatch ([t=1,e=l]=%d,1t=%d,2l=%d,2r=%d).",
				    p + 1, b + 1, edges[p][b][0][0], edges[p][b][0][1], edges[p][b][1][0], edges[p][b][1][1]);
			}
		}
	}
*/




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
#endif
    return kTRUE;
}

void R3BTofdReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BTofdReader)

