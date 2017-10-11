// -------------------------------------------------------------------------
// -----         3BCalifaMapped2CrystalCalPar source file              -----
// -----             Created 22/07/14  by H.Alvarez                    -----
// -----            Modified 20/03/17  by P.Cabanelas                  -----
// -------------------------------------------------------------------------
#include <iostream>

#include "TMath.h"
#include "TVector3.h"
#include "TGeoMatrix.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TGeoManager.h"

#include "R3BCalifaMappedData.h"
#include "R3BCalifaMapped2CrystalCalPar.h"

using std::cout;
using std::cerr;
using std::endl;


R3BCalifaMapped2CrystalCalPar::R3BCalifaMapped2CrystalCalPar() : 
   FairTask("R3B CALIFA Calibration Parameters Finder "),
   fCalifaMappedDataCA(NULL), 
   fRatioPidEnergy(NULL), fNumEvents(NULL), fNumChannels(0),
   nEvents(0), fOutputFile(NULL)
{
}


R3BCalifaMapped2CrystalCalPar::~R3BCalifaMapped2CrystalCalPar()
{
   if(fRatioPidEnergy)
      delete[] fRatioPidEnergy;
   if(fNumEvents)
      delete[] fNumEvents;
}


// -----   Public method Init   --------------------------------------------
InitStatus R3BCalifaMapped2CrystalCalPar::Init()
{
  Register();
  return kSUCCESS;
}


void R3BCalifaMapped2CrystalCalPar::SetParContainers()
{
  // Get run and runtime database
//  FairRunAna* run = FairRunAna::Instance();
//  if (!run) Fatal("R3BCalifaMapped2CrystalCalPar::SetParContainers", "No analysis run");
//
//  FairRuntimeDb* rtdb = run->GetRuntimeDb();
//  if (!rtdb) Fatal("R3BCalifaMapped2CrystalCalPar::SetParContainers", "No runtime database");
//
//  fCalifaCrystalCalPar = (R3BCalifaCrystalCalPar*)(rtdb->getContainer("R3BCalifaCrystalCalPar"));
//  
//  if ( fVerbose && fCalifaCrystalCalPar ) {
//    LOG(INFO) << "R3BCalifaMapped2CrystalCalPar::SetParContainers() "<< FairLogger::endl;
//    LOG(INFO) << "Container R3BCalifaCrystalCalPar loaded " << FairLogger::endl;
//  }
}


//Register: Protected method
void R3BCalifaMapped2CrystalCalPar::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager* fMan = FairRootManager::Instance();
  if(!fMan) {
    Fatal("Init", "No FairRootManager");
    return;
  }
  fCalifaMappedDataCA = (TClonesArray*) fMan->GetObject("CalifaMappedData");
  
  // Parameter retrieval from par container
  // ...
  // Table for crystal parameters from Geometry
  // Initialization of variables, histograms, ...

  return;
}



// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCalifaMapped2CrystalCalPar::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void R3BCalifaMapped2CrystalCalPar::Exec(Option_t* opt)
{

  // Reset entries in output arrays, local arrays
//  Reset();

   if(++nEvents % 10000 == 0)
      LOG(INFO) << nEvents << FairLogger::endl;

   Int_t nHits = fCalifaMappedDataCA->GetEntries();
   if(!nHits)
      return;

   // Increase array size
   R3BCalifaMappedData *rawHit = dynamic_cast<R3BCalifaMappedData*>(fCalifaMappedDataCA->At(nHits-1));
   if(rawHit->GetCrystalId() >= fNumChannels)
   {
      Double_t *tmp = new Double_t[rawHit->GetCrystalId() + 1];
      UInt_t *tmpNum = new UInt_t[rawHit->GetCrystalId() + 1];
      if(fRatioPidEnergy)
      {
         for(int i = 0; i < fNumChannels; i++)
         {
            tmp[i] = fRatioPidEnergy[i];
            tmpNum[i] = fNumEvents[i];
         }
         for(int i = fNumChannels; i <= rawHit->GetCrystalId(); i++)
         {
            tmp[i] = 0;
            tmpNum[i] = 0;
         }

         delete[] fRatioPidEnergy;
         delete[] fNumEvents;

      }
      fRatioPidEnergy = tmp;
      fNumEvents = tmpNum;
      fNumChannels = rawHit->GetCrystalId() + 1;
   }

   Int_t crystalId;
   Double_t pidSum;
   for(int i = 0; i < nHits; i++)
   {
      rawHit = dynamic_cast<R3BCalifaMappedData*>(fCalifaMappedDataCA->At(i));
      crystalId = rawHit->GetCrystalId();

      pidSum = rawHit->GetNf() + rawHit->GetNs();
      if(pidSum > 0)
      {
         fRatioPidEnergy[crystalId] += (rawHit->GetEnergy() / pidSum);
         fNumEvents[crystalId]++;
      }
   }
}


// ---- Public method Reset   --------------------------------------------------
void R3BCalifaMapped2CrystalCalPar::Reset()
{
  // Clear the CA structure
  
}




// ---- Public method Finish   --------------------------------------------------
void R3BCalifaMapped2CrystalCalPar::Finish()
{
   FILE *fout = NULL;
   if(fOutputFile)
   {
      fout = fopen(fOutputFile, "w");
      if(!fout)
      {
         cerr << "Could not open " << fOutputFile << " for writing!\n";
      }
      else
         fprintf(fout, "# CrystalId Ratio NumEvents\n");
   }

   // Calculate average
   for(int i = 0; i < fNumChannels; i++)
   {
      if(fNumEvents[i] == 0)
         continue;

      fRatioPidEnergy[i] /= (Double_t)fNumEvents[i];
      
      cout << "Channel " << i << ": " << fRatioPidEnergy[i] << " (" << fNumEvents[i] << ")" << endl;
      if(fout)
         fprintf(fout, "%d %lf %u\n", i, fRatioPidEnergy[i], fNumEvents[i]);
   }
}


void R3BCalifaMapped2CrystalCalPar::SetOutputFile(const char *outFile)
{
   fOutputFile = const_cast<char*>(outFile);
}

ClassImp(R3BCalifaMapped2CrystalCalPar)
