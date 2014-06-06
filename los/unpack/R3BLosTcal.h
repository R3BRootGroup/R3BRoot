// ------------------------------------------------------------
// -----                     R3BLosTcal                   -----
// -----          Created 22-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#ifndef R3BLOSTCAL
#define R3BLOSTCAL

#include "FairTask.h"

class TClonesArray;
class R3BLosCalPar;
class R3BLosTCalPar;

class R3BLosTcal : public FairTask
{
  public:
    R3BLosTcal();
    R3BLosTcal(const char* name, Int_t iVerbose = 1);
    virtual ~R3BLosTcal();

    virtual InitStatus Init();
    virtual void SetParContainers();
    virtual InitStatus ReInit();
    virtual void Exec(Option_t* option);
    virtual void FinishEvent();
    virtual void FinishTask();
    
    inline void SetNofModules(Int_t nModules)
    {
        fNofModules = nModules;
    }

  private:
    TClonesArray* fRawHit;
    TClonesArray* fHit;
    Int_t fNHit;
    R3BLosCalPar* fTcalPar;
    Int_t fNofModules;
    
    Double_t Interpolate(Int_t tdc, R3BLosTCalPar* par);
    
    Bool_t FindChannel(Int_t channel, R3BLosTCalPar** par);

  public:
    ClassDef(R3BLosTcal, 1)
};

#endif
