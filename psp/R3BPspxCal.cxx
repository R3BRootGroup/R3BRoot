// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxCal                              -----
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
#include "R3BPspxMappedItem.h"
#include "R3BPspxCalItem.h"
#include "R3BPspxCal.h"

R3BPspxCal::R3BPspxCal()
    : fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BPspxCalItem"))
{
}

R3BPspxCal::R3BPspxCal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BPspxCalItem"))
{
}

R3BPspxCal::~R3BPspxCal()
{
}

InitStatus R3BPspxCal::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fMappedItems = (TClonesArray*)fMan->GetObject("R3BPspxMappedItem"); // = branch name in TTree
    if (!fMappedItems)
    {
		printf("Couldnt get handle on PSPX mapped items\n");
		return kFATAL;
	}
    //fCalItems = (TClonesArray*)fMan->GetObject("R3BPspxCalItem");
    fCalibration = NULL; // care about that later
    FairRootManager::Instance()->Register("PspxCalItem", "Land", fCalItems, kTRUE);

    return kSUCCESS;
}

void R3BPspxCal::Exec(Option_t* option)
{
    if (!fMappedItems) 
    {
		printf("Cannot access PSPX mapped items\n");
		return;
	}
    
	Int_t nMapped = fMappedItems->GetEntries();
	for (Int_t i = 0; i < nMapped; i++)
	{
		R3BPspxMappedItem* mItem = (R3BPspxMappedItem*)fMappedItems->At(i);

		// calibrate and add to fCalItems
		// [...]
		Float_t e=mItem->GetEnergy();
		
		new ((*fCalItems)[fCalItems->GetEntriesFast()])
			R3BPspxCalItem(mItem->GetDetector(), mItem->GetChannel(), e); // det,channel,energy

	}

    
}

void R3BPspxCal::FinishTask()
{
}


//void R3BPspxCal::WriteHistos() {}

ClassImp(R3BPspxCal)
