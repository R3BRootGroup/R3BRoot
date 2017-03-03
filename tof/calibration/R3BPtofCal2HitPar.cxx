#include "R3BPtofCal2HitPar.h"
#include "R3BPaddleCalData.h"
#include "R3BPtofHitPar.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1D.h"
#include "TF1.h"

using namespace std;

R3BPtofCal2HitPar::R3BPtofCal2HitPar()
    : FairTask("R3BPtofCal2HitPar", 1)
    , fBeamCharge(6)
    , fYPosition(0)
    , fCalData(NULL)
    , fPar(NULL)
    , fNEvents(0)
{
}

R3BPtofCal2HitPar::R3BPtofCal2HitPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fBeamCharge(6)
    , fYPosition(0)
    , fCalData(NULL)
    , fNEvents(0)
    , fPar(NULL)
{
}

R3BPtofCal2HitPar::~R3BPtofCal2HitPar()
{
  if(fZScaleHistos[0])
    for(Int_t i = 0; i < 12; i++){
      delete fZScaleHistos[i];
      delete fTOffsetHistos[i];
    }
  
  if(fPar)
    delete fPar;
}

InitStatus R3BPtofCal2HitPar::Init()
{	
   FairRootManager* fMan = FairRootManager::Instance();

   if (! fMan) {
      FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found!");
      return kFATAL;
   }

   fCalData = (TClonesArray*) fMan->GetObject("PtofCal");
   if (NULL == fCalData) {
      FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch PtofCal not found!");
      return kFATAL;
   }
   
  fPar = (R3BPtofHitPar*) FairRuntimeDb::instance()->getContainer("PtofHitPar");
  if (NULL == fPar) {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "PtofHitPar not found!");
    return kFATAL;
  }
   
  for(Int_t i = 0; i < 12; i++){
    std::string str1 = "h_Module" + std::to_string(i) + "_ZScale";
    const char* c1 = str1.c_str();
    fZScaleHistos[i] = new TH1D(c1, c1, 10000, 0, 100);

    std::string str2 = "h_Module" + std::to_string(i) + "TOffset";
    const char* c2 = str2.c_str();
    fTOffsetHistos[i] = new TH1D(c2, c2, 20000, -100, 100);
  }
   
   return kSUCCESS;
}

void R3BPtofCal2HitPar::Exec(Option_t* option)
{  
  Int_t nData = fCalData->GetEntriesFast();
  
  for(Int_t i = 0; i < nData; i++){
    R3BPaddleCalData* cdata = (R3BPaddleCalData*)fCalData->At(i);
    Int_t id = (cdata->GetPlane()-1)*6 + cdata->GetBar() - 1;
    
    Double_t Energy = sqrt((cdata->fTime1T_ns-cdata->fTime1L_ns)*(cdata->fTime2T_ns-cdata->fTime2L_ns));
    Double_t TDiff = cdata->fTime2L_ns - cdata->fTime1L_ns;
    
    fZScaleHistos[id]->Fill(Energy);
    fTOffsetHistos[id]->Fill(TDiff);
  }
  
}

void R3BPtofCal2HitPar::FinishEvent()
{
}

void R3BPtofCal2HitPar::FinishTask()
{
  TF1 g = TF1("GAUS", "gaus(0)");
  
  for(Int_t i = 0; i < 12; i++){
    
    Double_t max, scale = 0, offset = 0;
    
    if(fZScaleHistos[i]->GetEntries() > 10000){
      max = fZScaleHistos[i]->GetBinCenter(fZScaleHistos[i]->GetMaximumBin());
      g.SetParameter(1, max);
      fZScaleHistos[i]->Fit(&g, "qn", "", max - 1, max + 1);
      scale = fBeamCharge/g.GetParameter(1);
      
      max = fTOffsetHistos[i]->GetBinCenter(fTOffsetHistos[i]->GetMaximumBin());
      g.SetParameter(1, max);
      fTOffsetHistos[i]->Fit(&g, "qn", "", max - 0.2, max + 0.2);
      offset = g.GetParameter(1);
    }
    
    fPar->SetParAt(i+1, 0.5*offset, -0.5*offset, scale, 6.5);
  }
  fPar->setChanged();
}

ClassImp(R3BPtofCal2HitPar)
