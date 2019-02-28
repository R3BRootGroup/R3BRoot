#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BWhiterabbitReader.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_whiterabbit.h"
}

R3BWhiterabbitReader::R3BWhiterabbitReader(EXT_STR_h101_whiterabbit *data,
    UInt_t offset, UInt_t whiterabbit_id)
	: R3BReader("R3BWhiterabbitReader")
	, fNEvent(0)
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
	, fWhiterabbitId(whiterabbit_id)
	, fEventHeader(nullptr)
{
}

R3BWhiterabbitReader::~R3BWhiterabbitReader()
{
}

Bool_t R3BWhiterabbitReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_whiterabbit_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_whiterabbit, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		LOG(error) << "Failed to setup structure information.";
		return kFALSE;
	}

	FairRootManager* mgr = FairRootManager::Instance();
	fEventHeader = (R3BEventHeader *)mgr->GetObject("R3BEventHeader");

	fData->TIMESTAMP_MASTER_ID = 0;

	return kTRUE;
}

Bool_t R3BWhiterabbitReader::Read()
{
	if (!fData->TIMESTAMP_MASTER_ID) {
		return kTRUE;
	}
printf("%08x %08x %08x %08x %08x \n",
fData->TIMESTAMP_MASTER_ID,
fData->TIMESTAMP_MASTER_WR_T4, fData->TIMESTAMP_MASTER_WR_T3,
fData->TIMESTAMP_MASTER_WR_T2, fData->TIMESTAMP_MASTER_WR_T1);
	if (fWhiterabbitId != fData->TIMESTAMP_MASTER_ID) {
		char strMessage[1000];
		snprintf(strMessage, sizeof strMessage,
				"Event %u: Whiterabbit ID mismatch: expected %u, got %u.\n",
				fEventHeader->GetEventno(),
				fWhiterabbitId, fData->TIMESTAMP_MASTER_ID);
		LOG(error) << strMessage;
	}

	if (fEventHeader != nullptr) {
		uint64_t timestamp =
			  ((uint64_t) fData->TIMESTAMP_MASTER_WR_T4 << 48)
			| ((uint64_t) fData->TIMESTAMP_MASTER_WR_T3 << 32)
			| ((uint64_t) fData->TIMESTAMP_MASTER_WR_T2 << 16)
			|  (uint64_t) fData->TIMESTAMP_MASTER_WR_T1;

		fEventHeader->SetTimeStamp(timestamp);
		fNEvent = fEventHeader->GetEventno();
	} else {
		fNEvent++;
	}

	fData->TIMESTAMP_MASTER_ID = 0;
	return kTRUE;
}

void R3BWhiterabbitReader::Reset()
{
	fNEvent = 0;
}

ClassImp(R3BWhiterabbitReader)

