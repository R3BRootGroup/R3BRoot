// Every CXX-file includes its h-file:
#include "R3BHandShakeClusterFinder.h"

// Default constructor definition:
R3BHandShakeClusterFinder::R3BHandShakeClusterFinder() : FairTask("R3BHandShakeClusterFinder")
{
    // Class content:
    //fClusteringEngine = new ClusteringEngine();
    fArraySignals = new TClonesArray("R3BSignal");
    fArrayClusters = new TClonesArray("R3BSignalCluster");
    
    // Set default parameters:
    TimeRadius = 1.0;
    SpaceRadius = 7.5;
    XRadius = 7.5;
    YRadius = 7.5;
    ZRadius = 7.5;
    ClusterCondition_Tag = 0;
    EventCounter = 0;
    nEvents = 1;
    
    // Initialize Nuclear dataBase:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheOutputFile = 0;
    OutputNameTag = "";
    ThisDetector = "NeuLAND";
    ClusterCondition = "Block";
    ComputeSingleClusterRatio = kFALSE;
    SingleMultCounter = 0;
    SingleClusterCounter = 0;
    TheOutputPath = "./";
    TotalNumberOfThreads = 1;
    CurrentThread = 0;
    
    // Control histograms:
    Signal_Time = 0;
    Signal_Energy = 0;
    Signal_X = 0;
    Signal_Y = 0;
    Signal_Z = 0;
    
    Cluster_Time = 0;
    Cluster_Energy = 0;
    Cluster_X = 0;
    Cluster_Y = 0;
    Cluster_Z = 0;
    
    Cluster_Numbers = 0;
    Cluster_Size = 0;
}

// Destructor definition:
R3BHandShakeClusterFinder::~R3BHandShakeClusterFinder()
{
    if(fArraySignals) {fArraySignals->Clear(); delete fArraySignals;}
    if(fArrayClusters) {fArrayClusters->Clear(); delete fArrayClusters;}
    
    //delete fClusteringEngine;
    delete TheNuclei;
}

