// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                       R3BCalifaHitPar                             -----
// -----              Created 21/07/2014 by H. Alvarez-Pol                 -----
// -----               Modified 19/12/2016 by P.Cabanelas                  -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALIFAHITPAR_H
#define R3BCALIFAHITPAR_H

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"


class R3BCalifaHitPar : public FairParGenericSet
{
 public :
  R3BCalifaHitPar (const char* name="R3BCalifaHitPar",
		       const char* title="The Hit Finder Parameters",
		       const char* context="TestDefaultContext");
  ~R3BCalifaHitPar(void){};
  void clear(void){};
  void putParams(FairParamList* list);
  Bool_t getParams(FairParamList* list);
  
  void Print(Option_t* option="") const;
  /** Accessor functions **/
  const Double_t GetThreshold(){return fThreshold;}
  const Double_t GetCrystalResolution(){return fCrystalResolution;}
  const Double_t GetDeltaPolar(){return fDeltaPolar;}
  const Double_t GetDeltaAzimuthal(){return fDeltaAzimuthal;}
  const Double_t GetDeltaAngleClust(){return fDeltaAngleClust;}
  const Int_t GetClusteringAlgorithmSelector(){return fClusteringAlgorithmSelector;}
  const Double_t GetParCluster1(){return fParCluster1;}
   
  void SetThreshold(Double_t value){fThreshold=value;}
  void SetCrystalResolution(Double_t value){fCrystalResolution=value;}
  void SetDeltaPolar(Double_t value){fDeltaPolar=value;}
  void SetDeltaAzimuthal(Double_t value){fDeltaAzimuthal=value;}
  void SetDeltaAngleClust(Double_t value){fDeltaAngleClust=value;}
  void SetClusteringAlgorithmSelector(Int_t value){fClusteringAlgorithmSelector=value;}
  void SetParCluster1(Double_t value){fParCluster1=value;}
  
 private:   
  // Minimum energy requested in a crystal to be considered in a calorimeter Hit
  Double_t fThreshold;
  // Experimental resolution @ 1 MeV
  Double_t fCrystalResolution;
  // Angular window (polar)
  Double_t fDeltaPolar;
  // Angular window (azimuthal)
  Double_t fDeltaAzimuthal;
  // Angular opening used for the clustering condition
  Double_t fDeltaAngleClust;
  // Clustering algorithm selector
  Int_t fClusteringAlgorithmSelector;
  // Clustering parameter 1
  Double_t fParCluster1;
 
  ClassDef(R3BCalifaHitPar,1); //
};

#endif /* !R3BCALIFAHITPAR_H*/

