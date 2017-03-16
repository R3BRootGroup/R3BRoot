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
  if(fData.size() > 0)
    for(Int_t plane = fPlanes - 1; plane >= 0; plane--)
      for(Int_t bar = fPaddles - 1; bar >= 0; bar--)
	for(Int_t side = 1; side <=2; side++)
	  fData.at(plane).at(bar).at(side)->Delete();
    
  
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
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, " Branch: NeulandMappedData not found in Tree.");
    return kFATAL;
  } 
  header = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
  if (!header)
  {
    FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, " Branch: R3BEventHeader not found in Tree.");
    return kFATAL;
  }
  
  TH1F* histo;

  for(Int_t plane = 1; plane <= fPlanes; plane++){
    std::vector<std::vector<TH1F*>> v_plane;
    for(Int_t bar = 1; bar <= fPaddles; bar++){     
      std::vector<TH1F*> v_bar;
      for(Int_t side = 1; side <= 2; side++){
	std::string s = "h_pedpar_" + std::to_string(plane) + "_" + std::to_string(bar) + "_" + std::to_string(side);
	histo = new TH1F(s.c_str(), "PedPar", 2001, -0.5, 2000.5);
	v_bar.push_back(histo);
      }
      v_plane.push_back(v_bar);
    }
    fData.push_back(v_plane);
  }
  fPar = (R3BNeulandQCalPar*) FairRuntimeDb::instance()->getContainer("NeulandQCalPar");
  
  return kSUCCESS;
}

void R3BNeulandMapped2QCalPar::Exec(Option_t* option)
{  
  R3BNeulandMappedData* hit;
  Int_t nHits = fHits->GetEntries();
  
  if (header->GetTrigger() != 2)
    return;
  
  if (nHits < nPMTs)
    return;
  
  for(Int_t i = 0; i < nHits; i++){
    hit = (R3BNeulandMappedData*)fHits->At(i);
    if (!hit)
      continue;

    fData.at(hit->GetPlane() - 1).at(hit->GetPaddle() - 1).at(hit->GetSide() - 1)->Fill(hit->GetQdcData());
  }
}

void R3BNeulandMapped2QCalPar::FinishTask()
{
  Int_t i = 0;
  TH1F* pars = new TH1F("QCalPar", "Pedestal Offset", nPMTs, 0.5, nPMTs + 0.5);
  TH1F* histo;
  fPar->SetSize(nPMTs);
  for(Int_t plane = 1; plane <= fPlanes; plane++)
    for(Int_t bar = 1; bar <= fPaddles; bar++)
      for(Int_t side = 1; side <= 2; side++){
	histo = fData.at(plane-1).at(bar-1).at(side-1);
	Int_t maxBinPos = histo->GetBinCenter(histo->GetMaximumBin());
	pars->SetBinContent(i++ + 1, maxBinPos);
	fPar->SetParAt(plane, bar, side, maxBinPos);
      }
  pars->Write();
  fPar->setChanged();
}

ClassImp(R3BNeulandMapped2QCalPar)
