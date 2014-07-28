//--------------------------------------------------------------------
//
// Author: Vadim Wagner
// Last Update 10.06.14
//
//--------------------------------------------------------------------

Int_t r3blandsim(Int_t nNeutrons, Int_t nEvents=10000,Int_t beamE=600, Int_t erel=500,Int_t d =14)
{
  // Load the Main Simulation macro
  TString macro_r3ball = getenv("VMCWORKDIR");
  macro_r3ball += "/macros/r3b/r3ball.C";
  gROOT->LoadMacro(macro_r3ball.Data());
  
  char str[100];
  
  // Event Generator Data
  sprintf(str, "%1dSn_%1dn_%dAMeV_%dkeV.dat",132- nNeutrons,nNeutrons,beamE,erel);
  TString EventDataFile = TString(str);
  
  // Output files
  sprintf(str,"%1dAMeV.%1dn.%1dkeV.%1dm.root", beamE,nNeutrons, erel, d);
  TString OutFile = "r3bsim."+ TString(str);
  TString ParFile = "r3bpar."+ TString(str);
  
  TString fMC ="TGeant3";
  TString fGene="ascii";
  Bool_t fUserPList= kTRUE;

  // Target
  TString target="LiH";
  
  TString det;
  if(d==14){
    det = "neuland_v12a_14m.geo.root";
  }
  else if(d==35){
    det = "neuland_v12a_35m.geo.root";
  }
  else{
   cout << "Unknown detector distance!" << endl;
   return;
  }
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
  r3ball(nEvents,detGeo,target,fEventDisplay,fMC,fGene,fUserPList,fR3BMagnet,2500.,OutFile,ParFile,EventDataFile);  
}

