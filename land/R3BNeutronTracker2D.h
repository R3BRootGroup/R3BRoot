// -----------------------------------------------------------------------------
// -----                      R3BNeutronTracker2D                          -----
// -----                Created 29-02-2012 by D.Kresan                     -----
// -----            Based on algorithm developed by M.Heil                 -----
// -----------------------------------------------------------------------------

#ifndef R3BNEUTRONTRACKER2D_H
#define R3BNEUTRONTRACKER2D_H

#include <vector>
#include <map>

#include "FairTask.h"

#include "R3BPrimPart.h"
#include "R3BNeuLandCluster.h"

class TClonesArray;
class TH1F;
class TH2F;
class R3BNeutronTrack;


class R3BNeutronTracker2D : public FairTask
{
 public:
  /** Default constructor **/  
  R3BNeutronTracker2D();
  
  /** Destructor **/
  ~R3BNeutronTracker2D();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();

  void UseBeam(Double_t beam_energy, Double_t beam_beta);
  
  void ReadCalibrFile(char *name);

 private:
  R3BNeutronTrack* AddHit(TVector3 pos_in, TVector3 pos_out,
                          TVector3 momOut, Double_t time);
  TClonesArray* fLandPoints;
  TClonesArray* fLandMCTrack; 
  TClonesArray* fLandDigi;
  TClonesArray* fLandFirstHits;

  // Parameter class
  R3BLandDigiPar* fLandDigiPar;

  TClonesArray* fNeutronTracks;

  // Add here control hist for tracker
  TH1F *hNeutmult;
  TH1F *hErel1;
  TH1F *hErel2;
  TH1F *hErel3;
  TH1F *hErel4;

  TH1F *hMinv;
  TH1F *hMinv0;
  TH1F *hExce;
  TH1F *hExce0;
  TH1F *hMinv1;
  TH1F *hMinv2;
  TH1F *hDeltaPx1;
  TH1F *hDeltaPy1;
  TH1F *hDeltaPz1;
  TH1F *hDeltaPx2;
  TH1F *hDeltaPy2;
  TH1F *hDeltaPz2;
  TH1F *hClusters1;  

  TH1F *hDeltaX;
  TH1F *hDeltaY;
  TH1F *hDeltaZ;
  TH1F *hDeltaT;

  TH1F *hFirstHitZ;

  TH1F *hDeltaP1;
  TH1F *hDeltaP2;
  TH1F *hDeltaP3;
  TH1F *hDeltaP4;
  TH1F *hDeltaP5;
  TH1F *hDeltaP6;

  TH1F *hDelta;

  TH1F *h_theta_cand;
  TH1F *h_beta_min;
  TH1F *h_beta_max;
  TH1F *h_ntracks;
  TH1F *h_nhits;
  TH1F *h_theta;

  TH1F *h_nofclusters;
  TH1F *h_etot;
  TH2F *h_ncl_etot;
  TH2F *h_ncl_etot_1;
  TH2F *h_ndigi_etot;
  TH2F *h_ncl1_etot;


  Int_t npaddles;
  Int_t nplanes;
  Int_t printing;

  Double_t beamEnergy;
  Double_t beamBeta;
  Double_t plength; // half length of paddle
  Double_t att; // light attenuation factor [1/cm]
  Double_t amu; //atomic mass unit in MeV/c**2
  Double_t mNeutron; //mass of neutron in amu
  Double_t cMedia; // speed of light in material in cm/ns
  Double_t calFactor; //calibration factor energy of LAND paddles
  Int_t eventNo;
  R3BPrimPart **PRIM_part;
  R3BPrimPart **PRIM_prot;
  R3BPrimPart **PRIM_frag;
  R3BPrimPart **PRIM_gamma;

  Int_t Nclusters;
  Double_t dio;
  Double_t sumTotalEnergy;

  Float_t fKappa;
  Float_t fCuts[5];

  Int_t fNofClusters;
  Double_t fEtot;
  TClonesArray *fArrayCluster;
  std::vector<R3BNeuLandCluster*> fVectorCluster;
  Int_t fNofClusters1;
  Int_t fNofTracks;
  Int_t fTracks[1000][300];
  Int_t fNofHits[1000];
  Bool_t fMapTracks[1000];
  Int_t nTemp;
  Int_t nNeut;
  Double_t fMinvTrue;
  Double_t fMinv;
  Double_t fExceTrue;
  Double_t fExce;
  Int_t fNPrimNeut;
  Int_t fNPrimProt;
  Int_t fNPrimGamma;
  Int_t fNofFrag;

  Int_t nPrim;

  Double_t temp[3000][14];
  Int_t nentries;

  virtual void SetParContainers();

  void CreateHistograms();
  Int_t AdvancedMethod();
  void CalculateMassInv();
  void CalculateExce();
  void SortClustersBeta();
  void NextIteration(Int_t curIndex, R3BNeuLandCluster *curClus);
  Bool_t IsElastic(R3BNeuLandCluster *c1, R3BNeuLandCluster *c2);

 public: 
  ClassDef(R3BNeutronTracker2D,1)  
};


#endif
