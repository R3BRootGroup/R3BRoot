#include "FairLogger.h"

#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "R3BPspxReader.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxMappedPar.h"

extern "C" {
#include "ext_data_client.h"
#include "ext_h101_psp.h"
}

R3BPspxReader::R3BPspxReader(EXT_STR_h101_PSP* data, UInt_t offset)
	: R3BReader("R3BPspxReader")
	, fData(data)
	, fOffset(offset)
	, fLogger(FairLogger::GetLogger())
    , fArray(new TClonesArray("R3BPspxMappedData"))
{
}

R3BPspxReader::~R3BPspxReader()
{}

Bool_t R3BPspxReader::Init(ext_data_struct_info *a_struct_info)
{
	int ok;

	EXT_STR_h101_PSP_ITEMS_INFO(ok, *a_struct_info, fOffset,
	    EXT_STR_h101_PSP, 0);

	if (!ok) {
		perror("ext_data_struct_info_item");
		fLogger->Error(MESSAGE_ORIGIN,
		    "Failed to setup structure information.");
		return kFALSE;
	}

	// Register output array in tree
	FairRootManager::Instance()->Register("PspxMappedData", "Land", fArray, kTRUE);
	
	return kTRUE;
}

// ----  Initialisation  ----------------------------------------------
void R3BPspxReader::SetParContainers()
{
  LOG(INFO) << "R3BPspxReader :: SetParContainers() " 
	    << FairLogger::endl;

  fMappedPar = (R3BPspxMappedPar*) FairRuntimeDb::instance()->getContainer("R3BPspxMappedPar");
	    
  // Get Base Container
  //FairRunAna* ana = FairRunAna::Instance();
  //FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  //fMappedPar = (R3BPspxMappedPar*) (rtdb->getContainer("R3BPspxMappedPar"));
  
  if (!fMappedPar)
  {
         LOG(ERROR) << "Could not get access to R3BPspxMappedPar-Container." << FairLogger::endl;
         return;
  }

  fMappedPar->printparams();
}
// --------------------------------------------------------------------


// ---- ReInit  -------------------------------------------------------
Bool_t R3BPspxReader::ReInit()
{

  LOG(INFO) << " R3BPspxReader :: ReInit() " 
	    << FairLogger::endl;


  //FairRunAna* ana = FairRunAna::Instance();
  //FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  //fDigiPar = (FairTutorialDet2DigiPar*) (rtdb->getContainer("FairTutorialDet2DigiPar"));

  fMappedPar = (R3BPspxMappedPar*) FairRuntimeDb::instance()->getContainer("R3BPspxMappedPar");
 
  if (!fMappedPar)
  {
         LOG(ERROR) << "Could not get access to R3BPspxMappedPar-Container." << FairLogger::endl;
         return kFALSE;
  }
	    
  return kTRUE;
}
// --------------------------------------------------------------------

Bool_t R3BPspxReader::Read()
{
	// Convert plain raw data to multi-dimensional array
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
* Each detector has 65 channels:
* 16 * 2 channels for vertical strips
* 16 * 2 channels for horizontal strips
* 1 channel for the cathode
* 
* However, one detector has either vertical OR horizontal strips, hence
* 32 channels are always empty.
* 
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
* 
* 65) cathode
* 
*/

// loop over all detectors
	for (int d=0;d<fMappedPar->GetPspxParStation();d++)
	{
		if (fMappedPar->GetPspxParStrip().At(d)==0) continue; // skip PSP3 (via MADC32)
		
		uint32_t numChannels = data->PSPX[d].M; // not necessarly number of hits! (b/c multi hit)
		
		// loop over channels
		uint32_t curChannelStart=0;     // index in v for first item of current channel
		for (int i=0;i<numChannels;i++) 
		{
			uint32_t channel=data->PSPX[d].MI[i]; // or 1..65
			uint32_t nextChannelStart=data->PSPX[d].ME[i];  // index in v for first item of next channel
			
			// if we had multi hit data, we would need to read
			// j=curChannelStart; j < nextChannelStart; j++.
			// For the PSPs, however, we take the first hit only:
			uint32_t energy=data->PSPX[d].v[curChannelStart];
			
			// The first 22 bits are energy. Bit 23 should be 0. Bit 24 is sign.
			if(energy!=0xEEEEEE){ //get rid of error message code

				if (d==0) printf("PSP1 Ch %d Energy: %d = 0x%x\n",i,energy,energy);
			    //TODO Ask Ralf about Continue
			    //if(d==0 || d==1 || d==3 || (d==4 && channel==65)){ // for s438b
			    //  if(!(energy&0x800000)) continue;
			    //  energy=energy-0x800000; //subtracting the sign bit for some channels (set to 1 for negative values)
			    //}
			    
			    if((energy&0x800000)) energy=energy-0x800000; //subtracting the sign bit (set to 1 for negative values)
			    
			    new ((*fArray)[fArray->GetEntriesFast()])
				    R3BPspxMappedData(d+1,channel,energy); // det,channel,energy counting from 1 for detectors and channels
			}
			
			curChannelStart=nextChannelStart;
		}
	}
}

void R3BPspxReader::Reset()
{
    // Reset the output array
    fArray->Clear();
}

ClassImp(R3BPspxReader)

