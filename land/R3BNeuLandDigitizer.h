#ifndef R3BNEULANDDIGITIZER_H
#define R3BNEULANDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;


class R3BNeuLandDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BNeuLandDigitizer();


  /** Destructor **/
  ~R3BNeuLandDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();

  protected:
  TClonesArray* fLandPoints;
  TClonesArray* fLandMCTrack; 


  //- Control Hitograms
  TH1F *h_eloss_p[6];
  TH1F *h_pxout_p[6];
  TH1F *h_pyout_p[6];
  TH1F *h_pzout_p[6];
  TH1F *h_xin_p[6];
  TH1F *h_yin_p[6];
  TH1F *h_zin_p[6];
  TH1F *h_mom_p[6];
  TH1F *h_etot_p[6];
  TH1F *h_ekin_p[6];
  TH1F *h_time_p[6];
  TH2F *h_etot_eloss_p[6];

  TH1F *h_eloss_e[6];
  TH1F *h_etot_e[6];
  TH1F *h_ekin_e[6];
  TH1F *h_time_e[6];
  TH2F *h_etot_eloss_e[6];

  TH1F *h_pdg;
  TH1F *h_detid;
  TH1F *h_cellid;
  TH1F *h_trackid;
  TH1F *h_mot0trackid;
  TH1F *h_toteloss;
  TH1F *h_cellhits;
  TH1F *h_ne_p;
  TH1F *h_ne_e;
  TH1F *h_ne_exp_p;
  TH1F *h_ne_exp_e;
  TH1F *h_qind_p;
  TH1F *h_qind_e;
  TH1F *h_tentr_e;
  TH1F *h_tentr_p;
  TH1F *h_qindtot;
  TH1F *h_time;
  TH1F *h_stripflag;
  TH1F *h_xav;
  TH2F *h_gamma_eloss_p;
  TH2F *h_gamma_eloss_e;
  TH2F *h_pdg_vs_m0pdg;
  TH2F *h_trackid_vs_m0trackid;
  TH2F *h_cellhits_vs_totaleloss;
  TH2F *h_qind_tof_p;
  TH2F *h_qind_tof_e;

  TH1F *h_diff;
  TH1F *h_diffx;
  TH1F *h_diffy;
  TH1F *h_diffz;

  Int_t no_interaction;
  Int_t distcount;
  Int_t NoOfPEvt;

  private:
  virtual void SetParContainers();

 
  ClassDef(R3BNeuLandDigitizer,1);
  
};

#endif
