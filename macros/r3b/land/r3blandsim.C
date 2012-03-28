Int_t r3blandsim(Int_t beamEnergy, Int_t nn, Int_t erel)
{
  // Load the Main Simulation macro
  gROOT->LoadMacro("r3ball.C");
  
  TString fMC ="TGeant3";
  TString fGene="ascii";
  Bool_t fUserPList= kTRUE;

  // Target type
  TString target="LiH";

  TObjString det1("SCINTNEULAND");

  TObjArray fDetList;
  fDetList.Add(&det1);

  Int_t nEvents = 10000;

  Bool_t fEventDisplay=kFALSE;

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
	 fR3BMagnet,
	 beamEnergy,
	 nn,
	 erel);  
}

