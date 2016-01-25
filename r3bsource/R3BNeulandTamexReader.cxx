#include "FairLogger.h"
#include "R3BNeulandTamexReader.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_raw_nnp_tamex.h"
#include "ext_h101_full.h"
}

/* Number of Neuland planes */
#define N_PLANES 2

R3BNeulandTamexReader::R3BNeulandTamexReader(EXT_STR_h101* data)
	: R3BReader("R3BNeulandTamexReader")
	, fNEvent(0)
	, fData(data)
	, fLogger(FairLogger::GetLogger())
{
}

R3BNeulandTamexReader::~R3BNeulandTamexReader()
{}

Bool_t R3BNeulandTamexReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_raw_nnp_tamex_ITEMS_INFO(ok, *a_struct_info,
	    EXT_STR_h101, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

	return kTRUE;
}

Bool_t R3BNeulandTamexReader::Read()
{
	EXT_STR_h101_onion_t *data = (EXT_STR_h101_onion_t *) fData;

	/* Display data */
	fLogger->Info(MESSAGE_ORIGIN, "  Event data:");

	for (int plane = 0; plane < N_PLANES; ++plane) {
		for (int pm = 0; pm < 2; ++pm) {
			fLogger->Info(MESSAGE_ORIGIN, "  P %2d T %1d "
				"tcB %5d tcBM %2d tfB %5d tfBM %2d\n",
				plane, pm,
				data->NN_P[plane].tc_T[pm].B,
				data->NN_P[plane].tc_T[pm].BM,
				data->NN_P[plane].tf_T[pm].B,
				data->NN_P[plane].tf_T[pm].BM);
		}
	}

	fNEvent = fData->EVENTNO;
}

void R3BNeulandTamexReader::Reset()
{
	fNEvent = 0;
}

ClassImp(R3BNeulandTamexReader)

