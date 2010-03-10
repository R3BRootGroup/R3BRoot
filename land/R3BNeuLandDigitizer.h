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

  //- Table data 
  Double_t x_pos[200],y_pos[200],z_pos[200];
  Double_t tof[200],pm[200][2];
  Double_t paddle_E[200][9];
  Double_t part_E[50],part_n[50],first_hit[4];
  Int_t nuhits[200];

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
  TH2F *h_etot_eloss_p[6];

  TH1F *h_eloss_e[6];
  TH1F *h_etot_e[6];
  TH2F *h_etot_eloss_e[6];

  TH1F *h_pdg;
  TH1F *h_detid;
  TH1F *h_segid;
  TH1F *h_cellid;
  TH1F *h_ne;
  TH1F *h_ch;
  TH1F *h_time;
  TH1F *h_energy;
  TH1F *h_xpos;
  TH1F *h_ypos;
  TH1F *h_zpos_e;
  TH1F *h_zpos_p;
  TH2F *h_xpos1_vs_ypos1;
  TH2F *h_xpos2_vs_ypos2;
  TH2F *h_xpos3_vs_ypos3;
  TH2F *h_xpos4_vs_ypos4;
  TH2F *h_xpos5_vs_ypos5;
  TH2F *h_xpos6_vs_ypos6;
  TH2F *h_pdg_vs_eloss;
  TH2F *h_zpos_vs_pdg;
  TH2F *h_zpos_vs_eloss;
  TH2F *h_zpos_vs_time;
  TH2F *h_pdg_vs_m0pdg;
  TH2F *h_pdg_vs_m1pdg;
  TH2F *h_pdg_vs_m2pdg;
  TH2F *h_pdg_vs_m3pdg;
  TH2F *h_energy_vs_eloss;
  TH2F *h_energy_vs_pdg;
  TH2F *h_energy_vs_zpos;
  TH2F *h_trackid_vs_m0trackid;
  TH2F *h_trackid_vs_m1trackid;
  TH2F *h_trackid_vs_m2trackid;
  TH2F *h_trackid_vs_m3trackid;
  Int_t no_interaction;
  Int_t mot0PID;
  Int_t mot1PID;
  Int_t mot2PID;
  Int_t mot3PID;

  private:
  virtual void SetParContainers();

 
  ClassDef(R3BNeuLandDigitizer,1);
  
};

#endif
