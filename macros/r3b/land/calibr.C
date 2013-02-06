#include "drawStyle.C"



const Float_t kappa = 0.08;
const Float_t misId[] = {0.05, 0.07, 0.12, 0.01};



// -----------------------------------------------------------------------------
void calibr_2D(Int_t beamE, Int_t erel, Int_t d)
{
  TPstyle();


  // ----- Load libraries --------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGenVector");
  gSystem->Load("libGeoBase");
  gSystem->Load("libFairDB");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BLand");
  // -----------------------------------------------------------------


  // ----- Files -----------------------------------------------------
  char strDir[] = "/Users/kresan/neuland";
  TString inFile[4];
  TFile *file[4];
  TH2F *h[4];
  for(Int_t i = 0; i < 4; i++) {
    char str[100];
    sprintf(str, "%1dAMeV.%1dn.%1dkeV.%1dm", beamE, i+1, erel, d);
    inFile[i] = TString(strDir) + "/r3bcalibr." + TString(str) + ".root";
    file[i] = new TFile(inFile[i]);
    h[i] = (TH2F*) file[i]->Get("h_ncl_etot");
    Style(h[i], "Total deposited energy (MeV)", "Number of clusters");
  }
  // -----------------------------------------------------------------


  // -----------------------------------------------------------------
  Float_t cuts[5];
  cuts[0] = 30.*((Float_t)beamE)/600.;

  for(Int_t i = 1; i < 5; i++) {
    cuts[i] = cuts[i-1] + 100.;
    Float_t int0 = h[i-1]->GetEntries();
    Float_t int1 = Integral_2D(h[i-1], cuts[i-1], cuts[i]);
    Float_t int2 = Integral_2D(h[i-1], cuts[i], 1500.);
    Float_t eff1 = int1 / int0;
    Float_t eff2 = int2 / int0;
    while(eff2 > misId[i-1]) {
      cuts[i] += 1.;
      int1 = Integral_2D(h[i-1], cuts[i-1], cuts[i]);
      int2 = Integral_2D(h[i-1], cuts[i], 1500.);
      eff1 = int1 / int0;
      eff2 = int2 / int0;
      cout << "loop : " << i << "   " << int1 << " : " << eff1 << "  " << eff2 << endl;
      if(eff2 <= misId[i-1] && eff1 > 0.4) {
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
  sprintf(strPsFile, "calibr_%1dAMeV_%1dkeV_%1dm.eps", beamE, erel, d);
  TPostScript *ps1 = new TPostScript(strPsFile, -113);
  ps1->Range(32, 32);
  c1->Divide(2, 2);
  for(Int_t i = 0; i < 4; i++) {
    c1->cd(i+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.12);
    gPad->SetTickx();
    gPad->SetTicky();
    h[i]->Draw("COLZ");

    l[i]->Draw();
    l[i+1]->Draw();

    char strn[10];
    sprintf(strn, "%1dn", i+1);
    TPaveLabel *l1 = new TPaveLabel(0.76, 0.78, 0.85, 0.87, strn, "NDC");
    Style(l1);
    l1->Draw();
  }
  c1->cd(0);
  ps1->Close();
  // -----------------------------------------------------------------


  // Write out calibration file --------------------------------------
  char strOutFile[1000];
  sprintf(strOutFile, "%s/r3bcalibr.%1dAMeV.%1dkeV.%1dm.txt", strDir, beamE, erel, d);
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



