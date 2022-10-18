/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BCalifaCrystalCal2Hit.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BLogger.h"
#include "R3BTGeoPar.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"

#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaGeometry.h"
#include "R3BCalifaMappingPar.h"
#include <list>
#include <vector>

using namespace std;

struct califa_candidate {

  Int_t motherId;
  vector<Int_t> crystalList;
  Double_t energy;
  Double_t ns;
  Double_t nf;
  Double_t theta;
  Double_t phi;
  ULong64_t time;

};

bool compareByEnergy(R3BCalifaCrystalCalData *a , R3BCalifaCrystalCalData *b)
{
    return a->GetEnergy() < b->GetEnergy();
}


bool isInside(vector<Int_t> &vec, Int_t cryId)
{
    bool result = false;
    if( find(vec.begin(), vec.end(),cryId) != vec.end() )
    {
        result = true;
    }
    return result;
}


void RemoveUsedCrystals(vector<Int_t> &used , vector<R3BCalifaCrystalCalData*> &all , vector<R3BCalifaCrystalCalData*> &proton, vector<R3BCalifaCrystalCalData*> &gamma, vector<R3BCalifaCrystalCalData*> &saturated){

   for(int p = 0 ; p < used.size() ; p++){
    for(int s = 0 ; s < all.size() ; s++)

      if(all.at(s)->GetCrystalId() == used.at(p))
       all.erase(all.begin() + s);

     }

   for(int p = 0 ; p < used.size() ; p++){
    for(int s = 0 ; s < gamma.size() ; s++)

      if(gamma.at(s)->GetCrystalId() == used.at(p))
       gamma.erase(gamma.begin() + s);

     }

   for(int p = 0 ; p < used.size() ; p++){
     for(int s = 0 ; s < proton.size() ; s++)

      if(proton.at(s)->GetCrystalId() == used.at(p))
       proton.erase(proton.begin() + s);

     }


   for(int p = 0 ; p < used.size() ; p++){
     for(int s = 0 ; s < saturated.size() ; s++)

      if(saturated.at(s)->GetCrystalId() == used.at(p))
       saturated.erase(saturated.begin() + s);

     }


}


R3BCalifaCrystalCal2Hit::R3BCalifaCrystalCal2Hit()

    : FairTask("R3B CALIFA CrystalCal to Hit Finder")
    , fCrystalCalData(NULL)
    , fCalifaHitData(NULL)
    , fGeometryVersion(2021)
    , fCrystalThreshold(100.) // 100 keV
    , fProtonThreshold(15000) // Crystal saturation @ ~15 MeV
    , fGammaClusterThreshold(1000) // 1 MeV
    , fProtonClusterThreshold(50000) // 50000 MeV
    , fMap_Par(NULL)
    , fRoundWindow(0.25)             // 14 degrees
    , fSimulation(kFALSE)
    , fCalifaGeo(NULL)
    , fOnline(kFALSE)
    , fRand(0)
    , fRandFile("")
{

}

R3BCalifaCrystalCal2Hit::~R3BCalifaCrystalCal2Hit()
{
    R3BLOG(DEBUG1, "");
    if (fCalifaHitData)
        delete fCalifaHitData;
}

void R3BCalifaCrystalCal2Hit::SetParContainers()
{
    // Load CALIFA and target positions from containers
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();

    fCalifaGeoPar = (R3BTGeoPar*)rtdb->getContainer("CalifaGeoPar");
    fTargetGeoPar = (R3BTGeoPar*)rtdb->getContainer("TargetGeoPar");
    fMap_Par      = (R3BCalifaMappingPar *)rtdb->getContainer("califaMappingPar");

    return;
}

