// ------------------------------------------------------------
// -----                    R3BStarTrackEcal                   -----
// -----          Created 22-04-2014 by D.Kresan          -----
// ------------------------------------------------------------

#ifndef R3BSTARTRACKECAL
#define R3BSTARTRACKECAL

#include <map>

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;
class R3BStarTrackCalPar;
class R3BStarTrackECalPar;

class R3BStarTrackEcal : public FairTask
{
  public:
    R3BStarTrackEcal();
    R3BStarTrackEcal(const char* name, Int_t iVerbose = 1);
    virtual ~R3BStarTrackEcal();

    virtual InitStatus Init();
    virtual void SetParContainers();
    virtual InitStatus ReInit();
    virtual void Exec(Option_t* option);
    virtual void FinishEvent();
    virtual void FinishTask();
    
    inline void SetBeam(Bool_t isBeam = kTRUE)
    {
        fIsBeam = isBeam;
    }
    inline void SetNofModules(Int_t nADCs, Int_t n17)
    {
        fNofADCs = nADCs;
        fNof17 = n17;
    }

  private:
    Int_t fNEvents;
    std::map<Int_t, R3BStarTrackECalPar*> fMapPar;
    TClonesArray* fRawHit;
    TClonesArray* fAdc;
    Int_t fNAdc;
    R3BStarTrackCalPar* fEcalPar;
    Bool_t fIsBeam;
    Int_t fNofADCs;
    Int_t fNof17;
    std::map<Int_t, Bool_t> fMap17Seen;
    std::map<Int_t, Double_t> fMapStopTime;
    std::map<Int_t, Int_t> fMapStopClock;
    
    TH2F *fh_tres;
    
    TH1F *fh_mult;
    
    Double_t Interpolate(Int_t adc, R3BStarTrackECalPar* par);
    
    Bool_t FindChannel(Int_t channel, R3BStarTrackECalPar** par);

  public:
    ClassDef(R3BStarTrackEcal, 1)
};

#endif
