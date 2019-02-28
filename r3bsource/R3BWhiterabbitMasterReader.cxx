#include "FairLogger.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "R3BEventHeader.h"
#include "R3BWhiterabbitMasterReader.h"
#include "R3BWRMasterData.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_wrmaster.h"
}

R3BWhiterabbitMasterReader::R3BWhiterabbitMasterReader(EXT_STR_h101_WRMASTER *data,
    UInt_t offset, UInt_t whiterabbit_id)
	: R3BReader("R3BWhiterabbitMasterReader")
	, fNEvent(0)
	, fData(data)
	, fOffset(offset)
        , fOnline(kFALSE)
	, fLogger(FairLogger::GetLogger())
	, fWhiterabbitId(whiterabbit_id)
	, fEventHeader(nullptr)
        , fArray(new TClonesArray("R3BWRMasterData"))
{
}

R3BWhiterabbitMasterReader::~R3BWhiterabbitMasterReader()
{
  if (fArray){
	delete fArray;
  }
}

Bool_t R3BWhiterabbitMasterReader::Init(ext_data_struct_info *a_struct_info)
{
  Int_t ok;
  LOG(INFO) << "R3BWhiterabbitMasterReader::Init" << FairLogger::endl;
  EXT_STR_h101_WRMASTER_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_WRMASTER, 0);

  if (!ok) {
     LOG(ERROR)<<"R3BWhiterabbitMasterReader::Failed to setup structure information."<<FairLogger::endl;
     return kFALSE;
  }

  FairRootManager* mgr = FairRootManager::Instance();
  fEventHeader = (R3BEventHeader *)mgr->GetObject("R3BEventHeader");

  // Register output array in tree
  if(!fOnline){
    FairRootManager::Instance()->Register("WRMasterData", "WRMaster", fArray, kTRUE);
  }else{
    FairRootManager::Instance()->Register("WRMasterData", "WRMaster", fArray, kFALSE);
  }

  //fData->TIMESTAMP_MASTER_ID = 0;

  return kTRUE;
}

Bool_t R3BWhiterabbitMasterReader::Read()
{
	if (!fData->TIMESTAMP_MASTER_ID) {
		return kTRUE;
	}
/*printf("master %08x %08x %08x %08x %08x \n",
fData->TIMESTAMP_MASTER_ID,
fData->TIMESTAMP_MASTER_WR_T4, fData->TIMESTAMP_MASTER_WR_T3,
fData->TIMESTAMP_MASTER_WR_T2, fData->TIMESTAMP_MASTER_WR_T1);*/

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

		//fEventHeader->SetTimeStamp(timestamp);
		fNEvent = fEventHeader->GetEventno();
                new ((*fArray)[fArray->GetEntriesFast()]) R3BWRMasterData(timestamp);
	} else {
		fNEvent++;
	}

	//fData->TIMESTAMP_MASTER_ID = 0;
	return kTRUE;
}

void R3BWhiterabbitMasterReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    fNEvent = 0;
}

ClassImp(R3BWhiterabbitMasterReader)

