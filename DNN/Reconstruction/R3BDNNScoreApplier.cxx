// Every CXX-file includes its own header file:
#include "R3BDNNScoreApplier.h"

// Auxillary structures and sorting functions:
struct TheClusterSortingStructure
{
    R3BSignalCluster* TheCluster;
    Double_t TheScore;
};

Bool_t TheClusterSortFunction(const TheClusterSortingStructure &First, const TheClusterSortingStructure &Second)
{
    // Sort by decreasing cluster score.
    
    // Declare the default:
    Bool_t Answer = kFALSE;
    
    // Do the sorting:
    if (Second.TheScore < First.TheScore) {Answer = kTRUE;}
     
    // return the answer: 
    return Answer;
}

// Default constructor definition:
R3BDNNScoreApplier::R3BDNNScoreApplier() : FairTask("R3BDNNScoreApplier")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create the input TClonesArrays:
    fArrayClusters = new TClonesArray("R3BSignalCluster");
    fArrayDNNMult = new TClonesArray("ObjInteger");
    
    // Create the output TClonesArrays:
    fArrayPrims = new TClonesArray("R3BSignal");
    fArrayPrims_SingleTOF = new TClonesArray("R3BSignal");
    fArrayPrims_Max = new TClonesArray("R3BSignal");
    
    // Set input parameters:
    MaxMultiplicity = 1;
    NmaxClusters = 1;
    OutputPath = "./";
    TotalNumberOfScorers = 1;
    SimulationData_IsAvailable = kFALSE;
    MultiplicityChoice = "DNN";
    nOutputNeurons = 2;
    
    // Set other quantities:
    nEvents = 1;
    EventCounter = 0;
    TheOutputFile = 0;
    
    // Create the auxillary classes:
    Inputs = 0;
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheScorers = new AllScorers();
    FileGen = new R3BTextFileGenerator();
    FileGen->DisableErrorPrinting();
    
    // TextFiles:
    DNN_Step2_TextFile = 0;
}

// Destructor definition:
R3BDNNScoreApplier::~R3BDNNScoreApplier()
{
    // Delete all arrays:
    if(fArrayClusters) {fArrayClusters->Clear(); delete fArrayClusters;}
    if(fArrayDNNMult) {fArrayDNNMult->Clear(); delete fArrayDNNMult;}
    if(fArrayPrims) {fArrayPrims->Clear(); delete fArrayPrims;}
    if(fArrayPrims_SingleTOF) {fArrayPrims_SingleTOF->Clear(); delete fArrayPrims_SingleTOF;}
    if(fArrayPrims_Max) {fArrayPrims_Max->Clear(); delete fArrayPrims_Max;}
     
    // Delete other classes:
    delete TheNuclei;
    delete TheScorers;
}

