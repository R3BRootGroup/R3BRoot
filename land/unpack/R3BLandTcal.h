// ------------------------------------------------------------
// -----                    R3BLandTcal                   -----
// -----          Created 22-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#ifndef R3BLANDTCAL
#define R3BLANDTCAL

#include <map>

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;
class R3BLandCalPar;
class R3BLandTCalPar;
class R3BEventHeader;

class R3BLandTcal : public FairTask
{
  public:
    R3BLandTcal();
    R3BLandTcal(const char* name, Int_t iVerbose = 1);
    virtual ~R3BLandTcal();

    virtual InitStatus Init();
    virtual void SetParContainers();
    virtual InitStatus ReInit();
    virtual void Exec(Option_t* option);
    virtual void FinishEvent();
    virtual void FinishTask();
    
    inline void SetTrigger(Int_t trigger)
    {
        fTrigger = trigger;
    }
    
    inline void SetBeam(Bool_t isBeam = kTRUE)
    {
        fIsBeam = isBeam;
    }
    
    inline void SetNofModules(Int_t nPMTs, Int_t n17)
    {
        fNofPMTs = nPMTs;
        fNof17 = n17;
    }

  private:
    Int_t fNEvents;
    std::map<Int_t, R3BLandTCalPar*> fMapPar;
    R3BEventHeader* header;
    TClonesArray* fRawHit;
    TClonesArray* fPmt;
    Int_t fNPmt;
    R3BLandCalPar* fTcalPar;
    Int_t fTrigger;
    Bool_t fIsBeam;
    Int_t fNofPMTs;
    Int_t fNof17;
    std::map<Int_t, Bool_t> fMap17Seen;
    std::map<Int_t, Double_t> fMapStopTime;
    std::map<Int_t, Int_t> fMapStopClock;
    
    TH2F *fh_tres;
    
    TH1F *fh_mult;
    
    Double_t Interpolate(Int_t tdc, R3BLandTCalPar* par);
    
    Bool_t FindChannel(Int_t channel, R3BLandTCalPar** par);

  public:
    ClassDef(R3BLandTcal, 1)
};

#endif
