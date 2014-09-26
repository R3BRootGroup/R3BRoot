// ----------------------------------------------------------------
// -----                     R3BLandTcalFill                  -----
// -----             Created 04-04-2014 by D.Kresan           -----
// ----------------------------------------------------------------

#ifndef R3BLANDTCALFILL_H
#define R3BLANDTCALFILL_H

#include "FairTask.h"

class R3BLandCalPar;
class TClonesArray;
class TH1F;

class R3BLandTcalFill : public FairTask
{

  public:
    R3BLandTcalFill();
    R3BLandTcalFill(const char* name, Int_t iVerbose = 1);
    virtual ~R3BLandTcalFill();

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

    inline void SetNofModules(Int_t nPMTs, Int_t n17)
    {
        fNofPMTs = nPMTs;
        fNof17 = n17;
    }

  private:
    Int_t fUpdateRate;
    Int_t fMinStats;

    Int_t fNofPMTs;
    Int_t fNof17;

    Int_t iMin;
    Int_t iMax;
    TH1F** fhData;
    TH1F** fhTime;

    Int_t fNEvents;
    Bool_t fStoreDB;
    R3BLandCalPar* fCal_Par;
    TClonesArray* fHits;

    void CreateContainers();

    void CalculateParams(Int_t iModule);

    Int_t CalculateBin(Int_t iModule, Double_t& prev_time, Int_t ibin1, Int_t ngroup);

    void WriteContainer(Int_t iModule);

  public:
    ClassDef(R3BLandTcalFill, 1)
};

#endif
