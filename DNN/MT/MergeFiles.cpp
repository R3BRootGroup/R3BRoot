inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void MergeFiles(Int_t const TotalNumberOfThreads = 1, Int_t const Situation = -1)
{
    if ((TotalNumberOfThreads>0)&&(Situation>=0)&&(Situation<=4))
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
        Int_t nEvents = Inputs->GetInputInteger("R3BRoot_nEvents");
        Bool_t MergeMCFile = Inputs->GetInputBoolian("MT_Merge_MCFile");
        
        // Retrieve extra inputs:
        Int_t nDigiRuns = Inputs->GetInputInteger("NeuLAND_Digitizer_nDifferentRuns");
        if (nDigiRuns<1) {nDigiRuns = 1;}
        if (Situation==0) {nDigiRuns = 1;}
        Bool_t NeuLAND_IsOriginal = Inputs->GetInputBoolian("NeuLAND_Use_Original");
        Bool_t UseVETO = Inputs->GetInputBoolian("VETO_Include_in_SETUP");
        Bool_t UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
        Bool_t UseNEBVETO = Inputs->GetInputBoolian("NEBULA_VET_Include_in_SETUP");
        
        // Define branch names:
        Int_t nBranches = 0;
        Int_t ThisBranch = 0;
        TString* BranchName;
        TString* BranchTitle; 
        TString* FileName;
        
        if (Situation==0)
        {
            // Monte Carlo file:
            FileName = MCFile;
            ThisBranch = 0;
            nBranches = 3;
            if (UseVETO==kTRUE) {nBranches = nBranches + 1;}
            if (UseNEBULA==kTRUE) {nBranches = nBranches + 1;}
            if (UseNEBVETO==kTRUE) {nBranches = nBranches + 1;}
            BranchName = new TString[nBranches];
            BranchTitle = new TString[nBranches];
            
            BranchName[ThisBranch] = "R3BMCTrack";
            BranchTitle[ThisBranch] = "MCTrack";
            ThisBranch = ThisBranch + 1;
            
            if (NeuLAND_IsOriginal==kTRUE)
            {
                BranchName[ThisBranch] = "R3BNeulandPoint";
                BranchTitle[ThisBranch] = "NeulandPoints";
                ThisBranch = ThisBranch + 1;
            }
            else
            {
                BranchName[ThisBranch] = "R3BCADNeulandPoint";
                BranchTitle[ThisBranch] = "NeulandPoints";
                ThisBranch = ThisBranch + 1;
            }
            
            BranchName[ThisBranch] = "FairMCPoint";
            BranchTitle[ThisBranch] = "NeulandPrimaryNeutronInteractionPoints";
            ThisBranch = ThisBranch + 1;
            
            if (UseVETO==kTRUE)
            {
                BranchName[ThisBranch] = "R3BCADVETOPoint";
                BranchTitle[ThisBranch] = "VETOPoints";
                ThisBranch = ThisBranch + 1;
            }
            
            if (UseNEBULA==kTRUE)
            {
                BranchName[ThisBranch] = "R3BCADNEBULAPoint";
                BranchTitle[ThisBranch] = "NEBULAPoints";
                ThisBranch = ThisBranch + 1;
            }
            
            if (UseNEBVETO==kTRUE)
            {
                BranchName[ThisBranch] = "R3BCADNEBULAVETOPoint";
                BranchTitle[ThisBranch] = "NEBULAVETOPoints";
                ThisBranch = ThisBranch + 1;
            }
        }
        else if (Situation==1)
        {
            // Digi-file:
            FileName = DigiFile;
            ThisBranch = 0;
            nBranches = 1;
            if (UseVETO==kTRUE) {nBranches = nBranches + 1;}
            if (UseNEBULA==kTRUE) {nBranches = nBranches + 1;}
            if (UseNEBVETO==kTRUE) {nBranches = nBranches + 1;}
            BranchName = new TString[nBranches];
            BranchTitle = new TString[nBranches];
            
            BranchName[ThisBranch] = "R3BNeulandDigi";
            BranchTitle[ThisBranch] = "NeulandDigis";
            ThisBranch = ThisBranch + 1;
            
            if (UseVETO==kTRUE)
            {
                BranchName[ThisBranch] = "R3BNeulandDigi";
                BranchTitle[ThisBranch] = "VETODigis";
                ThisBranch = ThisBranch + 1;
            }
            
            if (UseNEBULA==kTRUE)
            {
                BranchName[ThisBranch] = "R3BNeulandDigi";
                BranchTitle[ThisBranch] = "NEBULADigis";
                ThisBranch = ThisBranch + 1;
            }
            
            if (UseNEBVETO==kTRUE)
            {
                BranchName[ThisBranch] = "R3BNeulandDigi";
                BranchTitle[ThisBranch] = "NEBULAVETODigis";
                ThisBranch = ThisBranch + 1;
            }
        }
        else if (Situation==2)
        {
            // Signal-file:
            FileName = SignalFile;
            ThisBranch = 0;
            nBranches = 6;
            if (UseVETO==kTRUE) {nBranches = nBranches + 1;}
            if (UseNEBULA==kTRUE) {nBranches = nBranches + 3;}
            if (UseNEBVETO==kTRUE) {nBranches = nBranches + 1;}
            BranchName = new TString[nBranches];
            BranchTitle = new TString[nBranches];
            
            BranchName[ThisBranch] = "TLorentzVector";
            BranchTitle[ThisBranch] = "MCNeutronTracks";
            ThisBranch = ThisBranch + 1;
            
            BranchName[ThisBranch] = "TLorentzVector";
            BranchTitle[ThisBranch] = "MCRecoilTracks";
            ThisBranch = ThisBranch + 1;
            
            BranchName[ThisBranch] = "ObjInteger";
            BranchTitle[ThisBranch] = "Gun_Multiplicity";
            ThisBranch = ThisBranch + 1;
            
            BranchName[ThisBranch] = "R3BSignal";
            BranchTitle[ThisBranch] = "Signals";
            ThisBranch = ThisBranch + 1;
            
            BranchName[ThisBranch] = "ObjInteger";
            BranchTitle[ThisBranch] = "NeuLAND_Multiplicity";
            ThisBranch = ThisBranch + 1;
            
            BranchName[ThisBranch] = "ObjInteger";
            BranchTitle[ThisBranch] = "Detected_Multiplicity";
            ThisBranch = ThisBranch + 1;
            
            if (UseNEBULA==kTRUE)
            {
                BranchName[ThisBranch] = "R3BSignal";
                BranchTitle[ThisBranch] = "NEBULASignals";
                ThisBranch = ThisBranch + 1;
            
                BranchName[ThisBranch] = "ObjInteger";
                BranchTitle[ThisBranch] = "NEBULA_Multiplicity";
                ThisBranch = ThisBranch + 1;
            
                BranchName[ThisBranch] = "ObjInteger";
                BranchTitle[ThisBranch] = "NEBULA_Detected_Multiplicity";
                ThisBranch = ThisBranch + 1;
            }
            
            if (UseVETO==kTRUE)
            {
                BranchName[ThisBranch] = "R3BSignal";
                BranchTitle[ThisBranch] = "VETOSignals";
                ThisBranch = ThisBranch + 1;
            }
            
            if (UseNEBVETO==kTRUE)
            {
                BranchName[ThisBranch] = "R3BSignal";
                BranchTitle[ThisBranch] = "NEBULAVETOSignals";
                ThisBranch = ThisBranch + 1;
            }
        }
        else if (Situation==3)
        {
            // ClusterFile-file:
            FileName = ClusterFile;
            ThisBranch = 0;
            nBranches = 1;
            if (UseNEBULA==kTRUE) {nBranches = nBranches + 1;}
            BranchName = new TString[nBranches];
            BranchTitle = new TString[nBranches];
            
            BranchName[ThisBranch] = "R3BSignalCluster";
            BranchTitle[ThisBranch] = "Clusters";
            ThisBranch = ThisBranch + 1;
            
            if (UseNEBULA==kTRUE)
            {
                BranchName[ThisBranch] = "R3BSignalCluster";
                BranchTitle[ThisBranch] = "NEBULAClusters";
                ThisBranch = ThisBranch + 1;
            }
        }
        else if (Situation==4)
        {
            FileName = FilteredFile;
            ThisBranch = 0;
            nBranches = 1;
            BranchName = new TString[nBranches];
            BranchTitle = new TString[nBranches];
            
            BranchName[ThisBranch] = "R3BSimpleCluster";
            BranchTitle[ThisBranch] = "TranslatedClusters";
            ThisBranch = ThisBranch + 1;
        }
        else 
        {
            // Default-setting:
            nBranches = -1;
        }
            
        // Take care of multithreading filenames. 
        TString nst = "";
        TString nT_tot_str = "";
        TString nT_cur_str = "";
        
        // NOTE: We do NOT reduce the number of events here, but adapt it 
        // to perfectly fit the availabe events in the files:
        nEvents = nEvents/TotalNumberOfThreads;
        nEvents = nEvents*TotalNumberOfThreads;
         
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
            // Check that we do not accidentally merge MC files;
            if (!((MergeMCFile==kFALSE)&&(Situation==0)))
            {
                // Activate the timer:
                TStopwatch timer;
                timer.Start();  
        
                // Define all input files:
                FairFileSource* TheSource = new FairFileSource(FileName[1]);
                
                if (TotalNumberOfThreads>1)
                {
                    for (Int_t k = 0; k<(TotalNumberOfThreads-1); ++k)
                    {
                        TheSource->AddFile(FileName[k+2]);
                    }
                }

                // Create the Master FairTask:
                FairRunAna* run = new FairRunAna();
                run->SetSource(TheSource);
                run->SetOutputFile(FileName[0]);
                ConnectParFileToRuntimeDb(ParFile[0],run->GetRuntimeDb());
                // NOTE: we use a ParFile that has to be manually generated in the .sh-scripts!
                
                // Create the R3BMerger. No parameters are required other then the constructor. We create one for each Digi Run.
                TString st = "";
                TString kstr = "";
                TString OutputNameTag = "";
                R3BMerger** TheMerger = new R3BMerger*[nDigiRuns];
        
                for (Int_t k = 0; k<nDigiRuns; ++k)
                {
                    // Supply each R3BMerger with an individual name tag:
                    kstr = st.Itoa(k,10);
                    OutputNameTag = "_DigiRun_" + kstr;
                    if (k==0) {OutputNameTag = "";} // So there is always one without tags that can be found by other tasks.
                    
                    // Then, create the actual mergers:
                    TheMerger[k] = new R3BMerger(nBranches,BranchName,BranchTitle);
                    TheMerger[k]->SetTag(OutputNameTag);
            
                    // Add each merger to the Mother FairTask:
                    run->AddTask(TheMerger[k]);
            
                    // Done.
                }

                // Run the FairTasks:
                run->Init();
                run->Run(0,nEvents); // Over all files together!

                // Finish the timer:
                timer.Stop();

                // Give final output:
                cout << endl;
                cout << "Macro finished succesfully!" << endl;
                cout << "Output file written: " << FileName[0] << endl;
                cout << "Parameter file written: " << ParFile[0] << endl;
                cout << "nEvents: " << nEvents << endl;
                cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << endl;
                cout << endl;
            }
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
             << "And a number 0,1,2,3,4 to assign the correct merging files.\n\n\n";
    }
}
