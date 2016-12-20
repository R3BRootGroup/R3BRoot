#include "r3ball.C"

void r3blandsim(Int_t nNeutrons = 4, Int_t nEvents = 100, Int_t beamE = 600, Int_t Erel = 500)
{
  Int_t d;
  d = 15;
  
  char str[100];
  
  // Event Generator Data
  sprintf(str, "%1dSn_%1dn_%dAMeV_%dkeV.dat",132- nNeutrons,nNeutrons,beamE,Erel);
  TString EventDataFile = TString(str);
  
  // Output files
  sprintf(str,"%1dAMeV.%1dn.%1dkeV.%1dm.root", beamE,nNeutrons, Erel, d);
  TString OutFile = "r3bsim."+ TString(str);
  TString ParFile = "r3bpar."+ TString(str);
  
  TString fMC ="TGeant3";
  TString fGene="ascii";
  Bool_t fUserPList= kTRUE;

  // Target
  TString target="LiH";
  
  TString det;
  det = "neuland_s2018.geo.root";

  TMap detGeo;
  detGeo.Add(new TObjString("TARGET"),        new TObjString("target_"+target+".geo.root"));
  detGeo.Add(new TObjString("ALADIN"),        new TObjString("aladin_v13a.geo.root"));
  detGeo.Add(new TObjString("CRYSTALBALL"),   new TObjString("cal_v13a.geo.root"));
  detGeo.Add(new TObjString("DCH"),           new TObjString("dch_v13a.geo.root"));
  detGeo.Add(new TObjString("SCINTNEULAND"),  new TObjString(det));

  Bool_t fEventDisplay = kTRUE;

  // Magnet Field definition
  Bool_t fR3BMagnet = kTRUE;

  // Main Sim function call
  r3ball(nEvents,&detGeo,target,fEventDisplay,fMC,fGene,fUserPList,fR3BMagnet,2500.,OutFile,ParFile,EventDataFile);  
}

