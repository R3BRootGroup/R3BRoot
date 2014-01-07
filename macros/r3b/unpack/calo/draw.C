
{
  TFile *file = new TFile("rawData.root");
  TTree *tree = (TTree*) file->Get("cbmsim");
  cout << tree->GetEntries() << endl;
 
  TH1F *h_en = new TH1F("h_en", "", 55, 280, 390);

  tree->Draw("CaloRawHit.fEnergy>>h_en", "", "");
  
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 450, 450);
  TPostScript *ps1 = new TPostScript("energy.eps", -113);
  ps1->Range(16, 16);
  h_en->Draw();
  ps1->Close();
}
