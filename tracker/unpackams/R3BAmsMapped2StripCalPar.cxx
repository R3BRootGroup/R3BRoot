// -------------------------------------------------------------------------
// -----         R3BAmsMapped2StripCalPar source file                  -----
// -----             Created 29/05/18  by J.L. Rodriguez-Sanchez       -----
// -------------------------------------------------------------------------
#include "R3BAmsMappedData.h"
#include "R3BAmsMapped2StripCalPar.h"
#include "R3BEventHeader.h"
#include "R3BAmsStripCalPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TGeoManager.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TF1.h"
#include "TMath.h"
#include "TVector3.h"
#include "TGeoMatrix.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

//R3BAmsMapped2StripCalPar: Default Constructor --------------------------
R3BAmsMapped2StripCalPar::R3BAmsMapped2StripCalPar() :
  FairTask("R3B AMS Pedestal Finder ",1),
  fStrip_Par(NULL),
  fAmsMappedDataCA(NULL),
  fNumDets(0),
  fNumStrips(0),
  fNumStripsS(0),
  fNumStripsK(0),
  fMaxSigma(5),
  fMinStadistics(0),
  fMapHistos_left(0),
  fMapHistos_right(0),
  fMapHistos_bins(0),
  fSigma(0),
  fMean(0),
  fOutputFile(NULL) {
  
}

//R3BAmsMapped2StripCalPar: Standard Constructor --------------------------
R3BAmsMapped2StripCalPar::R3BAmsMapped2StripCalPar(const char* name, Int_t iVerbose) :
  FairTask(name, iVerbose),
  fStrip_Par(NULL),
  fAmsMappedDataCA(NULL),
  fNumDets(0),
  fNumStrips(0),
  fNumStripsS(0),
  fNumStripsK(0),
  fMaxSigma(5),
  fMinStadistics(0),
  fMapHistos_left(0),
  fMapHistos_right(0),
  fMapHistos_bins(0),
  fSigma(0),
  fMean(0),
  fOutputFile(NULL) {
  
}

//R3BAmsMapped2StripCalPar: Destructor ----------------------------------------
R3BAmsMapped2StripCalPar::~R3BAmsMapped2StripCalPar() {
  
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAmsMapped2StripCalPar::Init() {

  LOG(INFO) << "R3BAmsMapped2StripCalPar: Init" << FairLogger::endl;

  char name[100];
  
  fh_Map_energy_strip = new TH1F*[fNumStrips*fNumDets];
  for(Int_t i=0;i<fNumStrips*fNumDets;i++){
    sprintf(name,"fh_Map_energy_strip_%i",i+1);
    fh_Map_energy_strip[i] = new TH1F(name,name,fMapHistos_bins,fMapHistos_left,fMapHistos_right);
  }
  
  FairRootManager* rootManager = FairRootManager::Instance();
  if (!rootManager) { return kFATAL;}
  
  fAmsMappedDataCA = (TClonesArray*)rootManager->GetObject("AmsMappedData");
  if (!fAmsMappedDataCA) { return kFATAL;}
  
  FairRuntimeDb* rtdb = FairRuntimeDb::instance();
  if (!rtdb) { return kFATAL;}
  
  fStrip_Par=(R3BAmsStripCalPar*)rtdb->getContainer("amsStripCalPar");
  if (!fStrip_Par) {
    LOG(ERROR)<<"R3BAmsMapped2StripCalPar::Init() Couldn't get handle on amsStripCalPar container"<<FairLogger::endl;
    return kFATAL;
  }
  
  return kSUCCESS;
}

// -----   Public method ReInit   --------------------------------------------
InitStatus R3BAmsMapped2StripCalPar::ReInit() {
  
  // MOVE PAR SETTINGS IN INIT TO SETPARCONTAINERS AND CALL ALSO IT HERE
  // SetParContainers();
  
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void R3BAmsMapped2StripCalPar::Exec(Option_t* opt) {
  
  Int_t nHits = fAmsMappedDataCA->GetEntries();
  if(!nHits) return;
  
  R3BAmsMappedData* MapHit;
  Int_t detId, stripId;
  
  for(Int_t i = 0; i < nHits; i++) {
    MapHit = (R3BAmsMappedData*)(fAmsMappedDataCA->At(i));
    detId = MapHit->GetDetectorId();
    stripId = MapHit->GetStripId();
    
    //Fill Histos
    fh_Map_energy_strip[stripId+fNumStrips*detId]->Fill(MapHit->GetEnergy());
  }
}

// ---- Public method Reset   --------------------------------------------------
void R3BAmsMapped2StripCalPar::Reset() {
  
}

void R3BAmsMapped2StripCalPar::FinishEvent() {
}

// ---- Public method Finish   --------------------------------------------------
void R3BAmsMapped2StripCalPar::FinishTask() {
  
  SearchPedestals();
  //fStrip_Par->printParams();
}

//------------------
void R3BAmsMapped2StripCalPar::SearchPedestals(){

  LOG(INFO) << "R3BAmsMapped2StripCalPar: Search pedestals" << FairLogger::endl;

  Int_t numPars =3;// by default number of parameters=3
  
  fStrip_Par->SetNumDets(fNumDets);
  fStrip_Par->SetNumStrips(fNumStrips);
  fStrip_Par->SetNumStripsS(fNumStripsS);
  fStrip_Par->SetNumStripsK(fNumStripsK);
  fStrip_Par->GetStripCalParams()->Set(numPars*fNumStrips*fNumDets);

  for(Int_t d=0;d<fNumDets;d++){
   for (Int_t i=0;i<fNumStrips;i++){
    
    if (fh_Map_energy_strip[i+d*fNumStrips]->GetEntries()>fMinStadistics){
      
      TF1 *f1 = new TF1 ("f1", "gaus", fMapHistos_left, fMapHistos_right);
      f1->SetParameter(1,100.);      
      f1->SetParameter(2,2.);

      fh_Map_energy_strip[i+d*fNumStrips]->Fit("f1","RQ0");

      //Fill container:
      fStrip_Par->SetStripCalParams(f1->GetParameter(0),numPars*i+d*numPars*fNumStrips);
      if(f1->GetParameter(2)<fMaxSigma){
       fStrip_Par->SetStripCalParams(f1->GetParameter(1),numPars*i+d*numPars*fNumStrips+1);
      }else{
       fStrip_Par->SetStripCalParams(-1,numPars*i+d*fNumStrips+1);//dead strip
      }
      fStrip_Par->SetStripCalParams(f1->GetParameter(2),numPars*i+d*numPars*fNumStrips+2);

    }else {
      fStrip_Par->SetStripCalParams(-1,numPars*i+d*numPars*fNumStrips+1);//dead strip
      fStrip_Par->SetStripCalParams(0,numPars*i+d*numPars*fNumStrips+2);
      LOG(WARNING)<<"Histogram NO Fitted, detector: " << d+1 << ", strip: "<< i+1 <<FairLogger::endl;
    }
   }
  }
  
  fStrip_Par->setChanged();
  return;

}

ClassImp(R3BAmsMapped2StripCalPar)
