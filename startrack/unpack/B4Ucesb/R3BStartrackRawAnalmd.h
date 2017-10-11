// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStartrackRawAna                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.04.2014 by M.Labiche                -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKRAWANA_H
#define R3BSTARTRACKRAWANA_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BStartrackRawAna : public FairTask {
public:
  R3BStartrackRawAna();
  virtual ~R3BStartrackRawAna();
  
  virtual InitStatus Init();
  
  virtual void Exec(Option_t *option);
  
  virtual void FinishTask();
   
private:
  Int_t fnEvents;
  
  TClonesArray *fRawData;
  
  TH1F *thw;
  TH1F *thh;
  TH1F *thm;
  TH1F *thsd;
  TH1F *tha;
  TH1F *thst;
  TH1F *the;
  TH1F *thts;
  TH1F *thtslbdiff;
  TH1F *thtsExt;
  TH1F *thtsExtlbdiff;
  TH1F *thif;
  TH1F *thic;
  
  void CreateHistos();
  
  void WriteHistos();
  
public:
  
  ClassDef(R3BStartrackRawAna, 0)
};


#endif


