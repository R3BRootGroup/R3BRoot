//Adapted For protons in phoswich endcap IEM - MAdrid CSIC. J Sanchez del Rio Saez (jose.sanchez@csic.es && josesrs@gmail.com)


eventDisplay_Protons_50MeV()
{
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  
  TFile* file = new TFile("r3bpar_Protons_50MeV.root");
  file->Get("FairBaseParSet"); 
  
  // -----   Runtime database   ---------------------------------------------
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parIn = new FairParRootFileIo();
  parIn->open("r3bpar_Protons_50MeV.root");
  rtdb->setFirstInput(parIn);
  rtdb->print();


  fRun->SetInputFile("r3bsim_Protons_50MeV.root");

  fRun->SetOutputFile("test.root");
  //fRun->LoadGeometry();

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  //FairMCPointDraw *LandPoints =   new FairMCPointDraw ("LandPoint",kOrange,  kFullSquare);

                                                               
  fMan->AddTask(Track);
  //fMan->AddTask(LandPoints);
    
  fMan->Init();
}
