// -------------------------------------------------------------------------
// -----         R3BCalifaMapped2CrystalCalPar source file             -----
// -----             Created 22/07/14  by H.Alvarez                    -----
// -----            Modified 20/03/17  by P.Cabanelas                  -----
// -----            Modified 11/12/17  by E.Galiana                    -----
// -------------------------------------------------------------------------
#include "R3BCalifaMappedData.h"
#include "R3BCalifaMapped2CrystalCalPar.h"
#include "R3BEventHeader.h"
#include "R3BCalifaCrystalCalPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
//#include "FairRunIdGenerator.h"
//#include "FairRtdbRun.h"
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
#include "TSpectrum.h"
#include "TGraph.h"
#include "TF1.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

//R3BCalifaMapped2CrystalCalPar: Default Constructor --------------------------
R3BCalifaMapped2CrystalCalPar::R3BCalifaMapped2CrystalCalPar() :
  FairTask("R3B CALIFA Calibration Parameters Finder ",1),
  fCal_Par(NULL),
  fCalifaMappedDataCA(NULL),
  fNumCrystals(0),
  fNumParam(0),
  fMinStadistics(0),
  fMapHistos_left(0),
  fMapHistos_right(0),
  fMapHistos_bins(0),
  fNumPeaks(0),
  fSigma(0),
  fThreshold(0),
  fEnergyPeaks(NULL),
  fOutputFile(NULL),
  fDebugMode(0)
{
  
}

//R3BCalifaMapped2CrystalCalPar: Standard Constructor --------------------------
R3BCalifaMapped2CrystalCalPar::R3BCalifaMapped2CrystalCalPar(const char* name, Int_t iVerbose) :
  FairTask(name, iVerbose),
  fCal_Par(NULL),
  fCalifaMappedDataCA(NULL),
  fNumCrystals(0),
  fNumParam(0),
  fMinStadistics(0),
  fMapHistos_left(0),
  fMapHistos_right(0),
  fMapHistos_bins(0),
  fNumPeaks(0),
  fSigma(0),
  fThreshold(0),
  fEnergyPeaks(NULL),
  fOutputFile(NULL),
  fDebugMode(0)
{
  
}

//R3BCalifaMapped2CrystalCalPar: Destructor ----------------------------------------
R3BCalifaMapped2CrystalCalPar::~R3BCalifaMapped2CrystalCalPar() {
  if(fEnergyPeaks)
    delete fEnergyPeaks;
  
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BCalifaMapped2CrystalCalPar::Init() {

  if(!fEnergyPeaks){
    fEnergyPeaks = new TArrayF;
    fEnergyPeaks->Set(fNumPeaks);
  }
  
  char name[100];
  
  fh_Map_energy_crystal = new TH1F*[fNumCrystals];
  for(Int_t i=0;i<fNumCrystals;i++){
    sprintf(name,"fh_Map_energy_crystal_%i",i+1);
    fh_Map_energy_crystal[i] = new TH1F(name,name,fMapHistos_bins,fMapHistos_left,fMapHistos_right);
  }
  
  FairRootManager* rootManager = FairRootManager::Instance();
  if (!rootManager) { return kFATAL;}
  
  fCalifaMappedDataCA = (TClonesArray*)rootManager->GetObject("CalifaMappedData");
  if (!fCalifaMappedDataCA) { return kFATAL;}
  
  FairRuntimeDb* rtdb = FairRuntimeDb::instance();
  if (!rtdb) { return kFATAL;}
  
  fCal_Par=(R3BCalifaCrystalCalPar*)rtdb->getContainer("califaCrystalCalPar");
  if (!fCal_Par) {
    LOG(ERROR)<<"R3BCalifaMapped2CrystalCalPar::Init() Couldn't get handle on califaCrystalCalPar container"<<FairLogger::endl;
    return kFATAL;
  }
  
  return kSUCCESS;
}

// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCalifaMapped2CrystalCalPar::ReInit() {
  
  // MOVE PAR SETTINGS IN INIT TO SETPARCONTAINERS AND CALL ALSO IT HERE
  // SetParContainers();
  
  return kSUCCESS;
}

// -----   Public method Exec   --------------------------------------------
void R3BCalifaMapped2CrystalCalPar::Exec(Option_t* opt) {
  
  Int_t nHits = fCalifaMappedDataCA->GetEntries();
  if(!nHits) return;
  
  R3BCalifaMappedData* MapHit;
  Int_t crystalId;
  
  for(Int_t i = 0; i < nHits; i++) {
    MapHit = (R3BCalifaMappedData*)(fCalifaMappedDataCA->At(i));
    crystalId = MapHit->GetCrystalId();
    
    //Fill Histos
    fh_Map_energy_crystal[crystalId-1]->Fill(MapHit->GetEnergy());
  }
}

