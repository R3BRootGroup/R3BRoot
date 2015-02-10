// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                            R3BLandRawAna                          -----
// -----                      Created  @ 01.2014 by M.I. Cherciu           -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BLANDRAWANA_H
#define R3BLANDRAWANA_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BLandRawAna : public FairTask
{
  public:
    R3BLandRawAna();
    R3BLandRawAna(const char* name, Int_t iVerbose);
    virtual ~R3BLandRawAna();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

  private:
    Int_t fnEvents;

    TClonesArray* fLandRawHit;
    TClonesArray* fLandRawHitMapped;
    TClonesArray* fLosRawHit;

    TH1F* fh_land_raw_sam;
    TH1F* fh_land_raw_gtb;
    TH1F* fh_land_raw_tacaddr;
    TH1F* fh_land_raw_tacch;
    TH1F* fh_land_raw_cal;
    TH1F* fh_land_raw_clock;
    TH1F* fh_land_raw_tac;
    TH1F* fh_land_raw_qdc;

    TH1F* fh_land_mapped_is17;
    TH1F* fh_land_mapped_barid;
    TH1F* fh_land_mapped_side;
    TH1F* fh_land_mapped_clock;
    TH1F* fh_land_mapped_tac;
    TH1F* fh_land_mapped_qdc;

    TH1F* fh_los_raw_ch;
    TH1F* fh_los_raw_tdc;
    TH1F* fh_los_raw_clock;

    void CreateHistos();

    void WriteHistos();

  public:
    ClassDef(R3BLandRawAna, 0)
};

#endif