// Initialization function:
InitStatus R3BHandShakeClusterFinder::Init()
{
    // Check programmed detectors:
    Bool_t Detector_IsProgrammed = kFALSE;
    if (ThisDetector=="NeuLAND") {Detector_IsProgrammed = kTRUE;}
    else if (ThisDetector=="NEBULA") {Detector_IsProgrammed = kTRUE;}
    if (Detector_IsProgrammed==kFALSE) {cout << "R3BClusterTranslator::Init(): Detector <" << ThisDetector << "> was not programmed!\n\n"; return kFATAL;}
    
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BClusterTranslator::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Continue by obtaining inputs:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Retrieve input values:
    if (ThisDetector=="NeuLAND")
    {
        ClusterCondition = Inputs->GetInputString("NeuLAND_ClusterCondition");
        XRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_X","cm");
        YRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_Y","cm");
        ZRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_Z","cm");
        TimeRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_Time","ns");
        SpaceRadius = TMath::Sqrt((XRadius*XRadius + YRadius*YRadius + ZRadius*ZRadius)/3.0);
        ComputeSingleClusterRatio = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_BiasSingleClusterEvents");
        TheOutputPath = Inputs->GetInputString("TheOutputPath");
    }
    
    if (ThisDetector=="NEBULA")
    {
        ClusterCondition = Inputs->GetInputString("NEBULA_ClusterCondition");
        XRadius = Inputs->GetInputDouble("NEBULA_ClusterRadius_X","cm");
        YRadius = Inputs->GetInputDouble("NEBULA_ClusterRadius_Y","cm");
        ZRadius = Inputs->GetInputDouble("NEBULA_ClusterRadius_Z","cm");
        TimeRadius = Inputs->GetInputDouble("NEBULA_ClusterRadius_Time","ns");
        SpaceRadius = TMath::Sqrt((XRadius*XRadius + YRadius*YRadius + ZRadius*ZRadius)/3.0);
        ComputeSingleClusterRatio = Inputs->GetInputBoolian("NeuLAND_DNNTextFile_BiasSingleClusterEvents");
        TheOutputPath = Inputs->GetInputString("TheOutputPath");
    }
    
    // Define type of clustering condition:
    if (ClusterCondition=="Block") {ClusterCondition_Tag = 1;}
    else if (ClusterCondition=="Sphere") {ClusterCondition_Tag = 2;}
    else {ClusterCondition_Tag = 0;}
    
    // Define clustering conditions:
    if (ClusterCondition_Tag==1)
    {
        // Block condition, with individual x, y and z-radii:
        fClusteringEngine.SetClusteringCondition([=](R3BSignal* a, R3BSignal* b) 
        {
            return ((TMath::Abs(a->GetPositionX() - b->GetPositionX())<XRadius)&&
                    (TMath::Abs(a->GetPositionY() - b->GetPositionY())<YRadius)&&
                    (TMath::Abs(a->GetPositionZ() - b->GetPositionZ())<ZRadius)&&
                    (TMath::Abs(a->GetTime() - b->GetTime())<TimeRadius));
        });
    }
    else if (ClusterCondition_Tag==2)
    {
        // Spherical condition:
        fClusteringEngine.SetClusteringCondition([=](R3BSignal* a, R3BSignal* b) 
        {
            return ((TMath::Sqrt((a->GetPositionX() - b->GetPositionX())*(a->GetPositionX() - b->GetPositionX()) + 
                                 (a->GetPositionY() - b->GetPositionY())*(a->GetPositionY() - b->GetPositionY()) +
                                 (a->GetPositionZ() - b->GetPositionZ())*(a->GetPositionZ() - b->GetPositionZ()))<SpaceRadius)&&
                                 (TMath::Abs(a->GetTime() - b->GetTime())<TimeRadius));
        });
    }
    else
    {
        // Square condition (default):
        fClusteringEngine.SetClusteringCondition([=](R3BSignal* a, R3BSignal* b) 
        {
            return ((TMath::Abs(a->GetPositionX() - b->GetPositionX())<SpaceRadius)&&
                    (TMath::Abs(a->GetPositionY() - b->GetPositionY())<SpaceRadius)&&
                    (TMath::Abs(a->GetPositionZ() - b->GetPositionZ())<SpaceRadius)&&
                    (TMath::Abs(a->GetTime() - b->GetTime())<TimeRadius));
        });
    }
    
    // Next, obtain the signals:
    if (ThisDetector=="NeuLAND")
    {
        if ((TClonesArray*)ioman->GetObject("Signals"+OutputNameTag) == nullptr)
        {
            cout << "I/O-manager FATAL: R3BHandShakeClusterFinder::Init No Signals!\n\n";
            return kFATAL;
        }
        fArraySignals = (TClonesArray*)ioman->GetObject("Signals"+OutputNameTag);
    }
    
    if (ThisDetector=="NEBULA")
    {
        if ((TClonesArray*)ioman->GetObject("NEBULASignals"+OutputNameTag) == nullptr)
        {
            cout << "I/O-manager FATAL: R3BHandShakeClusterFinder::Init No Signals!\n\n";
            return kFATAL;
        }
        fArraySignals = (TClonesArray*)ioman->GetObject("NEBULASignals"+OutputNameTag);
    }
    
    // And register the clusters:
    if (ThisDetector=="NeuLAND")
    {
        ioman->Register("Clusters"+OutputNameTag,"R3BSignalCluster",fArrayClusters,kTRUE);
    }
    
    if (ThisDetector=="NEBULA")
    {
        ioman->Register("NEBULAClusters"+OutputNameTag,"R3BSignalCluster",fArrayClusters,kTRUE);
    }
    
    // Generate control histograms:
    Signal_Time = new TH1D(ThisDetector+"Signal_Time"+OutputNameTag,"Signal_Time",200,0.0,100.0);
    Signal_Energy = new TH1D(ThisDetector+"Signal_Energy"+OutputNameTag,"Signal_Energy",200,0.0,500.0);
    Signal_X = new TH1D(ThisDetector+"Signal_X"+OutputNameTag,"Signal_X",200,-150.0,150.0);
    Signal_Y = new TH1D(ThisDetector+"Signal_Y"+OutputNameTag,"Signal_Y",200,-150.0,150.0);
    Signal_Z = new TH1D(ThisDetector+"Signal_Z"+OutputNameTag,"Signal_Z",200,1300.0,1800.0);
    
    Cluster_Time = new TH1D(ThisDetector+"Cluster_Time"+OutputNameTag,"Cluster_Time",200,0.0,100.0);
    Cluster_Energy = new TH1D(ThisDetector+"Cluster_Energy"+OutputNameTag,"Cluster_Energy",200,0.0,500.0);
    Cluster_X = new TH1D(ThisDetector+"Cluster_X"+OutputNameTag,"Cluster_X",200,-150.0,150.0);
    Cluster_Y = new TH1D(ThisDetector+"Cluster_Y"+OutputNameTag,"Cluster_Y",200,-150.0,150.0);
    Cluster_Z = new TH1D(ThisDetector+"Cluster_Z"+OutputNameTag,"Cluster_Z",200,1300.0,1800.0);
    
    Cluster_Numbers = new TH1D(ThisDetector+"Cluster_Numbers"+OutputNameTag,"Cluster_Numbers",100,0.0,99.0);
    Cluster_Size = new TH1D(ThisDetector+"Cluster_Size"+OutputNameTag,"Cluster_Size",100,0.0,99.0);
    
    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    SingleMultCounter = 0;
    SingleClusterCounter = 0;
    return kSUCCESS;
}

