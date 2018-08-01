// -------------------------------------------------------------------------
// -----         R3BAmsStripCal2Hit source file                        -----
// -----             Created 01/06/18  by J.L. Rodriguez-Sanchez       -----
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
#include "R3BAmsHitData.h"
#include "R3BAmsStripCal2Hit.h"

//R3BAmsStripCal2Hit: Default Constructor --------------------------
R3BAmsStripCal2Hit::R3BAmsStripCal2Hit() : 
  FairTask("R3B Hit-AMS Calibrator",1),
  fPitchK(104.),
  fPitchS(110.),
  fAmsStripCalDataCA(NULL),
  fAmsHitDataCA(NULL),
  fOnline(kFALSE)
{
}

//R3BAmsStripCal2HitPar: Standard Constructor --------------------------
R3BAmsStripCal2Hit::R3BAmsStripCal2Hit(const char* name, Int_t iVerbose) :
  FairTask(name, iVerbose),
  fPitchK(104.),
  fPitchS(110.),
  fAmsStripCalDataCA(NULL),
  fAmsHitDataCA(NULL),
  fOnline(kFALSE)
{  
}

//Virtual R3BAmsStripCal2Hit: Destructor
R3BAmsStripCal2Hit::~R3BAmsStripCal2Hit()
{
  LOG(INFO) << "R3BAmsStripCal2Hit: Delete instance" << FairLogger::endl;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAmsStripCal2Hit::Init()
{ 
  LOG(INFO) << "R3BAmsStripCal2Hit: Init" << FairLogger::endl;

  //INPUT DATA
  FairRootManager* rootManager = FairRootManager::Instance();
  if (!rootManager) { return kFATAL;}
  
  fAmsStripCalDataCA = (TClonesArray*)rootManager->GetObject("AmsStripCalData");
  if (!fAmsStripCalDataCA) { return kFATAL;}
   
  //OUTPUT DATA
  //Hit data
  fAmsHitDataCA = new TClonesArray("R3BAmsHitData",10);

  if(!fOnline){
  rootManager->Register("AmsHitData", "AMS Hit", fAmsHitDataCA, kTRUE);
  }else{
  rootManager->Register("AmsHitData", "AMS Hit", fAmsHitDataCA, kFALSE);
  }  


  return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BAmsStripCal2Hit::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BAmsStripCal2Hit::Exec(Option_t* option)
{
  
  //if(++nEvents % 10000 == 0)
  //LOG(INFO) << nEvents << FairLogger::endl;
  
  //Reset entries in output arrays, local arrays
  Reset();
  

  //Reading the Input -- Cal Data --
  Int_t nHits = fAmsStripCalDataCA->GetEntries();
  if(!nHits) return;
  
  R3BAmsStripCalData** calData;
  calData=new R3BAmsStripCalData*[nHits];
  Int_t detId;
  Int_t sideId;
  Int_t stripId;
  Double_t energy;

  const Int_t MaxNumDet=10;//Max number of AMS detectors set to 10
  Double_t SumEnergy[MaxNumDet];
  Int_t CountDet[MaxNumDet];
  Double_t CoG[MaxNumDet][2][2];
  for(Int_t i = 0; i < MaxNumDet; i++){
   SumEnergy[i]=0.;
   CountDet[i]=0;
   for(Int_t j = 0; j < 2; j++)for(Int_t k = 0; k < 2; k++)CoG[i][j][k]=0.;
  }

  //Position Z: FIXME from macro analysis
  Double_t PosZ[MaxNumDet]={-6.,-3.,3.,6.};

 
  for(Int_t i = 0; i < nHits; i++) {
    calData[i] = (R3BAmsStripCalData*)(fAmsStripCalDataCA->At(i));
    detId=calData[i]->GetDetId();
    sideId=calData[i]->GetSideId();
    stripId=calData[i]->GetStripId();
    energy=calData[i]->GetEnergy();

    CountDet[detId]++;
    CoG[detId][sideId][0] = CoG[detId][sideId][0] + energy*stripId;
    CoG[detId][sideId][1] = CoG[detId][sideId][1] + energy;
    SumEnergy[detId] = SumEnergy[detId] + energy;
  }

  // Add hits per detector:TODO only multiplicity one
  for(Int_t i = 0; i < MaxNumDet; i++){
    if(CountDet[i]>0)AddHitData(i,1,CoG[i][0][0]/CoG[i][0][1]*fPitchS/1000.,CoG[i][1][0]/CoG[i][1][1]*fPitchK/1000.,PosZ[i],SumEnergy[i]);
  }

  if(calData) delete calData;
  return;
}

// -----   Protected method Finish   --------------------------------------------
void R3BAmsStripCal2Hit::Finish()
{
  
}

// -----   Public method Reset   ------------------------------------------------
void R3BAmsStripCal2Hit::Reset()
{
  LOG(DEBUG) << "Clearing HitData Structure" << FairLogger::endl;
  if(fAmsHitDataCA)fAmsHitDataCA->Clear();
}


// -----   Private method AddHitData  --------------------------------------------
R3BAmsHitData* R3BAmsStripCal2Hit::AddHitData(Int_t detid, Int_t numhit, Double_t x, Double_t y, Double_t z, Double_t energy)
{
  //It fills the R3BAmsHitData
  TClonesArray& clref = *fAmsHitDataCA;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BAmsHitData(detid,numhit,x,y,z,energy);
}

ClassImp(R3BAmsStripCal2Hit)
