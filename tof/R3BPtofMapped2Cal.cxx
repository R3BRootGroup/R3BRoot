// ------------------------------------------------------------
// -----                  R3BPtofMapped2TCal                -----
// -----          Created Feb 9th 2017 by R.Plag          -----
// ------------------------------------------------------------

/* March 2017
 * Reconstructing data into PaddleItems. Each paddle holds two times,
 * (for PM1 and PM2). This allows to plot
 * the time differences e.g. via evt->Draw(...) interactively (aka without
 * looping over all channels) which is crucial for a quick check of the
 * detector status during the experiment.
 *  
 * This code was copied from Tofd. Differences:
 * - how many planes?
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

#define IS_NAN(x) TMath::IsNaN(x)

R3BPtofMapped2Cal::R3BPtofMapped2Cal()
	: FairTask("TofdTcal", 1)
	, fMappedItems(NULL)
	, fCalItems(new TClonesArray("R3BPaddleCalData"))
	, fNofCalItems(0)
	, fNofTcalPars(0)
	, fTcalPar(NULL)
	, fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BPtofMapped2Cal::R3BPtofMapped2Cal(const char* name, Int_t iVerbose)
	: FairTask(name, iVerbose)
	, fMappedItems(NULL)
	, fCalItems(new TClonesArray("R3BPaddleCalData"))
	, fNofCalItems(0)
	, fNofTcalPars(0)
	, fTcalPar(NULL)
	, fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
}

R3BPtofMapped2Cal::~R3BPtofMapped2Cal()
{
	delete fCalItems;
	fCalItems = NULL;
	fNofCalItems = 0;
}

InitStatus R3BPtofMapped2Cal::Init()
{
	if (fTcalPar == NULL)
	{
		LOG(ERROR) << "Have no TCal parameter container" << FairLogger::endl;
		return kFATAL; 
	}

	fNofTcalPars = fTcalPar->GetNumModulePar();
	if (fNofTcalPars == 0)
	{
		LOG(ERROR) << "No TCal parameters in container PtofTCalPar" << FairLogger::endl;
		return kFATAL;
	}

	LOG(INFO) << "R3BPtofMapped2Cal::Init : read " << fNofTcalPars << " modules" << FairLogger::endl;

	FairRootManager* mgr = FairRootManager::Instance();
	if (NULL == mgr)
		FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");

	// get access to Mapped data
	fMappedItems = (TClonesArray*)mgr->GetObject("PtofMapped");
	if (NULL == fMappedItems)
		FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch PtofMapped not found");


	// request storage of Cal data in output tree
	mgr->Register("PtofCal", "Land", fCalItems, kTRUE);

	return kSUCCESS;
}



// Note that the container may still be empty at this point.
void R3BPtofMapped2Cal::SetParContainers()
{
	fTcalPar = (R3BTCalPar*)FairRuntimeDb::instance()->getContainer("PtofTCalPar");
	if (!fTcalPar)
		LOG(ERROR) << "Could not get access to PtofTCalPar-Container." << FairLogger::endl;
}



InitStatus R3BPtofMapped2Cal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}


 
void R3BPtofMapped2Cal::Exec(Option_t* option)
{
	Int_t nHits = fMappedItems->GetEntriesFast();

	for (Int_t ihit = 0; ihit < nHits; ihit++)
	{
		R3BPaddleTamexMappedData* mapped = (R3BPaddleTamexMappedData*)fMappedItems->At(ihit);
		R3BPaddleCalData*            cal = NULL;

		Int_t iPlane = mapped->GetPlaneId(); // 1..n; no need to check range
		Int_t iBar   = mapped->GetBarId();   // 1..n

		// convert times to ns
		for (int tube=0;tube<2;tube++) // PM1 and PM2
			for (int edge=0;edge<2;edge++) // loop over leading and trailing edge
			{
				// check if there is indeed data for this tube and edge:
				if (mapped->GetFineTime(tube , edge)==0) continue;

				// fetch calib params:
				R3BTCalModulePar* par = fTcalPar->GetModuleParAt(iPlane, iBar, tube*2 + edge + 1); // 1..4
				if (!par)
				{
					LOG(INFO) << "R3BPtofMapped2Cal::Exec : Tcal par not found, Plane: " << 
					iPlane << ", Bar: " << iBar << ", Tube: " << (tube+1) << FairLogger::endl;
					continue;
				}

				// create CAL item only if data AND calib params are available:
				if (!cal) 
					cal = new ((*fCalItems)[fNofCalItems]) R3BPaddleCalData(iPlane,iBar);


				// Convert TDC to [ns] ...
				Double_t time_ns = par->GetTimeVFTX( mapped->GetFineTime(tube , edge) );

				if (time_ns < 0. || time_ns > fClockFreq )
				{
					LOG(ERROR) << 
					"R3BPtofMapped2Cal::Exec : bad time in ns: Plane= " << iPlane << 
					", bar= " << iBar << ",tube= " << (tube+1) <<
					", time in channels = " << mapped->GetFineTime(tube,edge) <<
					", time in ns = " << time_ns  << FairLogger::endl;
					continue;
				}
		
				// ... and add clock time
				time_ns = fClockFreq - time_ns + mapped->GetCoarseTime(tube , edge) * fClockFreq;

				cal->SetTime(tube , edge , time_ns);
			}
	}
}

void R3BPtofMapped2Cal::FinishEvent()
{
	if (fCalItems)
	{
		fCalItems->Clear();
		fNofCalItems = 0;
	}
}

void R3BPtofMapped2Cal::FinishTask()
{
}

ClassImp(R3BPtofMapped2Cal)