// Execute-function:
void R3BHandShakeClusterFinder::Exec(Option_t*)
{    
    // Begin by resetting our outputs-vector:
    fArrayClusters->Clear();
    
    // Then, put our signals into an std::vector (only the pointers!)
    std::vector<R3BSignal*> TheSignals;
    TheSignals.clear();
    Int_t nSignals = fArraySignals->GetEntries();
    TheSignals.resize(nSignals);
    R3BSignal* ThisSignal;
    
    for (Int_t k = 0; k<nSignals; ++k)
    {
        // Transfer pointers:
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        TheSignals.at(k) = ThisSignal;
        
        // Fill control histograms:
        Signal_Energy->Fill(ThisSignal->GetEnergy());
        Signal_Time->Fill(ThisSignal->GetTime());
        Signal_X->Fill(ThisSignal->GetPositionX());
        Signal_Y->Fill(ThisSignal->GetPositionY());
        Signal_Z->Fill(ThisSignal->GetPositionZ());
    }
    
    // Next, cluster the pointers:
    std::vector<std::vector<R3BSignal*>> ClusteredSignals = fClusteringEngine.Clusterize(TheSignals);
    Int_t nClusters = ClusteredSignals.size();
    
    // And then, put the clustered signals into the TClonesArray:
    R3BSignalCluster* ThisCluster;
    Int_t ClusterSize;
    
    for (Int_t kclus = 0; kclus<nClusters; ++kclus)
    {
        // Determine what the size of the cluster is going to be:
        ClusterSize = ClusteredSignals.at(kclus).size();
        
        // Then, generate the new cluster:
        new ((*fArrayClusters)[kclus]) R3BSignalCluster();
        
        // Retrieve the current cluster:
        ThisCluster = (R3BSignalCluster*) fArrayClusters->At(kclus);
        
        // Allocate the memory for the new cluster:
        ThisCluster->AllocateMemory(ClusterSize);
       
        // Next, add the signals:
        for (Int_t ksig = 0; ksig<ClusterSize; ++ksig)
        {
            ThisCluster->SetSignalp(ksig,ClusteredSignals.at(kclus).at(ksig));
        }
        
        // Done.
    }
   
    // Clean our vectors:
    for (Int_t k = 0; k<nClusters; ++k)
    {
        ClusteredSignals.at(k).clear();
    }
    ClusteredSignals.clear();
    TheSignals.clear();
    
    // Log progress:
    EventCounter = EventCounter + 1;
    if ((EventCounter%1000==0)&&(OutputNameTag=="")) {std::cout << "R3BHandShakeClusterFinder Log: We processed " << EventCounter << " events.\n";}
}

