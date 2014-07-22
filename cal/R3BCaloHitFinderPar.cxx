// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                         R3BCaloHitFinderPar                           -----
// -----                Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
#include "R3BCaloHitFinderPar.h"
#include "TMath.h"

R3BCaloHitFinderPar::R3BCaloHitFinderPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  clear();
}

void R3BCaloHitFinderPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloHitFinderPar::putParams() called"<<std::endl;

  if(!list) return;
  list->add("fThreshold", (Double_t)fThreshold);
  list->add("fCrystalResolution", (Double_t)fCrystalResolution);
  list->add("fDeltaPolar", (Double_t)fDeltaPolar);
  list->add("fDeltaAzimuthal", (Double_t)fDeltaAzimuthal);
  list->add("fDeltaAngleClust", (Double_t)fDeltaAngleClust);
  list->add("fClusteringAlgorithmSelector", (Int_t)fClusteringAlgorithmSelector);
  list->add("fParCluster1", (Double_t)fParCluster1);
 
}


Bool_t R3BCaloHitFinderPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloHitFinderPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BCaloHitFinderPar::getParams() 1 ";
  
  if (!list->fill("fThreshold",&fThreshold,1)) return kFALSE;
  if (!list->fill("fCrystalResolution",&fCrystalResolution,1)) return kFALSE;
  if (!list->fill("fDeltaPolar",&fDeltaPolar,1)) return kFALSE;
  if (!list->fill("fDeltaAzimuthal",&fDeltaAzimuthal,1)) return kFALSE;
  if (!list->fill("fDeltaAngleClust",&fDeltaAngleClust,1)) return kFALSE;
  if (!list->fill("fClusteringAlgorithmSelector",&fClusteringAlgorithmSelector,1)) return kFALSE;
  if (!list->fill("fParCluster1",&fParCluster1,1)) return kFALSE;
 
  return kTRUE;
}


void R3BCaloHitFinderPar::Print(Option_t* option) const
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

ClassImp(R3BCaloHitFinderPar);
