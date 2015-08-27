
#ifndef _R3BTCAL_ENGINE_
#define _R3BTCAL_ENGINE_

#define TACQUILA_CLOCK_MHZ 40.002903
#define VFTX_CLOCK_MHZ 200

#include "TObject.h"

class TH1F;
class TF1;
class R3BTCalPar;

class R3BTCalEngine : public TObject
{
  public:
    R3BTCalEngine(R3BTCalPar* param, Int_t nModules, Int_t minStats = 10000);
    virtual ~R3BTCalEngine();

    void Fill(Int_t iModule, Int_t tdc);

    void CalculateParamTacquila();

  protected:
    Int_t CalculateBinTacquila(Int_t iModule, Double_t& prev_time, Int_t ibin, Int_t ngroup);

  private:
    Int_t fMinStats;
    Int_t fNModules;
    TH1F** fhData;
    TH1F** fhData100;
    TH1F** fhTime;
    TF1* f1;
    R3BTCalPar* fCal_Par;
    Int_t iMin;
    Int_t iMax;
    Double_t fClockFreq;

  public:
    ClassDef(R3BTCalEngine, 1)
};

#endif
