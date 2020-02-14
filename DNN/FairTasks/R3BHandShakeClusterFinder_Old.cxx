// Every CXX-file includes its h-file:
#include "R3BHandShakeClusterFinder.h"

// Default constructor definition:
R3BHandShakeClusterFinder::R3BHandShakeClusterFinder() : FairTask("R3BHandShakeClusterFinder")
    , fSignals("Signals")
    , fClusters("Clusters")
{
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
}

// Destructor definition:
R3BHandShakeClusterFinder::~R3BHandShakeClusterFinder()
{
    delete TheNuclei;
}

// Initialization function:
InitStatus R3BHandShakeClusterFinder::Init()
{
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
    TString ClusterCondition = Inputs->GetInputString("NeuLAND_ClusterCondition");
    XRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_X","cm");
    YRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_Y","cm");
    ZRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_Z","cm");
    TimeRadius = Inputs->GetInputDouble("NeuLAND_ClusterRadius_Time","ns");
    SpaceRadius = TMath::Sqrt((XRadius*XRadius + YRadius*YRadius + ZRadius*ZRadius)/3.0);
    
    // Define type of clustering condition:
    if (ClusterCondition=="Block") {ClusterCondition_Tag = 1;}
    else if (ClusterCondition=="Sphere") {ClusterCondition_Tag = 2;}
    else {ClusterCondition_Tag = 0;}
    
    // Define clustering conditions:
    if (ClusterCondition_Tag==1)
    {
        // Block condition, with individual x, y and z-radii:
        fClusteringEngine.SetClusteringCondition([=](const R3BSignal& a, const R3BSignal& b) 
        {
            return ((TMath::Abs(a.GetPositionX() - b.GetPositionX()<XRadius))&&
                    (TMath::Abs(a.GetPositionY() - b.GetPositionY()<YRadius))&&
                    (TMath::Abs(a.GetPositionZ() - b.GetPositionZ()<ZRadius))&&
                    (TMath::Abs(a.GetTime() - b.GetTime())<TimeRadius));
        });
    }
    else if (ClusterCondition_Tag==2)
    {
        // Spherical condition:
        fClusteringEngine.SetClusteringCondition([=](const R3BSignal& a, const R3BSignal& b) 
        {
            return ((TMath::Sqrt((a.GetPositionX() - b.GetPositionX())*(a.GetPositionX() - b.GetPositionX()) + 
                                 (a.GetPositionY() - b.GetPositionY())*(a.GetPositionY() - b.GetPositionY()) +
                                 (a.GetPositionZ() - b.GetPositionZ())*(a.GetPositionZ() - b.GetPositionZ()))<SpaceRadius)&&
                                 (TMath::Abs(a.GetTime() - b.GetTime())<TimeRadius));
        });
    }
    else
    {
        // Square condition (default):
        fClusteringEngine.SetClusteringCondition([=](const R3BSignal& a, const R3BSignal& b) 
        {
            return ((TMath::Abs(a.GetPositionX() - b.GetPositionX()<SpaceRadius))&&
                    (TMath::Abs(a.GetPositionY() - b.GetPositionY()<SpaceRadius))&&
                    (TMath::Abs(a.GetPositionZ() - b.GetPositionZ()<SpaceRadius))&&
                    (TMath::Abs(a.GetTime() - b.GetTime())<TimeRadius));
        });
    }
    
    // Next, initialize the input and output.
    // This is very easy, since the TCA-connectors are FairTasks themselves:
    try
    {
        fSignals.SetTag(OutputNameTag);
        fSignals.Init();
        fClusters.SetTag(OutputNameTag);
        fClusters.Init();
    }
    catch (const std::exception& e)
    {
        std::cout << "R3BHandShakeClusterFinder FATAL: failed to initialize!\n\n";
    }
    
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    return kSUCCESS;
}

// Execute-function:
void R3BHandShakeClusterFinder::Exec(Option_t*)
{
    // Get the std::vector of digis for this Event:
    std::vector<R3BSignal> TheSignals = fSignals.RetrieveObjects();
    Int_t nSignals = TheSignals.size();

    // Group them using the clustering condition set above: vector of digis -> vector of vector of digis
    std::vector<std::vector<R3BSignal>> ClusteredSignals = fClusteringEngine.Clusterize(TheSignals);
    Int_t nClusters = ClusteredSignals.size();

    // Convert std::vector<std::vector<Signal>> to std::vector<Cluster>:
    std::vector<R3BSignalCluster> TheClusters;
    TheClusters.resize(nClusters);
    Int_t ClusterSize = 0;
    
    for (Int_t nc = 0; nc<nClusters; ++nc)
    {
        ClusterSize = ClusteredSignals.at(nc).size();
        TheClusters.at(nc).Reserve(ClusterSize);
        
        for (Int_t ns = 0; ns<ClusterSize; ++ns)
        {
            TheClusters.at(nc).SetSignal(ns,ClusteredSignals.at(nc).at(ns));
        }
    }
    
    // Store the R3BSignalClusters:
    fClusters.Store(TheClusters);

    // Log progress:
    EventCounter = EventCounter + 1;
    if ((EventCounter%1000==0)&&(OutputNameTag=="")) {std::cout << "R3BHandShakeClusterFinder Log: We processed " << EventCounter << " events.\n";}
}

// Definition of Finish-function:
void R3BHandShakeClusterFinder::Finish() 
{
    TheOutputFile->cd();
}

// Generate ROOT-dictionary:
ClassImp(R3BHandShakeClusterFinder);
