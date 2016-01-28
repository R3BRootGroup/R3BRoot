// ---------------------------------------------------------------------------------------
// -----                              R3BLandCosmic1                                 -----
// -----         Copied from Land02/detector/land/n_gen_det_cosmic1 on 11-2015       -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLANDCOSMIC1_H
#define R3BLANDCOSMIC1_H

#include "FairTask.h"
#include "R3BLandCosmic1Util.h"

#include "stdint.h"
#include "stdlib.h"
#include <set>

class R3BLandTSyncPar;
class TClonesArray;
class TCanvas;
class TGraph;
class TF1;

typedef std::set<uint32_t> ident_no_set;
typedef std::pair<float,int> pair_value;

struct nc_diff
{
  float _pos_track;
  float _pos_diff;

  uint32_t  _ident_no;

};

struct n_calib_diff
{
public:
  bool analyse_history(ident_no_set& bad_fit_idents);
  bool calc_params(ident_no_set& bad_fit_idents,double v[2]);

public:
  std::vector<nc_diff> _data;

};

struct nc_mean
{
  float _mean_diff;

  float _mean_corr;

  uint32_t  _ident_no;
};


struct n_calib_mean
{

public:
  bool analyse_history(ident_no_set& bad_fit_idents);
  bool calc_params(ident_no_set& bad_fit_idents,val_err_inv& mean);

public:
  std::vector<nc_mean> _data;
};

struct pdl_pm_pair
{
  float _rate[2];
  Int_t _favourite[2];

  Int_t _others[2][5];

  Int_t _flag;
};

class pair_correlation
{
public:
  pair_correlation(){
    _total = 0;
  }
public:
  std::vector<std::vector<Int_t> > _corr;
  Int_t _total;
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

class R3BLandCosmic1 : public FairTask
{

  public:
    R3BLandCosmic1();
    R3BLandCosmic1(const char* name, Int_t iVerbose = 1);
    virtual ~R3BLandCosmic1();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();

    virtual void FinishTask();

    void WritePlot(Bool_t b){
      writePlot = b;
    }

    void SetMaxPaddleDistFromLine(Double_t d){
	MAX_PADDLE_DIST_FROM_LINE = d;
    }
    
    void SetPlanes(Int_t planes){
	fPlanes = planes;
    }

  private:

    Bool_t writePlot = kFALSE;
    Double_t MAX_PADDLE_DIST_FROM_LINE = 1.0;
    Int_t fPlanes = 60;
    Int_t fPaddles = 50;
    
    TClonesArray* fLandPmt;
    R3BLandTSyncPar* fTSyncPar;

    TCanvas* canvas;
    std::vector<std::vector<bar*> > bars;
    TGraph* x_plot;
    TGraph* y_plot;
    TF1* x_fit;
    TF1* y_fit;
    Int_t nData;
      
    std::vector<std::vector<n_calib_diff> > _collect_diff;
    std::vector<std::vector<n_calib_mean> > _collect_mean_within;
    std::vector<std::vector<std::vector<n_calib_mean> > > _collect_mean_cross;
    std::vector<std::vector<n_calib_diff> > _collect_diff_e;
    std::vector<std::vector<n_calib_mean> > _collect_mean_within_e;
    std::vector<std::vector<std::vector<n_calib_mean> > > _collect_mean_cross_e;

    uint32_t _used_ident_no;

    ident_no_set _bad_fit_idents;

    std::vector<std::vector<std::vector<std::vector<uint32_t> > > > _stats;

    pair_correlation _pairs_pm;
    pair_correlation _pairs_pdl;

  public:
    ClassDef(R3BLandCosmic1, 0)
};

#endif
