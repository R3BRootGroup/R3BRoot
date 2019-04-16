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
#include "TCanvas.h"
#include "TGraph.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

//R3BAmsMapped2StripCalPar: Default Constructor --------------------------
R3BAmsMapped2StripCalPar::R3BAmsMapped2StripCalPar() :
  FairTask("R3B AMS Pedestal Finder ",1),
  fStrip_Par(NULL),
  fAmsMappedDataCA(NULL),
  fNumDets(4),
  fNumStrips(1024),
  fNumStripsS(640),
  fNumStripsK(384),
  fMaxSigma(5),
  fMinStadistics(1000),
  fMapHistos_left(0),
  fMapHistos_right(4000),
  fMapHistos_bins(4000),
  fSigma(0),
  fMean(0),
  fPrint(kFALSE),
  fOutputFile(NULL) {
  
}

//R3BAmsMapped2StripCalPar: Standard Constructor --------------------------
R3BAmsMapped2StripCalPar::R3BAmsMapped2StripCalPar(const char* name, Int_t iVerbose) :
  FairTask(name, iVerbose),
  fStrip_Par(NULL),
  fAmsMappedDataCA(NULL),
  fNumDets(4),
  fNumStrips(1024),
  fNumStripsS(640),
  fNumStripsK(384),
  fMaxSigma(5),
  fMinStadistics(1000),
  fMapHistos_left(0),
  fMapHistos_right(4000),
  fMapHistos_bins(4000),
  fSigma(0),
  fMean(0),
  fPrint(kFALSE),
  fOutputFile(NULL) {
  
}

