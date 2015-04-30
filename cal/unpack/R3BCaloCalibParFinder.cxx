// -------------------------------------------------------------------------
// -----                 R3BCaloCalibParFinder source file             -----
// -----                  Created 22/07/14  by H.Alvarez               -----
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

#include "R3BCaloRawHit.h"
#include "R3BCaloCalibParFinder.h"

using std::cout;
using std::cerr;
using std::endl;


R3BCaloCalibParFinder::R3BCaloCalibParFinder() : 
   FairTask("R3B CALIFA Calibration Parameters Finder "),
   fCaloRawHitCA(NULL), 
   fRatioPidEnergy(NULL), fNumEvents(NULL), fNumChannels(0),
   nEvents(0), fOutputFile(NULL)
{
}


R3BCaloCalibParFinder::~R3BCaloCalibParFinder()
{
   if(fRatioPidEnergy)
      delete[] fRatioPidEnergy;
   if(fNumEvents)
      delete[] fNumEvents;
}


// -----   Public method Init   --------------------------------------------
InitStatus R3BCaloCalibParFinder::Init()
{
  Register();
  return kSUCCESS;
}


void R3BCaloCalibParFinder::SetParContainers()
{
  // Get run and runtime database
//  FairRunAna* run = FairRunAna::Instance();
//  if (!run) Fatal("R3BCaloCalibParFinder::SetParContainers", "No analysis run");
//
//  FairRuntimeDb* rtdb = run->GetRuntimeDb();
//  if (!rtdb) Fatal("R3BCaloCalibParFinder::SetParContainers", "No runtime database");
//
//  fCaloCalibPar = (R3BCaloCalibPar*)(rtdb->getContainer("R3BCaloCalibPar"));
//  
//  if ( fVerbose && fCaloCalibPar ) {
//    LOG(INFO) << "R3BCaloCalibParFinder::SetParContainers() "<< FairLogger::endl;
//    LOG(INFO) << "Container R3BCaloCalibPar loaded " << FairLogger::endl;
//  }
}


//Register: Protected method
void R3BCaloCalibParFinder::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager* fMan = FairRootManager::Instance();
  if(!fMan) {
    Fatal("Init", "No FairRootManager");
    return;
  }
  fCaloRawHitCA = (TClonesArray*) fMan->GetObject("CaloRawHit");
  
  // Parameter retrieval from par container
  // ...
  // Table for crystal parameters from Geometry
  // Initialization of variables, histograms, ...

  return;
}



// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCaloCalibParFinder::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}


// -----   Public method Exec   --------------------------------------------
void R3BCaloCalibParFinder::Exec(Option_t* opt)
{

  // Reset entries in output arrays, local arrays
//  Reset();

   if(++nEvents % 10000 == 0)
      LOG(INFO) << nEvents << FairLogger::endl;

   Int_t nHits = fCaloRawHitCA->GetEntries();
   if(!nHits)
      return;

   // Increase array size
   R3BCaloRawHit *rawHit = dynamic_cast<R3BCaloRawHit*>(fCaloRawHitCA->At(nHits-1));
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
      rawHit = dynamic_cast<R3BCaloRawHit*>(fCaloRawHitCA->At(i));
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
void R3BCaloCalibParFinder::Reset()
{
  // Clear the CA structure
  
}




// ---- Public method Finish   --------------------------------------------------
void R3BCaloCalibParFinder::Finish()
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


void R3BCaloCalibParFinder::SetOutputFile(const char *outFile)
{
   fOutputFile = const_cast<char*>(outFile);
}

ClassImp(R3BCaloCalibParFinder)
