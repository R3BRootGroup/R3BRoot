// ------------------------------------------------------------
// -----                  R3BPtofCal2Hit                  -----
// -----          Created Mar 1st 2017 by V.Wagner        -----
// ------------------------------------------------------------

#include "R3BPtofCal2Hit.h"
#include "R3BPtofHitPar.h"
#include "R3BPaddleCalData.h"
#include "R3BPtofHit.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TArrayD.h"
#include "TClonesArray.h"
#include "TMath.h"

R3BPtofCal2Hit::R3BPtofCal2Hit()
	: FairTask("TofdTcal", 1)
	, fCalItems(NULL)
	, fHitItems(new TClonesArray("R3BPtofHit"))
	, fHitPar(NULL)
{
}

R3BPtofCal2Hit::R3BPtofCal2Hit(const char* name, Int_t iVerbose)
	: FairTask(name, iVerbose)
	, fCalItems(NULL)
	, fHitItems(new TClonesArray("R3BPtofHit"))
	, fHitPar(NULL)
{
}

R3BPtofCal2Hit::~R3BPtofCal2Hit()
{
	delete fHitItems;
	fHitItems = NULL;
}

InitStatus R3BPtofCal2Hit::Init() {
   FairRootManager* fMan = FairRootManager::Instance();
   if (!fMan) {
      FairLogger::GetLogger()->Fatal (MESSAGE_ORIGIN, "FairRootManager not found");
   }

   fCalItems = (TClonesArray*) fMan->GetObject ("PtofCal");
   if (!fCalItems) {
      FairLogger::GetLogger()->Fatal (MESSAGE_ORIGIN, "PtofCalData not found");
   }

   // TODO Register the Hit-Data-Branch
    
   SetParameter();
   return kSUCCESS;
}

void R3BPtofCal2Hit::SetParContainers() {
   fHitPar = (R3BPtofHitPar*) FairRuntimeDb::instance()->getContainer("PtofHitPar");;
}


void R3BPtofCal2Hit::SetParameter(){
  //TODO set the parameters to the local fields
}

InitStatus R3BPtofCal2Hit::ReInit() {
   SetParContainers();
   SetParameter();
   return kSUCCESS;
}


 
void R3BPtofCal2Hit::Exec(Option_t* option)
{
  const Double_t paddle_width = 27.0;  //cm
  const Double_t paddle_height = 80.0; //cm
  const Double_t paddle_depth = 0.5;  //cm

  
  Int_t nHitData = 0; // the position of the next HitData inside our TClonesArray -- see below
  
  // iterate over cal data and use the parameters to obtain hit-level-data  
  
  // use the mean of the leading edge times as the time
  
  // create a new instance of R3BPtofHit with our data and store it inside the TClonesArray:
  // new ((*fHitItems)[nHitData++]) R3BPtofHit(id, charge, time, x, y, z);
}

void R3BPtofCal2Hit::FinishEvent()
{
  fHitItems->Clear();
}

void R3BPtofCal2Hit::FinishTask()
{
}

ClassImp(R3BPtofCal2Hit)
