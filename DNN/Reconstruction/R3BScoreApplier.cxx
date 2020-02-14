// Every CXX-file includes its own header file:
#include "R3BScoreApplier.h"
#include "TradMedMultiplicity.h"

// Auxillary structures and sorting functions:
struct SignalSortingStructure
{
    R3BSignal* TheSignal;
    Double_t TheScore;
};

struct ClusterSortingStructure
{
    R3BSignalCluster* TheCluster;
    Double_t TheScore;
};

Bool_t SignalSortFunction(const SignalSortingStructure &First, const SignalSortingStructure &Second)
{
    // Sort by decreasing signal score.
    
    // Declare the default:
    Bool_t Answer = kFALSE;
    
    // Do the sorting:
    if (Second.TheScore < First.TheScore) {Answer = kTRUE;}
     
    // return the answer: 
    return Answer;
}

Bool_t ClusterSortFunction(const ClusterSortingStructure &First, const ClusterSortingStructure &Second)
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
R3BScoreApplier::R3BScoreApplier() : FairTask("R3BScoreApplier")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create the input TClonesArrays:
    fArraySignals = new TClonesArray("R3BSignal");
    fArrayClusters = new TClonesArray("R3BSignalCluster");
    fArrayMult = new TClonesArray("ObjInteger");
    
    // Create the output TClonesArrays:
    fArrayPrims_Signals_CutsMult = new TClonesArray("R3BSignal");
    fArrayPrims_Signals_DNNMult = new TClonesArray("R3BSignal");
    fArrayPrims_Signals_PerfectMult = new TClonesArray("R3BSignal");
    fArrayPrims_Clusters_CutsMult = new TClonesArray("R3BSignal");
    fArrayPrims_Clusters_DNNMult = new TClonesArray("R3BSignal");
    fArrayPrims_Clusters_PerfectMult = new TClonesArray("R3BSignal");
    
    // Set input parameters:
    MaxMultiplicity = 1;
    
    // Set other quantities:
    nEvents = 1;
    EventCounter = 0;
    TheOutputFile = 0;
    SignalTotalScoringResults = 0;
    ClusterTotalScoringResults = 0;
    SignalAvgScoringResults = 0;
    ClusterAvgScoringResults = 0;
    PerfectMultHist = 0;
    CutsMultHist = 0;
    DNNMultHist = 0;
    
    // Set TDR calibration cuts:
    UseCalibrationCuts = kFALSE;
    fCuts = 0;
    fKappa = -0.1;
    
    // Create the auxillary classes:
    Inputs = 0;
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheScorers = new AllScorers();
}

// Destructor definition:
R3BScoreApplier::~R3BScoreApplier()
{
    // Delete all arrays:
    if(fArraySignals)  {fArraySignals->Clear();  delete fArraySignals;}
    if(fArrayClusters) {fArrayClusters->Clear(); delete fArrayClusters;}
    if(fArrayMult)     {fArrayMult->Clear();    delete fArrayMult;}
    
    if(fArrayPrims_Signals_CutsMult) {fArrayPrims_Signals_CutsMult->Clear(); delete fArrayPrims_Signals_CutsMult;}
    if(fArrayPrims_Signals_DNNMult) {fArrayPrims_Signals_DNNMult->Clear(); delete fArrayPrims_Signals_DNNMult;}
    if(fArrayPrims_Signals_PerfectMult) {fArrayPrims_Signals_PerfectMult->Clear(); delete fArrayPrims_Signals_PerfectMult;}
    if(fArrayPrims_Clusters_CutsMult) {fArrayPrims_Clusters_CutsMult->Clear(); delete fArrayPrims_Clusters_CutsMult;}
    if(fArrayPrims_Clusters_DNNMult) {fArrayPrims_Clusters_DNNMult->Clear(); delete fArrayPrims_Clusters_DNNMult;}
    if(fArrayPrims_Clusters_PerfectMult) {fArrayPrims_Clusters_PerfectMult->Clear(); delete fArrayPrims_Clusters_PerfectMult;}
    
    // Delete other classes:
    delete TheNuclei;
    delete TheScorers;
}