InitStatus R3BCalifaCrystalCal2Hit::Init()
{
    R3BLOG(INFO, "");
    assert(!fCalifaHitData); // in case someone calls Init() twice.

    FairRootManager* ioManager = FairRootManager::Instance();
    R3BLOG_IF(fatal, !ioManager, "FairRootManager not found");

    fCrystalCalData = (TClonesArray*)ioManager->GetObject("CalifaCrystalCalData");

    // Register output array

    fCalifaHitData = new TClonesArray("R3BCalifaHitData");
    ioManager->Register("CalifaHitData", "CALIFA Hit", fCalifaHitData, !fOnline);

    fCalifaGeo = R3BCalifaGeometry::Instance();

    fCalifaGeo->Init(fGeometryVersion);

    R3BLOG_IF(ERROR, !fCalifaGeo->Init(fGeometryVersion), "Califa geometry not found");


    if(fRand){

    fAngularDistributions = new TH2F*[4864];

    char name[100];

    for(Int_t i = 0 ; i < 4864 ; i++ ){

        sprintf(name, "distributionCrystalID_%i",i+1);
        fHistoFile->GetObject(name,fAngularDistributions[i]);

      }
   }

    return kSUCCESS;
 }



InitStatus R3BCalifaCrystalCal2Hit::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}



