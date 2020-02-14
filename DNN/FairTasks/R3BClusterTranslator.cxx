// Every CXX-file includes its own header file:
#include "R3BClusterTranslator.h"

// Default constructor definition:
R3BClusterTranslator::R3BClusterTranslator() : FairTask("R3BClusterTranslator")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Arrays:
    fArrayDigis = 0;
    fArraySignals = 0;
    fArraySignalClusters = 0;
    fArraySimpleClusters = 0;
    nDigiRuns = 1;
    
    // Create the TextFile Generator:
    FileGen = new R3BTextFileGenerator();
    FileGen->DisableErrorPrinting();
    
    // Also create the second-step .txt-file generators:
    FileGen_Step2 = 0;
    
    // Initialize Nuclear dataBase:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    
    // Multithreading:
    NumberOfThreads = 1;
    CurrentThread = 0;
    
    // Random numbers:
    Generator = new TRandom3(0); // time seed!
    
    // Set other class values:
    SimulationData_IsAvailable = kTRUE;
    ValidationMode = kFALSE;
    UseTextFileBias = kFALSE;
    Detection_Efficiencies = 0;
    BiasFrequencies = 0;
    MaxMultiplicity = 1;
    nEvents = 1;
    EventCounter = 0;
    Target_T = 0.0;
    Target_X = 0.0;
    Target_Y = 0.0;
    Target_Z = 0.0;
    NoTextFile = kFALSE;
    TheOutputFile = 0;
    NormalizationMethod = "Default";
    ThisIsStep2 = kFALSE;
    TheOutputPath = "./";
    Step2_Structure = "ScoringPlus";
    ComputeSingleClusterRatio = kFALSE;
    SingleFraction = 0.5;
}

// Destructor definition:
R3BClusterTranslator::~R3BClusterTranslator()
{
    // Delete the arrays:
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        if(fArraySignals[k])        {fArraySignals[k]->Clear(); delete fArraySignals[k];}
        if(fArrayDigis[k])          {fArrayDigis[k]->Clear(); delete fArrayDigis[k];}
        if(fArraySignalClusters[k]) {fArraySignalClusters[k]->Clear(); delete fArraySignalClusters[k];}
        if(fArraySimpleClusters[k]) {fArraySimpleClusters[k]->Clear(); delete fArraySimpleClusters[k];}
    }
    
    // Delete histograms:
    delete Multiplicity;
    delete EnergyDeposition;
    delete AverageSize;
    delete NumberOfClustersHist;
    delete Calibration;
    
    // Delete other objects:
    delete FileGen;
    delete FileGen_Step2;
    delete TheNuclei;
    delete Generator;
    delete BiasFrequencies;
    delete Detection_Efficiencies;
}

void R3BClusterTranslator::SetTheThreads(Int_t const nT, Int_t const cT)
{
    if ((nT>0)&&(cT>=0)&&(cT<nT))
    {
        CurrentThread = cT;
        NumberOfThreads = nT;
    }
}

