#include "FairLogger.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxMappedPar.h"
#include "R3BPspxReader.h"
#include "TClonesArray.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_psp.h"
//#include "ext_h101_psp_jun16.h"
//#include "ext_h101.h"
}

R3BPspxReader::R3BPspxReader(EXT_STR_h101_PSP* data, UInt_t offset)
    : R3BReader("R3BPspxReader")
    , fData(data)
    , fOffset(offset)
    , fLogger(FairLogger::GetLogger())
    , fMappedItems(new TClonesArray("R3BPspxMappedData"))
{
}

R3BPspxReader::~R3BPspxReader() {}

/**
  * Initialize output data. Read input data.
  */
Bool_t R3BPspxReader::Init(ext_data_struct_info* a_struct_info)
{

    int ok;

    EXT_STR_h101_PSP_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_PSP, 0);

    if (!ok)
    {
        perror("ext_data_struct_info_item");
        fLogger->Error(MESSAGE_ORIGIN, "Failed to setup structure information.");
        return kFALSE;
    }

    // Register output array in tree
    FairRootManager::Instance()->Register("PspxMappedData", "Pspx", fMappedItems, kTRUE);

    return kTRUE;
}

// ----  Initialisation  ----------------------------------------------
/**
  * Initialize/Reads parameter file for conversion.
  */
void R3BPspxReader::SetParContainers()
{
    LOG(INFO) << "R3BPspxReader :: SetParContainers() " << FairLogger::endl;

    fMappedPar = (R3BPspxMappedPar*)FairRuntimeDb::instance()->getContainer("R3BPspxMappedPar");

    // Get Base Container
    // FairRunAna* ana = FairRunAna::Instance();
    // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    // fMappedPar = (R3BPspxMappedPar*) (rtdb->getContainer("R3BPspxMappedPar"));

    if (!fMappedPar)
    {
        LOG(ERROR) << "Could not get access to R3BPspxMappedPar-Container." << FairLogger::endl;
        return;
    }

    fMappedPar->printparams();
}

// ---- ReInit  -------------------------------------------------------
/**
  * Initialize/Reads parameter file for conversion.
  */
Bool_t R3BPspxReader::ReInit()
{
    LOG(INFO) << " R3BPspxReader :: ReInit() " << FairLogger::endl;

    // FairRunAna* ana = FairRunAna::Instance();
    // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    // fMappedPar = (R3BPspxMappedPar*) (rtdb->getContainer("R3BPspxMappedPar"));

    fMappedPar = (R3BPspxMappedPar*)FairRuntimeDb::instance()->getContainer("R3BPspxMappedPar");

    if (!fMappedPar)
    {
        LOG(ERROR) << "Could not get access to R3BPspxMappedPar-Container." << FairLogger::endl;
        return kFALSE;
    }

    return kTRUE;
}

// --------------------------------------------------------------------
/**
  * Does the unpacking to Mapped level. It is called for every event.
  * Converts plain raw data to multi-dimensional array.
  * Ignores energies with an error message.
  * Although, ucesb is multi-hit capable, only the first entry is used for the PSPX detectors.
  */
Bool_t R3BPspxReader::Read()
{
    EXT_STR_h101_PSP_onion* data = (EXT_STR_h101_PSP_onion*)fData;

    // Display data
    // fLogger->Info(MESSAGE_ORIGIN, "  Event data:");

    /*
      // this is the data structure we have to read:
      struct {
        uint32_t M;                 // number of channels with data
        uint32_t MI[65 / * M * /];  // channel number
        uint32_t ME[65 / * M * /];  // offset in v array for that channel
        uint32_t _;                 // num items in v
        uint32_t v[650 / * _ * /];  // the energy data
      } PSPX[5];

    * More info on the data format at:
    * https://forum.gsi.de/index.php?t=msg&th=4798&start=0&
    *
    * At the moment there are 2 types of detectors: X1 (strips on one side) & X5 (strips on both sides).
    * Each X1 (X5) detector has 65 (128) channels:
    * 16 (32) * 2 channels for vertical strips
    * 16 (32)* 2 channels for horizontal strips
    * 1 channel for the cathode (only X1)
    *
    * If one detector (e.g. X1) has either vertical OR horizontal strips, 32 channels are empty.
    *
    * Example:
    * Channel numbers:
    * 1) v1_bottom
    * 2) v1_top
    * 3) v2_bottom
    * 4) v2_top
    * ...
    * 33) h1_right
    * 34) h1_left
    * 35) h2_right
    * 36) h2_left
    * ...
    * 65) cathode
    */

    // loop over all detectors
    for (int d = 0; d < fMappedPar->GetPspxParDetector(); d++)
    {
        if (fMappedPar->GetPspxParStrip().At(d) == 0)
            continue; // skip PSPs that are read out with other electronics (e.g. MADC32)

        uint32_t numChannels = data->PSPX[d].M; // not necessarly number of hits! (b/c multi hit)

        // loop over channels
        uint32_t curChannelStart = 0; // index in v for first item of current channel
        for (int i = 0; i < numChannels; i++)
        {
            uint32_t channel = data->PSPX[d].MI[i];          // counting from 1 to max number of channels for an detector
            uint32_t nextChannelStart = data->PSPX[d].ME[i]; // index in v for first item of next channel

            // if we had multi hit data, we would need to read
            // j=curChannelStart; j < nextChannelStart; j++.
            // For the PSPs, however, we take the first hit only:
            int32_t energy = data->PSPX[d].v[curChannelStart];

            // For certain (old) unpackers: The first 22 bits are energy. Bit 23 should be 0. Bit 24 is sign.
            // comment from old s438b reader:  0xfff is energy data, Anything in 0xfffff000 indicates an error or
            // overflow. still usefull?
	    
           // if (energy == 0xEEEEEE) continue; // get rid of error message code: only valid for certain ucesb unpackers

//	    if (energy == -3075811 || energy == -3075810) {
//		LOG(ERROR)<< "R3BPspxReader::Read(): Error Code from Febex Unpacker" << FairLogger::endl;
//		continue; // get rid of error message code: -3075810 = 0xeeeeee2, -3075811 = 0xeeeee3
//	    }
	    
	    //if ((energy & 0x800000))
	    //    energy = -1*(energy - 0x800000); // subtracting the sign bit (set to 1 for negative values) and multiplying with -1 to get negative energy value, only possible for certain ucesb unpackers
	      
	    if(TMath::Abs(energy)<4194303){ // = 2^22 -1 max value possible for Febex wtih 22 bits for enery entry + additional sign bit #24
		new ((*fMappedItems)[fMappedItems->GetEntriesFast()]) R3BPspxMappedData(
		  d + 1, channel, energy); // det,channel,energy counting from 1 for detectors and channels
	    }

            curChannelStart = nextChannelStart;
        }
    }

    return kTRUE;
}

void R3BPspxReader::Reset() { fMappedItems->Clear(); }

ClassImp(R3BPspxReader)