void R3BCalifaCrystalCal2Hit::Exec(Option_t* opt)
{
    Reset(); // Reset entries in output arrays, local arrays

    const int  numCrystalHits = fCrystalCalData->GetEntries();

    R3BLOG(DEBUG, "Crystal hits at start:" << numCrystalHits);

    vector<R3BCalifaCrystalCalData*> allCrystalVec;
    vector<R3BCalifaCrystalCalData*> protonCandidatesVec;
    vector<R3BCalifaCrystalCalData*> gammaCandidatesVec;
    vector<R3BCalifaCrystalCalData*> saturatedCandidatesVec;

    Double_t cryEnergy;
    Int_t cryId;

    // ----- Real Data Processing ------
    if(!fSimulation){

    for(Int_t i = 0 ; i < numCrystalHits ; i++){

        cryId     = ((R3BCalifaCrystalCalData*)fCrystalCalData->At(i))->GetCrystalId();
        cryEnergy = ((R3BCalifaCrystalCalData*)fCrystalCalData->At(i))->GetEnergy();

        if(cryId <= 2432 && cryEnergy >= fGammaClusterThreshold)
         gammaCandidatesVec.push_back((R3BCalifaCrystalCalData*)fCrystalCalData->At(i));

        if(cryId > 2432 && cryEnergy >= fProtonClusterThreshold)
         protonCandidatesVec.push_back((R3BCalifaCrystalCalData*)fCrystalCalData->At(i));

        if(cryEnergy >= fCrystalThreshold)
         allCrystalVec.push_back((R3BCalifaCrystalCalData*)fCrystalCalData->At(i));

        if(cryId <= 2432 && cryEnergy > fSaturation)
         saturatedCandidatesVec.push_back((R3BCalifaCrystalCalData*)fCrystalCalData->At(i));

     }

    if(gammaCandidatesVec.size()==0 && protonCandidatesVec.size() == 0 && saturatedCandidatesVec.size() == 0)
     return;

 // Sort all vectors by energy
 std::sort(gammaCandidatesVec.begin(), gammaCandidatesVec.end(),compareByEnergy);
 std::sort(protonCandidatesVec.begin(), protonCandidatesVec.end(),compareByEnergy);

 // It does not make sense to sort saturated crystals by "energy"
 TVector3 mother_angles,angles;
 Double_t fRandTheta,fRandPhi;

 vector<Int_t> usedCrystals;

 // Proton clusters have priority
 while(protonCandidatesVec.size()){

  Int_t motherId = protonCandidatesVec.at(0)->GetCrystalId();

  califa_candidate cluster = {motherId,vector<Int_t>(),0.0,0.0,0.0,0.0,0.0};

  mother_angles = fCalifaGeo->GetAngles(motherId);

  if(fRand){

   fAngularDistributions[protonCandidatesVec.at(0)->GetCrystalId()- 1 - 2432]->GetRandom2(fRandPhi,fRandTheta);
   cluster.theta = TMath::DegToRad()*fRandTheta;
   cluster.phi = TMath::DegToRad()*fRandPhi;

  }

 else{

   cluster.theta = mother_angles.Theta();
   cluster.phi = mother_angles.Phi();

  }


  cluster.energy += protonCandidatesVec.at(0)->GetEnergy();
  cluster.nf += protonCandidatesVec.at(0)->GetNf();
  cluster.ns += protonCandidatesVec.at(0)->GetNs();
  cluster.crystalList.push_back(motherId);
  cluster.time = protonCandidatesVec.at(0)->GetTime();

  usedCrystals.push_back(motherId);



  // First we add all proton hits
  for(Int_t j = 0 ; j < allCrystalVec.size() ; j++ ){

    Int_t thisCryId = allCrystalVec.at(j)->GetCrystalId();

    if(thisCryId==motherId)
     continue;

    if(thisCryId > 2432 && !isInside(usedCrystals,thisCryId)) {

     angles = fCalifaGeo->GetAngles(thisCryId-2432);

     if(mother_angles.Angle(angles) <= fRoundWindow){

      usedCrystals.push_back(thisCryId);

      cluster.energy += allCrystalVec.at(j)->GetEnergy();
      cluster.nf += allCrystalVec.at(j)->GetNf();
      cluster.ns += allCrystalVec.at(j)->GetNs();
      cluster.crystalList.push_back(thisCryId);


      // if it is read in proton, then is not read in gamma
      if(fMap_Par->GetInUse(thisCryId-2432))
       usedCrystals.push_back(thisCryId-2432);

      }
    }
  }


  // Then we use the gamma ranges that could have some proton energy inside
  for(Int_t j = 0 ; j < allCrystalVec.size() ; j++ ){

    Int_t thisCryId = allCrystalVec.at(j)->GetCrystalId();

    if(thisCryId==motherId)
     continue;

    if(thisCryId <= 2432 && !isInside(usedCrystals,thisCryId)) {

     angles = fCalifaGeo->GetAngles(thisCryId);

     if(mother_angles.Angle(angles) <= fRoundWindow){

      usedCrystals.push_back(thisCryId);

      cluster.energy += allCrystalVec.at(j)->GetEnergy();
      cluster.nf += allCrystalVec.at(j)->GetNf();
      cluster.ns += allCrystalVec.at(j)->GetNs();
      cluster.crystalList.push_back(thisCryId);

      }
    }
  }



   AddHit(cluster.crystalList,cluster.energy,cluster.nf,cluster.ns,cluster.theta,cluster.phi,cluster.time,0);

   RemoveUsedCrystals(usedCrystals,allCrystalVec,protonCandidatesVec,gammaCandidatesVec,saturatedCandidatesVec);

}


 // Now gamma clusters

 while(gammaCandidatesVec.size()){

  Int_t motherId = gammaCandidatesVec.at(0)->GetCrystalId();

  califa_candidate cluster = {motherId,vector<Int_t>(),0.0,0.0,0.0,0.0,0.0};

  mother_angles = fCalifaGeo->GetAngles(motherId);

  if(fRand){

   fAngularDistributions[gammaCandidatesVec.at(0)->GetCrystalId()- 1]->GetRandom2(fRandPhi,fRandTheta);
   cluster.theta = TMath::DegToRad()*fRandTheta;
   cluster.phi = TMath::DegToRad()*fRandPhi;

  }

 else{

   cluster.theta = mother_angles.Theta();
   cluster.phi = mother_angles.Phi();

  }

  cluster.energy += gammaCandidatesVec.at(0)->GetEnergy();
  cluster.nf += gammaCandidatesVec.at(0)->GetNf();
  cluster.ns += gammaCandidatesVec.at(0)->GetNs();
  cluster.crystalList.push_back(motherId);
  cluster.time = gammaCandidatesVec.at(0)->GetTime();

  usedCrystals.push_back(motherId);


  for(Int_t j = 0 ; j < allCrystalVec.size() ; j++ ){

    Int_t thisCryId = allCrystalVec.at(j)->GetCrystalId();

    if(thisCryId==motherId)
     continue;

    if(thisCryId <= 2432 && !isInside(usedCrystals,thisCryId)) {

     angles = fCalifaGeo->GetAngles(thisCryId);

     if(mother_angles.Angle(angles) <= fRoundWindow){

      usedCrystals.push_back(thisCryId);

      cluster.energy += allCrystalVec.at(j)->GetEnergy();
      cluster.nf += allCrystalVec.at(j)->GetNf();
      cluster.ns += allCrystalVec.at(j)->GetNs();
      cluster.crystalList.push_back(thisCryId);

      // if it is read in gamma,and not used for protons then is not usable
      if(fMap_Par->GetInUse(thisCryId + 2432))
       usedCrystals.push_back(thisCryId + 2432);

      }
    }
  }

  //Then we use the proton ranges that could have some gamma energy inside (and not used for proton hits)
  for(Int_t j = 0 ; j < allCrystalVec.size() ; j++ ){

    Int_t thisCryId = allCrystalVec.at(j)->GetCrystalId();

    if(thisCryId==motherId)
     continue;

    if(thisCryId > 2432 && !isInside(usedCrystals,thisCryId)) {

     angles = fCalifaGeo->GetAngles(thisCryId - 2432);

     if(mother_angles.Angle(angles) <= fRoundWindow){

      usedCrystals.push_back(thisCryId);

      cluster.energy += allCrystalVec.at(j)->GetEnergy();
      cluster.nf += allCrystalVec.at(j)->GetNf();
      cluster.ns += allCrystalVec.at(j)->GetNs();
      cluster.crystalList.push_back(thisCryId);

      }
    }
  }


   AddHit(cluster.crystalList,cluster.energy,cluster.nf,cluster.ns,cluster.theta,cluster.phi,cluster.time,1);

   RemoveUsedCrystals(usedCrystals,allCrystalVec,protonCandidatesVec,gammaCandidatesVec,saturatedCandidatesVec);

    }

  // Saturated Ones
   while(saturatedCandidatesVec.size()){

    Int_t motherId = saturatedCandidatesVec.at(0)->GetCrystalId();

    califa_candidate cluster = {motherId,vector<Int_t>(),0.0,0.0,0.0,0.0,0.0};

    mother_angles = fCalifaGeo->GetAngles(motherId);

    if(fRand){

     fAngularDistributions[saturatedCandidatesVec.at(0)->GetCrystalId()- 1]->GetRandom2(fRandPhi,fRandTheta);
     cluster.theta = TMath::DegToRad()*fRandTheta;
     cluster.phi = TMath::DegToRad()*fRandPhi;

    }

   else{

     cluster.theta = mother_angles.Theta();
     cluster.phi = mother_angles.Phi();

    }

    cluster.energy += saturatedCandidatesVec.at(0)->GetEnergy();
    cluster.nf += saturatedCandidatesVec.at(0)->GetNf();
    cluster.ns += saturatedCandidatesVec.at(0)->GetNs();
    cluster.crystalList.push_back(motherId);
    cluster.time = saturatedCandidatesVec.at(0)->GetTime();

    usedCrystals.push_back(motherId);


    for(Int_t j = 0 ; j < allCrystalVec.size() ; j++ ){

      Int_t thisCryId = allCrystalVec.at(j)->GetCrystalId();

      if(thisCryId==motherId)
       continue;

      if(thisCryId > 2432 && !isInside(usedCrystals,thisCryId)) {

       angles = fCalifaGeo->GetAngles(thisCryId-2432);

       if(mother_angles.Angle(angles) <= fRoundWindow){

        usedCrystals.push_back(thisCryId);

        cluster.energy += allCrystalVec.at(j)->GetEnergy();
        cluster.nf += allCrystalVec.at(j)->GetNf();
        cluster.ns += allCrystalVec.at(j)->GetNs();
        cluster.crystalList.push_back(thisCryId);

        // if it is read in proton, it is not read in gamma
        if(fMap_Par->GetInUse(thisCryId - 2432))
         usedCrystals.push_back(thisCryId - 2432);

        }
      }
    }

    //Then we use the gamma ranges that could have some proton energy inside (near a saturation)
    for(Int_t j = 0 ; j < allCrystalVec.size() ; j++ ){

      Int_t thisCryId = allCrystalVec.at(j)->GetCrystalId();

      if(thisCryId==motherId)
       continue;

      if(thisCryId <= 2432 && !isInside(usedCrystals,thisCryId)) {

       angles = fCalifaGeo->GetAngles(thisCryId);

       if(mother_angles.Angle(angles) <= fRoundWindow){

        usedCrystals.push_back(thisCryId);

        cluster.energy += allCrystalVec.at(j)->GetEnergy();
        cluster.nf += allCrystalVec.at(j)->GetNf();
        cluster.ns += allCrystalVec.at(j)->GetNs();
        cluster.crystalList.push_back(thisCryId);

        }
      }
    }


     AddHit(cluster.crystalList,cluster.energy,cluster.nf,cluster.ns,cluster.theta,cluster.phi,cluster.time,2);

     RemoveUsedCrystals(usedCrystals,allCrystalVec,protonCandidatesVec,gammaCandidatesVec,saturatedCandidatesVec);


    }

  }


 // -------- Simulation Data --------
 else {

  for(Int_t i = 0 ; i < numCrystalHits ; i++){

        cryId     = ((R3BCalifaCrystalCalData*)fCrystalCalData->At(i))->GetCrystalId();
        cryEnergy = ((R3BCalifaCrystalCalData*)fCrystalCalData->At(i))->GetEnergy();

        if(cryEnergy >= fGammaClusterThreshold && cryEnergy < fProtonClusterThreshold)
         gammaCandidatesVec.push_back((R3BCalifaCrystalCalData*)fCrystalCalData->At(i));

        if(cryEnergy >= fProtonClusterThreshold)
         protonCandidatesVec.push_back((R3BCalifaCrystalCalData*)fCrystalCalData->At(i));

        if(cryEnergy >= fCrystalThreshold)
         allCrystalVec.push_back((R3BCalifaCrystalCalData*)fCrystalCalData->At(i));

     }

    if(gammaCandidatesVec.size()==0 && protonCandidatesVec.size() == 0)
     return;

 // Sort all vectors by energy
 std::sort(gammaCandidatesVec.begin(), gammaCandidatesVec.end(),compareByEnergy);
 std::sort(protonCandidatesVec.begin(), protonCandidatesVec.end(),compareByEnergy);

 TVector3 mother_angles,angles;
 Double_t fRandTheta,fRandPhi;

 vector<Int_t> usedCrystals;

 // Proton clusters have priority
 while(protonCandidatesVec.size()){

  Int_t motherId = protonCandidatesVec.at(0)->GetCrystalId();

  califa_candidate cluster = {motherId,vector<Int_t>(),0.0,0.0,0.0,0.0,0.0};

  mother_angles = fCalifaGeo->GetAngles(motherId);

  if(fRand){

   fAngularDistributions[protonCandidatesVec.at(0)->GetCrystalId()- 1]->GetRandom2(fRandPhi,fRandTheta);
   cluster.theta = TMath::DegToRad()*fRandTheta;
   cluster.phi = TMath::DegToRad()*fRandPhi;

  }

 else{

   cluster.theta = mother_angles.Theta();
   cluster.phi = mother_angles.Phi();

  }


  cluster.energy += protonCandidatesVec.at(0)->GetEnergy();
  cluster.nf += protonCandidatesVec.at(0)->GetNf();
  cluster.ns += protonCandidatesVec.at(0)->GetNs();
  cluster.crystalList.push_back(motherId);
  cluster.time = protonCandidatesVec.at(0)->GetTime();

  usedCrystals.push_back(motherId);



  // First we add all proton hits
  for(Int_t j = 0 ; j < allCrystalVec.size() ; j++ ){

    Int_t thisCryId = allCrystalVec.at(j)->GetCrystalId();

    if(thisCryId==motherId)
     continue;

    if(!isInside(usedCrystals,thisCryId)) {

     angles = fCalifaGeo->GetAngles(thisCryId);

     if(mother_angles.Angle(angles) <= fRoundWindow){

      usedCrystals.push_back(thisCryId);

      cluster.energy += allCrystalVec.at(j)->GetEnergy();
      cluster.nf += allCrystalVec.at(j)->GetNf();
      cluster.ns += allCrystalVec.at(j)->GetNs();
      cluster.crystalList.push_back(thisCryId);

      }
    }
  }

   AddHit(cluster.crystalList,cluster.energy,cluster.nf,cluster.ns,cluster.theta,cluster.phi,cluster.time,0);

   RemoveUsedCrystals(usedCrystals,allCrystalVec,protonCandidatesVec,gammaCandidatesVec,saturatedCandidatesVec);

}


 // Now gamma clusters

 while(gammaCandidatesVec.size()){

  Int_t motherId = gammaCandidatesVec.at(0)->GetCrystalId();

  califa_candidate cluster = {motherId,vector<Int_t>(),0.0,0.0,0.0,0.0,0.0};

  mother_angles = fCalifaGeo->GetAngles(motherId);

  if(fRand){

   fAngularDistributions[gammaCandidatesVec.at(0)->GetCrystalId()- 1]->GetRandom2(fRandPhi,fRandTheta);
   cluster.theta = TMath::DegToRad()*fRandTheta;
   cluster.phi = TMath::DegToRad()*fRandPhi;

  }

 else{

   cluster.theta = mother_angles.Theta();
   cluster.phi = mother_angles.Phi();

  }

  cluster.energy += gammaCandidatesVec.at(0)->GetEnergy();
  cluster.nf += gammaCandidatesVec.at(0)->GetNf();
  cluster.ns += gammaCandidatesVec.at(0)->GetNs();
  cluster.crystalList.push_back(motherId);
  cluster.time = gammaCandidatesVec.at(0)->GetTime();

  usedCrystals.push_back(motherId);


  for(Int_t j = 0 ; j < allCrystalVec.size() ; j++ ){

    Int_t thisCryId = allCrystalVec.at(j)->GetCrystalId();

    if(thisCryId==motherId)
     continue;

    if(!isInside(usedCrystals,thisCryId)) {

     angles = fCalifaGeo->GetAngles(thisCryId);

     if(mother_angles.Angle(angles) <= fRoundWindow){

      usedCrystals.push_back(thisCryId);

      cluster.energy += allCrystalVec.at(j)->GetEnergy();
      cluster.nf += allCrystalVec.at(j)->GetNf();
      cluster.ns += allCrystalVec.at(j)->GetNs();
      cluster.crystalList.push_back(thisCryId);

      }
    }
  }



   AddHit(cluster.crystalList,cluster.energy,cluster.nf,cluster.ns,cluster.theta,cluster.phi,cluster.time,1);

   RemoveUsedCrystals(usedCrystals,allCrystalVec,protonCandidatesVec,gammaCandidatesVec,saturatedCandidatesVec);

   }


 }

    return;
}

void R3BCalifaCrystalCal2Hit::Reset()
{
    // Clear the CA structure
    R3BLOG(DEBUG, "Clearing CalifaHitData Structure");
    if (fCalifaHitData)
        fCalifaHitData->Clear();
}

void R3BCalifaCrystalCal2Hit::SelectGeometryVersion(Int_t version)
{
    fGeometryVersion = version;
    R3BLOG(INFO, "to " << fGeometryVersion);
}



R3BCalifaHitData* R3BCalifaCrystalCal2Hit::AddHit(vector<Int_t> crystalList,
                                                  Double_t ene,
                                                  Double_t Nf,
                                                  Double_t Ns,
                                                  Double_t pAngle,
                                                  Double_t aAngle,
                                                  ULong64_t time,
                                                  Int_t clusterType)
{
    TClonesArray& clref = *fCalifaHitData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BCalifaHitData(crystalList,ene, Nf, Ns, pAngle, aAngle, time, clusterType);
}


ClassImp(R3BCalifaCrystalCal2Hit);