void R3BHandShakeClusterFinder::FinishEvent()
{
    Int_t nClusters = fArrayClusters->GetEntries();
    Int_t ClusterSize;
    Int_t ThisDetectedMultiplicity = 0;
    R3BSignal* ThisSignal;
    R3BSignalCluster* ThisCluster;
    
    // Fill control histograms:
    Cluster_Numbers->Fill((Int_t) nClusters);
    
    for (Int_t kclus = 0; kclus<nClusters; ++kclus)
    {
        ThisCluster = (R3BSignalCluster*) fArrayClusters->At(kclus);
        ClusterSize = ThisCluster->GetSize();
        ThisDetectedMultiplicity = ThisDetectedMultiplicity + ThisCluster->GetPrimaryMultSim();
        
        Cluster_Size->Fill(ClusterSize);
        
        for (Int_t ksig = 0; ksig<ClusterSize; ++ksig)
        {
            ThisSignal = (R3BSignal*) ThisCluster->Atp(ksig);
            
            Cluster_Energy->Fill(ThisSignal->GetEnergy());
            Cluster_Time->Fill(ThisSignal->GetTime());
            Cluster_X->Fill(ThisSignal->GetPositionX());
            Cluster_Y->Fill(ThisSignal->GetPositionY());
            Cluster_Z->Fill(ThisSignal->GetPositionZ());
        }
    }
    
    // Update Single Cluster Counters:
    if (ThisDetectedMultiplicity==1)
    {
        SingleMultCounter = SingleMultCounter + 1;
        
        if (nClusters==1)
        {
            SingleClusterCounter = SingleClusterCounter + 1;
        }
    }
    
    // Free up each cluster, AFTER the filling:
    for (Int_t kclus = 0; kclus<nClusters; ++kclus)
    {
        // Retrieve the current cluster:
        ThisCluster = (R3BSignalCluster*) fArrayClusters->At(kclus);
        
        // And free up its memory:
        ThisCluster->FreeMemory();
    }
    
    // and clear the TClonesArray:
    fArrayClusters->Clear();
    
    // Done.    
}

// Definition of Finish-function:
void R3BHandShakeClusterFinder::Finish() 
{
    TheOutputFile->cd();
    
    Signal_Time->Write(ThisDetector+"Signal_Time"+OutputNameTag,2);
    Signal_Energy->Write(ThisDetector+"Signal_Energy"+OutputNameTag,2);
    Signal_X->Write(ThisDetector+"Signal_X"+OutputNameTag,2);
    Signal_Y->Write(ThisDetector+"Signal_Y"+OutputNameTag,2);
    Signal_Z->Write(ThisDetector+"Signal_Z"+OutputNameTag,2);
    
    Cluster_Time->Write(ThisDetector+"Cluster_Time"+OutputNameTag,2);
    Cluster_Energy->Write(ThisDetector+"Cluster_Energy"+OutputNameTag,2);
    Cluster_X->Write(ThisDetector+"Cluster_X"+OutputNameTag,2);
    Cluster_Y->Write(ThisDetector+"Cluster_Y"+OutputNameTag,2);
    Cluster_Z->Write(ThisDetector+"Cluster_Z"+OutputNameTag,2);
    
    Cluster_Numbers->Write(ThisDetector+"Cluster_Numbers"+OutputNameTag,2);
    Cluster_Size->Write(ThisDetector+"Cluster_Size"+OutputNameTag,2);
    
    // Append single cluster file to the detection efficiency file:
    if (ComputeSingleClusterRatio==kTRUE)
    {
        // Reconstruct the FileName:
        TString st = "";
        TString kstr = "";
        TString nTstr = st.Itoa(TotalNumberOfThreads,10);
        TString cTstr = st.Itoa(CurrentThread,10);
        TString EffFileName = TheOutputPath + "/Detection_Efficiency";
        if (TotalNumberOfThreads>1)
        {
            EffFileName = EffFileName + "_nT" + nTstr + "_cT" + cTstr + ".txt";
        }
        else
        {
            EffFileName = EffFileName + ".txt";
        }
        
        // Connect to the file for appending:
        std::ofstream EffWriteTextFile(EffFileName.Data(), ios::app);
        
        // Test of the connection worked:
        if (!EffWriteTextFile)
        {
            std::cout << "### FATAL: We could not write the single cluster efficiency to the detection file!\n\n";
        }
        else
        {
            // Write:
            Double_t SingleFraction = 1.0/((Int_t) SingleMultCounter);
            SingleFraction = SingleFraction*((Int_t) SingleClusterCounter);
            EffWriteTextFile << SingleFraction << "\n";
            EffWriteTextFile.close();
        }
        
        // Done.
    }
}

void R3BHandShakeClusterFinder::SetMT(Int_t const nT, Int_t const cT)
{
    if (nT>0)
    {
        if (cT>=0)
        {
            if (cT<nT)
            {
                TotalNumberOfThreads = nT;
                CurrentThread = cT;
            }
        }
    }
}

// Generate ROOT-dictionary:
ClassImp(R3BHandShakeClusterFinder);
