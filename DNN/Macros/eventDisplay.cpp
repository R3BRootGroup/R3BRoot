void eventDisplay(Int_t const TotalNumberOfThreads = 1, Int_t const CurrentThread = 0)
{
    // Get inputs:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
  
    // Get FileNames:
    TString TheOutputPath = Inputs->GetInputString("TheOutputPath");
    TString MCFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_OutputFile");
    TString ParFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_ParFile");
    TString VisFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_PlotFile");
    
    // Take care of Multithreading:
    if (TotalNumberOfThreads>1)
    {
        TString nst = "";
        TString nT_tot_str = nst.Itoa(TotalNumberOfThreads,10);
        TString nT_cur_str = nst.Itoa(CurrentThread,10);
        MCFile = MCFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
        ParFile = ParFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
        VisFile = VisFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
    }
  
    // Next, perform the macro:
    FairRunAna *fRun= new FairRunAna();
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo*  parIo1 = new FairParRootFileIo();
    parIo1->open(ParFile.Data());
    rtdb->setFirstInput(parIo1);
    rtdb->print();
  
    fRun->SetSource(new FairFileSource(MCFile.Data()));
    fRun->SetOutputFile(VisFile.Data());
  
    FairEventManager *fMan= new FairEventManager();
    FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
    FairMCPointDraw *LandPoints =   new FairMCPointDraw ("LandPoint",kOrange,kFullSquare);
  
    fMan->AddTask(Track);
  
    fMan->AddTask(LandPoints);
  
    fMan->Init();
}
