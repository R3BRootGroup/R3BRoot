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
  fTOffset1.Set(12);
  fTOffset2.Set(12);
  fZScale.Set(12);
  fVEff.Set(12);
}

R3BPtofCal2Hit::R3BPtofCal2Hit(const char* name, Int_t iVerbose)
	: FairTask(name, iVerbose)
	, fCalItems(NULL)
	, fHitItems(new TClonesArray("R3BPtofHit"))
	, fHitPar(NULL)
{
  fTOffset1.Set(12);
  fTOffset2.Set(12);
  fZScale.Set(12);
  fVEff.Set(12);
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

   fMan->Register ("PtofHit", "Ptof", fHitItems, kTRUE);
    
   SetParameter();
   return kSUCCESS;
}

void R3BPtofCal2Hit::SetParContainers() {
   fHitPar = (R3BPtofHitPar*) FairRuntimeDb::instance()->getContainer("PtofHitPar");;
}


void R3BPtofCal2Hit::SetParameter(){

  for(Int_t paddle = 0; paddle < 12; paddle++){
    fTOffset1[paddle] = fHitPar->GetTOffsetAt(paddle+1, 1);
    fTOffset2[paddle] = fHitPar->GetTOffsetAt(paddle+1, 2);
    fZScale[paddle] = fHitPar->GetZScaletAt(paddle+1);
    fVEff[paddle] = fHitPar->GetVEfftAt(paddle+1);
  }
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
  
  const Double_t plane_offset = 0.0;  //cm
  
  Int_t nItems  = fCalItems->GetEntriesFast();
  Int_t nHitData = 0;
  
  for(Int_t i = 0; i < nItems; i++){
   R3BPaddleCalData* caldata = (R3BPaddleCalData*) fCalItems->At(i); 
   
   Int_t id = (caldata->GetPlane()-1)*6 + caldata->GetBar() - 1;
   
   Double_t t1 = caldata->fTime1L_ns + fTOffset1[id];
   Double_t t2 = caldata->fTime2L_ns + fTOffset2[id];
   Double_t time = (t1+t2)*0.5;
   
   Double_t charge = sqrt((caldata->fTime1T_ns - caldata->fTime1L_ns)*(caldata->fTime2T_ns - caldata->fTime2L_ns))*fZScale[id];
   
   Double_t xx = (id%6 - 3 + (id<6 ? 0 : -0.5))*paddle_width;
   Double_t yy = (t2-t1)*fVEff[id];
   Double_t zz = paddle_depth * 0.5 + (id < 6 ? 0 : paddle_depth + plane_offset)  ;
   
   new ((*fHitItems)[nHitData++]) R3BPtofHit(id+1, charge, time, xx, yy, zz);
  }
}

void R3BPtofCal2Hit::FinishEvent()
{
  fHitItems->Clear();
}

void R3BPtofCal2Hit::FinishTask()
{
}

ClassImp(R3BPtofCal2Hit)
