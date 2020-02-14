#include <fstream>

using namespace std;

void GenerateFileNames(Int_t const TotalNumberOfThreads = 1, Int_t const Situation = -1)
{
    if ((TotalNumberOfThreads>0)&&(Situation>=0)&&(Situation<=8))
    {
        // Connect to InputsFile:
        R3BInputClass* Inputs = new R3BInputClass();
        Inputs->DisableErrorPrinting();
        Inputs->LinkFile("../InputFiles/InputsFile.txt");
        Inputs->ReadFile();
        
        // Retrieve General Inputs. Create arrays for MT purposes:
        TString TheOutputPath = Inputs->GetInputString("TheOutputPath");
        TString* MCFile = new TString[TotalNumberOfThreads+1]; MCFile[0] = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_OutputFile");
        TString* ParFile = new TString[TotalNumberOfThreads+1]; ParFile[0] = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_ParFile");
        TString* VisFile = new TString[TotalNumberOfThreads+1]; VisFile[0] = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_PlotFile");
        TString* DigiFile = new TString[TotalNumberOfThreads+1]; DigiFile[0] = TheOutputPath + Inputs->GetInputString("NeuLAND_Digitizer_OutputFile");
        TString* SignalFile = new TString[TotalNumberOfThreads+1]; SignalFile[0] = TheOutputPath + Inputs->GetInputString("NeuLAND_SignalFile");
        TString* ClusterFile = new TString[TotalNumberOfThreads+1]; ClusterFile[0] = TheOutputPath + Inputs->GetInputString("NeuLAND_ClusterFile");
        TString* FilteredFile = new TString[TotalNumberOfThreads+1]; FilteredFile[0] = TheOutputPath + Inputs->GetInputString("NeuLAND_FilteredClusterFile");
        TString* TradMedFile = new TString[TotalNumberOfThreads+1]; TradMedFile[0] = TheOutputPath + Inputs->GetInputString("NeuLAND_TraditionalMethod_ReconstructionFile");
        TString* DistFile = new TString[TotalNumberOfThreads+1]; DistFile[0] = TheOutputPath + Inputs->GetInputString("NeuLAND_Distributions");
                
        // Take care of multithreading filenames. 
        TString nst = "";
        TString nT_tot_str = "";
        TString nT_cur_str = "";
         
        for (Int_t k = 0; k<TotalNumberOfThreads; ++k)
        {
            // Count:
            nT_tot_str = nst.Itoa(TotalNumberOfThreads,10);
            nT_cur_str = nst.Itoa(k,10);
                
            // Adapt fileNames accordingly:
            MCFile[k+1] = MCFile[0]; MCFile[k+1] = MCFile[k+1].ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            ParFile[k+1] = ParFile[0]; ParFile[k+1] = ParFile[k+1].ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            VisFile[k+1] = VisFile[0]; VisFile[k+1] = VisFile[k+1].ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            DigiFile[k+1] = DigiFile[0]; DigiFile[k+1] = DigiFile[k+1].ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            SignalFile[k+1] = SignalFile[0]; SignalFile[k+1] = SignalFile[k+1].ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            ClusterFile[k+1] = ClusterFile[0]; ClusterFile[k+1] = ClusterFile[k+1].ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            FilteredFile[k+1] = FilteredFile[0]; FilteredFile[k+1] = FilteredFile[k+1].ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            TradMedFile[k+1] = TradMedFile[0]; TradMedFile[k+1] = TradMedFile[k+1].ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
            DistFile[k+1] = DistFile[0]; DistFile[k+1] = DistFile[k+1].ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
        }
        
        // Then, execute the macro if the Inputs Manager has no errors:
        if (Inputs->ContainsNoErrors()==kTRUE)
        {
            // Generate a new TextFile:
            ofstream TheTextFile = std::ofstream("./FileNames.txt", std::ofstream::out);
            
            // Select the proper situation:
            TString* FileName;
            if (Situation==0) {FileName = MCFile;}
            else if (Situation==1) {FileName = ParFile;}
            else if (Situation==2) {FileName = VisFile;}
            else if (Situation==3) {FileName = DigiFile;}
            else if (Situation==4) {FileName = SignalFile;}
            else if (Situation==5) {FileName = ClusterFile;}
            else if (Situation==6) {FileName = FilteredFile;}
            else if (Situation==7) {FileName = TradMedFile;}
            else if (Situation==8) {FileName = DistFile;}
            else 
            {
                FileName = new TString[TotalNumberOfThreads+1];
                for (Int_t k = 0; k<(TotalNumberOfThreads+1); ++k) {FileName[k] = "";}
            }
            
            // Write these names to a .txt-file:
            for (Int_t k = 0; k<(TotalNumberOfThreads+1); ++k)
            {
                TheTextFile << FileName[k].Data() << "\n";
            }
            
            // And close up:
            TheTextFile.close();
        }
        else
        {
            // Then, print what has gone wrong:
            Inputs->PrintAllErrors();
        }
    }
    else
    {
        cout << "\n\nEnter a strictly positive number for the number of threads!\n"
             << "and a >=0 & <=8 number for the situation!\n\n\n";
    }
}
