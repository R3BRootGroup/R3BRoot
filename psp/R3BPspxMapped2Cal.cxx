// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxMapped2Cal                              -----
// -----                    Created  21-01-2016 by R. Plag                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
/* Convert Mapped data to Cal data, meaning: apply offset and gain 
 * to the energy values.
 */

#include <iostream>
using namespace std;

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BEventHeader.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxCalData.h"
#include "R3BPspxMapped2Cal.h"

R3BPspxMapped2Cal::R3BPspxMapped2Cal()
    : fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BPspxCalData"))
{
}

R3BPspxMapped2Cal::R3BPspxMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BPspxCalData"))
{
}

R3BPspxMapped2Cal::~R3BPspxMapped2Cal()
{
}

InitStatus R3BPspxMapped2Cal::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fMappedItems = (TClonesArray*)fMan->GetObject("PspxMapped"); // = branch name in TTree
    if (!fMappedItems)
    {
		printf("Couldnt get handle on PSPX mapped items\n");
		return kFATAL;
	}
    //fCalItems = (TClonesArray*)fMan->GetObject("R3BPspxMapped2CalItem");
    fCalibration = NULL; // care about that later
    FairRootManager::Instance()->Register("PspxCal", "Land", fCalItems, kTRUE);

    return kSUCCESS;
}

void R3BPspxMapped2Cal::Exec(Option_t* option)
{
    if (!fMappedItems) 
    {
		printf("Cannot access PSPX mapped items\n");
		return;
	}
    
	Int_t nMapped = fMappedItems->GetEntries();
	for (Int_t i = 0; i < nMapped; i++)
	{
		R3BPspxMappedData* mItem = (R3BPspxMappedData*)fMappedItems->At(i);

		// calibrate and add to fCalItems
		// [...]
		Float_t e=mItem->GetEnergy();
		
		new ((*fCalItems)[fCalItems->GetEntriesFast()])
			R3BPspxCalData(mItem->GetDetector(), mItem->GetChannel(), e); // det,channel,energy

	}

    
}

void R3BPspxMapped2Cal::FinishTask()
{
}


//void R3BPspxMapped2Cal::WriteHistos() {}

ClassImp(R3BPspxMapped2Cal)
