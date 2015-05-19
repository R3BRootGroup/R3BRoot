// ----------------------------------------------------------------
// -----                     R3BLosTcalFill                   -----
// -----             Created 18-04-2014 by D.Kresan           -----
// ----------------------------------------------------------------

#ifndef R3BLOSTCALFILL_H
#define R3BLOSTCALFILL_H

#include "FairTask.h"

class R3BLosCalPar;
class TClonesArray;
class TH1F;
class TF1;

class R3BLosTcalFill : public FairTask
{

  public:
    R3BLosTcalFill();
    R3BLosTcalFill(const char* name, Int_t iVerbose = 1);
    virtual ~R3BLosTcalFill();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();

    virtual void FinishTask();

    inline void SetStoreDB(Bool_t st)
    {
        fStoreDB = st;
    }

    inline void SetUpdateRate(Int_t rate)
    {
        fUpdateRate = rate;
    }

    inline void SetMinStats(Int_t minStats)
    {
        fMinStats = minStats;
    }

    inline void SetNofModules(Int_t nCh)
    {
        fNofCh = nCh;
    }

  private:
    Int_t fUpdateRate;
    Int_t fMinStats;

    Int_t fNofCh;

    Int_t iMin;
    Int_t iMax;
    TH1F** fhData;
    TH1F** fhData100;
    TH1F** fhTime;
    TF1* f1;

    Int_t fNEvents;
    Bool_t fStoreDB;
    R3BLosCalPar* fCal_Par;
    TClonesArray* fHits;

    void CreateContainers();

    void CalculateParams(Int_t iModule);

    Int_t CalculateBin(Int_t iModule, Double_t& prev_time, Int_t ibin1, Int_t ngroup);

    void WriteContainer(Int_t iModule);

  public:
    ClassDef(R3BLosTcalFill, 1)
};

#endif
