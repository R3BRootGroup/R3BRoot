#include "FairLogger.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BLosReader.h"
#include "R3BLosMappedItem.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_los.h"
#include "ext_h101.h"
}

R3BLosReader::R3BLosReader(EXT_STR_h101* data)
	: R3BReader("R3BLosReader")
	, fData(data)
	, fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BLosMappedItem"))
{
}

R3BLosReader::~R3BLosReader()
{}

Bool_t R3BLosReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_LOS_ITEMS_INFO(ok, *a_struct_info, EXT_STR_h101, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

    // Register output array in tree
    FairRootManager::Instance()->Register("R3BLosMappedItem", "Land", fArray, kTRUE);

	return kTRUE;
}

Bool_t R3BLosReader::Read()
{
	// Convert plain raw data to multi-dimensional array
    EXT_STR_h101_onion* data = (EXT_STR_h101_onion*)fData;

/*
  struct {
    struct {
      uint32_t TF;
      uint32_t TC;
    } _[4];
  } LOS[2];
*/

	for (int d=0;d<2;d++)
		for (int c=0;c<4;c++)
		{
			if (data->LOS[d]._[c].TF==0) continue; // no time converted
			
			new ((*fArray)[fArray->GetEntriesFast()])
				R3BLosMappedItem(d,						// detector
								 c,						// channel
								 data->LOS[d]._[c].TC,  // coarse time
								 data->LOS[d]._[c].TF); // fine time
		}	
}

void R3BLosReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BLosReader)

