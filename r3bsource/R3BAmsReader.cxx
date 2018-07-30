#include "FairLogger.h"
#include "R3BAmsReader.h"
  
#include "TRandom.h"
#include "TMath.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BAmsMappedData.h"
#include "ext_data_struct_info.hh"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_ams.h"
}
R3BAmsReader::R3BAmsReader(EXT_STR_h101_AMS* data,
    UInt_t offset)
	: R3BReader("R3BAmsReader"),
  fNEvent(0),
  fData(data),
  fOffset(offset),
  fLogger(FairLogger::GetLogger()),
  fArray(new TClonesArray("R3BAmsMappedData")) {
}

R3BAmsReader::~R3BAmsReader() {
  if (fArray){
		delete fArray;
	}
}

Bool_t R3BAmsReader::Init(ext_data_struct_info *a_struct_info) {
	int ok;

	EXT_STR_h101_AMS_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_AMS, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Debug(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

    // Register output array in tree
    FairRootManager::Instance()->Register("AmsMappedData", "AMS", fArray, kTRUE);

	return kTRUE;
}

Bool_t R3BAmsReader::Read() {
  EXT_STR_h101_AMS_onion_t *data =
	    (EXT_STR_h101_AMS_onion_t *) fData;

	/* Display data */
	fLogger->Debug(MESSAGE_ORIGIN, "R3BAmsReader::Read() Event data");

 //First AMS detector
 for (int strip = 0; strip < fData->SST1; ++strip) {
  new ((*fArray)[fArray->GetEntriesFast()]) R3BAmsMappedData(0,strip,fData->SST1E[strip]);
 }

 //Second AMS detector
 for (int strip = 0; strip < fData->SST2; ++strip) {
  new ((*fArray)[fArray->GetEntriesFast()]) R3BAmsMappedData(1,strip,fData->SST2E[strip]);
 }

 //Third AMS detector
 for (int strip = 0; strip < fData->SST3; ++strip) {
  new ((*fArray)[fArray->GetEntriesFast()]) R3BAmsMappedData(2,strip,fData->SST3E[strip]);
 }

 //Fourth AMS detector
 for (int strip = 0; strip < fData->SST4; ++strip) {
  new ((*fArray)[fArray->GetEntriesFast()]) R3BAmsMappedData(3,strip,fData->SST4E[strip]);
 }

    fNEvent += 1;
    return kTRUE;
}

void R3BAmsReader::Reset() {
    // Reset the output array
    fArray->Clear();
//	fNEvent = 0;
}

ClassImp(R3BAmsReader)
