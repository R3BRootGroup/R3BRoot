// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                            R3BLandRawAna                          -----
// -----                    Created 23.04.2013 by D.Kresan                 -----
// -----------------------------------------------------------------------------

#ifndef R3BLANDRAWANA_H
#define R3BLANDRAWANA_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;


class R3BLandRawAna : public FairTask {
 public:
  R3BLandRawAna();
  virtual ~R3BLandRawAna();

  virtual InitStatus Init();

  virtual void Exec(Option_t *option);

  virtual void FinishTask();

 private:
  TClonesArray *fRawData;

  TH1F *fh_padId;
  TH2F *fh_qdc_padId;

  void CreateHistos();

  void WriteHistos();

 public:
  ClassDef(R3BLandRawAna, 0)
};


#endif


