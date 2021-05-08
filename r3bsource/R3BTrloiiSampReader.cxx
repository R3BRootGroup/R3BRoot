#include "R3BTrloiiSampReader.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BSamplerMappedData.h"
#include "TClonesArray.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_samp.h"
}

using namespace std;

R3BTrloiiSampReader::R3BTrloiiSampReader(EXT_STR_h101_SAMP* data, UInt_t offset)
    : R3BReader("R3BTrloiiSampReader")
    , fNEvent(0)
    , fData(data)
    , fOffset(offset)
    , fLogger(FairLogger::GetLogger())
    , fEventHeader(nullptr)
    , fArray(new TClonesArray("R3BSamplerMappedData"))
{
}

R3BTrloiiSampReader::~R3BTrloiiSampReader()
{
    if (fArray)
    {
        delete fArray;
    }
}

Bool_t R3BTrloiiSampReader::Init(ext_data_struct_info* a_struct_info)
{
    int ok;

    EXT_STR_h101_SAMP_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_SAMP, 0);

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        LOG(ERROR) << "Failed to setup structure information.";
        return kFALSE;
    }

    auto mgr = FairRootManager::Instance();
    fEventHeader = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    FairRootManager::Instance()->Register("SamplerMapped", "Land", fArray, kTRUE);
    fArray->Clear();

    fData->SAMP = 0;

    return kTRUE;
}

Bool_t R3BTrloiiSampReader::Read()
{
    if (fEventHeader)
    {
        fNEvent = fEventHeader->GetEventno();
    }
    else
    {
        fNEvent++;
    }

    for (uint32_t i = 0; i < fData->SAMP; ++i)
    {
        auto ch = fData->SAMPI[i];
        auto v = fData->SAMPv[i];

        new ((*fArray)[fArray->GetEntriesFast()]) R3BSamplerMappedData(v);

        // std::cout << "Test" << v << '\n';
    }

    return kTRUE;
}

void R3BTrloiiSampReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BTrloiiSampReader)
