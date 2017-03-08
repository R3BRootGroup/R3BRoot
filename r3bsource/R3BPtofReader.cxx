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
#define N_TUBES_PER_PADDLE 2

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
	FairRootManager::Instance()->Register("PtofMapped", "Land", fArray,
	    kTRUE);


	// initial clear (set number of hits to 0)
	EXT_STR_h101_PTOF_onion* data = (EXT_STR_h101_PTOF_onion*)fData;
	for (int d = 0; d < MAX_PTOF_PLANES; d++) {
		for (int t = 0; t < N_TUBES_PER_PADDLE; t++) {
			data->PTOF_P[d].T[t].TFLM = 0;
			data->PTOF_P[d].T[t].TFTM = 0;
		}
	}

	return kTRUE;
}

Bool_t R3BPtofReader::ReadLeadingEdges(EXT_STR_h101_PTOF_onion *data,
    int d, int t)
{
	// # of channels with data. not necessarly number
	// of hits! (b/c multi hit)
	uint32_t numChannels = data->PTOF_P[d].T[t].TFLM;

	// loop over channels
	// index in v for first item of current channel
	uint32_t curChannelStart = 0;
	for (int i = 0; i < numChannels; i++)
	{
		// bar number 1..65
		uint32_t bar;
		// index in v for first item of next channel
		uint32_t nextChannelStart;

		bar = data->PTOF_P[d].T[t].TFLMI[i];
		nextChannelStart = data->PTOF_P[d].T[t].TFLME[i];

		for (int j = curChannelStart; j < nextChannelStart; j++)
		{
			ReadLeadingEdgeChannel(data, d, t, bar, j);
		}

		curChannelStart = nextChannelStart;
	}
	return kTRUE;
}

Bool_t R3BPtofReader::ReadTrailingEdges(EXT_STR_h101_PTOF_onion *data,
    int d, int t)
{
	// # of channels with data. not necessarly number
	// of hits! (b/c multi hit)
	uint32_t numChannels = data->PTOF_P[d].T[t].TFTM;

	// loop over channels
	// index in v for first item of current channel
	uint32_t curChannelStart = 0;
	for (int i = 0; i < numChannels; i++)
	{
		// or 1..65
		uint32_t bar;
		// index in v for first item of next channel
		uint32_t nextChannelStart;
		nextChannelStart = data->PTOF_P[d].T[t].TFTME[i];
		bar = data->PTOF_P[d].T[t].TFTMI[i];

		for (int j = curChannelStart; j < nextChannelStart; j++)
		{
			ReadTrailingEdgeChannel(data, d, t, bar, j);
		}

		curChannelStart=nextChannelStart;
	}
	return kTRUE;
}

#define MAX_TIME_DIFF_PADDLE_PMT 20 /* 20 * 5 ns = 100 ns */
Bool_t R3BPtofReader::ReadLeadingEdgeChannel(EXT_STR_h101_PTOF_onion *data,
    int d, int t, uint32_t bar, int ch)
{
	R3BPaddleTamexMappedData* mapped = NULL;

	/*
	 * see if we can find a mappedData with
	 * matching PM1 leading coarse time
	 */
	if (t == 1)
	{
		int n = fArray->GetEntriesFast();
		int coarse = data->PTOF_P[d].T[t].TCLv[ch];

		for (int k = 0; k < n; k++)
		{
			R3BPaddleTamexMappedData* hit =
			    (R3BPaddleTamexMappedData*)fArray->At(k);

			/*
			 * if leading time1 within 100ns window
			 * and time2 not yet set
			 */
			if ((hit->fCoarseTime2LE == 0)
			    && (abs(hit->fCoarseTime1LE - coarse)
				    <= MAX_TIME_DIFF_PADDLE_PMT))
			{
				mapped = hit;
				break;
			}
		}
	}

	/*
	 * Add a new hit to the array, if t == 0 or
	 * No corresponding hit in PM1 found
	 */
	if (!mapped) mapped = new ((*fArray)[fArray->GetEntriesFast()])
		R3BPaddleTamexMappedData(d + 1, bar); // plane, bar

	/* Fill leading edge time members */
	if (t == 0)
	{
		// PM1
		mapped->fCoarseTime1LE = data->PTOF_P[d].T[t].TCLv[ch];
		mapped->fFineTime1LE   = data->PTOF_P[d].T[t].TFLv[ch];
	} else {
		// PM2
		mapped->fCoarseTime2LE = data->PTOF_P[d].T[t].TCLv[ch];
		mapped->fFineTime2LE   = data->PTOF_P[d].T[t].TFLv[ch];
	}
	return kTRUE;
}

