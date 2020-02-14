Bool_t R3BTextFileGenerator::Initialize(Int_t const Local_nEvents)
{
    // prepares the entire TextFile Generator to be used
    // inside a FairTask. It is required that an inputs class 
    // is provided. Also, a FairRootManager* pointer has to be
    // provided in order to allow for linking to all 
    // TClonesArrays. It returns a boolian wether or not the 
    // initialization worked:
    Bool_t Succes = kFALSE;
    
    // Check that the inputs actually exist:
    if (Inputs->IsLinked()==kFALSE)
    {
        ErrorMessage("You did not provide a suitable Inputs-Class linked to a file!");
        Succes = kFALSE;
    }
    else
    {
        // Then, we test if the FairRootManager* was provided:
        if (IO_Manager_IsLinked==kFALSE)
        {
            ErrorMessage("In order to use Initialize(), you have to provide a valid FairRootManager* ioman!");
        }
        else
        {
            // Begin with retrieving the required inputs:
            PreselectionMethod = Inputs->GetInputString("NeuLAND_DNNTextFile_PreselectionMethod");
            InfoUse = Inputs->GetInputString("NeuLAND_DNNTextFile_InfoStructure");
            InfoUse_Step2 = Inputs->GetInputString("NeuLAND_DNNTextFile_Step2Structure");
            SetBoolianInputs(Inputs->GetInputString("NeuLAND_DNNTextFile_BoolianInputStructure"));
            SetBoolianOutputs(Inputs->GetInputString("NeuLAND_DNNTextFile_BoolianOutputStructure"));
            NmaxClusters = Inputs->GetInputInteger("NeuLAND_DNNTextFile_MaxNumberOfClusters");
            NmaxSignals = Inputs->GetInputInteger("NeuLAND_DNNTextFile_MaxNumberOfSignals");
            FileOption = Inputs->GetInputString("NeuLAND_DNNTextFile_SoftwareStructure");
            IO_Significance = Inputs->GetInputInteger("NeuLAND_DNNTextFile_Significance");
            MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
            NbarsPerPlane = Inputs->GetInputInteger("NeuLAND_Number_of_Bars_per_plane");
            NDoublePlanes = Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes");
            ParticleType = Inputs->GetInputString("NeuLAND_ParticleType_ToBeDetected");
            NeutronMass = TheNuclei->GetMass(ParticleType,"MeV"); 
            TimeNormalization_Option = Inputs->GetInputString("NeuLAND_DNNTextFile_TimeNormaliation_Method");
            TimeNormalization_LeftBoundary = Inputs->GetInputDouble("NeuLAND_DNNTextFile_TimeNormaliation_LeftBoundary","ns");
            TimeNormalization_RightBoundary = Inputs->GetInputDouble("NeuLAND_DNNTextFile_TimeNormaliation_RightBoundary","ns");
            Target_T = Inputs->GetTargetTime();
            Target_X = Inputs->GetInputDouble("TARGET_center_x_position","cm");
            Target_Y = Inputs->GetInputDouble("TARGET_center_y_position","cm");
            Target_Z = Inputs->GetInputDouble("TARGET_center_z_position","cm");
            MarkusFormat_AddRestNeutrons = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_MarkusFormat_AddRestNeutronsToOutput");
            SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
            NeuLAND_Center_X = Inputs->GetInputDouble("NeuLAND_center_x_position","cm");
            NeuLAND_Center_Y = Inputs->GetInputDouble("NeuLAND_center_y_position","cm");
            NeuLAND_Front_Z = Inputs->GetInputDouble("NeuLAND_front_z_position","cm");
            NeuLAND_Rot_X = Inputs->GetInputDouble("NeuLAND_x_rotation_angle","degree");
            NeuLAND_Rot_Y = Inputs->GetInputDouble("NeuLAND_y_rotation_angle","degree");
            NeuLAND_Rot_Z = Inputs->GetInputDouble("NeuLAND_z_rotation_angle","degree");
            NeuLAND_Active_Bar_Thickness = Inputs->GetInputDouble("NeuLAND_BC408_BarThickness","cm");
            NeuLAND_Total_Bar_Length = Inputs->GetInputDouble("NeuLAND_Total_BarLength","cm");
            NeuLAND_TotalBarThicknessXY = Inputs->GetTotalPaddleThicknessXY();
            NeuLAND_TotalBarThicknessZ = Inputs->GetTotalPaddleThicknessZ();
            CompressBatches = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_ForceBatchesEqualLength");
            nDigiRuns = Inputs->GetInputInteger("NeuLAND_Digitizer_nDifferentRuns");
            EventBatch = Inputs->GetInputInteger("NeuLAND_DNNTextFile_EventBatchSize");
            ValidationMode = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_ValidationMode");
            ExecuteNormalization = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_NormalizeInputs");
            IncludeTarget = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_IncludeTargetInputs");
            nOutputFlags_PerCluster = Inputs->GetInputInteger("NeuLAND_DNNTextFile_Step2_nOutputFlags_PerCluster");
            Step2_MultiplicityChoice = Inputs->GetInputString("NeuLAND_Scoring_DNNcalculation_SelectMultiplicity");
            ComputeSingleClusterRatio = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_BiasSingleClusterEvents");
            DataTimeNormaliation_UsePrimTimes = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_DataTimeNormaliation_UsePrimTimes");
            ParentOutputPath = Inputs->GetInputString("TheOutputPath");
            TString OutputPath = Inputs->GetInputString("TheOutputPath");
            TString st = "";
            TString kstr = "";
            
            // Obtain local inputs needed only inside this function:
            Int_t Local_MaxNrSignals = Inputs->GetInputInteger("NeuLAND_DNNTextFile_MaxNumberOfSignals");
            TString Local_NormalizationMethod = Inputs->GetInputString("NeuLAND_DNNTextFile_NormalizationMethod");
            
            // Initialize global random numvber generator:
            TheGenerator = new TRandom3(0); // 0=seed with time!
            
            // Decide if we actually do the second reconstruction step or not:
            if (Step2_IsSelected==kTRUE) {InfoUse = InfoUse_Step2;}
            
            if (nDigiRuns<=0) 
            {
                ErrorMessage("The number of Digitizer Runs has to be >=1!");
                Succes = kFALSE;
            }
            else
            {
                // Modify the max. number of clusters in the event of step 2:
                if (Step2_IsSelected==kTRUE)
                {
                    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
                    {
                        if (k==0) {Step2_MaxNClusters.push_back(0);}
                        else {Step2_MaxNClusters.push_back(2*k+1);}
                    }
                    
                    if (Step2_MaxNClusters.size()>Select_Determined_Multiplicity)
                    {
                        NmaxClusters = Step2_MaxNClusters.at(Select_Determined_Multiplicity);
                    }
                }
                
                // Initialize the Boolian arrays with default values:
                CurrentDigiRun = 0;
                Signals_IsLinked = new Bool_t[nDigiRuns];
                Filtered_IsLinked = new Bool_t[nDigiRuns];
                Clusters_IsLinked = new Bool_t[nDigiRuns];
                Digis_IsLinked = new Bool_t[nDigiRuns];
                Mults_AreLinked = new Bool_t[nDigiRuns];
            
                for (Int_t k = 0; k<nDigiRuns; ++k)
                {
                    Signals_IsLinked[k] = kFALSE;
                    Filtered_IsLinked[k] = kFALSE;
                    Clusters_IsLinked[k] = kFALSE;
                    Digis_IsLinked[k] = kFALSE;
                    Mults_AreLinked[k] = kFALSE;
                }
            
                // Next, attempt loading the TCLonesArrays:
                Bool_t LinkTest = kTRUE;
                Bool_t ThisLinkTest;
                ThisLinkTest = LinkDigis();            if ((ThisLinkTest==kFALSE)&&(InfoUse=="IO_Signals_MarkusPolleryd")) {LinkTest = kFALSE;}
                ThisLinkTest = LinkSignals();          if  (ThisLinkTest==kFALSE)                                          {LinkTest = kFALSE;}
                ThisLinkTest = LinkClusters();         if  (ThisLinkTest==kFALSE)                                          {LinkTest = kFALSE;}
                ThisLinkTest = LinkFilteredClusters(); if  (ThisLinkTest==kFALSE)                                          {LinkTest = kFALSE;}
                ThisLinkTest = LinkNeutronTracks();    if ((ThisLinkTest==kFALSE)&&(SimulationData_IsAvailable==kTRUE))    {LinkTest = kFALSE;}
                ThisLinkTest = LinkMultiplicities();   if ((ThisLinkTest==kFALSE)&&(SimulationData_IsAvailable==kTRUE))    {LinkTest = kFALSE;}
                
                // Also do the DNN multiplicity prediction (always call it, because the arrays should be initialized!)
                ThisLinkTest = LinkDNNMultiplicity();
                
                // See if it is a problem if the linking succeeded or not:
                if (Step2_IsSelected==kTRUE)
                {
                    if (ValidationMode==kTRUE)
                    {
                        if (ThisLinkTest==kFALSE) 
                        {
                            LinkTest = kFALSE;
                        }
                    }
                }
                
                // See if it worked:
                if (LinkTest==kFALSE)
                {
                    ErrorMessage("Some of the TClonesArray* objects could not be located!");
                    
                    if (AreNeutronsLinked()==kFALSE) {ErrorMessage("The MCNeutronTracks could not be loaded!");}
                    
                    for (Int_t k = 0; k<nDigiRuns; ++k)
                    {
                        kstr = st.Itoa(k,10);
                        if (AreSignalsLinked(k)==kFALSE) {ErrorMessage("The Signals could not be loaded for DigiRun "+kstr+"!");}
                        if (AreClustersLinked(k)==kFALSE) {ErrorMessage("The SignalClusters could not be loaded for DigiRun "+kstr+"!");}
                        if (AreFilteredClustersLinked(k)==kFALSE) {ErrorMessage("The Filtered Clusters could not be loaded for DigiRun "+kstr+"!");}
                        if (AreDigisLinked(k)==kFALSE) {ErrorMessage("The Digis could not be loaded for DigiRun "+kstr+"!");}
                        if (AreMultsLinked(k)==kFALSE) {ErrorMessage("Some of the Multiplicities could not be loaded for DigiRun "+kstr+"!");}
                        if (AreDNNMultsLinked(k)==kFALSE) {ErrorMessage("The DNN predicted Multiplicity could not be loaded for DigiRun "+kstr+"!");}
                    }
                    
                    Succes = kFALSE;
                }
                else
                {
                    // EventBatch should be positive or zero (and not bigger then the number of events):
                    if (EventBatch<0) {EventBatch = 0;}
                    if (EventBatch>Local_nEvents) {EventBatch = Local_nEvents;}
            
                    // See if the Output Path exists:
                    Bool_t OutputPath_Exists = DoesFileExist(OutputPath.Data());
                    
                    if (OutputPath_Exists==kFALSE)
                    {
                        ErrorMessage("The output path from the Inputs-class is not a valid path!");
                        Succes = kFALSE;
                    }
                    else
                    {
                        // Next, see if we have to use a sub-directory:
                        if (EventBatch!=0)
                        {
                            // Add the subdirectory:
                            OutputPath = OutputPath + "/" + SubFolder + "/";
                
                            // See if this exists:
                            if (NumberOfThreads==1)
                            {
                                Bool_t SubDir_Exists = DoesFileExist(OutputPath.Data());
                
                                // Create the subdirectory if is does not yet exist:
                                if (SubDir_Exists==kFALSE)
                                {
                                    Int_t DirSuc = mkdir(OutputPath.Data(), S_IRWXU | S_IRWXG | S_IROTH);
                    
                                    // check succes:
                                    if (DirSuc!=0)
                                    {
                                        ErrorMessage("We failed to create the subdirectory while we had to!");
                                        return kFALSE;
                                    }
                                }
                            }
                        }
            
                        // Then, we can now define the full TextFile Name:
                        FileName = OutputPath + Inputs->GetInputString("NeuLAND_DNNTextFile");
                        
                        // Next, Obtain the scorers:
                        TheScorers->LinkInputsClass(Inputs);
                        Bool_t ScoreTest = TheScorers->Initialize();
                            
                        if (ScoreTest==kFALSE) 
                        {
                            ErrorMessage("We failed to initialize the scorers!");
                            Succes = kFALSE;
                        }
                        else
                        {
                            // Read the Scorer data:
                            TheScorers->ReadScorers();
                                
                            // See if there are any errors with the scorers:
                            if (TheScorers->ContainsNoErrors()==kFALSE)
                            {
                                ErrorMessage("There are problems with reading the scorers!");
                                TheScorers->PrintAllErrors();
                                Succes = kFALSE;
                            }
                            else
                            {
                                // Next, Define the network structure:
                                Int_t Local_N_inputs = 1;
                                Int_t Local_N_outputs = 1;
                                Bool_t ValidNetwork = kTRUE;
    
                                // Check that if fileformat is Markus, the information format is that too:
                                if ((GetFileFormat()=="MARKUS")&&(InfoUse!="IO_Signals_MarkusPolleryd"))
                                {
                                    ErrorMessage("FileFormat & Network Structure have to be MARKUS at the same time!");
                                    ValidNetwork = kFALSE;
                                }
            
                                // Separately check each allowed Network Structure for consistency:
                                if (InfoUse=="IO_2to6_TradMed_BabyNetwork")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = 2;
                                    Local_N_outputs = MaxMultiplicity+1;
                                    if (IncludeTarget==kTRUE) {Local_N_inputs = Local_N_inputs + 4;}
                                }
                                else if (InfoUse=="IO_3to6_TradMed_BabyNetwork")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = 3;
                                    Local_N_outputs = MaxMultiplicity+1;
                                    if (IncludeTarget==kTRUE) {Local_N_inputs = Local_N_inputs + 4;}
                                }
                                else if (InfoUse=="IO_Signals_StandardNetwork")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = 6*Local_MaxNrSignals;
                                    Local_N_outputs = Local_MaxNrSignals;
                                    if (IncludeTarget==kTRUE) {Local_N_inputs = Local_N_inputs + 4;}
                                }
                                else if (InfoUse=="IO_Signals_MultNetwork")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = 6*Local_MaxNrSignals;
                                    Local_N_outputs = MaxMultiplicity+1;
                                    if (IncludeTarget==kTRUE) {Local_N_inputs = Local_N_inputs + 4;}
                                }
                                else if (InfoUse=="IO_Signals_MarkusPolleryd")
                                {
                                    // Then, Network structure is dnamical, so inputs & outputs do not matter:
                                    Local_N_inputs = 1;
                                    Local_N_outputs = 1;
                
                                    // But, the file format and the network structure have to match!
                                    if (GetFileFormat()!="MARKUS") 
                                    {
                                        ErrorMessage("FileFormat & Network Structure have to be MARKUS at the same time!");
                                        ValidNetwork = kFALSE;
                                    }
                                }
                                else if (InfoUse=="IO_Signals_ElenaHoemann")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = 2 + 4*NbarsPerPlane*NDoublePlanes;
                                    Local_N_outputs = MaxMultiplicity;
                                }
                                else if (InfoUse=="IO_Signals_Elena_9002_5")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = 2 + 6*NbarsPerPlane*NDoublePlanes;
                                    Local_N_outputs = MaxMultiplicity;
                                }
                                else if (InfoUse=="IO_Signals_Elena_9004_5")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = 4 + 6*NbarsPerPlane*NDoublePlanes;
                                    Local_N_outputs = MaxMultiplicity;
                                }
                                else if (InfoUse=="IO_Signals_Elena_6004_5")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = 4 + 4*NbarsPerPlane*NDoublePlanes;
                                    Local_N_outputs = MaxMultiplicity;
                                }
                                else if (InfoUse=="IO_Signals_Elena_12004_5")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = 4 + 8*NbarsPerPlane*NDoublePlanes;
                                    Local_N_outputs = MaxMultiplicity;
                                }
                                else if (InfoUse=="ScoringPlus")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = ((TheScorers->GetNumberOfClusterScores()) + 4)*NmaxClusters;
                                    Local_N_outputs = NmaxClusters*nOutputFlags_PerCluster;
                                }   
                                else if (InfoUse=="ScoringPlus_OneCluster")
                                {
                                    // Set Correct number of inputs and outputs:
                                    Local_N_inputs = (TheScorers->GetNumberOfClusterScores()) + 4;
                                    Local_N_outputs = nOutputFlags_PerCluster;
                                }   
                                else
                                {
                                    Local_N_inputs = 2;
                                    Local_N_outputs = 1;
                                    ErrorMessage("The Network Structure "+InfoUse+" was not programmed!");
                                    ValidNetwork = kFALSE;
                                }
        
                                // Determine if the network structure was succesful:
                                if (ValidNetwork==kFALSE)
                                {
                                    ErrorMessage("The Input/Output structure and/or the File Format not agree!");
                                    Succes = kFALSE;
                                }
                                else
                                {
                                    // Process the correct number of inputs & outputs:
                                    Nevents = Local_nEvents;
                                    N_inputs = Local_N_inputs;
                                    N_outputs = Local_N_outputs;
            
                                    // Then, we can finally finish our initialization procedure.
                                    TotalClusterScore_All = new TH1D("TotalClusterScore_All"+SubFolder,"TotalClusterScore_All"+SubFolder,NmaxClusters,0.0,((Int_t) NmaxClusters));
                                    TotalClusterScore_Prim = new TH1D("TotalClusterScore_Prim"+SubFolder,"TotalClusterScore_Prim"+SubFolder,NmaxClusters,0.0,((Int_t) NmaxClusters));
                                    
                                    // Initialize the normalization:
                                    Bool_t TnormTest = kTRUE;
                                    
                                         if (TimeNormalization_Option=="Default") {TimeNormalization_NormName = "T_Default";}
                                    else if (TimeNormalization_Option=="Custom")  {TimeNormalization_NormName = "T_Custom";}
                                    else if (TimeNormalization_Option=="Data")    {TimeNormalization_NormName = "T_DataDriven";}
                                    else 
                                    {
                                        ErrorMessage("The Time normalization option <"+TimeNormalization_Option+"> was not programmed!");
                                        Succes = kFALSE;
                                        TnormTest = kFALSE;
                                    }
                                    
                                    if (TnormTest==kTRUE)
                                    {
                                        NormClassDefinition();
                                        NormClassSelection(Local_NormalizationMethod);
                                    
                                        // Modify the number of events and the batch size to the number 
                                        // of digiruns:
                                        EventBatch = EventBatch*nDigiRuns;
                                        Nevents = Nevents*nDigiRuns;
                                    
                                        // Create the TextFile:
                                        CreateFile();
    
                                        // Then, finally return the succes statement:
                                        Succes = kTRUE;
                                    
                                        // Then, close all blocks:
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Print errors:
    if (Succes==kFALSE) {PrintAllErrors();}
    
    // And return the answer:
    return Succes;
}
