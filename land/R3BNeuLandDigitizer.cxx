#include "R3BNeuLandDigitizer.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"


#include "TVector3.h"
#include "TMath.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TH2F.h"
#include <string>
#include <iostream>


#include "R3BNeuLandPoint.h"
#include "R3BMCTrack.h"

using std::cout;
using std::endl;

		

R3BNeuLandDigitizer::R3BNeuLandDigitizer() :
  FairTask("R3BNeuLand Digitization scheme ") { 
}


R3BNeuLandDigitizer::~R3BNeuLandDigitizer() {
}


InitStatus R3BNeuLandDigitizer::Init() {
  
  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fLandPoints = (TClonesArray*) ioman->GetObject("NeuLandPoint");
  fLandMCTrack = (TClonesArray*) ioman->GetObject("MCTrack");
  
  // Register output array LandDigi
  //fDigis = new TClonesArray("R3BNeuLandDigi",1000);
  //ioman->Register("LandDigi", "Digital response in Land", fDigis, kTRUE);

  // Initialize control histograms
  TString elossNameP = "Eloss_P";
  TString elossNameE = "Eloss_E";
  TString PxOutNameP = "PxOut_P";
  TString PyOutNameP = "PyOut_P";
  TString PzOutNameP = "PzOut_P";
  TString xInNameP = "xIn_P";
  TString yInNameP = "yIn_P";
  TString zInNameP = "zIn_P";
  TString momNameP = "Momentum_P";
  TString etotNameP = "TotE_P";
  TString timeNameP = "Time_P";
  TString timeNameE = "Time_E";
  TString etotNameE = "TotE_E";
  TString etot_elossNameP = "TotE_P_vs_Eloss_P";
  TString etot_elossNameE = "TotE_E_vs_Eloss_E";
  TString ekinNameP = "Ekin_P";
  TString ekinNameE = "Ekin_E";

  TString elossTitleP = "Eloss_P [keV] ";
  TString elossTitleE = "Eloss_E [keV] ";
  TString PxOutTitleP = "PxOut_P [MeV/c] ";
  TString PyOutTitleP = "PyOut_P [MeV/c] ";
  TString PzOutTitleP = "PzOut_P [MeV/c] ";
  TString xInTitleP = "xIn_P [mm] ";
  TString yInTitleP = "yIn_P [mm] ";
  TString zInTitleP = "zIn_P [mm] ";
  TString momTitleP = "Momentum_P [MeV/c] ";
  TString etotTitleP = "TotE_P [MeV] ";
  TString timeTitleP = "Time_P [ns] ";
  TString timeTitleE = "Time_E [ns] ";
  TString etotTitleE = "TotE_E [keV] ";
  TString etot_elossTitleP = "TotE_P [MeV] vs Eloss_P [keV] ";
  TString etot_elossTitleE = "TotE_E [keV] vs Eloss_E [keV] ";
  TString ekinTitleP = "Ekin_P [MeV] ";
  TString ekinTitleE = "Ekin_E [keV] ";

  TString elossNamesP[6];
  TString elossNamesE[6];
  TString PxOutNamesP[6];
  TString PyOutNamesP[6];
  TString PzOutNamesP[6];
  TString xInNamesP[6];
  TString yInNamesP[6];
  TString zInNamesP[6];
  TString momNamesP[6];
  TString etotNamesP[6];
  TString timeNamesP[6];
  TString timeNamesE[6];
  TString etotNamesE[6];
  TString etot_elossNamesP[6];
  TString etot_elossNamesE[6];
  TString ekinNamesP[6];
  TString ekinNamesE[6];

  TString elossTitlesP[6];
  TString elossTitlesE[6];
  TString PxOutTitlesP[6];
  TString PyOutTitlesP[6];
  TString PzOutTitlesP[6];
  TString xInTitlesP[6];
  TString yInTitlesP[6];
  TString zInTitlesP[6];
  TString momTitlesP[6];
  TString etotTitlesP[6];
  TString etotTitlesE[6];
  TString timeTitlesP[6];
  TString timeTitlesE[6];
  TString etot_elossTitlesP[6];
  TString etot_elossTitlesE[6];
  TString ekinTitlesP[6];
  TString ekinTitlesE[6];

  TString elossXtitP = "energy loss of protons [keV]";
  TString elossXtitE = "energy loss of electrons [keV]";
  TString PxOutXtitP = "momentum x component of protons [MeV/c]";
  TString PyOutXtitP = "momentum y component of protons [MeV/c]";
  TString PzOutXtitP = "momentum z component of protons [MeV/c]";
  TString xInXtitP = "entering position x component of protons [mm]";
  TString yInXtitP = "entering position y component of protons [mm]";
  TString zInXtitP = "entering position z component of protons [mm]";
  TString momXtitP = "momentum of protons [MeV/c]";
  TString etotXtitP = "total energy of protons [MeV]";
  TString timeXtitP = "time for protons [ns]";
  TString timeXtitE = "time for electrons [ns]";
  TString etotXtitE = "total energy for electrons [keV]";
  TString ekinXtitP = "kinetic energy for protons [MeV]";
  TString ekinXtitE = "kinetic energy for electrons [keV]";

  TString cntTitle = "counts";

  char num[] = "012345";
  char *nm;
  nm = num;

  for(Int_t i = 0; nm[i]; i++) {
    elossNamesP[i] = elossNameP + nm[i];
    elossNamesE[i] = elossNameE + nm[i];
    PxOutNamesP[i] = PxOutNameP + nm[i];
    PyOutNamesP[i] = PyOutNameP + nm[i];
    PzOutNamesP[i] = PzOutNameP + nm[i];
    xInNamesP[i] = xInNameP + nm[i];
    yInNamesP[i] = yInNameP + nm[i];
    zInNamesP[i] = zInNameP + nm[i];
    momNamesP[i] = momNameP + nm[i];
    etotNamesP[i] = etotNameP + nm[i];
    timeNamesP[i] = timeNameP + nm[i];
    timeNamesE[i] = timeNameE + nm[i];
    etotNamesE[i] = etotNameE + nm[i];
    etot_elossNamesP[i] = etot_elossNameP + nm[i];
    etot_elossNamesE[i] = etot_elossNameE + nm[i];
    ekinNamesP[i] = ekinNameP + nm[i];
    ekinNamesE[i] = ekinNameE + nm[i];

    elossTitlesP[i] = elossTitleP + nm[i];
    elossTitlesE[i] = elossTitleE + nm[i];
    PxOutTitlesP[i] = PxOutTitleP + nm[i];
    PyOutTitlesP[i] = PyOutTitleP + nm[i];
    PzOutTitlesP[i] = PzOutTitleP + nm[i];
    xInTitlesP[i] = xInTitleP + nm[i];
    yInTitlesP[i] = yInTitleP + nm[i];
    zInTitlesP[i] = zInTitleP + nm[i];
    momTitlesP[i] = momTitleP + nm[i];
    etotTitlesP[i] = etotTitleP + nm[i];
    timeTitlesP[i] = timeTitleP + nm[i];
    timeTitlesE[i] = timeTitleE + nm[i];
    etotTitlesE[i] = etotTitleE + nm[i];
    etot_elossTitlesP[i] = etot_elossTitleP + nm[i];
    etot_elossTitlesE[i] = etot_elossTitleE + nm[i];
    ekinTitlesP[i] = ekinTitleP + nm[i];
    ekinTitlesE[i] = ekinTitleE + nm[i];
  }

  h_detid = new TH1F("DetID","Detector ID where deposition occured", 100, 99.5, 199.5);
  h_detid->SetXTitle("detector ID");
  h_detid->SetYTitle(cntTitle);
  h_cellid = new TH1F("CellID","Gas cell ID where deposition occured", 10, 0.5, 10.5);
  h_cellid->SetXTitle("cell ID");
  h_cellid->SetYTitle(cntTitle);

  h_trackid = new TH1F("TrackID","Track ID which deposited energy", 100, -1.5, 98.5);
  h_trackid->SetXTitle("track ID");
  h_trackid->SetYTitle(cntTitle);
  h_pdg = new TH1F("PDG","Code of particle deposited energy in gas", 3220, -220.5, 2999.5);
  h_pdg->SetXTitle("particle ID");
  h_pdg->SetYTitle(cntTitle);
  h_mot0trackid = new TH1F("Mot0TrackID","Mother of Track ID which deposited energy", 100, -1.5, 98.5);
  h_mot0trackid->SetXTitle("mother track ID");
  h_mot0trackid->SetYTitle(cntTitle);

  h_toteloss = new TH1F("TotEloss", "Total energy loss in an event", 3100, -10.05, 299.95);
  h_toteloss->SetXTitle("total energy loss in an event [keV]");
  h_toteloss->SetYTitle(cntTitle);
  h_cellhits = new TH1F("Cell_Hits","Number of hits in the gas cells in an event", 16, -0.5, 15.5);
  h_cellhits->SetXTitle("number of hits in the gas cells in an event");
  h_cellhits->SetYTitle(cntTitle);

  h_pdg_vs_m0pdg = new TH2F("PDG_vs_m0PDG","PDG vs m0PDG", 3220, -220.5, 2999.5, 3220, -220.5, 2999.5);
  h_pdg_vs_m0pdg->SetXTitle("particle ID");
  h_pdg_vs_m0pdg->SetYTitle("mother particle ID");
  h_trackid_vs_m0trackid = new TH2F("TrackId_vs_Mot0TrackId","TrackId vs Mother of TrackId",
				    100, -1.5, 98.5, 100, -1.5, 98.5);
  h_trackid_vs_m0trackid->SetXTitle("track ID");
  h_trackid_vs_m0trackid->SetYTitle("mother track ID");
  h_cellhits_vs_totaleloss = new TH2F("Cell_Hits_vs_Total Eloss","Number of hits vs Total Eloss in an event", 16, -0.5, 15.5, 3100, -10.05 , 299.95);
  h_cellhits_vs_totaleloss->SetXTitle("number of hits in the gas cells in an event");
  h_cellhits_vs_totaleloss->SetYTitle("total energy loss in an event [keV]");


  for(Int_t i=0; i<6; i++) {
    h_time_p[i] = new TH1F(timeNamesP[i], timeTitlesP[i], 1000, -0.001, 1.999);
    h_time_e[i] = new TH1F(timeNamesE[i], timeTitlesE[i], 1000, -0.0015, 2.9985);
    h_etot_p[i] = new TH1F(etotNamesP[i], etotTitlesP[i], 1100, 900.5, 2000.5);
    h_etot_e[i] = new TH1F(etotNamesE[i], etotTitlesE[i], 950, 495, 9995);
    h_ekin_p[i] = new TH1F(ekinNamesP[i], ekinTitlesP[i], 1100, -0.5, 1099.5);
    h_ekin_e[i] = new TH1F(ekinNamesE[i], ekinTitlesE[i], 10000, -0.5, 9999.5);
    h_pxout_p[i] = new TH1F(PxOutNamesP[i], PxOutTitlesP[i], 1000, -499.5, 500.5);
    h_pyout_p[i] = new TH1F(PyOutNamesP[i], PyOutTitlesP[i], 1000, -499.5, 500.5);
    h_pzout_p[i] = new TH1F(PzOutNamesP[i], PzOutTitlesP[i], 1100, -99.5, 1000.5);
    h_mom_p[i] = new TH1F(momNamesP[i], momTitlesP[i], 1050, -49.5, 1000.5);

    h_eloss_p[i] = new TH1F(elossNamesP[i], elossTitlesP[i], 3100, -10.05, 299.95);
    h_eloss_e[i] = new TH1F(elossNamesE[i], elossTitlesE[i], 3100, -10.05, 299.95);

    h_etot_eloss_p[i] = new TH2F(etot_elossNamesP[i], etot_elossTitlesP[i], 1100, 899.5, 1999.5, 3100, -10.05, 299.95);
    h_etot_eloss_e[i] = new TH2F(etot_elossNamesE[i], etot_elossTitlesE[i], 950, 495, 9995, 3100, -10.05, 299.95);

    h_xin_p[i] = new TH1F(xInNamesP[i], xInTitlesP[i], 200, 4.05, 24.05);
    h_yin_p[i] = new TH1F(yInNamesP[i], yInTitlesP[i], 200, -9.95, 10.05);
    h_zin_p[i] = new TH1F(zInNamesP[i], zInTitlesP[i], 160, 3.95, 19.95);

    
    h_time_p[i]->SetXTitle(timeXtitP);
    h_time_e[i]->SetXTitle(timeXtitE);
    h_etot_p[i]->SetXTitle(etotXtitP);
    h_etot_e[i]->SetXTitle(etotXtitE);
    h_ekin_p[i]->SetXTitle(ekinXtitP);
    h_ekin_e[i]->SetXTitle(ekinXtitE);
    h_pxout_p[i]->SetXTitle(PxOutXtitP);
    h_pyout_p[i]->SetXTitle(PyOutXtitP);
    h_pzout_p[i]->SetXTitle(PzOutXtitP);
    h_mom_p[i]->SetXTitle(momXtitP);

    h_eloss_p[i]->SetXTitle(elossXtitP);
    h_eloss_e[i]->SetXTitle(elossXtitE);

    h_etot_eloss_p[i]->SetXTitle(etotXtitP);
    h_etot_eloss_e[i]->SetXTitle(etotXtitE);

    h_xin_p[i]->SetXTitle(xInXtitP);
    h_yin_p[i]->SetXTitle(yInXtitP);
    h_zin_p[i]->SetXTitle(zInXtitP);

    h_time_p[i]->SetYTitle(cntTitle);
    h_time_e[i]->SetYTitle(cntTitle);
    h_etot_p[i]->SetYTitle(cntTitle);
    h_etot_e[i]->SetYTitle(cntTitle);
    h_ekin_p[i]->SetYTitle(cntTitle);
    h_ekin_e[i]->SetYTitle(cntTitle);
    h_pxout_p[i]->SetYTitle(cntTitle);
    h_pyout_p[i]->SetYTitle(cntTitle);
    h_pzout_p[i]->SetYTitle(cntTitle);
    h_mom_p[i]->SetYTitle(cntTitle);

    h_eloss_p[i]->SetYTitle(cntTitle);
    h_eloss_e[i]->SetYTitle(cntTitle);

    h_etot_eloss_p[i]->SetYTitle(elossXtitP);
    h_etot_eloss_e[i]->SetYTitle(elossXtitE);

    h_xin_p[i]->SetYTitle(cntTitle);
    h_yin_p[i]->SetYTitle(cntTitle);
    h_zin_p[i]->SetYTitle(cntTitle);

  }


  h_ne_p = new TH1F("NeP","Number of electrons based on Eloss P",200,-1.5,198.5);
  h_ne_p->SetXTitle("number of initial avalanche electrons based on Eloss for protons");
  h_ne_p->SetYTitle(cntTitle);
  h_ne_e = new TH1F("NeE","Number of electrons based on Eloss E",200,-1.5,198.5);
  h_ne_e->SetXTitle("number of initial avalanche electrons based on Eloss for electrons");
  h_ne_e->SetYTitle(cntTitle);
  h_ne_exp_p = new TH1F("NeExpP","Expected number of electrons P",200,-1.5,198.5);
  h_ne_exp_p->SetXTitle("expected number of initial avalanche electrons based on Lippmann for protons");
  h_ne_exp_p->SetYTitle(cntTitle);
  h_ne_exp_e = new TH1F("NeExpE","Expected number of electrons E",200,-1.5,198.5);
  h_ne_exp_e->SetXTitle("expected number of initial avalanche electrons based on Lippmann for electrons");
  h_ne_exp_e->SetYTitle(cntTitle);
  h_gamma_eloss_p = new TH2F("GammaP vs ElossP", "Gamma P vs Eloss P", 200, -0.005, 1.995, 3100, -10.05, 299.95);
  h_gamma_eloss_p->SetXTitle("Lorentz factor gamma for protons");
  h_gamma_eloss_p->SetYTitle(elossXtitP);
  h_gamma_eloss_e = new TH2F("GammaE vs ElossE", "Gamma E vs Eloss E", 1000, 60.005, 79.995, 200, -0.05, 19.95);
  h_gamma_eloss_e->SetXTitle("Lorentz factor gamma for electrons");
  h_gamma_eloss_e->SetYTitle(elossXtitE);
  h_qind_p = new TH1F("QindP","Induced charge P [pC]", 100, 0.5, 100.5);
  h_qind_p->SetXTitle("individual induced charge for protons [pC]");
  h_qind_p->SetYTitle(cntTitle);
  h_qind_e = new TH1F("QindE","Induced charge E [pC]", 100, -0.1, 19.9);
  h_qind_e->SetXTitle("individual induced charge for electrons [pC]");
  h_qind_e->SetYTitle(cntTitle);
  h_tentr_p = new TH1F("TentrP","Time for entries P [pC]", 500, -0.001, 0.999);
  h_tentr_p->SetXTitle("individual time for protons [ns]");
  h_tentr_p->SetYTitle(cntTitle);
  h_tentr_e = new TH1F("TentrE","Time for entries E [pC]", 500, -0.001, 0.999);
  h_tentr_e->SetXTitle("individual time for electrons [ns]");
  h_tentr_e->SetYTitle(cntTitle);
  h_qind_tof_p = new TH2F("QindP vs TOFP","Induced charge P [pC] vs TOF P [ns]", 100, 0.5, 100.5, 500, -0.001, 0.999);
  h_qind_tof_p->SetXTitle("individual induced charge for protons [pC]");
  h_qind_tof_p->SetYTitle("individual TOF for protons [ns]");
  h_qind_tof_e = new TH2F("QindE vs TOFE","Induced charge E [pC] vs TOF E [ns]", 100, -0.1, 19.9, 500, -0.001, 0.999);
  h_qind_tof_e->SetXTitle("individual induced charge for electrons [pC]");
  h_qind_tof_e->SetYTitle("individual TOF for electrons [ns]");
  h_qindtot = new TH1F("QindTotal","Total induced charge [pC]", 100, -0.1, 19.9);
  h_qindtot->SetXTitle("total induced charge [pC]");
  h_qindtot->SetYTitle(cntTitle);
  h_stripflag = new TH1F("StripFlag","StripFlag", 17, -1.5, 15.5);
  h_stripflag->SetXTitle("number of strip hit");
  h_stripflag->SetYTitle(cntTitle);
  h_xav = new TH1F("xAv","Average x hit position [mm]", 1000, -150, 150);
  h_xav->SetXTitle("average x hit position [mm]");
  h_xav->SetYTitle(cntTitle);

  h_diff = new TH1F("Diff","Diff", 10000, 0, 10);
  h_diffx = new TH1F("Diffx","Diffx", 100, -100, 100);
  h_diffy = new TH1F("Diffy","Diffy", 100, -100, 100);
  h_diffz = new TH1F("Diffz","Diffz", 10000, -40, 40);

  TString qindNameE = "QindStrE";
  TString tofNameE = "TOFStrE";
  TString qindtotNameE = "QindTotStrE";

  TString qindTitleE = "Individual induced charge E (strip) [pC] ";
  TString tofTitleE = "Individual TOF E (strip) [ns]";
  TString qindtotTitleE = "Total induced charge E (strip) [pC] ";

  TString qindNamesE[8];
  TString tofNamesE[8];
  TString qindtotNamesE[8];

  TString qindTitlesE[8];
  TString tofTitlesE[8];
  TString qindtotTitlesE[8];

  TString qindXtitE = "induced charge for electrons [pC]";
  TString tofXtitE = "TOF for electrons [ns]";
  TString qindtotXtitE = "induced charge [pC]";

  char num1[] = "01234567";
  char *nm1;
  nm1 = num1;

  for(Int_t i = 0; nm1[i]; i++) {
    qindNamesE[i] = qindNameE + nm1[i];
    tofNamesE[i] = tofNameE + nm1[i];
    qindtotNamesE[i] = qindtotNameE + nm1[i];

    qindTitlesE[i] = qindTitleE + nm1[i];
    tofTitlesE[i] = tofTitleE + nm1[i];
    qindtotTitlesE[i] = qindtotTitleE + nm1[i];
  }

  for(Int_t i=0; i<8; i++) {
    h_qindstr_e[i] = new TH1F(qindNamesE[i],qindTitlesE[i], 100, -0.1, 19.9);
    h_qindstr_e[i]->SetXTitle(qindXtitE);
    h_qindstr_e[i]->SetYTitle(cntTitle);
    h_tofstr_e[i] = new TH1F(tofNamesE[i],tofTitlesE[i], 500, -0.001, 0.999);
    h_qindstr_e[i]->SetXTitle(tofXtitE);
    h_qindstr_e[i]->SetYTitle(cntTitle);
    h_qindtotstr[i] = new TH1F(qindtotNamesE[i],qindtotTitlesE[i], 100, -0.1, 19.9);
    h_qindtotstr[i]->SetXTitle(qindtotXtitE);
    h_qindtotstr[i]->SetYTitle(cntTitle);
  }

  return kSUCCESS;

}

