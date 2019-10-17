#include "FairLogger.h"
#include "R3BCalifaFebexReader.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BCalifaMappedData.h"
#include "ext_data_struct_info.hh"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_raw_califa_febex.h"
}

R3BCalifaFebexReader::R3BCalifaFebexReader(EXT_STR_h101_CALIFA* data,
    UInt_t offset)
	: R3BReader("R3BCalifaFebexReader"),
  fNEvent(0),
  fData(data),
  fOffset(offset),
  fOnline(kFALSE),
  fArray(new TClonesArray("R3BCalifaMappedData")) {
}

R3BCalifaFebexReader::~R3BCalifaFebexReader() {
  if (fArray){
		delete fArray;
	}
}

Bool_t R3BCalifaFebexReader::Init(ext_data_struct_info *a_struct_info) {
  Int_t ok;
  LOG(INFO) << "R3BCalifaFebexReader::Init";
  EXT_STR_h101_CALIFA_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_CALIFA, 0);

  if (!ok) {
        LOG(ERROR)<<"R3BCalifaFebexReader::Failed to setup structure information.";
	return kFALSE;
  }

  // Register output array in tree
  if(!fOnline){
    FairRootManager::Instance()->Register("CalifaMappedData", "Califa", fArray, kTRUE);
  }else{
    FairRootManager::Instance()->Register("CalifaMappedData", "Califa", fArray, kFALSE);
  }  

  return kTRUE;
}

Bool_t R3BCalifaFebexReader::Read() {
  //EXT_STR_h101_CALIFA_onion_t *data =
	//    (EXT_STR_h101_CALIFA_onion_t *) fData;

  /* Display data */
  LOG(DEBUG)<<"R3BCalifaFebexReader::Read() Event data.";

  //SELECT THE FOR LOOP BASED ON THE MAPPING...
  for (int crystal = 0; crystal < fData->CALIFA_ENE; ++crystal) {

    UShort_t channelNumber = fData->CALIFA_ENEI[crystal];
    int16_t energy = fData->CALIFA_ENEv[crystal];
    int16_t nf = fData->CALIFA_NFv[crystal];
    int16_t ns = fData->CALIFA_NSv[crystal];
    uint64_t timestamp = ((uint64_t) fData->CALIFA_TSMSBv[crystal] << 32)
      | (uint64_t) fData->CALIFA_TSLSBv[crystal];
    uint32_t tot = 0; //not decoded yet
    UChar_t error = 0; //??

    new ((*fArray)[fArray->GetEntriesFast()]) R3BCalifaMappedData(channelNumber, energy, nf, ns, timestamp, error, tot);
	}
    fNEvent += 1;
    return kTRUE;
}

void R3BCalifaFebexReader::Reset() {
    // Reset the output array
    fArray->Clear();
//	fNEvent = 0;
}

ClassImp(R3BCalifaFebexReader)
