// -----------------------------------------------------------------------------
// -----                               R3BLandAna                          -----
// -----                     Created 22-04-2014 by D.Kresan                -----
// -----------------------------------------------------------------------------

#ifndef R3BLANDANA_H
#define R3BLANDANA_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BLandAna : public FairTask
{
  public:
    R3BLandAna();
    R3BLandAna(const char* name, Int_t iVerbose);
    virtual ~R3BLandAna();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();
    
    inline void SetNofBars(Int_t nofBars) { fNofBars = nofBars; }
    
    inline void SetFirstPlaneHorisontal() { fFirstPlaneHorisontal = kTRUE; }
    
    inline void SetMinimalLOSSignals(Int_t n) { fMinimalLOSSignals = n; }
    
    inline void SetTimeOffset(Double_t t) { fTimeOffset = t; }

  private:
    Int_t fNofBars;
    Int_t fMinimalLOSSignals = 4;
    Double_t fTimeOffset;
    Bool_t fFirstPlaneHorisontal;
    
    Int_t fnEvents;

    TClonesArray* fLandDigi;
    TClonesArray* fLosHit;

    TH1F* fh_land_barid;
    TH2F* fh_land_qdcbarid;
    TH1F* fh_land_tof;
    TH2F* fh_land_qdctof;
    TH2F* fh_land_timebarid;
    TH2F* fh_land_tofbarid;
    TH2F* fh_land_betabarid;
    TH2F* fh_land_yx;
    TH2F* fh_land_yx1;
    TH2F* fh_land_yx2;
    TH2F* fh_land_lbarid;
    TH2F* fh_land_ltime;
    
    TH1F* fh_land_beta;
    TH1F* fh_land_qdc;
    TH1F* fh_land_qdc_cut;
    
    TH1F* fh_los_time;
    
    TH2F* fh_los_corr;
    
    TH1F* fh_land_norm_tof_bar;
    TH1F* fh_land_norm_tof_plane;
    TH1F* fh_land_norm_tof_total;
        
    void CreateHistos();

    void WriteHistos();

  public:
    ClassDef(R3BLandAna, 0)
};

#endif