// ---- Public method Reset   --------------------------------------------------
void R3BCalifaMapped2CrystalCalPar::Reset() {
  
}

void R3BCalifaMapped2CrystalCalPar::FinishEvent() {
}

// ---- Public method Finish   --------------------------------------------------
void R3BCalifaMapped2CrystalCalPar::FinishTask() {
  
  SearchPeaks();
  
  //obtain an output file
  /*FILE *fout = NULL;
    if(fOutputFile)
    {
    fout = fopen(fOutputFile, "w");
    if(!fout)
    {
    cerr << "Could not open " << fOutputFile << " for writing!\n";
    }
    else
    fprintf(fout, "# CrystalId Ratio NumEvents\n");
    }*/
  
  
}

//------------------
void R3BCalifaMapped2CrystalCalPar::SearchPeaks(){
  
  Int_t nfound=0;
  Int_t idx[fNumPeaks];
  
  Int_t numPars;
  if (fNumParam){numPars=fNumParam;}
  else {numPars=2;}//by default! number of parameters=2
  
  fCal_Par->SetNumCrystals(fNumCrystals);
  fCal_Par->SetNumParametersFit(fNumParam);
  fCal_Par->GetCryCalParams()->Set(numPars*fNumCrystals);

  TSpectrum *ss= new TSpectrum(fNumPeaks);  
  
  for (Int_t i=0;i<fNumCrystals;i++){
    
    if (fh_Map_energy_crystal[i]->GetEntries()>fMinStadistics){
      
      if(fDebugMode) nfound = ss->Search(fh_Map_energy_crystal[i],fSigma,"",fThreshold);
      else  nfound = ss->Search(fh_Map_energy_crystal[i],fSigma,"goff",fThreshold);
      //	std::cout<< i << " " << nfound <<" "<< fThreshold << std::endl;
      fChannelPeaks = (Double_t*) ss->GetPositionX();
      TMath::Sort(nfound, fChannelPeaks, idx, kTRUE);

      
      //Calibrated Spectrum
      Double_t X[fNumPeaks+1];
      Double_t Y[fNumPeaks+1];
      
      X[0]=0.;
      Y[0]=0.;
      
      for (Int_t j=0;j<fNumPeaks;j++){
	X[j+1]=fChannelPeaks[idx[fNumPeaks-j-1]];
	Y[j+1]=fEnergyPeaks->GetAt(fNumPeaks-j-1);

	std::cout<<"CrystalId="<<i+1<<" "<< j+1  <<" "<< X[j+1]  << std::endl;
      }
      
      TF1 *f1;
     
      if (fNumParam){
	
	if (fNumParam==1){
	  f1 = new TF1 ("f1", "[0]*x", fMapHistos_left, fMapHistos_right);
	}
	if (fNumParam==2){
	  f1 = new TF1 ("f1", "[0]+[1]*x", fMapHistos_left, fMapHistos_right);
	}
	if (fNumParam==3){
	  f1 = new TF1 ("f1", "[0]+[1]*x+[2]*pow(x,2)", fMapHistos_left, fMapHistos_right);
	}
	if (fNumParam==4){
	  f1 = new TF1 ("f1", "[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)", fMapHistos_left, fMapHistos_right);
	}
	if (fNumParam==5){
	  f1 = new TF1 ("f1", "[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)+[4]*pow(x,4)", fMapHistos_left, fMapHistos_right);
	}
	if (fNumParam>5){
	  std::cout<<endl<<"ERROR: The number of fit parameters can not be higher than 5!"<<endl;
	}
      }else{
	std::cout<<endl<<"No imput number of fit parameters, therefore, by default NumberParameters=2"<<endl;
	f1 = new TF1 ("f1", "[0]+[1]*x", fMapHistos_left, fMapHistos_right);
      }
      
      TGraph* graph = new TGraph (fNumPeaks+1, X, Y);
      graph->Fit("f1","Q");//Quiet mode (minimum printing)

      
      for(Int_t h=0; h<numPars;h++){
	fCal_Par->SetCryCalParams(f1->GetParameter(h),numPars*i+h);
	Double_t par;
	par=f1->GetParameter(h);
      }  

      if(fDebugMode) fh_Map_energy_crystal[i]->Write();
      
    }else {std::cout<<"Histogram NO Fitted number "<<i+1<<endl;}
    
  }
  
  delete ss;
  fCal_Par->setChanged();
  return;
}

/*
  void R3BCalifaMapped2CrystalCalPar::SetOutputFile(const char *outFile)
  {
  fOutputFile = const_cast<char*>(outFile);
  }
*/
ClassImp(R3BCalifaMapped2CrystalCalPar)
