// -------------------------------------------------------------------------
// -----         R3BAmsMapped2StripCal source file                     -----
// -----             Created 29/05/18  by J.L. Rodriguez-Sanchez       -----
// -------------------------------------------------------------------------

//ROOT headers
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

//Fair headers
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include <iomanip>

//Califa headers
#include "R3BAmsStripCalData.h"
#include "R3BAmsMappedData.h"
#include "R3BAmsMapped2StripCal.h"
#include "R3BAmsStripCalPar.h"

//R3BAmsMapped2StripCal: Default Constructor --------------------------
R3BAmsMapped2StripCal::R3BAmsMapped2StripCal() : 
  FairTask("R3B AMS Calibrator",1),
  NumDets(0),
  NumStrips(0),
  NumStripsS(0),
  NumStripsK(0),
  NumParams(0),
  MaxSigma(5),
  CalParams(NULL),
  fCal_Par(NULL),
  fAmsMappedDataCA(NULL),
  fAmsStripCalDataCA(NULL),
  fOnline(kFALSE)
{
}

//R3BAmsMapped2StripCalPar: Standard Constructor --------------------------
R3BAmsMapped2StripCal::R3BAmsMapped2StripCal(const char* name, Int_t iVerbose) :
  FairTask(name, iVerbose),
  NumDets(0),
  NumStrips(0),
  NumStripsS(0),
  NumStripsK(0),
  NumParams(0),
  MaxSigma(5),
  CalParams(NULL),
  fCal_Par(NULL),
  fAmsMappedDataCA(NULL),
  fAmsStripCalDataCA(NULL),
  fOnline(kFALSE)
{  
}

//Virtual R3BAmsMapped2StripCal: Destructor
R3BAmsMapped2StripCal::~R3BAmsMapped2StripCal()
{
  LOG(INFO) << "R3BAmsMapped2StripCal: Delete instance" << FairLogger::endl;
}


void R3BAmsMapped2StripCal::SetParContainers() {
  
  //Parameter Container
  //Reading amsStripCalPar from FairRuntimeDb
  FairRuntimeDb* rtdb = FairRuntimeDb::instance();
  if (!rtdb) { 
    LOG(ERROR)<<"FairRuntimeDb not opened!"<<FairLogger::endl;
  }
  
  fCal_Par=(R3BAmsStripCalPar*)rtdb->getContainer("amsStripCalPar");
  if (!fCal_Par) {
    LOG(ERROR)<<"R3BAmsMapped2StripCalPar::Init() Couldn't get handle on amsStripCalPar container"<<FairLogger::endl;
  }
  else{
    LOG(INFO)<<"R3BAmsMapped2StripCalPar:: amsStripCalPar container open"<<FairLogger::endl;
  }
}


