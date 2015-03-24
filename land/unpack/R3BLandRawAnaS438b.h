// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BLandRawAnaS438b                      -----
// -----                    Created  27-02-2015 by D. Kresan               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BLandRawAnaS438bS438B_H
#define R3BLandRawAnaS438bS438B_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;

class R3BLandRawAnaS438b : public FairTask
{
  public:
    R3BLandRawAnaS438b();
    R3BLandRawAnaS438b(const char* name, Int_t iVerbose);
    virtual ~R3BLandRawAnaS438b();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

  private:
    Int_t fnEvents;

    R3BEventHeader *fHeader;
    TClonesArray* fLandRawHitMapped;
    TClonesArray* fCaloRawHit;

    TH1F* fh_trigger;
    TH1F* fh_trigger_cut;
    TH2F* fhNhits;
    TH1F* fh_land_qdc;
    TH2F* fh_land_qdc_ncalo;
    TH2F* fh_eland_ecalo;

    void CreateHistos();

    void WriteHistos();

  public:
    ClassDef(R3BLandRawAnaS438b, 0)
};

#endif
