// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        from R3BCaloCrystalAna                     -----
// -----                    Created 18/07/14  by H.Alvarez                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKSTRIPANA_H
#define R3BSTARTRACKSTRIPANA_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BStartrackStripAna : public FairTask {
public:
  R3BStartrackStripAna();
  virtual ~R3BStartrackStripAna();
  
  virtual InitStatus Init();
  
  virtual void Exec(Option_t *option);
  
  virtual void FinishTask();
   
private:
  Int_t fnEvents;
  
  TClonesArray *fSiDetData;
    
  //TH1F *thWordType;
  //TH1F *thHitBit;
  TH1F *thModuleID;
  TH1F *thSide;
  TH1F *thAsicID;
  TH1F *thStripID;
  TH1F *thEnergy;
  TH1F *thTime;
  
  void CreateHistos();
  
  void WriteHistos();
  
public:
  
  ClassDef(R3BStartrackStripAna, 0)
};


#endif


