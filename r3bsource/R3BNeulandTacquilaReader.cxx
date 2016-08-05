
#include "R3BNeulandTacquilaReader.h"

#include "TClonesArray.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "R3BNeulandMappedData.h"

#include "ext_data_struct_info.hh"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_raw_nnp.h"
}

R3BNeulandTacquilaReader::R3BNeulandTacquilaReader(EXT_STR_h101_raw_nnp* data,
    UInt_t offset)
    : R3BReader("R3BNeulandTacquilaReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BNeulandMappedData"))
{
}

R3BNeulandTacquilaReader::~R3BNeulandTacquilaReader()
{
}

Bool_t R3BNeulandTacquilaReader::Init(ext_data_struct_info* a_struct_info)
{
    // Initialize input UCESB structure
    Int_t ok;
    EXT_STR_h101_raw_nnp_ITEMS_INFO(ok, *a_struct_info, fOffset,
	EXT_STR_h101_raw_nnp, 1);
    if (!ok)
    {
        // Throw error
        perror("ext_data_struct_info_item");
        fLogger->Error(MESSAGE_ORIGIN, "Failed to setup structure information.");
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("NeulandMappedData", "Land", fArray, kTRUE);

    return kTRUE;
}

Bool_t R3BNeulandTacquilaReader::Read()
{
    // Convert plain raw data to multi-dimensional array
    EXT_STR_h101_raw_nnp_onion* data = (EXT_STR_h101_raw_nnp_onion*)fData;

    // Loop over all planes, bars and PMT's
    Int_t barId;
    for (Int_t i = 0; i < 8; i++)
    {
        for (Int_t j = 0; j < 50; j++)
        {
            for (Int_t k = 0; k < 2; k++)
            {
                // Signal channel
                UInt_t tdc1 = data->NNP[i]._[j]._[k].TAC;
                UInt_t clock1 = data->NNP[i]._[j]._[k].CLK;
                UInt_t qdc = data->NNP[i]._[j]._[k].ADC;

                // Stop signal (17-th channel)
                UInt_t tdc2 = data->NNP[i]._[j]._[k].T;
                //UInt_t clock2 = data->NNP[i]._[j]._[k].S;

                // Calculate global bar index
                barId = i * 50 + j + 1;

                // Store data in output
                if (tdc1)
                {
                    new ((*fArray)[fArray->GetEntriesFast()])
                        R3BNeulandMappedData(0, 0, 0, 0, 63 - clock1, 4095 - tdc1, qdc, i + 1, j + 1, k + 1, kFALSE);
                }
                if (tdc2)
                {
                    new ((*fArray)[fArray->GetEntriesFast()])
                        R3BNeulandMappedData(0, 0, 0, 0, 0, 4095 - tdc2, 0, i + 1, j + 1, k + 1 + 2, kTRUE);
                }
            }
        }
    }

    fNEvent += 1;

    return kTRUE;
}

void R3BNeulandTacquilaReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BNeulandTacquilaReader)
