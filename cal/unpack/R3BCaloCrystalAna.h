// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BCaloCrystalAna                          -----
// -----                    Created 18/07/14  by H.Alvarez                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALOCRYSTALANA_H
#define R3BCALOCRYSTALANA_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BCaloCrystalAna : public FairTask {
public:
  R3BCaloCrystalAna();
  virtual ~R3BCaloCrystalAna();
  
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
  
  ClassDef(R3BCaloCrystalAna, 0)
};


#endif


