
#ifndef R3B_PROPAGATE_H
#define R3B_PROPAGATE_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;
class R3BFieldPar;
class R3BTPropagator;
class R3BTGeoPar;

class R3BPropagate : public FairTask
{
  public:
    R3BPropagate(const char* name, Bool_t vis = kFALSE, Int_t verbose = 1);
    virtual ~R3BPropagate();

    virtual InitStatus Init();
    virtual InitStatus ReInit();

    virtual void SetParContainers();

    virtual void Exec(const Option_t* = "");

    virtual void Finish();

  private:
    Bool_t InitPropagator();

    R3BFieldPar* fFieldPar;

    R3BTPropagator* fPropagator;

    R3BTGeoPar* fmTofGeo;

    TClonesArray* fArraymTofPoint;
    TClonesArray* fArrayMCTracks;
    TClonesArray* fArrayGeoTracks;

    TH2F* fh2;
    TH1F* fh_res_1;

    Int_t Ni;
    Double_t x1[10000], y1[10000];

    Int_t fNEvents;

    Bool_t fVis;

    ClassDef(R3BPropagate, 1)
};

#endif //! R3B_PROPAGATE_H
