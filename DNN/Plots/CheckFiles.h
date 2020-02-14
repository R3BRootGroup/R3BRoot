void R3B_DNN_Plots::CheckFiles()
{
    // Checks whether the required files exist, and, if so, links to them.
    
    // At first, we need to decide if we can retrieve inputs:
    if (Inputs==0)
    {
        ErrorMessage("You did not link this class to an inputs class!");
    }
    else
    {
        // then check if this class is linked:
        if (Inputs->IsLinked()==kFALSE)
        {
            ErrorMessage("You should provide an inputs-class to this class that is actually linked to a file!");
        }
        else
        {
            // Now we are in business. Start by retrieving the file names from the inputs-class:
            TString TheOutputPath = Inputs->GetInputString("TheOutputPath");
            TString MCFile_str = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_OutputFile");
            TString ParFile_str = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_ParFile");
            TString VisFile_str = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_PlotFile");
            TString DigiFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_Digitizer_OutputFile");
            TString JanFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_Digitizer_JanMayer_Processing");
            TString SignalFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_SignalFile");
            TString ClusterFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_ClusterFile");
            TString FilteredFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_FilteredClusterFile");
            TString TradMedFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_TraditionalMethod_ReconstructionFile");
            TString ScoreFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_ScoringOutputFile");
            TString DistFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_Distributions");
            TString DNNFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_DNN_OutputFile");
            TString FinalFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_Reconstruction_FinalFile");
            TString BetaFile_str = TheOutputPath + Inputs->GetInputString("BetaReconstruction_OutputFile");
            
            TString DNNTextFile_str = TheOutputPath + Inputs->GetInputString("NeuLAND_DNNTextFile");
            TString CutsFile_str = TheOutputPath + "./CutsFile.txt";
            TString InputsFile_str = "../InputFiles/InputsFile.txt";
            UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
            
            // Next, test all of them for existance:
            FileStat_t stom;
            Int_t TheTest;
            
            // Test .root-files:
            TheTest = gSystem->GetPathInfo(MCFile_str.Data(),stom);
            if (TheTest==0) 
            {
                MCFile = new TFile(MCFile_str.Data(),"read"); \
                MCFile_Exists = kTRUE;
            }
            else
            {
                MCFile = 0;
                MCFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(ParFile_str.Data(),stom);
            if (TheTest==0) 
            {
                ParFile = new TFile(ParFile_str.Data(),"read"); \
                ParFile_Exists = kTRUE;
            }
            else
            {
                ParFile = 0;
                ParFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(VisFile_str.Data(),stom);
            if (TheTest==0) 
            {
                VisFile = new TFile(VisFile_str.Data(),"read"); \
                VisFile_Exists = kTRUE;
            }
            else
            {
                VisFile = 0;
                VisFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(DigiFile_str.Data(),stom);
            if (TheTest==0) 
            {
                DigiFile = new TFile(DigiFile_str.Data(),"read"); \
                DigiFile_Exists = kTRUE;
            }
            else
            {
                DigiFile = 0;
                DigiFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(JanFile_str.Data(),stom);
            if (TheTest==0) 
            {
                JanFile = new TFile(JanFile_str.Data(),"read"); \
                JanFile_Exists = kTRUE;
            }
            else
            {
                JanFile = 0;
                JanFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(SignalFile_str.Data(),stom);
            if (TheTest==0) 
            {
                SignalFile = new TFile(SignalFile_str.Data(),"read"); \
                SignalFile_Exists = kTRUE;
            }
            else
            {
                SignalFile = 0;
                SignalFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(ClusterFile_str.Data(),stom);
            if (TheTest==0) 
            {
                ClusterFile = new TFile(ClusterFile_str.Data(),"read"); \
                ClusterFile_Exists = kTRUE;
            }
            else
            {
                ClusterFile = 0;
                ClusterFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(FilteredFile_str.Data(),stom);
            if (TheTest==0) 
            {
                FilteredFile = new TFile(FilteredFile_str.Data(),"read"); \
                FilteredFile_Exists = kTRUE;
            }
            else
            {
                FilteredFile = 0;
                FilteredFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(TradMedFile_str.Data(),stom);
            if (TheTest==0) 
            {
                TradMedFile = new TFile(TradMedFile_str.Data(),"read"); \
                TradMedFile_Exists = kTRUE;
            }
            else
            {
                TradMedFile = 0;
                TradMedFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(ScoreFile_str.Data(),stom);
            if (TheTest==0) 
            {
                ScoreFile = new TFile(ScoreFile_str.Data(),"read"); \
                ScoreFile_Exists = kTRUE;
            }
            else
            {
                ScoreFile = 0;
                ScoreFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(DistFile_str.Data(),stom);
            if (TheTest==0) 
            {
                DistFile = new TFile(DistFile_str.Data(),"read"); \
                DistFile_Exists = kTRUE;
            }
            else
            {
                DistFile = 0;
                DistFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(DNNFile_str.Data(),stom);
            if (TheTest==0) 
            {
                DNNFile = new TFile(DNNFile_str.Data(),"read"); \
                DNNFile_Exists = kTRUE;
            }
            else
            {
                DNNFile = 0;
                DNNFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(FinalFile_str.Data(),stom);
            if (TheTest==0) 
            {
                FinalFile = new TFile(FinalFile_str.Data(),"read"); \
                FinalFile_Exists = kTRUE;
            }
            else
            {
                FinalFile = 0;
                FinalFile_Exists = kFALSE;
            }
            
            TheTest = gSystem->GetPathInfo(BetaFile_str.Data(),stom);
            if (TheTest==0) 
            {
                BetaFile = new TFile(BetaFile_str.Data(),"read"); \
                BetaFile_Exists = kTRUE;
            }
            else
            {
                BetaFile = 0;
                BetaFile_Exists = kFALSE;
            }
            
            
            // Test .txt-files:
            DNNTextFile.open(DNNTextFile_str.Data());
            if (DNNTextFile.is_open()) {DNNTextFile_Exists = kTRUE;} else {DNNTextFile_Exists = kFALSE;}
            
            InputsFile.open(InputsFile_str.Data());
            if (InputsFile.is_open()) {InputsFile_Exists = kTRUE;} else {InputsFile_Exists = kFALSE;}
            
            CutsFile.open(CutsFile_str.Data());
            if (CutsFile.is_open()) {CutsFile_Exists = kTRUE;} else {CutsFile_Exists = kFALSE;}
            
            // Done.
        }
    }
}