// FairTask initialization function:
InitStatus R3BClusterTranslator::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BClusterTranslator::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check if the inputsclass is linked to a file:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: The inputs class was not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain all required inputs:
    nDigiRuns = Inputs->GetInputInteger("NeuLAND_Digitizer_nDifferentRuns");
    MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
    NoTextFile = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_SkipBuilding");
    NormalizationMethod = Inputs->GetInputString("NeuLAND_DNNTextFile_NormalizationMethod");
    ValidationMode = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_ValidationMode");
    Step2_Structure = Inputs->GetInputString("NeuLAND_DNNTextFile_Step2Structure");
    Target_X = Inputs->GetInputDouble("TARGET_center_x_position","cm");
    Target_Y = Inputs->GetInputDouble("TARGET_center_y_position","cm");
    Target_Z = Inputs->GetInputDouble("TARGET_center_z_position","cm");
    Target_T = Inputs->GetTargetTime();
    TheOutputPath = Inputs->GetInputString("TheOutputPath");
    SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    ComputeSingleClusterRatio = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_BiasSingleClusterEvents");
    
    if ((SimulationData_IsAvailable==kFALSE)||(ValidationMode==kTRUE)) {UseTextFileBias = kFALSE;}
    else {UseTextFileBias = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_UseTextFileBias");}
    
    // Check the number of DigiRuns:
    if (nDigiRuns<=0) {cout << "FATAL: Number of Digitizer runs has to be >=1!\n\n"; return kFATAL;}
    
    // Initialize the Arrays:
    fArrayDigis = new TClonesArray*[nDigiRuns];
    fArraySignals = new TClonesArray*[nDigiRuns];
    fArraySignalClusters = new TClonesArray*[nDigiRuns];
    fArraySimpleClusters = new TClonesArray*[nDigiRuns];
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        fArraySignals[k] = new TClonesArray("R3BSignal");
        fArrayDigis[k] = new TClonesArray("R3BNeulandDigi");
        fArraySignalClusters[k] = new TClonesArray("R3BSignalCluster");
        fArraySimpleClusters[k] = new TClonesArray("R3BSimpleCluster");
    }

    // Connect input & output arrays:
    TString st = "";
    TString kstr = "";
    TString OutputNameTag = "";
        
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        // Define the OutputNameTags:
        kstr = st.Itoa(k,10);
        OutputNameTag = "_DigiRun_" + kstr;
        if (k==0) {OutputNameTag = "";} // So there is always one without tags that can be found by other tasks.
        
        // Retrieve the clusters:
        if ((TClonesArray*)ioman->GetObject("Clusters"+OutputNameTag) == nullptr)
        {
            cout << "I/O-manager FATAL: R3BClusterTranslator::Init No R3BSignalCluster" + OutputNameTag + "!\n\n";
            return kFATAL;
        }
        fArraySignalClusters[k] = (TClonesArray*)ioman->GetObject("Clusters"+OutputNameTag);
    
        // Register R3BSimpleClusters:
        ioman->Register("TranslatedClusters"+OutputNameTag,"R3BSimpleCluster",fArraySimpleClusters[k],kTRUE);
    }
    
    // Then, set up control histograms:
    Double_t EBeamPerN = Inputs->GetAvgBeamEnergyPerNucleon();
    Double_t Eboundary = EBeamPerN*((Int_t) MaxMultiplicity);
    Int_t Ebins = (Int_t) (Eboundary/10.0);
    Int_t ClusBins = MaxMultiplicity*12;
    TString nstr = "";
    
    Multiplicity = new TH1I*[nDigiRuns];
    EnergyDeposition = new TH1D*[nDigiRuns];
    AverageSize = new TH1D*[nDigiRuns];
    NumberOfClustersHist = new TH1I*[nDigiRuns];
    Calibration = new TH2D**[nDigiRuns];
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        // Define the OutputNameTags:
        kstr = st.Itoa(k,10);
        OutputNameTag = "_DigiRun_" + kstr;
        if (k==0) {OutputNameTag = "";} // So there is always one without tags that can be found by other tasks.
    
        Multiplicity[k] = new TH1I("Multiplicity"+OutputNameTag,"Incoming particle Multiplicity",MaxMultiplicity+1,0,MaxMultiplicity+1);
        EnergyDeposition[k] = new TH1D("Total_Energy_Deposition"+OutputNameTag,"Total_Energy_Deposition",Ebins,0.0,Eboundary);
        AverageSize[k] = new TH1D("Average_ClusterSize"+OutputNameTag,"Average_ClusterSize",20,0.0,20.0);
        NumberOfClustersHist[k] = new TH1I("NumberOfClusters"+OutputNameTag,"Number of Clusters",ClusBins+1,0,ClusBins+1);
        Calibration[k] = new TH2D*[MaxMultiplicity+1];
    
        for (Int_t n = 0; n<(MaxMultiplicity+1); ++n)
        {
            nstr = st.Itoa(n,10);
            Calibration[k][n] = new TH2D("TDR_Multiplicity_Calibration_"+nstr+OutputNameTag,"TDR_Multiplicity_Calibration_"+nstr,Ebins*10,0.0,Eboundary,ClusBins+1,0.0,((Int_t) (ClusBins+1)));
        }
    }
    
    // Read Bias numbers:
    if (UseTextFileBias==kTRUE)
    {
        // Declare the reading array:
        Detection_Efficiencies = new Double_t[MaxMultiplicity+1];
    
        // Define Filename:
        TString EffFileName = TheOutputPath + "/Detection_Efficiency";
        TString tst = "";
        TString nTstr = tst.Itoa(NumberOfThreads,10);
        TString cTstr = tst.Itoa(CurrentThread,10);
        
        if (NumberOfThreads>1)
        {
            EffFileName = EffFileName + "_nT" + nTstr + "_cT" + cTstr + ".txt";
        }
        else
        {
            EffFileName = EffFileName + ".txt";
        }
    
        // Open the TextFile:
        ifstream EffTextFile;
        EffTextFile.open(EffFileName.Data());
    
        // Test if it worked:
        if (!EffTextFile.is_open())
        {
            cout << "R3BClusterTranslator::Init() FATAL: No Efficiency file for biasing the textfile writing!\n";
            return kFATAL;
        }
        else
        {
            // Then, read the numbers:
            for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
            {
                EffTextFile >> Detection_Efficiencies[k];
            }
            
            // Also the single fraction if it is there:
            if (ComputeSingleClusterRatio==kTRUE)
            {
                EffTextFile >> SingleFraction;
            }
            
            // And close the file again:
            EffTextFile.close();
        }
        
        // Next, renormalize to the 1n efficiency & progress further:
        if (MaxMultiplicity>1)
        {
            if (Detection_Efficiencies[1]>0.0)
            {
                for (Int_t k = 2; k<(MaxMultiplicity+1); ++k)
                {
                    Detection_Efficiencies[k] = Detection_Efficiencies[k]/Detection_Efficiencies[1];
                    Detection_Efficiencies[k] = 1.0/Detection_Efficiencies[k];
                }
                Detection_Efficiencies[1] = 1.0;
            }
        }
        
        // Also the single fraction:
        if (ComputeSingleClusterRatio==kTRUE)
        {
            Detection_Efficiencies[1] = 1.0/(1.0 - SingleFraction);
        }
        
        // Then, declare the bias Frequency array:
        BiasFrequencies = new Int_t[MaxMultiplicity+1];
        BiasFrequencies[0] = 1;
        Bool_t AddOne = kFALSE;
        
        // Compute other freqncies:
        for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
        {
            BiasFrequencies[k] = 1;
        }
        
        // NOTE: The actual bias frequencies will be computed along the way with random numbers, to account
        // for the fact that 1/eff is not always an integer.
    }
    
    // Next, Initizalize the TextFile Generator:
    Bool_t TextFileSucces = kTRUE;
    Bool_t ThisSucces = kTRUE;
    if (NoTextFile==kFALSE)
    {
        // Multiplicity DNN output:
        if ((ValidationMode==kFALSE)||((ValidationMode==kTRUE)&&(ThisIsStep2==kFALSE)))
        {
            FileGen->LinkInputClass(Inputs);
            FileGen->LinkIOManager(ioman);
            FileGen->SetSubFolder("DNN_DataBase");
            FileGen->SelectStep1();
            FileGen->SetTheThreads(NumberOfThreads,CurrentThread);
            ThisSucces = FileGen->Initialize(nEvents);
            if (ThisSucces==kFALSE) {FileGen->PrintAllErrors(); TextFileSucces = kFALSE;}
        }
        
        // ScoringPlus method output:
        if ((NormalizationMethod!="Elena")&&((ValidationMode==kFALSE)||((ValidationMode==kTRUE)&&(ThisIsStep2==kTRUE))))
        {
            FileGen_Step2 = new R3BTextFileGenerator*[MaxMultiplicity];
        
            for (Int_t k = 0; k<MaxMultiplicity; ++k)
            {
                kstr = st.Itoa(k+1,10);
                FileGen_Step2[k] = new R3BTextFileGenerator();
                FileGen_Step2[k]->LinkInputClass(Inputs);
                FileGen_Step2[k]->LinkIOManager(ioman);
                FileGen_Step2[k]->SetSubFolder("DNN_Step2_Mult"+kstr);
                FileGen_Step2[k]->SelectStep2(k+1);
                FileGen_Step2[k]->SetTheThreads(NumberOfThreads,CurrentThread);
                ThisSucces = FileGen_Step2[k]->Initialize(nEvents);
                if (ThisSucces==kFALSE) {FileGen_Step2[k]->PrintAllErrors(); TextFileSucces = kFALSE;}
            }
        }
    }
    
    // Then, we can return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    if (TextFileSucces==kFALSE) {return kFATAL;}
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BClusterTranslator::Exec(Option_t *option)
{
    // Before we do anything else, first update the bias frequencies:
    if (UseTextFileBias==kTRUE)
    {
        Int_t IntFreq;
        Double_t DoubleFreq;
        Double_t IntFreq_d;
        Double_t RandomNumber;
        
        if (MaxMultiplicity>=2)
        {
            for (Int_t k = 2; k<(MaxMultiplicity+1); ++k)
            {
                IntFreq = (Int_t) Detection_Efficiencies[k];
                DoubleFreq = Detection_Efficiencies[k];
                IntFreq_d = (Int_t) IntFreq;
                RandomNumber = Generator->Uniform(1.0);
                BiasFrequencies[k] = IntFreq;
                if (RandomNumber<TMath::Abs(DoubleFreq - IntFreq_d)) {BiasFrequencies[k] = BiasFrequencies[k] + 1;}
            }
        }
    }
    
    // NOTE: FairTask will automatically load the inputs into the TClonesArrays.
    
    // Declare what we need:
    Int_t nClusters;
    Int_t Simulated_Multiplicity;
    Double_t Total_EnergyDeposition;
    Double_t Average_ClusterSize;
    Int_t ClusterSize;
    Int_t nPrims_Sim;
    Int_t nPrims_Exp;
    Double_t ClusterEnergy = 0.0;
    R3BSignalCluster* SigCluster;
    R3BSimpleCluster* TransCluster = new R3BSimpleCluster();
    
    // Loop over the DigiRuns:
    for (Int_t CurrentDigiRun = 0; CurrentDigiRun<nDigiRuns; ++CurrentDigiRun)
    {
        // Clean the output vector:
        fArraySimpleClusters[CurrentDigiRun]->Clear("C");
    
        // Reset Ensemble properties:
        nClusters = fArraySignalClusters[CurrentDigiRun]->GetEntries();
        Simulated_Multiplicity = 0;
        Total_EnergyDeposition = 0.0;
        Average_ClusterSize = 0.0;
    
        // Then, loop over the clusters:
        for (Int_t k = 0; k<nClusters; ++k)
        {
            // Retrieve the cluster:
            SigCluster = (R3BSignalCluster*) fArraySignalClusters[CurrentDigiRun]->At(k);
        
            // Retrieve its properties:
            ClusterSize = SigCluster->GetSize();
            ClusterEnergy = SigCluster->GetEnergy();
            nPrims_Sim = SigCluster->GetPrimaryMultSim();
            nPrims_Exp = SigCluster->GetPrimaryMultExp();
        
            // Reset the TransCluster for a new translation:
            TransCluster->MakeEmpty();
        
            // Transfer properties:
            TransCluster->SetnPrimsSim(nPrims_Sim);
            TransCluster->SetnPrimsExp(nPrims_Exp);
            TransCluster->SetClusterEnergy(ClusterEnergy);
            TransCluster->SetClusterSize(ClusterSize);
        
            // Compute ensembles:
            Simulated_Multiplicity = Simulated_Multiplicity + nPrims_Sim;
            Total_EnergyDeposition = Total_EnergyDeposition + ClusterEnergy;
            Average_ClusterSize = Average_ClusterSize + ((Int_t) ClusterSize);
        
            // Add it to the TClonesArray of the output:
            new ((*fArraySimpleClusters[CurrentDigiRun])[k]) R3BSimpleCluster(TransCluster);
         
            // Close the loop over clusters: 
        }
        
        // Insert a failsafe:
        if (Simulated_Multiplicity>MaxMultiplicity) {Simulated_Multiplicity = MaxMultiplicity;}
        if (Simulated_Multiplicity<0) {Simulated_Multiplicity = 0;}
    
        // Calculate further with ensemble properties:
        if (nClusters>0) {Average_ClusterSize = Average_ClusterSize/((Int_t) nClusters);} else {Average_ClusterSize = 0.0;}
    
        // Fill our histograms:
        Multiplicity[CurrentDigiRun]->Fill(Simulated_Multiplicity);
        EnergyDeposition[CurrentDigiRun]->Fill(Total_EnergyDeposition);
        AverageSize[CurrentDigiRun]->Fill(Average_ClusterSize);
        NumberOfClustersHist[CurrentDigiRun]->Fill(nClusters);
        if ((Simulated_Multiplicity>=0)&&(Simulated_Multiplicity<(MaxMultiplicity+1))) {Calibration[CurrentDigiRun][Simulated_Multiplicity]->Fill(Total_EnergyDeposition,((Int_t) nClusters));}
    
        // Close the loop over the DigiRuns:
    }
    
    // Next, generate our TextFile:
    if (NoTextFile==kFALSE)
    {
        // Standard output:
        if ((ValidationMode==kFALSE)||((ValidationMode==kTRUE)&&(ThisIsStep2==kFALSE)))
        {
            for (Int_t CurrentDigiRun = 0; CurrentDigiRun<nDigiRuns; ++CurrentDigiRun)
            {
                FileGen->SetCurrentDigiRun(CurrentDigiRun);
                FileGen->Fill_Network_IO();
                if (UseTextFileBias==kFALSE) {FileGen->WriteEvent();}
                else {FileGen->WriteEventNTimes(BiasFrequencies[Simulated_Multiplicity]);}
            }
        }
        
        // Step2 output:
        if ((NormalizationMethod!="Elena")&&((ValidationMode==kFALSE)||((ValidationMode==kTRUE)&&(ThisIsStep2==kTRUE))))
        {
            for (Int_t k = 0; k<MaxMultiplicity; ++k)
            {
                for (Int_t CurrentDigiRun = 0; CurrentDigiRun<nDigiRuns; ++CurrentDigiRun)
                {
                    FileGen_Step2[k]->SetCurrentDigiRun(CurrentDigiRun);
                    
                    if (Step2_Structure=="ScoringPlus_OneCluster")
                    {
                        FileGen_Step2[k]->ProcessOneEvent_SingleCluster();
                    }
                    else
                    {
                        FileGen_Step2[k]->Fill_Network_IO();
                        if (UseTextFileBias==kFALSE) {FileGen->WriteEvent();}
                        else {FileGen->WriteEventNTimes(BiasFrequencies[Simulated_Multiplicity]);}
                    }
                }
            }
        }
        
    }
    
    // That is it. Delete-stuff:
    delete TransCluster;
    
    // Log progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) 
    {
        if (UseTextFileBias==kTRUE)
        {
            for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
            {
                cout << k << "n freq = " << BiasFrequencies[k] << " | ";
            }
        }
        
        cout << "R3BClusterTranslator Log: We processed " << EventCounter << " events.\n";
    }
}
        