// FairTask initialization function:
InitStatus R3BScoreApplier::Init()
{    
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BScoreApplier::Init: No FairRootManager\n\n";
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
    
    // Obtain the R3BSignals:
    if ((TClonesArray*)ioman->GetObject("Signals") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BScoreApplier::Init No Signals!\n\n";
        return kFATAL;
    }
    fArraySignals = (TClonesArray*)ioman->GetObject("Signals");
    
    // Obtain the R3BSignalClusters:
    if ((TClonesArray*)ioman->GetObject("Clusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BScoreApplier::Init No Clusters!\n\n";
        return kFATAL;
    }
    fArrayClusters = (TClonesArray*)ioman->GetObject("Clusters");
    
    // Obtain the DNN multiplicity:
    if ((TClonesArray*)ioman->GetObject("DNN_Multiplicity") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BScoreApplier::Init No DNN Multiplicity!\n\n";
        return kFATAL;
    }
    fArrayMult = (TClonesArray*)ioman->GetObject("DNN_Multiplicity");
    
    // Register the reconstruction outcomes:
    ioman->Register("PrimaryHits_ScoringPlus_Signals_CutsMult","R3BSignal",fArrayPrims_Signals_CutsMult,kTRUE);
    ioman->Register("PrimaryHits_ScoringPlus_Signals_DNNMult","R3BSignal",fArrayPrims_Signals_DNNMult,kTRUE);
    ioman->Register("PrimaryHits_ScoringPlus_Signals_PerfectMult","R3BSignal",fArrayPrims_Signals_PerfectMult,kTRUE);
    ioman->Register("PrimaryHits_ScoringPlus_Clusters_CutsMult","R3BSignal",fArrayPrims_Clusters_CutsMult,kTRUE);
    ioman->Register("PrimaryHits_ScoringPlus_Clusters_DNNMult","R3BSignal",fArrayPrims_Clusters_DNNMult,kTRUE);
    ioman->Register("PrimaryHits_ScoringPlus_Clusters_PerfectMult","R3BSignal",fArrayPrims_Clusters_PerfectMult,kTRUE);
    
    // Create control histograms:
    SignalTotalScoringResults = new TH1D*[MaxMultiplicity+1];
    ClusterTotalScoringResults = new TH1D*[MaxMultiplicity+1];
    SignalAvgScoringResults = new TH1D*[MaxMultiplicity+1];
    ClusterAvgScoringResults = new TH1D*[MaxMultiplicity+1];
    TString st = "";
    TString kstr = "";
    TString HistName = "";
    Double_t Length = (Int_t) (MaxMultiplicity+1);
    
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
    {
        kstr = st.Itoa(k,10);
        HistName = "Signal_TotalScoringResult_n"+kstr;
        SignalTotalScoringResults[k] = new TH1D(HistName,HistName,200,0.0,Length);
        HistName = "Cluster_TotalScoringResult_n"+kstr;
        ClusterTotalScoringResults[k] = new TH1D(HistName,HistName,200,0.0,Length);
        HistName = "Signal_AvgScoringResult_n"+kstr;
        SignalAvgScoringResults[k] = new TH1D(HistName,HistName,200,0.0,1.0);
        HistName = "Cluster_AvgScoringResult_n"+kstr;
        ClusterAvgScoringResults[k] = new TH1D(HistName,HistName,200,0.0,1.0);
    }
    
    PerfectMultHist = new TH1I("PerfectMultHist","PerfectMultHist",MaxMultiplicity+1,0,MaxMultiplicity+1);
    CutsMultHist = new TH1I("CutsMultHist","CutsMultHist",MaxMultiplicity+1,0,MaxMultiplicity+1);
    DNNMultHist = new TH1I("DNNMultHist","DNNMultHist",MaxMultiplicity+1,0,MaxMultiplicity+1);
    
    // Next, initialize the scoring class:
    TheScorers->LinkInputsClass(Inputs);
    Bool_t ScoreTest = TheScorers->Initialize();
    
    // Check for errors:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    if (TheScorers->ContainsNoErrors()==kFALSE) {TheScorers->PrintAllErrors(); return kFATAL;}
    if (ScoreTest==kFALSE) {std::cout << "\n\nAllScorers Initialization Failed!\n\n"; return kFATAL;}
    
    // Load the previously determined scorers:
    TheScorers->ReadScorers();
    
    // And obtain the TDR Cuts for multiplicity determination:
    ReadCalibrationFile();
    
    // Return the succes statement:
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BScoreApplier::Exec(Option_t *option)
{
    // NOTE: FairTask will automatically load the TClonesArrays of
    // fArraySignals and fArrayPrims. So all we ned to do is reset 
    // the output arrays::
    fArrayPrims_Signals_CutsMult->Clear();
    fArrayPrims_Signals_DNNMult->Clear();
    fArrayPrims_Signals_PerfectMult->Clear();
    fArrayPrims_Clusters_CutsMult->Clear();
    fArrayPrims_Clusters_DNNMult->Clear();
    fArrayPrims_Clusters_PerfectMult->Clear();
    
    // -------------------------------------------------------------------
    
    // Next, sort the individual signals according to their total score:
    Int_t nSignals = fArraySignals->GetEntries();
    std::vector<SignalSortingStructure> TheSignals;
    TheSignals.clear();
    TheSignals.resize(nSignals);
    R3BSignal* ThisSignal;
    Double_t ThisSignalScore = 0.0;
    
    for (Int_t k = 0; k<nSignals; ++k)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        ThisSignalScore = TheScorers->ObtainSignalScore(ThisSignal);
        TheSignals.at(k).TheSignal = ThisSignal;
        TheSignals.at(k).TheScore = ThisSignalScore;
    }
    
    std::sort(TheSignals.begin(), TheSignals.end(), SignalSortFunction);
    
    // -------------------------------------------------------------------
    
    // Then, sort the clusters according to their total score:
    Int_t nClusters = fArrayClusters->GetEntries();
    std::vector<ClusterSortingStructure> TheClusters;
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
    
    std::sort(TheClusters.begin(), TheClusters.end(), ClusterSortFunction);
    
    // -------------------------------------------------------------------
    
    // Next, we must fill our output arrays. Begin by obtaining our multiplicities:
    Int_t Cuts_Mult = ApplyCalibrationCuts(); CutsMultHist->Fill(Cuts_Mult);
    Int_t DNN_Mult = GetDNNMultiplicity(); DNNMultHist->Fill(DNN_Mult);
    Int_t Perfect_Mult = GetPerfectMultiplicity(); PerfectMultHist->Fill(Perfect_Mult);
    Int_t nPrimHits = 0;
    
    // --- fArrayPrims_Signals_CutsMult ---
    nPrimHits = Cuts_Mult;
    if (nPrimHits>TheSignals.size()) {nPrimHits = TheSignals.size();}
    
    for (Int_t k = 0; k<nPrimHits; ++k)
    {
        new ((*fArrayPrims_Signals_CutsMult)[k]) R3BSignal(TheSignals.at(k).TheSignal);
        ((R3BSignal*) fArrayPrims_Signals_CutsMult->At(k))->SetPrimaryExp();
    }
    
    // --- fArrayPrims_Signals_DNNMult ---
    nPrimHits = DNN_Mult;
    if (nPrimHits>TheSignals.size()) {nPrimHits = TheSignals.size();}
    
    for (Int_t k = 0; k<nPrimHits; ++k)
    {
        new ((*fArrayPrims_Signals_DNNMult)[k]) R3BSignal(TheSignals.at(k).TheSignal);
        ((R3BSignal*) fArrayPrims_Signals_DNNMult->At(k))->SetPrimaryExp();
    }
    
    // --- fArrayPrims_Signals_PerfectMult ---
    nPrimHits = Perfect_Mult;
    if (nPrimHits>TheSignals.size()) {nPrimHits = TheSignals.size();}
    
    for (Int_t k = 0; k<nPrimHits; ++k)
    {
        new ((*fArrayPrims_Signals_PerfectMult)[k]) R3BSignal(TheSignals.at(k).TheSignal);
        ((R3BSignal*) fArrayPrims_Signals_PerfectMult->At(k))->SetPrimaryExp();
    }
    
    // --- fArrayPrims_Clusters_CutsMult ---
    nPrimHits = Cuts_Mult;
    if (nPrimHits>TheClusters.size()) {nPrimHits = TheClusters.size();}
    
    for (Int_t k = 0; k<nPrimHits; ++k)
    {
        // Retrieve the current cluster:
        ThisCluster = TheClusters.at(k).TheCluster;
            
        // Next, retrieve the primary signal within that cluster:
        ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
            
        // Then, create the primary hit:
        new ((*fArrayPrims_Clusters_CutsMult)[k]) R3BSignal(ThisSignal);
        ((R3BSignal*) fArrayPrims_Clusters_CutsMult->At(k))->SetPrimaryExp();
    }
    
    // --- fArrayPrims_Clusters_DNNMult ---
    nPrimHits = DNN_Mult;
    if (nPrimHits>TheClusters.size()) {nPrimHits = TheClusters.size();}
    
    for (Int_t k = 0; k<nPrimHits; ++k)
    {
        // Retrieve the current cluster:
        ThisCluster = TheClusters.at(k).TheCluster;
            
        // Next, retrieve the primary signal within that cluster:
        ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
            
        // Then, create the primary hit:
        new ((*fArrayPrims_Clusters_DNNMult)[k]) R3BSignal(ThisSignal);
        ((R3BSignal*) fArrayPrims_Clusters_DNNMult->At(k))->SetPrimaryExp();
    }
    
    // --- fArrayPrims_Clusters_PerfectMult ---
    nPrimHits = Perfect_Mult;
    if (nPrimHits>TheClusters.size()) {nPrimHits = TheClusters.size();}
    
    for (Int_t k = 0; k<nPrimHits; ++k)
    {
        // Retrieve the current cluster:
        ThisCluster = TheClusters.at(k).TheCluster;
            
        // Next, retrieve the primary signal within that cluster:
        ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
            
        // Then, create the primary hit:
        new ((*fArrayPrims_Clusters_PerfectMult)[k]) R3BSignal(ThisSignal);
        ((R3BSignal*) fArrayPrims_Clusters_PerfectMult->At(k))->SetPrimaryExp();
    }
    
    // -------------------------------------------------------------------
    
    // Fill control histograms:
    Double_t TotalSignalScore = 0.0;
    Double_t TotalClusterScore = 0.0;
    
    for (Int_t k = 0; k<nSignals; ++k)
    {
        TotalSignalScore = TotalSignalScore + TheSignals.at(k).TheScore;
    }
    
    for (Int_t k = 0; k<nClusters; ++k)
    {
        TotalClusterScore = TotalClusterScore + TheClusters.at(k).TheScore;
    }
    
    SignalTotalScoringResults[Perfect_Mult]->Fill(TotalSignalScore);
    ClusterTotalScoringResults[Perfect_Mult]->Fill(TotalClusterScore);
    SignalAvgScoringResults[Perfect_Mult]->Fill(TotalSignalScore/((Int_t) nClusters));
    ClusterAvgScoringResults[Perfect_Mult]->Fill(TotalClusterScore/((Int_t) nClusters));
    
    // Then, log the progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "R3BScoreApplier Log: We processed " << EventCounter << " events.\n";}
}

// Function to determine the perfect (detected) multiplicity:
Int_t R3BScoreApplier::GetPerfectMultiplicity()
{
    // Determine the multiplicity from the simulations:
    Int_t nPrims = 0;
    Int_t nSignals = fArraySignals->GetEntries();
    R3BSignal* ThisSignal;
    Bool_t Signal_IsPrimary = kFALSE;
    
    for (Int_t k = 0; k<nSignals; ++k)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        Signal_IsPrimary = ThisSignal->IsPrimarySim();
        
        if (Signal_IsPrimary==kTRUE) {nPrims = nPrims + 1;}
    }
    
    // Fail safe:
    if (nPrims>MaxMultiplicity) {nPrims = MaxMultiplicity;}
    
    return nPrims;
}

// Function to retrieve the multiplicity as determined by the DNN:
Int_t R3BScoreApplier::GetDNNMultiplicity()
{
    ObjInteger* ThisObjInt = (ObjInteger*) fArrayMult->At(0);
    Int_t TheMultiplicity = ThisObjInt->GetNumber();
    return TheMultiplicity;
}
        
// Definition of Finish-function:
void R3BScoreApplier::Finish()
{   
    TheOutputFile->cd();
    
    // Write our control histograms:
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
    {
        SignalTotalScoringResults[k]->Write(SignalTotalScoringResults[k]->GetName(),2);
        ClusterTotalScoringResults[k]->Write(ClusterTotalScoringResults[k]->GetName(),2);
        SignalAvgScoringResults[k]->Write(SignalAvgScoringResults[k]->GetName(),2);
        ClusterAvgScoringResults[k]->Write(ClusterAvgScoringResults[k]->GetName(),2);
    }
    
    PerfectMultHist->Write("PerfectMultHist",2);
    DNNMultHist->Write("DNNMultHist",2);
    CutsMultHist->Write("CutsMultHist",2);

    // Done.
}

// Generate ROOT dictionary:
ClassImp(R3BScoreApplier);
