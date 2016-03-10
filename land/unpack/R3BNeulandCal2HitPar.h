// ---------------------------------------------------------------------------------------
// -----                           R3BNeulandCal2HitPar                              -----
// -----         Copied from Land02/detector/land/n_gen_det_cosmic1 on 11-2015       -----
// ---------------------------------------------------------------------------------------

#ifndef R3BNEULANDCAL2HITPAR_H
#define R3BNEULANDCAL2HITPAR_H

#include "FairTask.h"
#include "R3BLandCosmic1Util.h"

#ifdef __APPLE__ 
#include <_types/_uint8_t.h>
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h>
#include <_types/_uint64_t.h>
#else
#include <stdint.h>
#endif

#include "stdlib.h"
#include <set>

class R3BNeulandHitPar;
class TClonesArray;
class TCanvas;
class TGraph;
class TF1;

typedef std::set<uint32_t> ident_no_set;
typedef std::pair<Float_t,Int_t> pair_value;

struct nc_diff
{
  Float_t _pos_track;
  Float_t _pos_diff;

  uint32_t  _ident_no;

};

struct n_calib_diff
{
public:
  Bool_t analyse_history(ident_no_set& bad_fit_idents);
  Bool_t calc_params(ident_no_set& bad_fit_idents,Double_t y0[2], Double_t dydx[2]);

public:
  std::vector<nc_diff> _data;

};

struct nc_mean
{
  Float_t _mean_diff;

  Float_t _mean_corr;

  uint32_t  _ident_no;
};


struct n_calib_mean
{

public:
  Bool_t analyse_history(ident_no_set& bad_fit_idents);
  Bool_t calc_params(ident_no_set& bad_fit_idents,val_err_inv& mean);

public:
  std::vector<nc_mean> _data;
};

struct bar{
  Int_t fBarId;
  Int_t fPdl;
  Double_t fTime [2];
  Double_t fQdc [2];

  bar(Int_t barId, Int_t pdl) : fBarId(barId), fPdl(pdl){
    fTime[0] = 0;
    fTime[1] = 0;
    fQdc[0] = 0;
    fQdc[1] = 0;
  }
};

class R3BNeulandCal2HitPar : public FairTask
{

  public:
    R3BNeulandCal2HitPar();
    R3BNeulandCal2HitPar(const char* name, Int_t iVerbose = 1);
    virtual ~R3BNeulandCal2HitPar();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();

    virtual void FinishTask();

    void SetMaxPaddleDistFromLine(Double_t d){
	MAX_PADDLE_DIST_FROM_LINE = d;
    }
    
    void SetPlanes(Int_t planes){
	fPlanes = planes;
    }

  private:

    Double_t MAX_PADDLE_DIST_FROM_LINE = 1.0;
    Int_t fPlanes = 60;
    Int_t fPaddles = 50;
    
    TClonesArray* fLandPmt;
    R3BNeulandHitPar* fPar;

    std::vector<std::vector<bar*> > bars;
    TGraph* x_plot;
    TGraph* y_plot;
    TF1* x_fit;
    TF1* y_fit;
    
    Int_t nData = 0;
    Int_t fEventNumber = 0;
    
    std::vector<std::vector<n_calib_diff> > _collect_diff;
    std::vector<std::vector<n_calib_mean> > _collect_mean_within;
    std::vector<std::vector<std::vector<n_calib_mean> > > _collect_mean_cross;
    std::vector<std::vector<n_calib_diff> > _collect_diff_e;
    std::vector<std::vector<n_calib_mean> > _collect_mean_within_e;
    std::vector<std::vector<std::vector<n_calib_mean> > > _collect_mean_cross_e;

    uint32_t _used_ident_no;

    ident_no_set _bad_fit_idents;

  public:
    ClassDef(R3BNeulandCal2HitPar, 0)
};

#endif