// FairTask Finish function:
void R3BClusterTranslator::Finish()
{
    // Finish the TextFile:
    if (NoTextFile==kFALSE)
    {
        if ((ValidationMode==kFALSE)||((ValidationMode==kTRUE)&&(ThisIsStep2==kFALSE)))
        {
            FileGen->FinishTextFile();
            FileGen->PrintAllErrors();
        }
        
        if ((NormalizationMethod!="Elena")&&((ValidationMode==kFALSE)||((ValidationMode==kTRUE)&&(ThisIsStep2==kTRUE))))
        {
            for (Int_t k = 0; k<MaxMultiplicity; ++k)
            {
                FileGen_Step2[k]->FinishTextFile();
                FileGen_Step2[k]->PrintAllErrors();
            }
        }
    }
    
    // Write histograms:
    TheOutputFile->cd();
    TString st = "";
    TString kstr = "";
    TString nstr = "";
    TString OutputNameTag = "";
    
    for (Int_t k = 0; k<nDigiRuns; ++k)
    {
        // Define the OutputNameTags:
        kstr = st.Itoa(k,10);
        OutputNameTag = "_DigiRun_" + kstr;
        if (k==0) {OutputNameTag = "";} // So there is always one without tags that can be found by other tasks.
    
        Multiplicity[k]->Write("Multiplicity"+OutputNameTag,2);
        EnergyDeposition[k]->Write("Total_Energy_Deposition"+OutputNameTag,2);
        AverageSize[k]->Write("Average_CLusterSize"+OutputNameTag,2);
        NumberOfClustersHist[k]->Write("Number_of_Clusters"+OutputNameTag,2);
        
        for (Int_t n = 0; n<(MaxMultiplicity+1); ++n) 
        {
            nstr = st.Itoa(n,10); 
            Calibration[k][n]->Write("TDR_Calibration_"+nstr+OutputNameTag,2);
        }
    }
}

// Generate ROOT dictionary:
ClassImp(R3BClusterTranslator);
