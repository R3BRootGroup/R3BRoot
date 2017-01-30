#include "drawStyle.C"



Float_t kappa = 0.04;
const Float_t misId[] = {0.1, 0.1, 0.1, 0.05};

Float_t Integral_2D(TH2F *h1, Float_t cut1, Float_t cut2);

// -----------------------------------------------------------------------------
void calibr_2D(Int_t beamE, Int_t Erel, Float_t Kappa)
{
  Int_t d;
  d = 15;
  kappa = Kappa;
  TPstyle();


  // ----- Files -----------------------------------------------------
  Float_t cuts[5];
  cuts[0] = 5.;
  char strDir[] = ".";
  TString inFile[4];
  TFile *file[4];
  TH2F *h[4];

  for(Int_t i = 0; i < 4; i++) {
    char str[1000];
    sprintf(str, "%1dAMeV.%1dn.%1dkeV.%1dm", beamE, i+1, Erel, d);
    inFile[i] = TString(strDir) + "/r3bcalibr." + TString(str) + ".root";
    file[i] = new TFile(inFile[i]);
    h[i] = (TH2F*) file[i]->Get("h_ncl_etot");
    Style(h[i], "Total deposited energy (MeV)", "Number of clusters");
  }
  for(Int_t i = 1; i < 5; i++) {
    cuts[i] = cuts[i-1] + 300.;
    Float_t int0 = h[i-1]->GetEntries();
    Float_t int1 = Integral_2D(h[i-1], cuts[i-1], cuts[i]);
    Float_t int2 = Integral_2D(h[i-1], cuts[i], 1500.);
    Float_t eff1 = int1 / int0;
    Float_t eff2 = int2 / int0;
    while(eff2 > misId[i-1]) {
      if(eff2 >= (2.*misId[i-1])) {
	cuts[i] += 20.;
      } else if(eff2 >= (1.2*misId[i-1])) {
	cuts[i] += 10.;
      } else {
	cuts[i] += 1.;
      }
      int1 = Integral_2D(h[i-1], cuts[i-1], cuts[i]);
      int2 = Integral_2D(h[i-1], cuts[i], 3000.);
      eff1 = int1 / int0;
      eff2 = int2 / int0;
      cout << "loop : " << i << "   " << cuts[i] << " : " << eff1 << "  " << eff2 << endl;
      if(eff2 <= misId[i-1]){// && eff1 > 0.4) {
	break;
      }
    }
    cout << cuts[i] << ",  " << eff1 << endl;
  }

  TLine *l[5];
  for(Int_t i = 0; i < 5; i++) {
    l[i] = new TLine(cuts[i], 0., 0., kappa*cuts[i]);
    l[i]->SetLineWidth(2.);
  }
  // -----------------------------------------------------------------


  // Draw separate distributions -------------------------------------
  gStyle->SetPaperSize(32, 32);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 900, 900);
  char strPsFile[1000];
  sprintf(strPsFile, "calibr_%1dAMeV_%1dkeV_%1dm.eps", beamE, Erel, d);
  TPostScript *ps1 = new TPostScript(strPsFile, -113);
  ps1->Range(32, 32);
  c1->Divide(2,2);
  for(Int_t i = 0; i < 4; i++) {
    c1->cd(i+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    gPad->SetTickx();
    gPad->SetTicky();
    gPad->SetLogz();
    h[i]->SetMinimum(1);
    h[i]->SetMaximum(10);
    h[i]->Draw("COLZ");

    l[i]->Draw();
    l[i+1]->Draw();

    char strn[10];
    sprintf(strn, "%1dn", i+1);
    TPaveLabel *l1 = new TPaveLabel(0.76, 0.78, 0.85, 0.87, strn, "NDC");
    Style(l1);
    l1->Draw();
  }
  
  ps1->Close();
  // -----------------------------------------------------------------


  // Write out calibration file --------------------------------------
  char strOutFile[1000];
  sprintf(strOutFile, "%s/r3bcalibr.%1dAMeV.%1dkeV.%1dm.txt", strDir, beamE, Erel, d);
  ofstream *ofile = new ofstream(strOutFile);
  (*ofile) << kappa << endl;
  for(Int_t i = 0; i < 5; i++) {
    (*ofile) << cuts[i] << endl;
  }
  ofile->close();
  // -----------------------------------------------------------------
}
// -----------------------------------------------------------------------------
void calibr_2D(Int_t beamE, Int_t Erel)
{
  Int_t d;
  if(Erel == 100) {
    d = 35; 
  } else {
    d = 14;
  }
  
  TPstyle();


  // ----- Files -----------------------------------------------------
  char strDir[] = ".";
  TString inFile[4];
  TFile *file[4];
  TH2F *h[4];

  for(Int_t i = 0; i < 4; i++) {
    char str[1000];
    sprintf(str, "%1dAMeV.%1dn.%1dkeV.%1dm", beamE, i+1, Erel, d);
    inFile[i] = TString(strDir) + "/r3bcalibr." + TString(str) + ".root";
    file[i] = new TFile(inFile[i]);
    h[i] = (TH2F*) file[i]->Get("h_ncl_etot");
    Style(h[i], "Total deposited energy (MeV)", "Number of clusters");
  }
  

  // ---------------calculate kappa------------------------------------

  Int_t dy;
  Int_t dyold;
  Int_t dx;
  Int_t dxold;
  Int_t sum;
  Int_t times = 0;
  Int_t TotalSum;
  Int_t trashholdx = 10;
  Int_t trashholdy = 100;
  Int_t maxX = h[0]->GetXaxis()->GetLast();
  Int_t maxY = h[0]->GetYaxis()->GetLast();
  Bool_t r;
  
  
  for(Int_t i = 0; i < maxX; i++){
    for(Int_t j = 0; j < maxY; j++){      
      TotalSum += h[0]->GetBinContent(i,j);
    }
  }
  
  do{
    times++;
    r = false;
    dx = 0;
    dy = 0;
    
    
    for(Int_t i = 0; i < maxX; i++){
      sum = 0;
      for(Int_t j = 0; j < maxY; j++){      
	sum += h[0]->GetBinContent(i + 1,j + 1);
      }
      if(sum > trashholdx){
	dx++;
      }
    }

    for(Int_t i = 0; i < maxY; i++){
      sum = 0;
      for(Int_t j = 0; j < maxX; j++){      
	sum += h[0]->GetBinContent(j + 1,i + 1);
      }
      if(sum > trashholdy){
	dy++;
      }
    }
    
    if(trashholdx != (int) (TotalSum * 0.5f  / dx)){
      trashholdx = TotalSum * 0.5f  / dx;
      r = true;
    }
    if(trashholdy != (int) (TotalSum * 0.5f  / dy)){
      trashholdy = TotalSum * 0.5f  / dy;
      r = true;
    }   
  } while(r && times <100);
  
  kappa = h[0]->GetYaxis()->GetBinUpEdge(dy + 1)/h[0]->GetXaxis()->GetBinUpEdge(dx + 1);
  
  Float_t cuts[5];
  cuts[0] = 10.*((Float_t)beamE)/600.;
  cuts[1] = cuts[0] + 1.;
  Float_t int0 = h[0]->GetEntries();
  Float_t K = kappa;
  Float_t max = 0;
  Float_t bestKappa;
  
  for(kappa = K*0.9f; kappa <= K*1.1f; kappa += K*0.01f){

    Float_t int1 = Integral_2D(h[0], cuts[0], cuts[1]);
    Float_t int2 = Integral_2D(h[0], cuts[0], 1500.);
    Float_t eff1 = int1 / int0;
    Float_t eff2 = int2 / int0;
    while(eff2 > misId[0]) {
      if(eff2 >= (2.*misId[0])) {
	cuts[1] += 20.;
      } else if(eff2 >= (1.2*misId[0])) {
	cuts[1] += 10.;
      } else {
	cuts[1] += 1.;
      }
      int1 = Integral_2D(h[0], cuts[0], cuts[1]);
      int2 = Integral_2D(h[0], cuts[1], 1500.);
      eff1 = int1 / int0;
      eff2 = int2 / int0;
      if(eff2 <= misId[0]){// && eff1 > 0.4) {
	break;
      }
    }
    cout << kappa << ":   " << eff1 << "   " << cuts[1] << endl;
    if(eff1 < max){
      max = eff1;
      bestKappa = kappa;
    }
    cuts[1]-=25;
  }
  kappa = bestKappa;
  cout << "kappa = " << kappa << endl;
  
  return;
  
  // ---------calculate cuts-----------------------------------


  for(Int_t i = 1; i < 5; i++) {
    cuts[i] = cuts[i-1] + 1.;
    Float_t int0 = h[i-1]->GetEntries();
    Float_t int1 = Integral_2D(h[i-1], cuts[i-1], cuts[i]);
    Float_t int2 = Integral_2D(h[i-1], cuts[i], 1500.);
    Float_t eff1 = int1 / int0;
    Float_t eff2 = int2 / int0;
    while(eff2 > misId[i-1]) {
      if(eff2 >= (2.*misId[i-1])) {
	cuts[i] += 20.;
      } else if(eff2 >= (1.2*misId[i-1])) {
	cuts[i] += 10.;
      } else {
	cuts[i] += 1.;
      }
      int1 = Integral_2D(h[i-1], cuts[i-1], cuts[i]);
      int2 = Integral_2D(h[i-1], cuts[i], 1500.);
      eff1 = int1 / int0;
      eff2 = int2 / int0;
      cout << "loop : " << i << "   " << cuts[i] << " : " << eff1 << "  " << eff2 << endl;
      if(eff2 <= misId[i-1]){// && eff1 > 0.4) {
	break;
      }
    }
    cout << cuts[i] << ",  " << eff1 << endl;
  }

  TLine *l[5];
  for(Int_t i = 0; i < 5; i++) {
    l[i] = new TLine(cuts[i], 0., 0., kappa*cuts[i]);
    l[i]->SetLineWidth(2.);
  }
  // -----------------------------------------------------------------


  // Draw separate distributions -------------------------------------
  gStyle->SetPaperSize(32, 32);
  TCanvas *c1 = new TCanvas("c1", "", 10, 10, 900, 900);
  char strPsFile[1000];
  sprintf(strPsFile, "calibr_%1dAMeV_%1dkeV_%1dm.eps", beamE, Erel, d);
  TPostScript *ps1 = new TPostScript(strPsFile, -113);
  ps1->Range(32, 32);
  c1->Divide(2,2);
  for(Int_t i = 0; i < 4; i++) {
    c1->cd(i+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    gPad->SetTickx();
    gPad->SetTicky();
    gPad->SetLogz();
    h[i]->SetMinimum(1);
    h[i]->SetMaximum(10);
    h[i]->Draw("COLZ");

    l[i]->Draw();
    l[i+1]->Draw();

    char strn[10];
    sprintf(strn, "%1dn", i+1);
    TPaveLabel *l1 = new TPaveLabel(0.76, 0.78, 0.85, 0.87, strn, "NDC");
    Style(l1);
    l1->Draw();
  }
  
  ps1->Close();
  // -----------------------------------------------------------------


  // Write out calibration file --------------------------------------
  char strOutFile[1000];
  sprintf(strOutFile, "%s/r3bcalibr.%1dAMeV.%1dkeV.%1dm.txt", strDir, beamE, Erel, d);
  ofstream *ofile = new ofstream(strOutFile);
  (*ofile) << kappa << endl;
  for(Int_t i = 0; i < 5; i++) {
    (*ofile) << cuts[i] << endl;
  }
  ofile->close();
  // -----------------------------------------------------------------
}
// -----------------------------------------------------------------------------




// -----------------------------------------------------------------------------
Float_t Integral_2D(TH2F *h1, Float_t cut1, Float_t cut2)
{
  if(NULL == h1) {
    return 0.;
  }

  Float_t integral = 0.;

  Float_t c;
  Float_t e;
  for(Int_t ic = 0; ic < 150; ic++) {
    c = (Float_t)(ic+0.5);
    for(Int_t ie = 0; ie < 150; ie++) {
      e = (Float_t)(ie+0.5)*10.;
      if(c >= ( (0.-kappa*cut1)/(cut1-0.)*(e-0.)+kappa*cut1 )) {
	if(c < ( (0.-kappa*cut2)/(cut2-0.)*(e-0.)+kappa*cut2 )) {
	  integral += h1->GetBinContent(ie+1, ic+1);
	}
      }
    }
  }

  return integral;
}
// -----------------------------------------------------------------------------



