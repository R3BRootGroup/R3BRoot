#include "FairLogger.h"
#include "R3BUnpackReader.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_unpack.h"
#include "ext_h101_full.h"
}

R3BUnpackReader::R3BUnpackReader(EXT_STR_h101* data)
	: R3BReader("R3BUnpackReader")
	, fNEvent(0)
	, fData(data)
	, fLogger(FairLogger::GetLogger())
{
}

R3BUnpackReader::~R3BUnpackReader()
{}

Bool_t R3BUnpackReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_unpack_ITEMS_INFO(ok, *a_struct_info, EXT_STR_h101, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

	return kTRUE;
}

Bool_t R3BUnpackReader::Read()
{
	/* Display data */
	fLogger->Info(MESSAGE_ORIGIN, "  Event data:");
	fLogger->Info(MESSAGE_ORIGIN, "  %10d (d%10d): %2d\n",
		fData->EVENTNO,
		fData->EVENTNO - fNEvent,
		fData->TRIGGER);

	fNEvent = fData->EVENTNO;

    return kTRUE;
}

void R3BUnpackReader::Reset()
{
	fNEvent = 0;
}

ClassImp(R3BUnpackReader)

