#include "FairLogger.h"
#include "R3BNeulandTamexReader.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "R3BNeulandTamexMappedItem.h"
#include "ext_data_struct_info.hh"

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
    , fArray(new TClonesArray("R3BNeulandTamexMappedItem"))
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
	
    // Register output array in tree
    FairRootManager::Instance()->Register("NeulandTamexMappedItem", "Land", fArray, kTRUE);

	return kTRUE;
}

Bool_t R3BNeulandTamexReader::Read()
{
	EXT_STR_h101_onion_t *data = (EXT_STR_h101_onion_t *) fData;

	/* Display data */
	fLogger->Info(MESSAGE_ORIGIN, "  Event data:");

	for (int plane = 0; plane < N_PLANES; ++plane) {
		for (int pm = 0; pm < 2; ++pm) {
			
            int idx = 0;
            int start = 0; 
            int stop = 0; 
            int bar;
            int cLE,fLE,cTE,fTE;
           			

            // the counter for coarse time and fine time should be always the same:
            if (data->NN_P[plane].tcl_T[pm].BM != data->NN_P[plane].tfl_T[pm].BM ||
                data->NN_P[plane].tcl_T[pm].B != data->NN_P[plane].tfl_T[pm].B ||
                data->NN_P[plane].tct_T[pm].BM != data->NN_P[plane].tft_T[pm].BM ||
                data->NN_P[plane].tct_T[pm].B != data->NN_P[plane].tft_T[pm].B ){
				fLogger->Info(MESSAGE_ORIGIN, "  Bad event, counter of coarse times and fine times do not match \n");
				exit;
			}
			
           // the counter for leading and trailing edge should be always the same:
            if (data->NN_P[plane].tcl_T[pm].B != data->NN_P[plane].tct_T[pm].B ||
                data->NN_P[plane].tfl_T[pm].B != data->NN_P[plane].tft_T[pm].B ){
				fLogger->Info(MESSAGE_ORIGIN, "  Bad event, mismatch of trailing and leading edges \n");
				exit;
			}

            for (int hit=0; hit<data->NN_P[plane].tcl_T[pm].BM;hit++){
				bar = data->NN_P[plane].tcl_T[pm].BMI[hit];
				stop = data->NN_P[plane].tcl_T[pm].BME[hit];
//				fLogger->Info(MESSAGE_ORIGIN, "  bar %d, multihit %d \n", bar, stop-start);
				
                for (int multi=start; multi<stop; multi++){
				    cLE = data->NN_P[plane].tcl_T[pm].Bv[multi];
				    fLE = data->NN_P[plane].tfl_T[pm].Bv[multi];					
				    cTE = data->NN_P[plane].tct_T[pm].Bv[multi];
				    fTE = data->NN_P[plane].tft_T[pm].Bv[multi];					
										
//				    fLogger->Info(MESSAGE_ORIGIN, " leading coarse time %d, fine time %d \n", cLE, fLE);					
//				    fLogger->Info(MESSAGE_ORIGIN, " trailing coarse time %d, fine time %d \n",cTE, fTE);					
			    
                    new ((*fArray)[fArray->GetEntriesFast()])
                        R3BNeulandTamexMappedItem(plane, bar, pm, cLE, fLE, cTE, fTE, kFALSE);
 
		        }		
		        start = stop;		
	        }
		}
	}

//	fNEvent = fData->EVENTNO;
    fNEvent += 1;

}

void R3BNeulandTamexReader::Reset()
{
    // Reset the output array
    fArray->Clear();
    
//	fNEvent = 0;
}

ClassImp(R3BNeulandTamexReader)

