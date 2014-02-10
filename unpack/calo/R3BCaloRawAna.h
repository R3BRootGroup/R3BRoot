// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          R3BCaloRawAna                            -----
// -----                           Version 0.1                             -----
// -----                    Created 03.02.2014 by Y.Gonzalez               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALORAWANA_H
#define R3BCALORAWANA_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BCaloRawAna : public FairTask {
public:
  R3BCaloRawAna();
  virtual ~R3BCaloRawAna();
  
  virtual InitStatus Init();
  
  virtual void Exec(Option_t *option);
  
  virtual void FinishTask();
   
private:
  Int_t fnEvents;
  
  TClonesArray *fRawData;
  
  TH1F *thc;
  TH1F *the;
  TH1F *tht;
  
  void CreateHistos();
  
  void WriteHistos();
  
public:
  
  ClassDef(R3BCaloRawAna, 0)
};


#endif