void R3BAmsMapped2StripCal::SetParameter(){
  
  //--- Parameter Container ---
  NumDets=fCal_Par->GetNumDets();//Number of Detectors
  NumStrips=fCal_Par->GetNumStrips();//Number of Strips
  NumStripsS=fCal_Par->GetNumStripsS();//Number of Strips S-side
  NumStripsK=fCal_Par->GetNumStripsK();//Number of Strips K-side
  NumParams=fCal_Par->GetNumParametersFit();//Number of Parameters

  LOG(INFO)<<"R3BAmsMapped2StripCal: Nb detectors: "<< NumDets <<FairLogger::endl;
  LOG(INFO)<<"R3BAmsMapped2StripCal: Nb strips: "<< NumStrips <<FairLogger::endl;
  LOG(INFO)<<"R3BAmsMapped2StripCal: Nb strips S-side: "<< NumStripsS <<FairLogger::endl;
  LOG(INFO)<<"R3BAmsMapped2StripCal: Nb strips K-side: "<< NumStripsK <<FairLogger::endl;
  LOG(INFO)<<"R3BAmsMapped2StripCal: Nb parameters from pedestal fit: "<< NumParams <<FairLogger::endl;
  
  CalParams= new TArrayF();
  Int_t array_size = NumDets*NumStrips*NumParams;
  CalParams->Set(array_size);	
  CalParams=fCal_Par->GetStripCalParams();//Array with the Cal parameters

  //Count the number of dead strips per AMS detector
  for(Int_t d = 0; d < NumDets; d++){
  Int_t numdeadstrips=0;
  for(Int_t i = 0; i < NumStrips; i++)if(CalParams->GetAt(NumParams*i+1+NumStrips*d*NumParams)==-1)numdeadstrips++;
  LOG(INFO)<<"R3BAmsMapped2StripCal: Nb of dead strips in AMS detector " <<d<< ": "<< numdeadstrips <<FairLogger::endl;
  }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAmsMapped2StripCal::Init()
{
  LOG(INFO) << "R3BAmsMapped2StripCal: Init" << FairLogger::endl;

  //INPUT DATA
  FairRootManager* rootManager = FairRootManager::Instance();
  if (!rootManager) { return kFATAL;}
  
  fAmsMappedDataCA = (TClonesArray*)rootManager->GetObject("AmsMappedData");
  if (!fAmsMappedDataCA) { return kFATAL;}
   
  //OUTPUT DATA
  //Calibrated data
  fAmsStripCalDataCA = new TClonesArray("R3BAmsStripCalData",10);

  if(!fOnline){
  rootManager->Register("AmsStripCalData", "AMS strip Cal", fAmsStripCalDataCA, kTRUE);
  }else{
  rootManager->Register("AmsStripCalData", "AMS strip Cal", fAmsStripCalDataCA, kFALSE);
  }

  SetParameter();
  return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BAmsMapped2StripCal::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BAmsMapped2StripCal::Exec(Option_t* option)
{
  
  //if(++nEvents % 10000 == 0)
  //LOG(INFO) << nEvents << FairLogger::endl;
  
  //Reset entries in output arrays, local arrays
  Reset();
  
  if (!fCal_Par) {
    LOG(ERROR)<<"NO Container Parameter!!"<<FairLogger::endl;
  }  
  
  //Reading the Input -- Mapped Data --
  Int_t nHits = fAmsMappedDataCA->GetEntries();
  if(nHits!=NumStrips*NumDets && nHits>0)LOG(WARNING) << "R3BAmsMapped2StripCal: nHits!=NumStrips*NumDets"<<FairLogger::endl;
  if(!nHits) return;
  
  R3BAmsMappedData** mappedData;
  mappedData=new R3BAmsMappedData*[nHits];
  Int_t detId;
  Int_t sideId;
  Int_t stripId;
  Double_t energy;
  Double_t pedestal=0.;
  Double_t sigma=0.;
  Double_t SynAdcs[16*NumDets];

  
  //Look for ADC baselines
  for(Int_t i = 0; i < 16*NumDets; i++)SynAdcs[i]=0.;
  
  Int_t nbadc=0;
  Int_t minnb=4;
  Double_t h=0;
  /*
  for(Int_t i = 0; i < nHits; i++) {
    mappedData[i] = (R3BAmsMappedData*)(fAmsMappedDataCA->At(i));

    detId   = mappedData[i]->GetDetectorId();
    stripId = mappedData[i]->GetStripId();

    pedestal=CalParams->GetAt(NumParams*stripId+1);
    sigma=CalParams->GetAt(NumParams*stripId+2);
    energy  = mappedData[i]->GetEnergy()-pedestal;

//LOG(WARNING)<<  pedestal << " "<<  sigma << " " <<  energy << " "<<nbadc<<FairLogger::endl;

    if(energy<sigma*3. && i<64*(nbadc+1)){
     if(pedestal>-1){
      SynAdcs[nbadc]=SynAdcs[nbadc]+energy;
      h=h+1.;
      if(h==minnb){
       SynAdcs[nbadc]=SynAdcs[nbadc]/h;
       nbadc++;
       i=64*nbadc-1;
       h=0;
      }
     }
    }else{
     SynAdcs[nbadc]=0.;
     LOG(WARNING) << "R3BAmsMapped2StripCal: NO baseline found for ADC "<<nbadc<<FairLogger::endl;
     nbadc++;
    }
  }
  */
  
  nbadc=0;
  for(Int_t i = 0; i < nHits; i++) {
    mappedData[i] = (R3BAmsMappedData*)(fAmsMappedDataCA->At(i));
    detId   = mappedData[i]->GetDetectorId();
    stripId = mappedData[i]->GetStripId();

    pedestal=CalParams->GetAt(NumParams*stripId+1+detId*NumStrips*NumParams);
    sigma=CalParams->GetAt(NumParams*stripId+2+detId*NumStrips*NumParams);

    if(stripId<NumStripsS){
     sideId=0;
    }else{
     sideId=1;
     stripId=stripId-NumStripsS;
    }
    if(i%64==0 && i>0){nbadc++;}
    energy  = mappedData[i]->GetEnergy()-pedestal-SynAdcs[nbadc];

    if(energy>4.*sigma && pedestal!=-1)
    AddCalData(detId,sideId,stripId,energy);
  }
  
  
  if(mappedData) delete mappedData;
  return;
}

// -----   Protected method Finish   --------------------------------------------
void R3BAmsMapped2StripCal::Finish()
{
  
}

// -----   Public method Reset   ------------------------------------------------
void R3BAmsMapped2StripCal::Reset()
{
  LOG(DEBUG) << "Clearing StripCalData Structure" << FairLogger::endl;
  if(fAmsStripCalDataCA)fAmsStripCalDataCA->Clear();
}


// -----   Private method AddCalData  --------------------------------------------
R3BAmsStripCalData* R3BAmsMapped2StripCal::AddCalData(Int_t detid, Int_t sideid, Int_t stripid, Double_t energy)
{
  //It fills the R3BAmsStripCalData
  TClonesArray& clref = *fAmsStripCalDataCA;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BAmsStripCalData(detid,sideid,stripid,energy);
}

ClassImp(R3BAmsMapped2StripCal)
