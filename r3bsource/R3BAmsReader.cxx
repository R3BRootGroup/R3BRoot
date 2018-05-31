#include "FairLogger.h"
#include "R3BAmsReader.h"

                    
#include "TRandom.h"
#include "TMath.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BAmsMappedData.h"
#include "ext_data_struct_info.hh"

/*FIXME
extern "C" {
#include "ext_data_client.h"
#include "ext_h101_ams.h"
}*/

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_raw_califa_febex.h"
}

//R3BAmsReader::R3BAmsReader(EXT_STR_h101_AMS* data,
R3BAmsReader::R3BAmsReader(EXT_STR_h101_CALIFA* data,
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

	//EXT_STR_h101_AMS_ITEMS_INFO(ok, *a_struct_info, fOffset, FIXME
	  //  EXT_STR_h101_AMS, 0);

	EXT_STR_h101_CALIFA_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_CALIFA, 0);

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
 // EXT_STR_h101_AMS_onion_t *data =FIXME
//	    (EXT_STR_h101_AMS_onion_t *) fData;

  EXT_STR_h101_CALIFA_onion_t *data =
	    (EXT_STR_h101_CALIFA_onion_t *) fData;

	/* Display data */
	fLogger->Debug(MESSAGE_ORIGIN, "R3BAmsReader::Read() Event data");

  UShort_t detector = 0;

  //SELECT THE FOR LOOP BASED ON THE MAPPING...
//  for (int strip = 0; strip < fData->AMS_ENE; ++strip) { FIXME

  for (int strip = 0; strip < 1024; ++strip) {

    UShort_t stripNumber = strip;//fData->AMS_ENEI[strip];
    uint16_t energy = 400.+100.*cos(strip/500.*3.1415*2.) + gRandom->Gaus(0.,10.);//fData->AMS_ENEv[strip];

    //First detector must be zero!!
    if(fNEvent<2000)new ((*fArray)[fArray->GetEntriesFast()]) R3BAmsMappedData(detector,stripNumber, energy);
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