void R3BNeuLandDigitizer::SetParContainers() {
  
  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
  
  //FairRuntimeDb* db = run->GetRuntimeDb();
  //if ( ! db ) Fatal("SetParContainers", "No runtime database");
  
  
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void R3BNeuLandDigitizer::Exec(Option_t* opt) {
  
  //-Reset entries in output arrays
  //-Reset local arrays 
  Reset();
  
  Int_t nentries = fLandPoints->GetEntries();
  
  Int_t eventId[nentries];
  Int_t detId[nentries];
  Int_t cellId[nentries];

  Int_t TrackId[nentries];
  R3BMCTrack* aTrack[nentries];
  Int_t PID[nentries];
  Int_t Mot0TrackId[nentries];
  R3BMCTrack* aMot0Track[nentries];
  Int_t mot0PID[nentries];

  Double_t time[nentries];
  Double_t energy[nentries];
  Double_t mass[nentries];
  Double_t ekin[nentries];
  Double_t gamma[nentries];
  Double_t PxOut[nentries];
  Double_t PyOut[nentries];
  Double_t PzOut[nentries];
  Double_t momentum[nentries];

  Double_t eloss[nentries];
  Double_t totaleloss[nentries];
  Int_t cellhits[nentries];

  Double_t xIn[nentries];
  Double_t yIn[nentries];
  Double_t xOut[nentries];
  Double_t yOut[nentries];
  Double_t zIn[nentries];
  Double_t zOut[nentries];
  Double_t xAv[nentries];
  Double_t yAv[nentries];

  Bool_t pDetEvt[nentries];

  Int_t NofClusters[nentries];
  Int_t NeExp[nentries];
  Int_t Ne[nentries];
  Double_t Qind[nentries];
  Double_t QindStr[nentries][8];
  Double_t Tentr[nentries];
  Double_t TentrStr[nentries][8];
  Double_t TOF[nentries];
  Double_t TOFStr[nentries][8];

  Int_t StripFlag[nentries];
  Double_t xmin[8];
  Double_t xmax[8];

  Double_t QindTotal = 0.0;
  Double_t QindTotStr[8];
  Bool_t StripFlagAfter[8];
  
  // count the number of events in which no energy deposition occurred
  if(nentries == 0) no_interaction = no_interaction + 1;
  // end counting events in which no energy deposition occurred

  // fill arrays
  if(nentries > 0) {
    for (Int_t l=0; l<nentries; l++) {

      R3BNeuLandPoint* land_obj = (R3BNeuLandPoint*) fLandPoints->At(l);

      eventId[l] = land_obj->GetEventID();
      detId[l] = land_obj->GetDetID();
      cellId[l] = land_obj->GetCellID();

      TrackId[l] = land_obj->GetTrackID();
      aTrack[l] = (R3BMCTrack*) fLandMCTrack->At(TrackId[l]);
      PID[l] = aTrack[l]->GetPdgCode();
      if(PID[l] == 2212) {
	pDetEvt[l] = kTRUE;
      }
      else {
	pDetEvt[l] = kFALSE;
      }
      if(TrackId[l] > 0) {
	Mot0TrackId[l] = land_obj->GetMot0TrackID();
	aMot0Track[l] = (R3BMCTrack*) fLandMCTrack->At(Mot0TrackId[l]);
	mot0PID[l] = aMot0Track[l]->GetPdgCode();
      }
      else {
	Mot0TrackId[l] = 0;
	mot0PID[l] = 0;
      }

      time[l] = land_obj->GetTime();
      energy[l] = aTrack[l]->GetEnergy();
      mass[l] = aTrack[l]->GetMass();
      ekin[l] = (energy[l] - mass[l]); // MeV
      gamma[l] = energy[l] / mass[l];
      if(PID[l] == 2212) {
	energy[l] = energy[l] * 1E3; //MeV
	ekin[l] = ekin[l] * 1E3; //MeV
      }
      if(PID[l] == 11) {
	energy[l] = energy[l] * 1E6; //keV
	ekin[l] = ekin[l] * 1E6; //keV
      }
      PxOut[l] = land_obj->GetPxOut() * 1E3; // MeV/c
      PyOut[l] = land_obj->GetPyOut() * 1E3; // MeV/c
      PzOut[l] = land_obj->GetPzOut() * 1E3; // MeV/c
      momentum[l] = sqrt( PxOut[l] * PxOut[l] + PyOut[l] * PyOut[l] + PzOut[l] * PzOut[l]);

      eloss[l] = land_obj->GetEnergyLoss();
      totaleloss[l] = land_obj->GetTotalEloss();
      cellhits[l] = land_obj->GetCellHits();
      
      xIn[l] = land_obj->GetXIn() * 10; // mm
      yIn[l] = land_obj->GetYIn() * 10; // mm
      xOut[l] = land_obj->GetXOut() * 10; // mm
      yOut[l] = land_obj->GetYOut() * 10; // mm
      zIn[l] = land_obj->GetZIn() * 10; // mm
      zOut[l] = land_obj->GetZOut() * 10; // mm
      xAv[l] = ( xIn[l] + xOut[l] ) / 2; // mm
      yAv[l] = ( yIn[l] + yOut[l] ) / 2; // mm

      delta_AnodeGap = 3; // thickness of gap between the anode strips in mm
      Double_t delta_Strip = 25; // strip width in mm
      Double_t ShX = (8 * delta_Strip + 7 * delta_AnodeGap)/2; // half length in mm

      for(Int_t i=0; i<8; i++) {

	xmin[i] = -ShX + i * (delta_Strip + delta_AnodeGap);
	xmax[i] = -ShX + i * (delta_Strip + delta_AnodeGap) + delta_Strip;

      }

      StripFlag[l] = 0;

      for(Int_t i=0; i<8; i++) {
	if(xmin[i] <= xAv[l] && xAv[l] <= xmax[i]) StripFlag[l] = i;
	if(xmax[i] < xAv[l] && xAv[l] < xmin[i+1]) StripFlag[l] = i+8;
      }

    } // nentries for
  } // nentries > 0
  // end filling arrays

  R3BNeuLandPoint* land_obj1 = (R3BNeuLandPoint*) fLandPoints->At(0);

  
  // analyze events
  if(nentries > 0) {
    //cout << " eventId: " << eventId[0] << endl;
      for (Int_t l=0; l<nentries; l++) {
       /*
      // event info
      cout << " cellId: " << cellId[l] << " PID: " << PID[l] << " mot0PID: " << mot0PID[l]
	   << " time: " << time[l] << " energy: " << energy[l] << " eloss: " << eloss[l] << endl;
      if(l == nentries-1) cout << " cellhits: " << cellhits[l] << " totaleloss: " << totaleloss[l] << endl;
      // end event info
      */

      // count the events in which proton is involved
      if(l == 0) {
	if(pDetEvt[0] == kTRUE) {
	  NoOfPEvt = NoOfPEvt + 1;
	}
      }
      if(l > 0) {
	if(pDetEvt[l-1] == kTRUE) {
	  pDetEvt[l] = kFALSE;
	  if(pDetEvt[l] == kTRUE) {
	    NoOfPEvt = NoOfPEvt + 1;
	  }
	}
      }
      // end counting events in which proton is involved

      // fill histograms for checking
      h_detid->Fill(detId[l]);
      h_cellid->Fill(cellId[l]);
      h_trackid->Fill(TrackId[l]);
      h_pdg->Fill(PID[l]);
      h_mot0trackid->Fill(Mot0TrackId[l]);
      
      if(l == nentries-1) {
	h_toteloss->Fill(totaleloss[l]);
	h_cellhits->Fill(cellhits[l]);
	h_cellhits_vs_totaleloss->Fill(cellhits[l], totaleloss[l]);
      }
      
      h_pdg_vs_m0pdg->Fill(PID[l], mot0PID[l]);
      h_trackid_vs_m0trackid->Fill(TrackId[l], Mot0TrackId[l]);
      
      switch(cellId[l]){
      case 1:
	if (PID[l] == 2212 ){
	  h_time_p[0]->Fill(time[l]);
	  h_etot_p[0]->Fill(energy[l]);
	  h_ekin_p[0]->Fill(ekin[l]);
	  h_pxout_p[0]->Fill(PxOut[l]);
	  h_pyout_p[0]->Fill(PyOut[l]);
	  h_pzout_p[0]->Fill(PzOut[l]);
	  h_mom_p[0]->Fill(momentum[l]);
	  
	  h_eloss_p[0]->Fill(eloss[l]);
	  h_etot_eloss_p[0]->Fill(energy[l],eloss[l]);
	  
	  h_xin_p[0]->Fill(xIn[l]);
	  h_yin_p[0]->Fill(yIn[l]);
	  h_zin_p[0]->Fill(zIn[l]);
	}
	if (PID[l] == 11 ){
	  h_time_e[0]->Fill(time[l]);
	  h_etot_e[0]->Fill(energy[l]);
	  h_ekin_e[0]->Fill(ekin[l]);
	  
	  h_eloss_e[0]->Fill(eloss[l]);
	  h_etot_eloss_e[0]->Fill(energy[l],eloss[l]);
	}
	break; // case 1
      case 2:
	if (PID[l] == 2212 ){
	  h_time_p[1]->Fill(time[l]);
	  h_etot_p[1]->Fill(energy[l]);
	  h_ekin_p[1]->Fill(ekin[l]);
	  h_pxout_p[1]->Fill(PxOut[l]);
	  h_pyout_p[1]->Fill(PyOut[l]);
	  h_pzout_p[1]->Fill(PzOut[l]);
	  h_mom_p[1]->Fill(momentum[l]);
	  
	  h_eloss_p[1]->Fill(eloss[l]);
	  h_etot_eloss_p[1]->Fill(energy[l],eloss[l]);
	  
	  h_xin_p[1]->Fill(xIn[l]);
	  h_yin_p[1]->Fill(yIn[l]);
	  h_zin_p[1]->Fill(zIn[l]);
	}
	if (PID[l] == 11 ){
	  h_time_e[1]->Fill(time[l]);
	  h_etot_e[1]->Fill(energy[l]);
	  h_ekin_e[1]->Fill(ekin[l]);
	  
	  h_eloss_e[1]->Fill(eloss[l]);
	  h_etot_eloss_e[1]->Fill(energy[l],eloss[l]);
	}
	break; // case 2
      case 3:
	if (PID[l] == 2212 ){
	  h_time_p[2]->Fill(time[l]);
	  h_etot_p[2]->Fill(energy[l]);
	  h_ekin_p[2]->Fill(ekin[l]);
	  h_pxout_p[2]->Fill(PxOut[l]);
	  h_pyout_p[2]->Fill(PyOut[l]);
	  h_pzout_p[2]->Fill(PzOut[l]);
	  h_mom_p[2]->Fill(momentum[l]);
	  
	  h_eloss_p[2]->Fill(eloss[l]);
	  h_etot_eloss_p[2]->Fill(energy[l],eloss[l]);
	  
	  h_xin_p[2]->Fill(xIn[l]);
	  h_yin_p[2]->Fill(yIn[l]);
	  h_zin_p[2]->Fill(zIn[l]);
	}
	if (PID[l] == 11 ){
	  h_time_e[2]->Fill(time[l]);
	  h_etot_e[2]->Fill(energy[l]);
	  h_ekin_e[2]->Fill(ekin[l]);
	  
	  h_eloss_e[2]->Fill(eloss[l]);
	  h_etot_eloss_e[2]->Fill(energy[l],eloss[l]);
	}
	break; // case 3
      case 4:
	if (PID[l] == 2212 ){
	  h_time_p[3]->Fill(time[l]);
	  h_etot_p[3]->Fill(energy[l]);
	  h_ekin_p[3]->Fill(ekin[l]);
	  h_pxout_p[3]->Fill(PxOut[l]);
	  h_pyout_p[3]->Fill(PyOut[l]);
	  h_pzout_p[3]->Fill(PzOut[l]);
	  h_mom_p[3]->Fill(momentum[l]);
	  
	  h_eloss_p[3]->Fill(eloss[l]);
	  h_etot_eloss_p[3]->Fill(energy[l],eloss[l]);
	  
	  h_xin_p[3]->Fill(xIn[l]);
	  h_yin_p[3]->Fill(yIn[l]);
	  h_zin_p[3]->Fill(zIn[l]);
	}
	if (PID[l] == 11 ){
	  h_time_e[3]->Fill(time[l]);
	  h_etot_e[3]->Fill(energy[l]);
	  h_ekin_e[3]->Fill(ekin[l]);
	  
	  h_eloss_e[3]->Fill(eloss[l]);
	  h_etot_eloss_e[3]->Fill(energy[l],eloss[l]);
	}
	break; // case 4
      case 5:
	if (PID[l] == 2212 ){
	  h_time_p[4]->Fill(time[l]);
	  h_etot_p[4]->Fill(energy[l]);
	  h_ekin_p[4]->Fill(ekin[l]);
	  h_pxout_p[4]->Fill(PxOut[l]);
	  h_pyout_p[4]->Fill(PyOut[l]);
	  h_pzout_p[4]->Fill(PzOut[l]);
	  h_mom_p[4]->Fill(momentum[l]);
	  
	  h_eloss_p[4]->Fill(eloss[l]);
	  h_etot_eloss_p[4]->Fill(energy[l],eloss[l]);
	  
	  h_xin_p[4]->Fill(xIn[l]);
	  h_yin_p[4]->Fill(yIn[l]);
	  h_zin_p[4]->Fill(zIn[l]);
	}
	if (PID[l] == 11 ){
	  h_time_e[4]->Fill(time[l]);
	  h_etot_e[4]->Fill(energy[l]);
	  h_ekin_e[4]->Fill(ekin[l]);
	  
	  h_eloss_e[4]->Fill(eloss[l]);
	  h_etot_eloss_e[4]->Fill(energy[l],eloss[l]);
	}
	break; // case 5
      case 6:
	if (PID[l] == 2212 ){
	  h_time_p[5]->Fill(time[l]);
	  h_etot_p[5]->Fill(energy[l]);
	  h_ekin_p[5]->Fill(ekin[l]);
	  h_pxout_p[5]->Fill(PxOut[l]);
	  h_pyout_p[5]->Fill(PyOut[l]);
	  h_pzout_p[5]->Fill(PzOut[l]);
	  h_mom_p[5]->Fill(momentum[l]);
	  
	  h_eloss_p[5]->Fill(eloss[l]);
	  h_etot_eloss_p[5]->Fill(energy[l],eloss[l]);
	  
	  h_xin_p[5]->Fill(xIn[l]);
	  h_yin_p[5]->Fill(yIn[l]);
	  h_zin_p[5]->Fill(zIn[l]);
	}
	if (PID[l] == 11 ){
	  h_time_e[5]->Fill(time[l]);
	  h_etot_e[5]->Fill(energy[l]);
	  h_ekin_e[5]->Fill(ekin[l]);
	  
	  h_eloss_e[5]->Fill(eloss[l]);
	  h_etot_eloss_e[5]->Fill(energy[l],eloss[l]);
	}
	break; // case 6
      } // cellId

      h_stripflag->Fill(StripFlag[l]);
      h_xav->Fill(xAv[l]);
      // end filling histograms for checking
      
      // determine the expected number of electrons produced
      NofClusters[l] = 0;
      NeExp[l] = 0;
      Ne[l] = 0;
      Qind[l] = 0.0;
      Tentr[l] = 0.0;
      TOF[l] = 0.0;

      Double_t FanoFactor = 0.25;
      Double_t NofClMean = ( 9.64747 + 2.37356 / (gamma[l]-1) - 3.67386 * exp(-7.62724E-2 * (gamma[l]-1)) ) * 0.3; // for RPCgas
      //Double_t NofClMean = ( 9.88155 + 2.65187 / (gamma[l]-1) - 3.59594 * exp(-9.50971E-2 * (gamma[l]-1)) ) * 0.3; // for Isobutane
      //Double_t NofClMean = ( 3.30233 + 7.73277E-1 / (gamma[l]-1) - 1.17864 * exp(-7.25717E-2 * (gamma[l]-1)) ) * 0.3; // for Methane
      Double_t NofClMeanUnc = sqrt(NofClMean * FanoFactor);
      NofClusters[l] = (Int_t) ( (2 * gRandom->Rndm() - 1) * NofClMeanUnc + NofClMean + 0.5 );

      for(Int_t k=1; k<=NofClusters[l]; k++) {
	Double_t Rnd = gRandom->Rndm();
	if(Rnd <= 0.66) NeExp[l] = NeExp[l] + 1; // for RPCgas
	else if(Rnd <= 0.855) NeExp[l] = NeExp[l] + 2;
	else if(Rnd <= 0.925) NeExp[l] = NeExp[l] + 3;
	else if(Rnd <= 0.960) NeExp[l] = NeExp[l] + 4;
	else if(Rnd <= 0.975) NeExp[l] = NeExp[l] + 5;
	else if(Rnd <= 0.985) NeExp[l] = NeExp[l] + 6;
	else if(Rnd <= 0.992) NeExp[l] = NeExp[l] + 7;
	else if(Rnd <= 0.997) NeExp[l] = NeExp[l] + 8;
	else if(Rnd <= 1.000) NeExp[l] = NeExp[l] + 9;
	/*
	if(Rnd <= 0.84) NeExp[l] = NeExp[l] + 1; // for Isobutane and Methane
	else if(Rnd <= 0.946) NeExp[l] = NeExp[l] + 2;
	else if(Rnd <= 0.971) NeExp[l] = NeExp[l] + 3;
	else if(Rnd <= 0.982) NeExp[l] = NeExp[l] + 4;
	else if(Rnd <= 0.9885) NeExp[l] = NeExp[l] + 5;
	else if(Rnd <= 0.993) NeExp[l] = NeExp[l] + 6;
	else if(Rnd <= 0.996) NeExp[l] = NeExp[l] + 7;
	else if(Rnd <= 0.9985) NeExp[l] = NeExp[l] + 8;
	else if(Rnd <= 1.000) NeExp[l] = NeExp[l] + 9;
	*/
      } // NofClusters for
      // end determining the expected number of electrons produced
      
      // calculating number of electrons based on eloss
      Double_t IonYield = 40; // eV, for RPCgas
      //Double_t IonYield = 15; // eV, for Isobutane
      //Double_t IonYield = 27; // eV, for Methane
      Double_t NeMean = 1E3 * eloss[l] / IonYield;
      Double_t NeMeanUnc = sqrt(NeMean * FanoFactor);

      Ne[l] = (Int_t) ((2 * gRandom->Rndm() - 1) * NeMeanUnc + NeMean + 0.5);
      //Ne[l] = (Int_t) (1E3 * eloss[l] / IonYield + 0.5);
      //cout << " NofClusters: " << NofClusters[l] << " NeMean: " << NeMean << " NeExp: " << NeExp[l] << endl;
      // end calculating number of electrons based on eloss
      
      
      // calculating the induced charge
      Double_t HV = 10; // kV
      Double_t NofGasGaps = 3; // Number of gas gaps in one segment of the detector
      Double_t tGasGap = 0.03; // thikness of gas gaps in cm
      Double_t eField = HV / NofGasGaps / tGasGap; // electric field in kV/cm
      Double_t vDrift = 72.3455 + 1.09136 * eField + 2.6152E-3 * pow(eField,2); // electron drift velocity in mm/ns
      Double_t dtStep = 0.0001; // time step size in ns
      Double_t dzStep = dtStep * vDrift; // length step size in mm
      Double_t alpha = 9.321 - 1.30858 * eField + 3.50827E-2 * pow(eField,2) - 1.06524E-4 * pow(eField,3); // Townsend coefficient in 1/mm
      Double_t eta = -8.8903 + 1473.86 / (eField - 22.0970); // Attachment coefficient in 1/mm
      Double_t k_ratio = eta / alpha;
      Double_t wField = 0.7; // weighting field in 1/mm
      Double_t e0 = 1.6022E-7; // elementary charge in pC
      Double_t Dtr = (135.721 - 0.456232 * eField + 1.01483E-3 * pow(eField,2)) / 10; // transversal diffusion coefficient in micrometer/mm
      Double_t Dtrans = sqrt(Dtr * dzStep / 1E3); // transversal diffusion coefficient in sqrt(mm) ????
      
      /*
      // info
      cout << " WField: " << eField
	   << " vDrift: " << vDrift
	   << " dzStep: " << dzStep << endl;
      cout << " alpha: " << alpha
	   << " eta: " << eta << endl;
      // end info
      */

      Int_t NofSteps[Ne[l]];
      Int_t NofStepsL = 0;
      Double_t zRnd[Ne[l]];
      
      for(Int_t b=0; b<Ne[l]; b++) {
	zRnd[b] = 10 * tGasGap * gRandom->Rndm();
      }

      Double_t CorrDist = 0.0; // Correlation distance in mm

      for(Int_t b=0; b<Ne[l]; b++) {
	for(Int_t c=0; c<b; c++) {
	  if( fabs(zRnd[b] - zRnd[c]) <= CorrDist ) {
	    //cout << " entry: " << l << " electron1: " << b << " zRnd1: " << zRnd[b]
	    //	 << " electron2: " << c << " zRnd2: " << zRnd[c] << endl;
	    zRnd[b] = ( zRnd[b] + zRnd[c] ) / 2;
	    NofSteps[c] = 0;
	    zRnd[c] = -1;
	    //cout << "modified zRnd1: " << zRnd[b] << endl;
	  }
	}
      }

      for(Int_t b=0; b<Ne[l]; b++) {
	if(zRnd[b] > -1) {
	  NofSteps[b] = (Int_t) (zRnd[b] / dzStep + 0.5);
	  if(NofSteps[b] > NofStepsL) NofStepsL = NofSteps[b];
	}
      }
      
      
      
      ULong64_t NeAvInd[Ne[l]][NofStepsL];
      Double_t RadInd[Ne[l]];

      Double_t sRnd;
      Double_t nbar;
      Double_t f1;
      Double_t f2;

      //cout << " entry: " << l <<  endl;

      // calculate the number of electrons in avalanche
      for(Int_t a=0; a<Ne[l]; a++) {

	//cout << " electron: " << a << " NofSteps: " << NofSteps[a] << " NofStepsL: " << NofStepsL << endl;
	
	NeAvInd[a][0] = 0;
	RadInd[a] = 0.0;

      	// Do the steps for the individual electrons
       	for(Int_t m=1; m<NofStepsL+1; m++) {
	  
	  NeAvInd[a][m] = 0;
	  sRnd = 0.0;
	  nbar = 0.0;
	  f1 = 0.0;
	  f2 = 0.0;
	  
	  if(m > NofSteps[a]) {
	    NeAvInd[a][m] = 0;
	    RadInd[a] = 0.0;
	  }
	  
	  else {
	    RadInd[a] = fabs(m * gRandom->Gaus(0, Dtrans * sqrt(dzStep))); // mm

	    sRnd = gRandom->Rndm();
	    nbar = exp( (alpha-eta) * m * dzStep );
	    
	    if(NeAvInd[a][m-1] >= 1.6E7) NeAvInd[a][m] = 1.6E7;
	    
	    else {
	      if ( sRnd < ( k_ratio * (nbar-1) / (nbar-k_ratio) ) ) {
		NeAvInd[a][m] = NeAvInd[a][m-1];
	      }
	      else {
		f1 = log( (nbar - k_ratio) * (sRnd - 1) / ( (k_ratio - 1) * nbar) );
		f2 = log( (nbar - 1) / (nbar - k_ratio) );
		NeAvInd[a][m] = (ULong64_t) (1 + f1 / f2 + 0.5);
	      } // else
	      if(NeAvInd[a][m] >= 1.6E7) NeAvInd[a][m] = 1.6E7;
	    } // else
	    
	  } // else
	  
	  //cout << "step: " << m << " NeAvInd end: " << NeAvInd[a][m] << " RadInd: " << RadInd[a] << endl;
	  
	} // for m < NofStepsL

      } // for a < Ne[l]
      
      ULong64_t NeAvStep;
      
      // calculate the induced charge
      //Do the steps for the individual electrons
      //cout << " entry: " << l <<  endl;
      for(Int_t m=1; m<NofStepsL+1; m++) {
	//cout << "step: " << m << endl;
	
	NeAvStep = 0;
	
	for(Int_t a=0; a<Ne[l]; a++) {
	  
	  NeAvStep += NeAvInd[a][m];
	  //if(NeAvStep >= ((Ne[l]/2) * 1.6E7)) NeAvStep = (Ne[l]/2) * 1.6E7;
	  
	  //cout << " electron: " << a << " NeAvStep: " << NeAvStep << endl;
	  
	} // a < Ne[l]
	
	Qind[l] += wField * e0 * NeAvStep * dzStep; // pC
	Tentr[l] += m * dtStep; // ns

	//if(Qind[l] < 0) {
	  //cout << " entry: " << l <<  endl;
	  //cout << "step: " << m << " NeAvStep end: " << NeAvStep << " Qind: " << Qind[l] << endl;
	//}
	
      } // m < NofStepsL
      

      TOF[l] = time[l] + Tentr[l];
      

      for(Int_t d=0; d<8; d++) {
	StripFlagAfter[d] = kFALSE;
      }

      for(Int_t d=0; d<8; d++) {
	if(d < 7){
	  QindStr[l][d] = 0;
	  QindStr[l][d+1] = 0;
	  TOFStr[l][d] = 0;
	  TOFStr[l][d+1] = 0;
	}
	if(d == 8) {
	  QindStr[l][d] = 0;
	  TOFStr[l][d] = 0;
	}
	if(StripFlag[l] == d) {
	  QindStr[l][d] = Qind[l];
	  TentrStr[l][d] = Tentr[l];
	  TOFStr[l][d] = TOF[l];
	  h_qindstr_e[d]->Fill(QindStr[l][d]);
	  h_tofstr_e[d]->Fill(TOFStr[l][d]);
	  StripFlagAfter[d] = kTRUE;
	}
	if(StripFlag[l] == d + 8) {
	  QindStr[l][d] = Qind[l] * (xAv[l] - xmin[d]) / delta_AnodeGap;
	  QindStr[l][d+1] = Qind[l] - QindStr[l][d];
	  TentrStr[l][d] = Tentr[l];
	  TentrStr[l][d+1] = Tentr[l];
	  TOFStr[l][d] = TOF[l];
	  TOFStr[l][d+1] = TOF[l];
	  h_qindstr_e[d]->Fill(QindStr[l][d]);
	  h_qindstr_e[d+1]->Fill(QindStr[l][d+1]);
	  h_tofstr_e[d]->Fill(TOFStr[l][d]);
	  h_tofstr_e[d+1]->Fill(TOFStr[l][d+1]);
	  StripFlagAfter[d] = kTRUE;
	  StripFlagAfter[d+1] = kTRUE;
	}
	//cout << " StripFlag: " << StripFlag[l] << " QindStr: " << QindStr[l][d] << endl;
      }

      //for(Int_t d=0; d<8; d++) {
      //if(StripFlag[l] == 10) {
      //  cout << " StripFlag = 10! " << " StripFlagAfter[" << d << "]: " << StripFlagAfter[d] << endl;
      //}
      //} 
      
      if (PID[l] == 2212) {
	h_ne_exp_p->Fill(NeExp[l]);
	h_ne_p->Fill(Ne[l]);
	h_gamma_eloss_p->Fill(gamma[l], eloss[l]);
	h_qind_p->Fill(Qind[l]);
	h_tentr_p->Fill(Tentr[l]);
	h_qind_tof_p->Fill(Qind[l], TOF[l]);
      }
      if (PID[l] == 11) {
	h_ne_exp_e->Fill(NeExp[l]);
	h_ne_e->Fill(Ne[l]);
	h_gamma_eloss_e->Fill(gamma[l], eloss[l]);
	h_qind_e->Fill(Qind[l]);
	h_tentr_e->Fill(Tentr[l]);
	h_qind_tof_e->Fill(Qind[l], TOF[l]);
      }
      

      Double_t TOFgate = 4.0; // ns

      for(Int_t j=0; j<l; j++) {
	if( fabs( TOF[l] - TOF[j] ) >= TOFgate) {
	  h_qindtot->Fill(Qind[l]);
	  break;
	}
	else {
	  QindTotal += Qind[l];
	  break;
	}
      }
      
      for(Int_t d=0; d<8; d++) {
	if(StripFlagAfter[d] == kTRUE) {
	  for(Int_t j=0; j<l; j++) {
	    if( fabs( TOFStr[l][d] - TOFStr[j][d] ) >= TOFgate) {
	      h_qindtotstr[d]->Fill(QindStr[l][d]);
	      break;
	    }
	    else {
	      QindTotStr[d] += QindStr[l][d];
	      break;
	    }
	  }
	}
      }
      
      // check the distance between entries
      for (Int_t j=0; j<l; j++) {
	Double_t dist = sqrt ( (xAv[l] - xAv[j]) * (xAv[l] - xAv[j]) + (yAv[l] - yAv[j]) * (yAv[l] - yAv[j]) );
	if(dist > 100){ // 100 mm
	  distcount = distcount + 1;
	} // dist > 100 mm
	else {
	  
	} // dist < 100 mm
	if(cellId[l] == cellId[j]) {
	  //cout  << " cellId["<< l << "] = " << "cellId[" << j << "]" << endl;
	} // cellID if
      } // nentries for j<l compare

      Double_t diff = sqrt( pow( (xIn[l] - xOut[l]), 2) + pow( (yIn[l] - yOut[l]), 2) );
      h_diff->Fill(diff);
      h_diffx->Fill(xIn[l] - xOut[l]);
      h_diffy->Fill(yIn[l] - yOut[l]);
      h_diffz->Fill(zIn[l] - zOut[l]);
      
    } // nentries for
    
    h_qindtot->Fill(QindTotal);

    for(Int_t d=0; d<8; d++) {
      if(StripFlagAfter[d] == kTRUE) h_qindtotstr[d]->Fill(QindTotStr[d]);
    }
    
  } // nentries > 0
  
  
  
}
// -------------------------------------------------------------------------

void R3BNeuLandDigitizer::Reset()
{
}   

void R3BNeuLandDigitizer::Finish()
{
  // here event. write histos
  //   cout << " -I- Digit Finish() called " << endl;
  // Write control histograms

  h_detid->Write();
  h_cellid->Write();

  h_trackid->Write();
  h_pdg->Write();
  h_mot0trackid->Write();
  h_trackid_vs_m0trackid->Write();
  h_pdg_vs_m0pdg->Write();

  h_toteloss->Write();
  h_cellhits->Write();
  h_cellhits_vs_totaleloss->Write();

  h_ne_exp_p->Write();
  h_ne_exp_e->Write();
  h_ne_p->Write();
  h_ne_e->Write();
  h_gamma_eloss_p->Write();
  h_gamma_eloss_e->Write();
  h_qind_p->Write();
  h_qind_e->Write();
  h_qindtot->Write();
  h_tentr_p->Write();
  h_tentr_e->Write();
  h_qind_tof_p->Write();
  h_qind_tof_e->Write();
  h_stripflag->Write();
  h_xav->Write();

  h_diff->Write();
  h_diffx->Write();
  h_diffy->Write();
  h_diffz->Write();

  for(Int_t i=0;i<6;i++){
    h_time_p[i]->Write();
    h_etot_p[i]->Write();
    h_ekin_p[i]->Write();
    h_time_e[i]->Write();
    h_etot_e[i]->Write();
    h_ekin_e[i]->Write();
    h_pxout_p[i]->Write();
    h_pyout_p[i]->Write();
    h_pzout_p[i]->Write();
    h_mom_p[i]->Write();

    h_eloss_p[i]->Write();
    h_eloss_e[i]->Write();
    h_etot_eloss_p[i]->Write();
    h_etot_eloss_e[i]->Write();

    h_xin_p[i]->Write();
    h_yin_p[i]->Write();
    h_zin_p[i]->Write();
  }

  for(Int_t i=0;i<8;i++){
    h_qindstr_e[i]->Write();
    h_tofstr_e[i]->Write();
    h_qindtotstr[i]->Write();
  }

  cout << " Number of primaries deposit no energy in gas layers: " << no_interaction << endl;
  cout << " Number of hits where distance is larger than 10 cm: " << distcount << endl;
  cout << " Number of events in which proton is detected: " << NoOfPEvt << endl;
}

ClassImp(R3BNeuLandDigitizer)