#define MAX_TIME_OVER_THRESHOLD 1000 /* 1000 * 5 ns = 5 us */
Bool_t R3BPtofReader::ReadTrailingEdgeChannel(EXT_STR_h101_PTOF_onion *data,
    int d, int t, uint32_t bar, int ch)
{
	R3BPaddleTamexMappedData* mapped = NULL;

	// see if we can find a mappedData with matching PM1 data
	int n = fArray->GetEntriesFast();
	int coarse = data->PTOF_P[d].T[t].TCTv[ch];
	// matching a trailing time is only useful if the corresponding
	// leading time is present. Meaning: no need to check trailing1
	// to leading2 and vice-versa. This reduces the combinations we have to 
	// check to:
	// match trailing2 to leading2
	// match trailing1 to leading1

	// do the if outside to most inner loop for performance reasons
	if (t == 0) {
		// PM1
		for (int k = 0; k < n; k++)
		{
			R3BPaddleTamexMappedData* hit =
			    (R3BPaddleTamexMappedData*)fArray->At(k);
			int tot = coarse - hit->fCoarseTime1LE;
			if ((tot <= MAX_TIME_OVER_THRESHOLD) && (tot >= 0)
			    && (hit->fCoarseTime1TE == 0) /* no trailing */
			    && (hit->fCoarseTime1LE != 0)) /* has leading */
			{
				mapped = hit;
				break;
			}
		}
	} else {
		// PM2
		for (int k = 0; k < n; k++)
		{
			R3BPaddleTamexMappedData* hit =
			    (R3BPaddleTamexMappedData*)fArray->At(k);
			int tot = coarse - hit->fCoarseTime2LE;
			if ((tot <= MAX_TIME_OVER_THRESHOLD) && (tot >= 0)
			    && (hit->fCoarseTime2TE == 0) /* no trailing */
			    && (hit->fCoarseTime2LE != 0)) /* has leading */
			{
				mapped = hit;
				break;
			}
		}
	}

	if (!mapped) mapped = new ((*fArray)[fArray->GetEntriesFast()])
		R3BPaddleTamexMappedData(d + 1, bar); // plane, bar

	if (t == 0)
	{
		// PM1
		// coarse time leading edge
		mapped->fCoarseTime1TE= data->PTOF_P[d].T[t].TCTv[ch];
		// fine time leading edge
		mapped->fFineTime1TE  = data->PTOF_P[d].T[t].TFTv[ch];
	} else {
		// PM2
		// coarse time leading edge
		mapped->fCoarseTime2TE= data->PTOF_P[d].T[t].TCTv[ch];
		// fine time leading edge
		mapped->fFineTime2TE  = data->PTOF_P[d].T[t].TFTv[ch];
	}
	return kTRUE;
}

Bool_t R3BPtofReader::Read()
{
	// Convert plain raw data to multi-dimensional array
	EXT_STR_h101_PTOF_onion* data = (EXT_STR_h101_PTOF_onion*)fData;


	for (int d = 0; d < MAX_PTOF_PLANES; d++)
	{
		for (int t = 0; t < N_TUBES_PER_PADDLE; t++)
		{
			ReadLeadingEdges(data, d, t);
			ReadTrailingEdges(data, d, t);
		}
	}
}

void R3BPtofReader::Reset()
{
	// Reset the output array
	fArray->Clear();
}

ClassImp(R3BPtofReader)

