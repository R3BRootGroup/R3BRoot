// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                    R3BCalifaCrystalCalDataAnalysis                -----
// -----                    Created 18/07/14  by H.Alvarez                 -----
// -----                   Modified 20/03/17  by P.Cabanelas               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALIFACRYSTALCALDATAANALYSIS_H
#define R3BCALIFACRYSTALCALDATAANALYSIS_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BCalifaCrystalCalDataAnalysis : public FairTask {
public:
  R3BCalifaCrystalCalDataAnalysis();
  virtual ~R3BCalifaCrystalCalDataAnalysis();
  
  virtual InitStatus Init();
  
  virtual void Exec(Option_t *option);
  
  virtual void FinishTask();
   
private:
  Int_t fnEvents;
  
  TClonesArray *fCrystalData;
    
  TH1F *thCrystalID;
  TH1F *thEnergy;
  TH1F *thNf;
  TH1F *thNs;
  TH1F *thTime;
  
  void CreateHistos();
  
  void WriteHistos();
  
public:
  
  ClassDef(R3BCalifaCrystalCalDataAnalysis, 1)
};


#endif


