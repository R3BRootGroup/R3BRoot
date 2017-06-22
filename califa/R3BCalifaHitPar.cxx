// ----------------------------------------------------------------------------
// -----                                                                  -----
// -----                         R3BCalifaHitPar                          -----
// -----               Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                Modified 19/12/2016 by P.Cabanelas                -----
// -----                                                                  -----
// ----------------------------------------------------------------------------

#include <iostream>
#include "R3BCalifaHitPar.h"
#include "TMath.h"

R3BCalifaHitPar::R3BCalifaHitPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BCalifaHitPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCalifaHitPar::putParams() called"<<std::endl;

  if(!list) return;
  list->add("fThreshold", (Double_t)fThreshold);
  list->add("fCrystalResolution", (Double_t)fCrystalResolution);
  list->add("fDeltaPolar", (Double_t)fDeltaPolar);
  list->add("fDeltaAzimuthal", (Double_t)fDeltaAzimuthal);
  list->add("fDeltaAngleClust", (Double_t)fDeltaAngleClust);
  list->add("fClusteringAlgorithmSelector", (Int_t)fClusteringAlgorithmSelector);
  list->add("fParCluster1", (Double_t)fParCluster1);
 
}


Bool_t R3BCalifaHitPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BCalifaHitPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BCalifaHitPar::getParams() 1 ";
  
  if (!list->fill("fThreshold",&fThreshold,1)) return kFALSE;
  if (!list->fill("fCrystalResolution",&fCrystalResolution,1)) return kFALSE;
  if (!list->fill("fDeltaPolar",&fDeltaPolar,1)) return kFALSE;
  if (!list->fill("fDeltaAzimuthal",&fDeltaAzimuthal,1)) return kFALSE;
  if (!list->fill("fDeltaAngleClust",&fDeltaAngleClust,1)) return kFALSE;
  if (!list->fill("fClusteringAlgorithmSelector",&fClusteringAlgorithmSelector,1)) return kFALSE;
  if (!list->fill("fParCluster1",&fParCluster1,1)) return kFALSE;
 
  return kTRUE;
}


void R3BCalifaHitPar::Print(Option_t* option) const
{
    std::cout<<"-I- CALIFA HitFinder Parameters:"<<std::endl;
    std::cout<<"fThreshold " << fThreshold <<std::endl;
    std::cout<<"fCrystalResolution " << fCrystalResolution <<std::endl;
    std::cout<<"fDeltaPolar " << fDeltaPolar <<std::endl;
    std::cout<<"fDeltaAzimuthal " << fDeltaAzimuthal <<std::endl;
    std::cout<<"fDeltaAngleClust " << fDeltaAngleClust <<std::endl;
    std::cout<<"fClusteringAlgorithmSelector " << fClusteringAlgorithmSelector <<std::endl;
    std::cout<<"fParCluster1 " << fParCluster1 <<std::endl;    
}

ClassImp(R3BCalifaHitPar);
