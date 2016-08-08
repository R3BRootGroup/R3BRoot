#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BUnpackReader.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_unpack.h"
}

R3BUnpackReader::R3BUnpackReader(EXT_STR_h101_unpack *data, UInt_t offset)
	: R3BReader("R3BUnpackReader")
	, fNEvent(0)
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
	, fHeader(new R3BEventHeader())
{
}

R3BUnpackReader::~R3BUnpackReader()
{
	if (fHeader)
	{
		delete fHeader;
	}
}

Bool_t R3BUnpackReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_unpack_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_unpack, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

	FairRootManager* mgr = FairRootManager::Instance();
	mgr->Register("R3BEventHeader", "EventHeader", fHeader, kTRUE);

	return kTRUE;
}

Bool_t R3BUnpackReader::Read()
{
	/* Display data */
//	fLogger->Info(MESSAGE_ORIGIN, "  Event data:");
//	fLogger->Info(MESSAGE_ORIGIN, "  %10d (d%10d): %2d\n",
//		fData->EVENTNO,
//		fData->EVENTNO - fNEvent,
//		fData->TRIGGER);

	fNEvent = fData->EVENTNO;

	if (0 == (fNEvent % 1000))
	{
		LOG(DEBUG1) << "R3BUnpackReader : event : " << fNEvent
		    << ", trigger : " << fData->TRIGGER << FairLogger::endl;
	}

	fHeader->SetTrigger(fData->TRIGGER);
	fHeader->SetEventno(fNEvent);

    return kTRUE;
}

void R3BUnpackReader::Reset()
{
	fNEvent = 0;
}

ClassImp(R3BUnpackReader)

