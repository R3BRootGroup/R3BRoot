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
   
  //TODO: get the pointer to the parameter-container "PtofHitPar" and store it in fPar (do not forget to typecast!)
  // The parameter-contrainer are handled by the singleton class FairRuntimeDb
   
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
  
  //TODO iterate of the Cal-Data and determine the energy and timeoffset for each event.
  // Fill the data into the histograms
  
}

void R3BPtofCal2HitPar::FinishEvent()
{
}

void R3BPtofCal2HitPar::FinishTask()
{
  
  // TODO determine the parameters:
  // you can get the maximum bin of a histogram with "->GetMaximumBin()" and the position of a bin with "->GetBinCenter(binNumber)"
  
  
  // for a more precise parameter you can make a gaus fit around the maximum and use the 
  // TF1("GAUS", "gaus(0)"); a gaus function parameters:   0: ampl   1: mean   2: sigma
  // histo->Fit(pointerToGausFunction, "qn", "", center - 1, center + 1)   
  //      Fits the histogram "histo" with a gaus function
  //      "qn" suppresses output
  //      the last two parameter determine the fit range
  // the fit paramter will be stored in the funtction and can be accessed with "->GetParameter(n)"  where n is the parameter index 
  

  //fPar->setChanged() hast to be called so the paramters will be stored
  fPar->setChanged();
}

ClassImp(R3BPtofCal2HitPar)
