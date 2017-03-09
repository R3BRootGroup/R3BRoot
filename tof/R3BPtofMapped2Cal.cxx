// ------------------------------------------------------------
// -----                  R3BPtofMapped2Cal               -----
// -----          Created March 8th 2017 by ____          -----
// -----                Folder: R3BRoot/tof               -----
// ------------------------------------------------------------

/* This class converts paddle data produced by Ptof from MAPPED level
 * to CAL level.
 * 
 * Data in MAPPED level is stored in class R3BPaddleTamexMappedData
 * Data in CAL level should be stored in class R3BPaddleCalData
 * 
 */


#include "R3BPtofMapped2Cal.h"

#include "R3BTCalEngine.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BTCalPar.h"
#include "R3BEventHeader.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"


R3BPtofMapped2Cal::R3BPtofMapped2Cal()
	: FairTask("TofdTcal", 1)
// ToDo: initialize all members with default values
{
}

R3BPtofMapped2Cal::R3BPtofMapped2Cal(const char* name, Int_t iVerbose)
	: FairTask(name, iVerbose)
// ToDo: initialize all members with default values
{
}

R3BPtofMapped2Cal::~R3BPtofMapped2Cal()
{
// ToDo: free all memory that was consumed by "new" below
}

InitStatus R3BPtofMapped2Cal::Init()
{
/* ToDo: initialize and check all member variables. These are:
		* check that we have a container with calibration parameters and
		  that this container is not empty
		* get access to the MAPPED data: initialize fMappedItems with
		  the PtofMapped-Object created by the Ptof reader. Hint: You
		  can call GetObject() at the FairRootManager to get a pointer
		  on the mapped items.
		* register the output array fCalItems at the FairRootManager.
		Remember: you need to return either kSUCCESS or kFATAL


		Convert the following pseudo code into real C++:
*/

	if (calibration container not available)
		print an error and stop

	fNofTcalPars = ...
	if (zero parameters available)
		print an error and stop

	get pointer to FairRootManager

	// get access to Mapped data
	fMappedItems = ...
	if (access to mapped items failed)
		print an error and fail 
	
	request storage of fCalItems in output tree

	return kSUCCESS;



/* You may find the following functions helpful:
 * 
 * fTcalPar->GetNumModulePar()
 * 		returns number of calibration parameter in container
 * 
 * FairRootManager* mgr = FairRootManager::Instance();
 * 		returns pointer to FairRootManager
 * 
 * (TClonesArray*)mgr->GetObject("PtofMapped");
 * 		returns pointer to array "PtofMapped"
 * 
 * FairRootManager::Register("PtofCal", "Land", fCalItems, kTRUE);
 * 		Registers array fCalItems in the output tree as branch "PtofCal"
 * 
 */
}



// Note that the container may still be empty at this point.
void R3BPtofMapped2Cal::SetParContainers()
{
// ToDo: fetch the container with calibration parameters from FairRuntimeDb
//       Error if the operation fails
}



InitStatus R3BPtofMapped2Cal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}


 
void R3BPtofMapped2Cal::Exec(Option_t* option)
{
/* ToDo: loop over all mapped items and convert all times to ns. 
 * 
 * Remember: Each mapped item contains 4 times: leading and trailing edge 
 * for PM 1 and PM 2. A time in mapped level consists of a coarse clock 
 * time and a fine TDC time.
 * 
 * For each time: 
 * Fetch the required calibration parameter from the fTcalPar container.
 * The containers GetModuleParAt(...) function returns an object of type
 * R3BTCalModulePar that you can use to convert the time.
 * 
 * Convert the fine time to ns by calling R3BTCalModulePar's GetTimeVFTX(...) 
 * 
 * Add the coarse time (in ns) to the converted fine time
 * 
 * Make sure the final time is stored in an object of type R3BPaddleCalData
 * which is added to the output array fCalItems. Remember that also the
 * output object R3BPaddleCalData contains 4 times!
 * 
 * Don't forget to handle errors.
 */
 
	// loop over all mapped items:
	for (all items in fMappedItems)
	{
		// get pointers to the data we work with:
		mapped = current mapped item we like to convert;
		cal = NULL; // will later hold the CAL item
		
		// fetch plane and bar from mapped item for later usage:
		iPlane = ...
		iBar = ...
 
		// loop over all times of the current mapped item:
		for (each PM) // PM 1 and PM 2
			for (each edge) // leading and trailing edge
			{
				// make sure this time is present (non-zero) in mapped item
				if (time not present) continue;
				
				// fetch calib parameter. Handle error if needed.
				par = ...;
				
				// now we have a time and a calib parameter: Ready to go!
				// create a new cal item if needed:
				if (!cal) cal = a new R3BPaddleCalData(iPlane,iBar);
				
				// convert fine time. Feel free to check the result for
				// reasonable values
				time_ns = ...
				
				// add coarse time
				time_ns = fClockFreq - time_ns + coarseTime * fClockFreq;
				
				// store time in cal item
				cal->SetTime(...)
			}
	}


/* You may find the following functions helpful:
 * 
 * fMappedItems->GetEntriesFast()
 * 		returns the number of items in the array
 * 
 * (R3BPaddleTamexMappedData*)fMappedItems->At( #hitNumber )
 * 		returns a pointer to the item at position #hitNumber in the array
 * 
 * mapped->GetPlaneId()
 * mapped->GetBarId()
 * mapped->GetFineTime(tube , edge)
 * 		see: R3BRoot/r3bdata/neulandData/R3BPaddleTamexMappedData.h
 * 
 * fTcalPar->GetModuleParAt(iPlane, iBar, tube*2 + edge + 1)
 * 		returns a pointer to the corresponding R3BTCalModulePar
 * 		see: R3BRoot/tcal/R3BTCalPar.h and
 * 		     R3BRoot/tcal/R3BTCalModulePar.h
 * 
 * new ((*fCalItems)[fNofCalItems]) R3BPaddleCalData(iPlane,iBar)
 * 		returns a pointer to a new item of type R3BPaddleCalData at 
 * 		position fNofCalItems in the array fCalItems. 
 * 
 * par->GetTimeVFTX( fineTime )
 * 		see: R3BRoot/tcal/R3BTCalModulePar.h
 * 
 * cal->SetTime(tube , edge , time_ns)
 * 		see: R3BRoot/r3bdata/tofData/R3BPaddleCalData.h
 * 
 * LOG(INFO) << "some text" << FairLogger::endl
 * LOG(ERROR) << "some text" << FairLogger::endl
*/
}

void R3BPtofMapped2Cal::FinishEvent()
{
// ToDo: clear all calItems (if any)
}

void R3BPtofMapped2Cal::FinishTask()
{
// Nothing to be done here
}

ClassImp(R3BPtofMapped2Cal)

// ToDo: update CMakeLists.txt and TofLinkDef.h in the current folder
// (R3BRoot/tof)
