// ----------------------------------------------------------------
// -----            R3BPtofMapped2TCalPar (TAMEX)             -----
// -----                 Folder: R3BRoot/tof                  -----
// ----------------------------------------------------------------


#include "R3BPtofMapped2CalPar.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BEventHeader.h"
#include "R3BTCalPar.h"
#include "R3BTCalEngine.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunIdGenerator.h"
#include "FairRtdbRun.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TF1.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

/* ToDo:
 * 
 * We need to establish a new parameter container for the Ptof:
 * Open file R3BRoot/tcal/R3BTCalContFact.cxx
 * See how and where the containers are created
 * Add a container for the Ptof
 * 
 */


R3BPtofMapped2CalPar::R3BPtofMapped2CalPar()
    : FairTask("R3BPtofMapped2CalPar", 1)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fCal_Par(NULL)
{
}

R3BPtofMapped2CalPar::R3BPtofMapped2CalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fUpdateRate(1000000)
    , fMinStats(100000)
    , fCal_Par(NULL)
{
}

R3BPtofMapped2CalPar::~R3BPtofMapped2CalPar()
{
// ToDo: Free the memory of the container and the engine
}
 
InitStatus R3BPtofMapped2CalPar::Init()
{
/* ToDo: Prepare all we need:
 * - get access to the mapped data
 * - get access to the calibration parameter container (to be filled)
 * - create the TCalEngine which produces the calibration parameters
 * Remember: you need to return either kSUCCESS or kFATAL
 * 
 * 
 * Convert the following pseudo code into real C++:
 */

	get access to the FairRootManager
	
	// get access to Mapped data
	fMappedItems = ...
	if (access to mapped items failed)
		print an error and fail 

	// get access to the parameter container
    fCal_Par = ...
	
	// tag the container as "changed" (to force writing to disk?)
	fCal_Par->setChanged();

	// create an instance of the TCalEngine
	fEngine = ...

	return kSUCCESS;


/* You may find the following functions helpful:
 * 
 * FairRootManager::Instance();
 * 		returns a pointer to an object of type FairRootManager
 * 
 * fairRootManager->GetObject("PtofMapped");
 * 		returns a pointer to a TClonesArray holding "PtofMapped" data
 * 
 * (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("PtofTCalPar");
 * 		returns a pointer of type R3BTCalPar* to the container called "PtofTCalPar"  
 *
 * new R3BTCalEngine(fCal_Par, fMinStats);
 * 		creates a new TCalEngine
 * 
 */
}

void R3BPtofMapped2CalPar::Exec(Option_t* option)
{
/* ToDo: Loop over all mapped items and fill the fine times into the
 * TCalEngine. That's all :-)
 * We do not need to care about the writing of the container to disk.
 * This is done in the macro.
 * 
 * 
 * Convert the following pseudo code into real C++:
 */


	// loop over all mapped items:
	for (all items in fMappedItems)
	{
		// get pointers to the data we work with:
		mapped = current mapped item we like to convert;

		// fetch plane and bar from mapped item for later usage:
		iPlane = ...
		iBar = ...

		// feel free to check plane and bar for reasonable values
		// ...

		// fill all four edges into the TcalEngine
		...
	}

/* You may find the following functions helpful:
 * 
 * fMapped->GetEntries();
 *		returns the number of items in the array

 * (R3BPaddleTamexMappedData*)fMapped->At( i );
 * 		returns a pointer to the item at position i in the array
 *
 * mapped->GetPlaneId()
 * mapped->GetBarId()
 * mapped->GetFineTime(tube , edge)
 * 		see: R3BRoot/r3bdata/neulandData/R3BPaddleTamexMappedData.h
 * 
 * fEngine->Fill(iPlane, iBar, iEdge, fineTime)
 * 		fills fineTime into the TcalEngine for plane iPlane and bar iBar
 * 		Convention for iEdge:
 * 		1: PM1 leading edge
 * 		2: PM1 trailing edge
 * 		3: PM2 leading edge
 * 		4: PM2 trailing edge
 * 
 * mapped->GetFineTime(pm,edge)
 * 		see: R3BRoot/r3bdata/neulandData/R3BPaddleTamexMappedData.h 
 */
}

void R3BPtofMapped2CalPar::FinishEvent()
{
}

void R3BPtofMapped2CalPar::FinishTask()
{
	fEngine->CalculateParamVFTX();
	fCal_Par->printParams();
}

ClassImp(R3BPtofMapped2CalPar)

// ToDo: update CMakeLists.txt and TofLinkDef.h in the current folder
// (R3BRoot/tof)

