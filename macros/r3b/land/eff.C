{
  char strDir[] = "./";
  char strFileName[1000];

  TFile *file;
  TH1F *h1;

  Float_t eff[4][4];
  Float_t err[4][4];
  Float_t eff_old[4][4];
  Float_t err_old[4][4];

  for(Int_t i = 0; i < 4; i++) {
    sprintf(strFileName, "%sr3breco.%1dAMeV.%1dn.%1dkeV.%1dm.old.root",
	  strDir, 600, i+1, 500, 14);
    file = new TFile(strFileName);
    h1 = (TH1F*) file->Get("Neutmult");
    for(Int_t j = 0; j < 4; j++) {
      eff_old[j][i] = 100. * (Float_t)h1->GetBinContent(j+2) / 10000.;
      err_old[j][i] = 100. * TMath::Sqrt((Float_t)h1->GetBinContent(j+2)) / 10000.;
    }
    file->Close();


    //    sprintf(strFileName, "%sr3breco.%1dAMeV.%1dn.%1dkeV.%1dm.root",
    //	    strDir, 600, i+1, 500, 14);
    sprintf(strFileName, "%s/r3breco.%1dAMeV.%1dn.%1dkeV.%1dm.root",
	    strDir, 600, i+1, 500, 14);
    file = new TFile(strFileName);
    h1 = (TH1F*) file->Get("h_ntracks");
    for(Int_t j = 0; j < 4; j++) {
      eff[j][i] = 100. * (Float_t)h1->GetBinContent(j+2) / 10000.;
      err[j][i] = 100. * TMath::Sqrt((Float_t)h1->GetBinContent(j+2)) / 10000.;
    }
    file->Close();
  }


  char str[100];
  
  cout << endl << endl;

  
  // -------------table output--------------------------------------
  cout << "1D:" << endl;
  cout << "             1n        |        2n        |       3n         |      4n" << endl;
  for(Int_t i = 0; i < 4; i++) {
    cout << "------------------------------------------------------------------------------" << endl;
    cout << (i+1) << "n  ";
    for(Int_t j = 0; j < 4; j++) {
      sprintf(str, "|  %5.2f +- %3.2f", eff_old[i][j], err_old[i][j]);
      cout << str << "   ";
    }
    cout << endl;
  }
  cout << endl << endl;
  
  cout << "2D:" << endl;
  cout << "             1n        |        2n        |       3n         |      4n" << endl;
  for(Int_t i = 0; i < 4; i++) {
    cout << "------------------------------------------------------------------------------" << endl;
    cout << (i+1) << "n  ";
    for(Int_t j = 0; j < 4; j++) {
      sprintf(str, "|  %5.2f +- %3.2f ", eff[i][j], err[i][j]);
      cout << str << "  ";
    }
    cout << endl;
  }
  cout << endl << endl;
  
  cout << "1D - 2D:" << endl;
  cout << "            1n         |        2n        |       3n         |      4n" << endl;
  for(Int_t i = 0; i < 4; i++) {
    cout << "------------------------------------------------------------------------------" << endl;
    cout << (i+1) << "n  ";
    for(Int_t j = 0; j < 4; j++) {
      sprintf(str, "| %6.2f +- %3.2f ", eff[i][j] - eff_old[i][j],sqrt(err[i][j]*err[i][j]+ err_old[i][j]*err_old[i][j]));
      cout << str << "  ";
    }
    cout << endl;
  }
  
  cout << endl << endl;
}

