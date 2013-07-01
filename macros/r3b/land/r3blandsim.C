
Int_t r3blandsim()
{
  TString dir = getenv("VMCWORKDIR");
  TString InFile = dir + TString("/input/signal.1500AMeV.dat");
  TString OutFile = "r3bsim.root";
  TString ParFile = "r3bpar.root";
  
  // Load the Main Simulation macro
  TString macro_r3ball = getenv("VMCWORKDIR");
  macro_r3ball += "/macros/r3b/r3ball.C";
  gROOT->LoadMacro(macro_r3ball.Data());
  
  TString fMC = "TGeant3";
  TString fGene = "ascii";
  Bool_t fUserPList = kTRUE;
  
  // Target type
  TString target = "LiH";
  
  TObjString det1("TARGET");
  TObjString det2("ALADIN");
  TObjString det3("GLAD");
  TObjString det4("CRYSTALBALL");
  TObjString det5("DCH");
  TObjString det6("LAND");
  TObjString det7("SCINTNEULAND");
  
  TObjArray fDetList;
  fDetList.Add(&det1);
  fDetList.Add(&det2);
//  fDetList.Add(&det3);
  fDetList.Add(&det4);
  fDetList.Add(&det5);
//  fDetList.Add(&det6);
  fDetList.Add(&det7);
  
  Int_t nEvents = 10;
  
  Bool_t fEventDisplay = kFALSE;
  
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
         OutFile,
         ParFile,
         InFile);
}