// FairTask initialization function:
InitStatus R3BDNNScoreApplier::Init()
{    
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BDNNScoreApplier::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain required inputs:
    MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
    NmaxClusters = Inputs->GetInputInteger("NeuLAND_DNNTextFile_MaxNumberOfClusters");
    OutputPath = Inputs->GetInputString("TheOutputPath");
    SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    MultiplicityChoice = Inputs->GetInputString("NeuLAND_Scoring_DNNcalculation_SelectMultiplicity");
    nOutputNeurons = Inputs->GetInputInteger("NeuLAND_DNNTextFile_Step2_nOutputFlags_PerCluster");
    
    // Obtain the R3BSignalClusters:
    if ((TClonesArray*)ioman->GetObject("Clusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BDNNScoreApplier::Init No Clusters!\n\n";
        return kFATAL;
    }
    fArrayClusters = (TClonesArray*)ioman->GetObject("Clusters");
    
    // Obtain the DNN multiplicity:
    if ((TClonesArray*)ioman->GetObject("DNN_Multiplicity") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BDNNScoreApplier::Init No DNN Multiplicity!\n\n";
        return kFATAL;
    }
    fArrayDNNMult = (TClonesArray*)ioman->GetObject("DNN_Multiplicity");
    
    // Register the reconstruction outcomes:
    ioman->Register("PrimaryHits_DNNScoringPlus","R3BSignal",fArrayPrims,kTRUE);
    ioman->Register("PrimaryHits_DNNScoringPlus_SingleTOF","R3BSignal",fArrayPrims_SingleTOF,kTRUE);
    ioman->Register("PrimaryHits_DNNScoringPlus_Max","R3BSignal",fArrayPrims_Max,kTRUE);
    
    // Next, initialize the scoring class:
    TheScorers->LinkInputsClass(Inputs);
    Bool_t ScoreTest = TheScorers->Initialize();
    
    // Check for errors:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    if (TheScorers->ContainsNoErrors()==kFALSE) {TheScorers->PrintAllErrors(); return kFATAL;}
    if (ScoreTest==kFALSE) {std::cout << "\n\nAllScorers Initialization Failed!\n\n"; return kFATAL;}
    
    // Load the previously determined scorers:
    TheScorers->ReadScorers();
    TotalNumberOfScorers = TheScorers->GetNumberOfClusterScores();
    
    // Ititialize a TextFileGenerator, so we can obtain the cluster boundaries:
    FileGen->LinkInputClass(Inputs);
    FileGen->LinkIOManager(ioman);
    FileGen->SetSubFolder("DNN_Stupid");
    FileGen->SelectStep2(MaxMultiplicity);
    FileGen->SetTheThreads(1,0);
    Bool_t ThisSucces = FileGen->Initialize(nEvents);
    DNN_ClusterBoundaries.clear();
    DNN_ClusterBoundaries = FileGen->GetStep2_MaxNClusters();
    
    // Link to the required .txt-files:
    TString MultName = OutputPath + "/PredictedMultiplicities.txt";
    TString Step2_Name = "";
    TString st = "";
    TString kstr = "";
    
    // Create array of Step2-files:
    DNN_Step2_TextFile = new ifstream[MaxMultiplicity];
    
    // Loop over the array:
    for (Int_t k = 0; k<MaxMultiplicity; ++k)
    {
        kstr = st.Itoa(k+1,10);
        Step2_Name = OutputPath + "/PredictedClusters_Mult" + kstr + ".txt";
        DNN_Step2_TextFile[k].open(Step2_Name.Data());
        if (!DNN_Step2_TextFile[k].is_open()) {cout << "The Step2 .txt-file for multiplicity " << k+1 << " could not be found!\n\n"; return kFATAL;}
         
        cout << " ==> DNN_ClusterBoundaries.at("<<k+1<<") = " << DNN_ClusterBoundaries.at(k+1) << "\n";
        
        // Check boundaries:
        if (DNN_ClusterBoundaries.size()<=(k+1))
        {
            DNN_ClusterBoundaries.push_back(NmaxClusters);
        }
    }
    
    // Return the succes statement:
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BDNNScoreApplier::Exec(Option_t *option)
{
    // NOTE: FairTask will automatically load the TClonesArrays of
    // fArraySignals and fArrayPrims. So all we ned to do is reset 
    // the output arrays::
    fArrayPrims->Clear();
    fArrayPrims_SingleTOF->Clear();
    fArrayPrims_Max->Clear();
    
    // -------------------------------------------------------------------
    
    // Then, sort the clusters according to their total score:
    Int_t nClusters = fArrayClusters->GetEntries();
    std::vector<TheClusterSortingStructure> TheClusters;
    TheClusters.clear();
    TheClusters.resize(nClusters);
    R3BSignalCluster* ThisCluster;
    Double_t ThisClusterScore = 0.0;
    
    for (Int_t k = 0; k<nClusters; ++k)
    {
        ThisCluster = (R3BSignalCluster*) fArrayClusters->At(k);
        ThisClusterScore = TheScorers->ObtainClusterScore(ThisCluster);
        TheClusters.at(k).TheCluster = ThisCluster;
        TheClusters.at(k).TheScore = ThisClusterScore;
    }
    
    std::sort(TheClusters.begin(), TheClusters.end(), TheClusterSortFunction);
    
    // -------------------------------------------------------------------
    
    // Next, we must replace the normal score with the DNN-determined score.
    std::string ThisLine;
    Int_t DNN_Multiplicity;
    Int_t DetectedMultiplicity;
    Int_t CurrentNumberOfClusters;
    Double_t DNNClusterScore;
    R3BSignal* ThisSignal;
    
    // Extract the DNN multiplicity:
    DNN_Multiplicity = 0;
    ObjInteger* ThisObjInt = (ObjInteger*) fArrayDNNMult->At(0);
    DNN_Multiplicity = ThisObjInt->GetNumber();
    
    // Also obtain the detected multiplicity:
    if (SimulationData_IsAvailable==kTRUE)
    {
        DetectedMultiplicity = 0;
        
        for (Int_t k = 0; k<nClusters; ++k)
        {
            DetectedMultiplicity = DetectedMultiplicity + TheClusters.at(k).TheCluster->GetPrimaryMultSim();
        }
    }
    
    // Choose the appropriate multiplicity:
    Int_t TheMultiplicity = DNN_Multiplicity;
    if (MultiplicityChoice=="Perfect") {TheMultiplicity = DetectedMultiplicity;}
    
    // Use a Fail safe:
    if (TheMultiplicity>MaxMultiplicity) {TheMultiplicity = MaxMultiplicity;}
    
    // Then, select the current number of clusters to evaluate:
    CurrentNumberOfClusters = DNN_ClusterBoundaries.at(TheMultiplicity); // has size n+1, since 0 is there too.
    if (CurrentNumberOfClusters>nClusters) {CurrentNumberOfClusters = nClusters;}
    
    // Next, retrieve that amount of cluster-scores from the respective .txt-file:
    for (Int_t k = 0; k<CurrentNumberOfClusters; ++k)
    {
        // Extract cluster score:
        DNN_Step2_TextFile[TheMultiplicity-1] >> DNNClusterScore;
        
        // Go to the next line:
        std::getline(DNN_Step2_TextFile[TheMultiplicity-1],ThisLine);
        
        // Put the new score into the vector:
        TheClusters.at(k).TheScore = DNNClusterScore;
        
        // Add a shift, to ensure that DNN scores are always higher:
        TheClusters.at(k).TheScore = TheClusters.at(k).TheScore + 2.0*((Int_t) TotalNumberOfScorers);
        
        // Done.
    }
    
    // Next, re-sort the vector:
    std::sort(TheClusters.begin(), TheClusters.end(), TheClusterSortFunction);
    
    // Also, find the minimum TOF-index. NOTE: AFTER THE LAST SORTING of course!!!
    Int_t MinTOF_Index = 0;
    Double_t MinTOF = 1e99;
    Double_t ThisTOF;
    
    for (Int_t k = 0; k<TheClusters.size(); ++k)
    {
        // Extract current TOF:
        ThisTOF = TheScorers->GetPrimaryClusterHit(TheClusters.at(k).TheCluster)->GetTime();
        
        // Compute minimum:
        if (ThisTOF<MinTOF)
        {
            MinTOF = ThisTOF;
            MinTOF_Index = k;
        }
    }
    
    // Then, take the first clusters and use them:
    if (nOutputNeurons<=2)
    {
        CurrentNumberOfClusters = TheMultiplicity;
        if (nClusters<TheMultiplicity) {CurrentNumberOfClusters = nClusters;}
    
        for (Int_t k = 0; k<CurrentNumberOfClusters; ++k)
        {
            // Retrieve the current cluster:
            ThisCluster = TheClusters.at(k).TheCluster;
            
            // Next, retrieve the primary signal within that cluster:
            ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
            
            // Then, create the primary hit:
            new ((*fArrayPrims)[k]) R3BSignal(ThisSignal);
            ((R3BSignal*) fArrayPrims->At(k))->SetPrimaryExp();
        }
        
        if (TheMultiplicity==1)
        {
            // Retrieve the min. TOF cluster:
            ThisCluster = TheClusters.at(MinTOF_Index).TheCluster;
            
            // Next, retrieve the primary signal within that cluster:
            ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
            
            // Then, create the primary hit:
            new ((*fArrayPrims_SingleTOF)[0]) R3BSignal(ThisSignal);
            ((R3BSignal*) fArrayPrims_SingleTOF->At(0))->SetPrimaryExp();
        }
        else
        {
            // Then, do the same as normal:
            for (Int_t k = 0; k<CurrentNumberOfClusters; ++k)
            {
                // Retrieve the current cluster:
                ThisCluster = TheClusters.at(k).TheCluster;
            
                // Next, retrieve the primary signal within that cluster:
                ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
            
                // Then, create the primary hit:
                new ((*fArrayPrims_SingleTOF)[k]) R3BSignal(ThisSignal);
                ((R3BSignal*) fArrayPrims_SingleTOF->At(k))->SetPrimaryExp();
            }
        }
            
    }
    else
    {
        // Then, we must distinguish based on the cluster scores:
        Int_t nFoundSignals = 0;
        Int_t RepeatCluster = 0;
        Int_t nNewIndex = 0;
        
        // Loop over all clusters (after sorting):
        for (Int_t k = 0; k<TheClusters.size(); ++k)
        {
            // Retrieve the current cluster:
            ThisCluster = TheClusters.at(k).TheCluster;
            
            // Retrieve the current score:
            ThisClusterScore = TheClusters.at(k).TheScore;
            
            // Then, decide how many times this cluster should be used:
            RepeatCluster = 1;
            
            for (Int_t n = 2; n<nOutputNeurons; ++n)
            {
                // Check if this cluster was indeed a multiple cluster:
                if (ThisClusterScore>(2.0*((Int_t) TotalNumberOfScorers) + ((Int_t) n)))
                {
                    // then, adjust the repeating of this cluster:
                    RepeatCluster = n;
                }
            }
            
            // Check that we indeed have that many signals left:
            if (RepeatCluster>(TheMultiplicity-nFoundSignals)) {RepeatCluster = TheMultiplicity-nFoundSignals;}
            
            // Then, extract the number of signals that we need:
            for (Int_t n = 0; n<RepeatCluster; ++n)
            {
                // Extract the signal:
                ThisSignal = TheScorers->GetSecondaryClusterHit(n,ThisCluster);
                
                // And add it to the primaries:
                new ((*fArrayPrims)[nNewIndex]) R3BSignal(ThisSignal);
                ((R3BSignal*) fArrayPrims->At(nNewIndex))->SetPrimaryExp();
                
                // And update the indices:
                nFoundSignals = nFoundSignals + 1;
                nNewIndex = nNewIndex + 1;
                
                // Done.
            }
        }
        
        if (TheMultiplicity==1)
        {
            // Retrieve the min. TOF cluster:
            ThisCluster = TheClusters.at(MinTOF_Index).TheCluster;
            
            // Next, retrieve the primary signal within that cluster:
            ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
            
            // Then, create the primary hit:
            new ((*fArrayPrims_SingleTOF)[0]) R3BSignal(ThisSignal);
            ((R3BSignal*) fArrayPrims_SingleTOF->At(0))->SetPrimaryExp();
        }
        else
        {
            nFoundSignals = 0;
            RepeatCluster = 0;
            nNewIndex = 0;
        
            // Loop over all clusters (after sorting):
            for (Int_t k = 0; k<TheClusters.size(); ++k)
            {
                // Retrieve the current cluster:
                ThisCluster = TheClusters.at(k).TheCluster;
            
                // Retrieve the current score:
                ThisClusterScore = TheClusters.at(k).TheScore;
            
                // Then, decide how many times this cluster should be used:
                RepeatCluster = 1;
            
                for (Int_t n = 2; n<nOutputNeurons; ++n)
                {
                    // Check if this cluster was indeed a multiple cluster:
                    if (ThisClusterScore>(2.0*((Int_t) TotalNumberOfScorers) + ((Int_t) n)))
                    {
                        // then, adjust the repeating of this cluster:
                        RepeatCluster = n;
                    }
                }
            
                // Check that we indeed have that many signals left:
                if (RepeatCluster>(TheMultiplicity-nFoundSignals)) {RepeatCluster = TheMultiplicity-nFoundSignals;}
            
                // Then, extract the number of signals that we need:
                for (Int_t n = 0; n<RepeatCluster; ++n)
                {
                    // Extract the signal:
                    ThisSignal = TheScorers->GetSecondaryClusterHit(n,ThisCluster);
                
                    // And add it to the primaries:
                    new ((*fArrayPrims_SingleTOF)[nNewIndex]) R3BSignal(ThisSignal);
                    ((R3BSignal*) fArrayPrims_SingleTOF->At(nNewIndex))->SetPrimaryExp();
                
                    // And update the indices:
                    nFoundSignals = nFoundSignals + 1;
                    nNewIndex = nNewIndex + 1;
                
                    // Done.
                }
            }
        }
    }
    
    // Done.
    
    // -------------------------------------------------------------------------
    
    // Also, make a cheat to obtain the best possible reconstruction for comparison purposes:
    if (SimulationData_IsAvailable==kTRUE)
    {
        // Count the number of marked clusters:
        Int_t nUsedSignals = 0;
        Int_t nMaxIndex = 0;
        Int_t ClusterLoop = 1;
        
        // loop over all clusters:
        for (Int_t k = 0; k<TheClusters.size(); ++k)
        {
            // Only considers clusters that have been evaluated by the DNN:
            if (TheClusters.at(k).TheScore > ((Int_t) TotalNumberOfScorers))
            {
                // Then, use the primary ones as long as we have multiplicity left:
                if (nUsedSignals<TheMultiplicity)
                {
                    if (TheClusters.at(k).TheCluster->GetPrimaryMultSim()>0)
                    {
                        // Loop over the number of primaries:
                        ThisCluster = TheClusters.at(k).TheCluster;
                        ClusterLoop = ThisCluster->GetPrimaryMultSim();
                        if (ClusterLoop>(nOutputNeurons-1)) {ClusterLoop = (nOutputNeurons-1);}
                        
                        for (Int_t n = 0; n<ClusterLoop; ++n)
                        {
                            // Only continue as long as we have signals left:
                            if (nUsedSignals<TheMultiplicity)
                            {
                                // Extract the appropriate signals:
                                ThisSignal = TheScorers->GetSecondaryClusterHit(n,ThisCluster);
                                
                                // Add it:
                                new ((*fArrayPrims_Max)[nMaxIndex]) R3BSignal(ThisSignal);
                                ((R3BSignal*) fArrayPrims_Max->At(nMaxIndex))->SetPrimaryExp();
                                
                                // Update indices:
                                nMaxIndex = nMaxIndex + 1;
                                nUsedSignals = nUsedSignals + 1;
                        
                                // Close blocks:
                            }
                        }
                    }
                }
            }
        }
        
        // Then, finish up the remaining multiplicity with secondary clusters:
        for (Int_t k = 0; k<TheClusters.size(); ++k)
        {
            if (nUsedSignals<TheMultiplicity)
            {
                if (TheClusters.at(k).TheCluster->GetPrimaryMultSim()==0)
                {
                    // Use these secondary clusters too:
                    nUsedSignals = nUsedSignals + 1;
                    ThisCluster = TheClusters.at(k).TheCluster;
                    ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
                    new ((*fArrayPrims_Max)[nMaxIndex]) R3BSignal(ThisSignal);
                    ((R3BSignal*) fArrayPrims_Max->At(nMaxIndex))->SetPrimaryExp();
                    nMaxIndex = nMaxIndex + 1;
                }
            }
        }
        
        // Done.
    }
                
    // Then, log the progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "R3BDNNScoreApplier Log: We processed " << EventCounter << " events.\n";}
}
        
// Definition of Finish-function:
void R3BDNNScoreApplier::Finish()
{   
    TheOutputFile->cd();
}

// Generate ROOT dictionary:
ClassImp(R3BDNNScoreApplier);
