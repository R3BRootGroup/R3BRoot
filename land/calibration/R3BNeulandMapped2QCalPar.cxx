#include "R3BNeulandMapped2PedPar.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "R3BNeulandMappedData.h"
#include "R3BTCalEngine.h"
#include "R3BEventHeader.h"
#include "R3BNeulandQCalPar.h"
#include "R3BNeulandMapped2QCalPar.h"
#include "FairRuntimeDb.h"

#define nPMTs 2*fPaddles*fPlanes

R3BNeulandMapped2QCalPar::R3BNeulandMapped2QCalPar(const char* name, Int_t iVerbose)
:FairTask(name, iVerbose)
   , fPar(NULL)
   , fHits(NULL) 
{

}

R3BNeulandMapped2QCalPar::R3BNeulandMapped2QCalPar()
:FairTask("R3BNeulandMapped2QCalPar")
   , fPar(NULL)
   , fHits(NULL) 
{

}

R3BNeulandMapped2QCalPar::~R3BNeulandMapped2QCalPar()
{
  for(Int_t i = fData.size()-1; i > 0 ; i++){
    fData.at(i)->Delete();
  }
  
  if (fPar)
    delete fPar;
}

InitStatus R3BNeulandMapped2QCalPar::Init()
{
  FairRootManager* fMan = FairRootManager::Instance();
  if (! fMan) {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, " FairRootManager not found.");
    return kFATAL;
  }
  fHits = (TClonesArray*) fMan->GetObject("NeulandMappedData");
  if (!fHits) {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, " Branch: NeulandCalData not found in Tree.");
    return kFATAL;
  } 
  header = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
  if (!header)
  {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, " Branch: R3BEventHeader not found in Tree.");
    return kFATAL;
  }
  
  TH1F* histo;

  for(Int_t i = 0; i < nPMTs; i++){
    std::string s = "h_pedpar_" + std::to_string(i+1);
    histo = new TH1F(s.c_str(), "PedPar", 2001, -0.5, 2000.5);
    fData.push_back(histo);
  }
  
  fPar = (R3BNeulandQCalPar*) FairRuntimeDb::instance()->getContainer("NeulandQCalPar");
  
  return kSUCCESS;
}

void R3BNeulandMapped2QCalPar::Exec(Option_t* option)
{  
  R3BNeulandMappedData* hit;
  Int_t nHits = fHits->GetEntries();
  Int_t iBar;
  Int_t iSide;
  
  if (header->GetTrigger() != 2)
    return;
  
  if (nHits < nPMTs)
    return;
  
  for(Int_t i = 0; i < nHits; i++){
    hit = (R3BNeulandMappedData*)fHits->At(i);
    if (!hit)
      continue;
    
    iBar = (hit->GetPlane() - 1) * fPaddles + hit->GetPaddle() - 1;
    iSide = hit->GetSide();

    if(iBar > 0)
      fData.at(iBar * 2 + iSide - 1)->Fill(hit->GetQdcData());
  }
}

void R3BNeulandMapped2QCalPar::FinishTask()
{
  TH1F* pars = new TH1F("QCalPar", "Pedestal Offset", nPMTs, 0.5, nPMTs + 0.5);
  
  fPar->SetSize(nPMTs);
  for(Int_t i = 0; i < nPMTs; i++){
    pars->SetBinContent(i + 1, fData.at(i)->GetMaximumBin());
    fPar->SetParAt(i,fData.at(i)->GetMaximumBin());
  }  
  pars->Write();
  fPar->setChanged();
}

ClassImp(R3BNeulandMapped2QCalPar)
