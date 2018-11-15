#ifndef R3BNEULANDCALTEST_H
#define R3BNEULANDCALTEST_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BNeulandCalTest : public FairTask
{
  public:
    R3BNeulandCalTest();
    R3BNeulandCalTest(const char* name, Int_t iVerbose);
    virtual ~R3BNeulandCalTest();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

  private:
    Int_t fnEvents;

    TClonesArray* fLandPmt;
    TClonesArray* fNeulandPmt;
    TClonesArray* fLosHit;

    TH2F* fh_los_corr;
    TH1F* fh_tof;
    TH2F* fh_qdctof;
    TH1F* fh_tdiff;

    TH2F* fh_qdctof_2;

    void CreateHistos();

    void WriteHistos();

  public:
    ClassDef(R3BNeulandCalTest, 1)
};

#endif
