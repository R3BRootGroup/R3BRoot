// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                     R3BCalifaMapped2CrystalCal                    -----
// -----                Created 18/07/2014 by H. Alvarez-Pol               -----
// -----                 Modified 20/03/2017 by P. Cabanelas               -----
// -----                 Modified 15/12/2017 by E. Galiana                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

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
#include "R3BCalifa.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappedData.h"
#include "R3BCalifaMapped2CrystalCal.h"
#include "R3BCalifaCrystalCalPar.h"

//R3BCalifaMapped2CrystalCal: Constructor
R3BCalifaMapped2CrystalCal::R3BCalifaMapped2CrystalCal() : FairTask("R3B CALIFA Calibrator"),
							   NumCrystals(0),
							   NumParams(0),
							   CalParams(NULL),
							   fCal_Par(NULL),
							   fCalifaMappedDataCA(NULL),
							   fCalifaCryCalDataCA(NULL)
{
}


//Virtual R3BCalifaMapped2CrystalCal: Destructor
R3BCalifaMapped2CrystalCal::~R3BCalifaMapped2CrystalCal()
{
  LOG(INFO) << "R3BCalifaMapped2CrystalCal: Delete instance" << FairLogger::endl;
}



void R3BCalifaMapped2CrystalCal::SetParContainers() {
  
  //Parameter Container
  //Reading califaCrystalCalPar from FairRuntimeDb
  FairRuntimeDb* rtdb = FairRuntimeDb::instance();
  if (!rtdb) { 
    LOG(ERROR)<<"FairRuntimeDb not opened!"<<FairLogger::endl;
  }
  
  fCal_Par=(R3BCalifaCrystalCalPar*)rtdb->getContainer("califaCrystalCalPar");
  if (!fCal_Par) {
    LOG(ERROR)<<"R3BCalifaMapped2CrystalCalPar::Init() Couldn't get handle on califaCrystalCalPar container"<<FairLogger::endl;
  }
  if (fCal_Par){
    std::cout<<"R3BCalifaMapped2CrystalCalPar:: califaCrystalCalPar container open"<<endl;
  }
}


void R3BCalifaMapped2CrystalCal::SetParameter(){
  
  //--- Parameter Container ---
  NumCrystals=fCal_Par->GetNumCrystals();//Number of Crystals
  NumParams=fCal_Par->GetNumParametersFit();//Number of Parameters
  
  CalParams= new TArrayF();
  Int_t array_size = NumCrystals*NumParams;
  CalParams->Set(array_size);	
  CalParams=fCal_Par->GetCryCalParams();//Array with the Cal parameters
  
  
}





// -----   Public method Init   --------------------------------------------
InitStatus R3BCalifaMapped2CrystalCal::Init()
{ 
  //INPUT DATA
  FairRootManager* rootManager = FairRootManager::Instance();
  if (!rootManager) { return kFATAL;}
  
  fCalifaMappedDataCA = (TClonesArray*)rootManager->GetObject("CalifaMappedData");
  if (!fCalifaMappedDataCA) { return kFATAL;}
   
  //OUTPUT DATA
  //Calibrated data
  fCalifaCryCalDataCA = new TClonesArray("R3BCalifaCrystalCalData",10);
  rootManager->Register("CalifaCrystalCalData", "CALIFA Crystal Cal", fCalifaCryCalDataCA, kTRUE);
  
  SetParameter();
  return kSUCCESS;
}




// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BCalifaMapped2CrystalCal::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BCalifaMapped2CrystalCal::Exec(Option_t* option)
{
  
  //if(++nEvents % 10000 == 0)
  //LOG(INFO) << nEvents << FairLogger::endl;
  
  //Reset entries in output arrays, local arrays
  Reset();
  
  if (!fCal_Par) {
    std::cout<<"NO Container Parameter!!"<<endl<<endl;
  }  
  
  //Reading the Input -- Mapped Data --
  Int_t nHits = fCalifaMappedDataCA->GetEntries();
  if(!nHits) return;
  
  R3BCalifaMappedData** mappedData;
  mappedData=new R3BCalifaMappedData*[nHits];
  UShort_t crystalId;
  Int_t channel;
  Int_t Nf;
  Int_t Ns;
  ULong_t Time;
  UChar_t Error;
  UShort_t Tot;
  Double_t energy;
  Double_t param0;
  Double_t param1;
  Double_t param2;
  Double_t param3;
  Double_t param4;
  
  
  for(Int_t i = 0; i < nHits; i++) {
    mappedData[i] = (R3BCalifaMappedData*)(fCalifaMappedDataCA->At(i));
    
    crystalId = mappedData[i]->GetCrystalId();
    channel   = mappedData[i]->GetEnergy();
    Nf        = mappedData[i]->GetNf();
    Ns        = mappedData[i]->GetNs();
    Time      = mappedData[i]->GetTime();
    Error     = mappedData[i]->GetError();
    Tot       = mappedData[i]->GetTot();
    
     
    //smearing of the channels
    Double_t ch;
    Double_t Rndm;
    Rndm=gRandom->Rndm()-1;
    ch=channel+Rndm;
      	
    
    if (NumParams==1){
      param1=CalParams->GetAt(crystalId-1);
      energy= param1*ch;
    }
    
    if (NumParams==2 || NumParams==0){   
      
      param0=CalParams->GetAt(2*crystalId-2);
      param1=CalParams->GetAt(2*crystalId-1);
      energy= param0+param1*ch;	
    }  
    
    if (NumParams==3){
		
      param0=CalParams->GetAt(3*crystalId-3);
      param1=CalParams->GetAt(3*crystalId-2);
      param2=CalParams->GetAt(3*crystalId-1);      
      energy= param0+param1*ch+param2*pow(ch,2);
    }  
    
    if (NumParams==4){
      param0=CalParams->GetAt(4*crystalId-4);
      param1=CalParams->GetAt(4*crystalId-3);
      param2=CalParams->GetAt(4*crystalId-2);
      param3=CalParams->GetAt(4*crystalId-1);
      energy= param0+param1*ch+param2*pow(ch,2)+param3*pow(ch,3);//smearing not introduced yet
    }  
    
    if (NumParams==5){
      param0=CalParams->GetAt(5*crystalId-5);
      param1=CalParams->GetAt(5*crystalId-4);
      param2=CalParams->GetAt(5*crystalId-3);
      param3=CalParams->GetAt(5*crystalId-2);
      param4=CalParams->GetAt(5*crystalId-1);
      energy= param0+param1*ch+param2*pow(ch,2)+param3*pow(ch,3)+param4*pow(ch,4);//smearing not introduced yet
    }  
    
    AddCalData(crystalId,energy,Nf,Ns,Time,Tot);
  }
  
  
  if(mappedData) delete mappedData;
  return;
}

// -----   Protected method Finish   --------------------------------------------
void R3BCalifaMapped2CrystalCal::Finish()
{
  
}

// -----   Public method Reset   ------------------------------------------------
void R3BCalifaMapped2CrystalCal::Reset()
{
  LOG(DEBUG) << "Clearing CrystalCalData Structure" << FairLogger::endl;
  if(fCalifaCryCalDataCA)fCalifaCryCalDataCA->Clear();
}


// -----   Private method AddCalData  --------------------------------------------
R3BCalifaCrystalCalData* R3BCalifaMapped2CrystalCal::AddCalData(Int_t id, Double_t energy, Double_t Nf, Double_t Ns, ULong64_t time, Double_t tot_energy=0)
{
  //It fills the R3BCalifaCrystalCalData
  TClonesArray& clref = *fCalifaCryCalDataCA;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) R3BCalifaCrystalCalData(id,energy,Nf,Ns,time,tot_energy);
}

ClassImp(R3BCalifaMapped2CrystalCal)
