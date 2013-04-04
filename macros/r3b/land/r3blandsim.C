Int_t r3blandsim()
{
  // Load the Main Simulation macro
  gROOT->LoadMacro("r3ball.C");
  
  TString fMC ="TGeant3";
  TString fGene="ascii";
  Bool_t fUserPList= kTRUE;

  // Target type
  TString target="LiH";

  TObjString det1("TARGET");
  TObjString det2("ALADIN");
//   TObjString det2("GLAD");
  TObjString det3("CRYSTALBALL");
  TObjString det4("DCH");
//   TObjString det5("LAND");
  TObjString det5("SCINTNEULAND");

  TObjArray fDetList;
  fDetList.Add(&det1);
  fDetList.Add(&det2);
  fDetList.Add(&det3);
  fDetList.Add(&det4);
  fDetList.Add(&det5);

  Int_t nEvents = 10;

  Bool_t fEventDisplay = kTRUE;

  // Magnet Field definition
  Bool_t fR3BMagnet = kTRUE;

  // Main Sim function call
  r3ball(nEvents,
	 fDetList,
	 target,
	 fEventDisplay,
	 fMC,
	 fGene,
	 fUserPList,
	 fR3BMagnet);
}