//R3BAmsMapped2StripCalPar: Destructor ----------------------------------------
R3BAmsMapped2StripCalPar::~R3BAmsMapped2StripCalPar() {
  
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAmsMapped2StripCalPar::Init() {

  LOG(INFO) << "R3BAmsMapped2StripCalPar: Init";

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
    LOG(ERROR)<<"R3BAmsMapped2StripCalPar::Init() Couldn't get handle on amsStripCalPar container";
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
  if(fPrint)PrintParamsDaq();
  //fStrip_Par->printParams();
}

//------------------
void R3BAmsMapped2StripCalPar::PrintParamsDaq(){

  LOG(INFO) << "R3BAmsMapped2StripCalPar: Print parameters for DAQ";

  FILE *fOut1=fopen("sidped.txt","wt");
  FILE *fOut2=fopen("sidsig_r.txt","wt");
  FILE *fOut3=fopen("sidsig.txt","wt");

  //Values taken from /u/land/.old/siderem 
  //This was implemented for the experiment s444
  //For other experiments one should revise the numbers
  Double_t cn_limit = 15.;
  Float_t threshold1 = 4.0;
  Float_t threshold2 = 2.;

  for(Int_t d=0;d<fNumDets;d++){
    if(d<2){
    fprintf(fOut1,"50%i20000\n",d+1);//sam+gtb+siderem+20000	
    fprintf(fOut2,"50%i20000\n",d+1);		
    fprintf(fOut3,"50%i20000\n",d+1);
        //std::cout << "50"<< d+1<<"20000\n"<< std::endl;
    }else{
    fprintf(fOut1,"51%i20000\n",d-1);//sam+gtb+siderem+20000	
    fprintf(fOut2,"51%i20000\n",d-1);		
    fprintf(fOut3,"51%i20000\n",d-1);
        //std::cout << "51"<< d-1<<"20000\n"<< std::endl;
    }
    for (Int_t i=1;i<=fNumStrips;i++){
     fprintf(fOut1,"%4x ", int(8.*parameters[d*fNumStrips +i-1][0]));
     fprintf(fOut2,"%4x ", int(8.*TMath::Min(threshold1*parameters[d*fNumStrips +i-1][1],cn_limit)));		
     fprintf(fOut3,"%4x ", int(8.*threshold2 * parameters[d*fNumStrips +i-1][1]));
     //std::cout << "\t " << int(parameters[d*fNumStrips +i-1][0]);
     if(i && !(i%16)){
	fprintf(fOut1,"\n");		
	fprintf(fOut2,"\n");		
	fprintf(fOut3,"\n");
        //std::cout << std::endl;
     }
    }
  }
  fclose(fOut1);
  fclose(fOut2);
  fclose(fOut3);
}

//------------------
void R3BAmsMapped2StripCalPar::SearchPedestals(){

  LOG(INFO) << "R3BAmsMapped2StripCalPar: Search pedestals";

  Int_t numPars =3;// by default number of parameters=3
  
  fStrip_Par->SetNumDets(fNumDets);
  fStrip_Par->SetNumStrips(fNumStrips);
  fStrip_Par->SetNumStripsS(fNumStripsS);
  fStrip_Par->SetNumStripsK(fNumStripsK);
  fStrip_Par->GetStripCalParams()->Set(numPars*fNumStrips*fNumDets);

  TCanvas* cPar=new TCanvas("AMS","",0,0,800,1000);;
  char Name[255];
  cPar->Divide(2,2);

  Int_t nbstrip=0;
  for(Int_t d=0;d<fNumDets*fNumStrips;d++)for (Int_t i=0;i<2;i++)parameters[d][i]=0.;

  for(Int_t d=0;d<fNumDets;d++){
   Double_t x[fNumStrips],y[fNumStrips];
   sprintf(Name, "AMS_%d", d);

   for (Int_t i=0;i<fNumStrips;i++){
    x[i]=0.;y[i]=0.;
    nbstrip=numPars*i+d*numPars*fNumStrips;

    if (fh_Map_energy_strip[i+d*fNumStrips]->GetEntries()>fMinStadistics){
      //Bins with a number of counts less than 30% of the maximum are set to zero 
      for(Int_t k2=0;k2<fMapHistos_bins;k2++){
       if(fh_Map_energy_strip[i+d*fNumStrips]->GetBinContent(k2)<0.3*fh_Map_energy_strip[i+d*fNumStrips]->GetMaximum())
         fh_Map_energy_strip[i+d*fNumStrips]->SetBinContent(k2,0);
      }

      TF1 *f1 = new TF1 ("f1", "gaus", fMapHistos_left, fMapHistos_right);
      f1->SetParameter(1,400.);      
      f1->SetParameter(2,2.);

      fh_Map_energy_strip[i+d*fNumStrips]->Fit("f1","RQ0");

      y[i]=f1->GetParameter(2);
      x[i]=i;

      //Parameters for DAQ
      parameters[d*fNumStrips+i][0]=f1->GetParameter(1);
      parameters[d*fNumStrips+i][1]=f1->GetParameter(2);

      //Fill container:
      fStrip_Par->SetStripCalParams(f1->GetParameter(0),nbstrip);
      if(f1->GetParameter(2)<fMaxSigma&&f1->GetParameter(2)>0.1){
       fStrip_Par->SetStripCalParams(f1->GetParameter(1),nbstrip+1);
       fStrip_Par->SetStripCalParams(f1->GetParameter(2),nbstrip+2);
      }else{
       fStrip_Par->SetStripCalParams(-1,nbstrip+1);//dead strip
       fStrip_Par->SetStripCalParams(0,nbstrip+2);
       //LOG(WARNING)<<"Dead strip, detector: " << d+1 << ", strip: "<< i+1 <<", "<< f1->GetParameter(2);
      }
      

    }else {
      fStrip_Par->SetStripCalParams(-1,nbstrip+1);//dead strip
      fStrip_Par->SetStripCalParams(0,nbstrip+2);
      LOG(WARNING)<<"Histogram NO Fitted, detector: " << d+1 << ", strip: "<< i+1;
    }
   }
  //Draw sigma for pedestals
  TGraph* gPar = new TGraph(fNumStrips,x,y);
  gPar->SetTitle(Name);
  gPar->SetMarkerStyle(20);
  gPar->SetMarkerColor(4);
  cPar->cd(d+1);
  TH1F* fhamspar = new TH1F(Name, Name, 1044, -10, 1034);
  fhamspar->SetMinimum(0);
  fhamspar->SetMaximum(20);
  fhamspar->GetXaxis()->SetTitle("Strip number");
  fhamspar->GetXaxis()->SetTitleOffset(1.4);
  fhamspar->GetYaxis()->SetTitle("Sigma [ADC units]");
  fhamspar->GetYaxis()->SetTitleOffset(1.4);
  fhamspar->GetXaxis()->CenterTitle(true);
  fhamspar->GetYaxis()->CenterTitle(true);
  gPar->SetHistogram(fhamspar);
  gPar->Draw("ap");
  }
  cPar->Write();
  fStrip_Par->setChanged();  
  return;

}

ClassImp(R3BAmsMapped2StripCalPar)
