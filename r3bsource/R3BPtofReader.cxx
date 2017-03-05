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
	  /* TODO: Initialise data members */
{
	(void)data;
	(void)offset;
}

R3BPtofReader::~R3BPtofReader()
{}

Bool_t R3BPtofReader::Init(ext_data_struct_info *a_struct_info)
{
	/*
	 * TODO:
	 * - Add detector specific data items to the ext_data_struct_info
	 *   Use the macro supplied in the ext_h101_ptof.h file.
	 *   Take care to make use of fOffset.
	 *
	 * - Check the 'ok' variable for any errors and report errors
	 *   using perror() and fLogger->Error().
	 *   Returning kFALSE from this Init() function indicates an error.
	 *
	 * - Register the output array in the tree
	 *   Use the Register() method of FairRootManager
	 *   Chose an appropriate name for the output branch
	 *
	 * - Initially clear the fData structure by setting all
	 *   'number of hits' values to 0.
	 *
	 * - Return with the appropriate value for success!
	 */
	(void)a_struct_info;
	return kFALSE;
}

Bool_t R3BPtofReader::ReadLeadingEdges(EXT_STR_h101_PTOF_onion *data,
    int d, int t)
{
	/* TODO:
	 * - Get the number of channels in this plane & tube
	 * - Loop over all channels with multi-hits
	 *   - Find out the bar index in this channel
	 *   - Find out where the current channel ends
	 *   - Loop over all hits in this channel
	 *     - Call ReadLeadingEdgeChannel(...)
	 * - Return success
	 *
	 * - Note:
	 *     Number of hits: <name>M
	 *     Array with channel indices: <name>MI
	 *     Array with next channel start indices: <name>ME
	 */
	(void)data;
	(void)d;
	(void)t;
	return kFALSE;
}

Bool_t R3BPtofReader::ReadTrailingEdges(EXT_STR_h101_PTOF_onion *data,
    int d, int t)
{
	/*
	 * TODO:
	 * - Do something similar to what is done in ReadLeadingEdges.
	 *   Just for the trailing edges.
	 */
	(void)data;
	(void)d;
	(void)t;
	return kFALSE;
}

#define MAX_TIME_DIFF_PADDLE_PMT 20 /* 20 * 5 ns = 100 ns */
Bool_t R3BPtofReader::ReadLeadingEdgeChannel(EXT_STR_h101_PTOF_onion *data,
    int d, int t, uint32_t bar, int ch)
{
	/*
	 * TODO:
	 *
	 * - Fill the leading edge times into the output array for either
	 *   PM1 or PM2, but note the following:
	 *
	 * - If we have a hit in PM2, try to find a corresponding item in the
	 *   already stored output array that contains a hit in PM1, under
	 *   the condition that it has a leading time within a 100 ns window
	 *   from this hit and not yet any leading edge time in PM2.
	 *
	 * - Note: Coarse time of this hit:
	 *             <name>TCLv[ch]
	 *         Coarse time of leading time hit in output array:
	 *             hit->fCoarseTime2LE
	 *         Use MAX_TIME_DIFF_PADDLE_PMT for the comparison
	 *
	 * - If no such hit is found, or if we have a hit in PM1, make a new
	 *   item in the output array using placement new, i.e.
	 *     new_hit = new ((*fArray)[index])R3BPaddleTamexMappedData(x,y);
	 *
	 * - In any case, fill the appropriate members of the (new) item with
	 *   the times from the ucesb structure:
	 *     - coarse time is <name>TCLv[ch]
	 *     - fine time is   <name>TFLv[ch]
	 *
	 * - return success!
	 */

	return kFALSE;
}

#define MAX_TIME_OVER_THRESHOLD 1000 /* 1000 * 5 ns = 5 us */
Bool_t R3BPtofReader::ReadTrailingEdgeChannel(EXT_STR_h101_PTOF_onion *data,
    int d, int t, uint32_t bar, int ch)
{
	/*
	 * TODO:
	 * - Fill the trailing edge times into the output data array
	 *
	 * - First try to find a matching item in the output array, which
	 *   has a leading edge time, and no trailing edge time set.
	 *   Then test, if the leading edge is at most 5 us away from the
	 *   current trailing time (too large time-over-threshold makes no
	 *   sense).
	 *   Note, that you have to make the test once for PM1 and once for
	 *   PM2.
	 *   The current coarse trailing time is in <name>TCTv[ch].
	 *
	 * - If no match is found, make a new item using placement new.
	 *
	 * - Then fill in the trailing edge time into the appropriate
	 *   data members of the data item.
	 *
	 * - return success!
	 */
	return kFALSE;
}

Bool_t R3BPtofReader::Read()
{
	/* TODO:
	 * - Convert fData to type EXT_STR_h101_PTOF_onion *.
	 *   This makes it easier to access all the members.
	 * - Loop over all planes and tubes and call the functions
	 *     - ReadLeadingEdges()
	 *     - ReadTrailingEdges()
	 * - Return success!
	 */
	return kFALSE;
}

void R3BPtofReader::Reset()
{
	// TODO: Reset the output array
}

ClassImp(R3BPtofReader)

