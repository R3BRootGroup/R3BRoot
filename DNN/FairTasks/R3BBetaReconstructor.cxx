// Every CXX-file includes its own header file:
#include "R3BBetaReconstructor.h"

// Default constructor definition:
R3BBetaReconstructor::R3BBetaReconstructor() : FairTask("R3BBetaReconstructor")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create arrays:
    fArrayNeuLANDClusters = new TClonesArray("R3BSignalCluster");
    fArrayNEBULAClusters = new TClonesArray("R3BSignalCluster");
    fArrayNeuLANDVETOSignals = new TClonesArray("R3BSignal");
    fArrayNEBULAVETOSignals = new TClonesArray("R3BSignal"); 
    
    fArrayPrimHits_NeuLAND = new TClonesArray("R3BSignal");
    fArrayPrimHits_NEBULA = new TClonesArray("R3BSignal");
    fArrayPrimHits_Combined = new TClonesArray("R3BSignal");
    
    // Input parameters:
    MaxMultiplicity = 1;
    UseNEBULA = kFALSE;
    UseVETO = kFALSE;
    UseNEBULAVETO = kFALSE;
    NeuLAND_VETOCondition = "None";
    NEBULA_VETOCondition = "None";
    Beta_LowerCut = 0.5;
    Beta_UpperCut = 0.9;
    Target_T = 0.0;
    Target_X = 0.0;
    Target_Y = 0.0;
    Target_Z = 0.0;
    ceff = 14.0;
    NEBULA_nVETOBarsPerModule = 2;
    NEBULA_nBarsPerModules = 30;
    NEBULA_nVETOBarsPerModule = 12;
    
    // Other parameters:
    EventCounter = 0;
    nEvents = 1;
    
    // Auxillary classes:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheScorers = new AllScorers();
}

// Destructor definition:
R3BBetaReconstructor::~R3BBetaReconstructor()
{
    if (fArrayNeuLANDClusters) {fArrayNeuLANDClusters->Clear(); delete fArrayNeuLANDClusters;}
    if (fArrayNEBULAClusters) {fArrayNEBULAClusters->Clear(); delete fArrayNEBULAClusters;}
    if (fArrayPrimHits_NeuLAND) {fArrayPrimHits_NeuLAND->Clear(); delete fArrayPrimHits_NeuLAND;}
    if (fArrayPrimHits_NEBULA) {fArrayPrimHits_NEBULA->Clear(); delete fArrayPrimHits_NEBULA;}
    if (fArrayPrimHits_Combined) {fArrayPrimHits_Combined->Clear(); delete fArrayPrimHits_Combined;}
    if (fArrayNeuLANDVETOSignals) {fArrayNeuLANDVETOSignals->Clear(); delete fArrayNeuLANDVETOSignals;}
    if (fArrayNEBULAVETOSignals) {fArrayNEBULAVETOSignals->Clear(); delete fArrayNEBULAVETOSignals;}
    
    // Delete other objects:
    delete TheNuclei;
    delete TheScorers;
}

// FairTask initialization function:
InitStatus R3BBetaReconstructor::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BBetaReconstructor::Init: No FairRootManager\n\n";
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
    MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
    UseVETO = Inputs->GetInputBoolian("VETO_Include_in_SETUP");
    UseNEBULA = Inputs->GetInputBoolian("NEBULA_Include_in_SETUP");
    UseNEBULAVETO = Inputs->GetInputBoolian("NEBULA_VET_Include_in_SETUP");
    NeuLAND_VETOCondition = Inputs->GetInputString("BetaReconstruction_NeuLAND_Select_VETOCondition");
    NEBULA_VETOCondition = Inputs->GetInputString("BetaReconstruction_NEBULA_Select_VETOCondition");
    Beta_LowerCut = Inputs->GetInputDouble("BetaReconstruction_Beta_LowerCut","dimless");
    Beta_UpperCut = Inputs->GetInputDouble("BetaReconstruction_Beta_UpperCut","dimless");
    Target_T = Inputs->GetTargetTime();
    Target_X = Inputs->GetInputDouble("TARGET_center_x_position","cm");
    Target_Y = Inputs->GetInputDouble("TARGET_center_y_position","cm");
    Target_Z = Inputs->GetInputDouble("TARGET_center_z_position","cm");
    ceff = Inputs->GetInputDouble("NeuLAND_Digitizer_Effective_LightSpeed","cm/ns");
    NEBULA_nModules = Inputs->GetInputInteger("NEBULA_Number_of_DoublePlaneModules");
    NEBULA_nBarsPerModules = 2*Inputs->GetInputInteger("NEBULA_Number_of_Bars_in_OnePlane");
    NEBULA_nVETOBarsPerModule = Inputs->GetInputInteger("NEBULA_VET_Number_of_Bars_in_plane");

    // Retrieve the clusters:
    if ((TClonesArray*)ioman->GetObject("Clusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BBetaReconstructor::Init No R3BSignalClusters!\n\n";
        return kFATAL;
    }
    fArrayNeuLANDClusters = (TClonesArray*)ioman->GetObject("Clusters");
    
    if (UseNEBULA==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("NEBULAClusters") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BBetaReconstructor::Init No NEBULA R3BSignalClusters!\n\n";
            return kFATAL;
        }
        fArrayNEBULAClusters = (TClonesArray*)ioman->GetObject("NEBULAClusters");
    }
    
    // Obtain VETO signals:
    if (UseVETO==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("VETOSignals") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BBetaReconstructor::Init No VETOSignals!\n\n";
            return kFATAL;
        }
        fArrayNeuLANDVETOSignals = (TClonesArray*)ioman->GetObject("VETOSignals");
    }
    
    if (UseNEBULAVETO==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("NEBULAVETOSignals") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BBetaReconstructor::Init No NEBULAVETOSignals!\n\n";
            return kFATAL;
        }
        fArrayNeuLANDVETOSignals = (TClonesArray*)ioman->GetObject("NEBULAVETOSignals");
    }
    
    // Register Output arrays:
    ioman->Register("BetaReconstructed_PrimHits_NeuLAND","R3BSignal",fArrayPrimHits_NeuLAND,kTRUE);
    
    if (UseNEBULA==kTRUE)
    {
        ioman->Register("BetaReconstructed_PrimHits_NEBULA","R3BSignal",fArrayPrimHits_NEBULA,kTRUE);
        ioman->Register("BetaReconstructed_PrimHits_Combined","R3BSignal",fArrayPrimHits_Combined,kTRUE);
    }
    
    // Next, initialize the scoring class:
    TheScorers->LinkInputsClass(Inputs);
    Bool_t ScoreTest = TheScorers->Initialize();
    if (TheScorers->ContainsNoErrors()==kFALSE) {TheScorers->PrintAllErrors(); return kFATAL;}
    if (ScoreTest==kFALSE) {std::cout << "\n\nAllScorers Initialization Failed!\n\n"; return kFATAL;}
    TheScorers->ReadScorers();
    
    // Then, we can return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BBetaReconstructor::Exec(Option_t *option)
{
    // Clean output arrays. Inputs will be handled automatically.
    fArrayPrimHits_NeuLAND->Clear();
    fArrayPrimHits_NEBULA->Clear();
    fArrayPrimHits_Combined->Clear();
    Int_t PrimHitIndex_NeuLAND = 0;
    Int_t PrimHitIndex_NEBULA = 0;
    Int_t PrimHitIndex_Combined = 0;
    
    // Declare what we need:
    Int_t nClusters_NeuLAND = fArrayNeuLANDClusters->GetEntries();
    Int_t nSignals_VETO = fArrayNeuLANDVETOSignals->GetEntries();
    R3BSignalCluster* ThisCluster;
    R3BSignal* ThisSignal;
    R3BSignal* VETOSignal;
    Double_t ThisBeta;
    Double_t VETO_TOF;
    Double_t NeuLAND_TOF;
    Double_t ThisTOF;
    TVector3 ClusterVect;
    TVector3 VETOVect;
    Double_t Theta;
    Double_t Phi;
    Double_t Cluster_ShiftedTime;
    Double_t VETOSignal_Radius;
    Int_t nMinimizations;
    Double_t ThisMinimum;
    Int_t ThisMinClusIndex;
    Int_t ThisMinVETOINdex;
    
    // =========================================================================================
    
    // NOTE: Deal with NeuLAND:
    
    // Obtain min. NeuLAND TOF:
    NeuLAND_TOF = 1e99;
    for (Int_t kclus = 0; kclus<nClusters_NeuLAND; ++kclus)
    {
        ThisCluster = (R3BSignalCluster*) fArrayNeuLANDClusters->At(kclus);
        ThisTOF = TheScorers->ComputeClusterScoreValue_TOF(ThisCluster);
        if (ThisTOF<NeuLAND_TOF) {NeuLAND_TOF = ThisTOF;}
    }
    
    // Obtain min. VETO TOF:
    VETO_TOF = 1e99;
    if ((UseVETO==kTRUE)&&(nSignals_VETO>0))
    {
        for (Int_t ksig = 0; ksig<nSignals_VETO; ++ksig)
        {
            VETOSignal = (R3BSignal*) fArrayNeuLANDVETOSignals->At(ksig);
            ThisTOF = VETOSignal->GetTime();
            if (ThisTOF<VETO_TOF) {VETO_TOF = ThisTOF;}
        }
    }
    
    // Do minimization to link VETO signals to NeuLAND clusters for the advanced VETO condition:
    std::vector<Bool_t> Cluster_IsLinked;
    Cluster_IsLinked.resize(nClusters_NeuLAND);
    for (Int_t kclus = 0; kclus<nClusters_NeuLAND; ++kclus) {Cluster_IsLinked.at(kclus) = kFALSE;}
    
    if ((nClusters_NeuLAND>0)&&(nSignals_VETO>0)&&(UseVETO==kTRUE))
    {
        // Create a distance array for the minimization:
        Double_t** DistanceArray = new Double_t*[nSignals_VETO];
        for (Int_t ksig = 0; ksig<nSignals_VETO; ++ksig)
        {
            DistanceArray[ksig] = new Double_t[nClusters_NeuLAND];
        }
        
        // Then, loop over the entire array to fill it:
        for (Int_t ksig = 0; ksig<nSignals_VETO; ++ksig)
        {
            // Obtain the VETO signal:
            VETOSignal = (R3BSignal*) fArrayNeuLANDVETOSignals->At(ksig);
            
            // loop over the clusters:
            for (Int_t kclus = 0; kclus<nClusters_NeuLAND; ++kclus)
            {
                // Obtain the current cluster:
                ThisCluster = (R3BSignalCluster*) fArrayNeuLANDClusters->At(kclus);
                
                // And its head:
                ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
                
                // If this cluster is not in the beta-window, then exclude it from the search:
                ThisBeta = TheScorers->ComputeClusterScoreValue_Beta(ThisCluster);
                
                /// Check:
                if ((ThisBeta<=Beta_LowerCut)||(ThisBeta>=Beta_UpperCut))
                {
                    DistanceArray[ksig][kclus] = 1e99;
                }
                else
                {
                    // Fill two vectors:
                    ClusterVect.SetX(ThisSignal->GetPositionX() - Target_X);
                    ClusterVect.SetX(ThisSignal->GetPositionY() - Target_Y);
                    ClusterVect.SetX(ThisSignal->GetPositionZ() - Target_Z);
                    
                    VETOVect.SetX(VETOSignal->GetPositionX() - Target_X);
                    VETOVect.SetX(VETOSignal->GetPositionY() - Target_Y);
                    VETOVect.SetX(VETOSignal->GetPositionZ() - Target_Z);
                    
                    // Rotate the cluster vector along z:
                    Theta = ClusterVect.Theta();
                    Phi = ClusterVect.Phi();
                    
                    VETOVect.RotateZ(-1.0*Phi);
                    VETOVect.RotateY(-1.0*Theta);
                    
                    ClusterVect.RotateZ(-1.0*Phi);
                    ClusterVect.RotateY(-1.0*Theta);
                    
                    // Compute Cluster Shifted Time:
                    Cluster_ShiftedTime = TheScorers->ComputeClusterScoreValue_TOF(ThisCluster);
                    Cluster_ShiftedTime = Cluster_ShiftedTime*VETOVect.Mag()/ClusterVect.Mag();
                    
                    // Compute VETO radius:
                    VETOSignal_Radius = TMath::Sqrt(VETOVect.X()*VETOVect.X() + VETOVect.Y()*VETOVect.Y());
                    
                    // Fill in the distance array:
                    DistanceArray[ksig][kclus] = TMath::Sqrt(VETOSignal_Radius*VETOSignal_Radius + ceff*ceff*(Cluster_ShiftedTime - VETOSignal->GetTime())*(Cluster_ShiftedTime - VETOSignal->GetTime()));
                    
                    // Done.
                }
            }
        }
                    
        // Next, do the minimizations:            
        nMinimizations = nSignals_VETO;
        if (nMinimizations>nClusters_NeuLAND) {nMinimizations = nClusters_NeuLAND;}
            
        for (Int_t time = 0; time<nMinimizations; ++time)
        {    
            // Reset:
            ThisMinimum = 1e98;
            ThisMinClusIndex = -1;
            ThisMinVETOINdex = -1;
            
            // Find the overall minimization (except the 1e99):
            for (Int_t ksig = 0; ksig<nSignals_VETO; ++ksig)
            {
                for (Int_t kclus = 0; kclus<nClusters_NeuLAND; ++kclus)
                {
                    if (DistanceArray[ksig][kclus]<ThisMinimum)
                    {
                        ThisMinimum = DistanceArray[ksig][kclus];
                        ThisMinClusIndex = kclus;
                        ThisMinVETOINdex = ksig;
                    }
                }
            }
            
            // Then, if we find a minimum, then mark this cluster:
            if ((ThisMinClusIndex>=0)&&(ThisMinVETOINdex>=0))
            {
                // Apply TOF condition too:
                VETOSignal = (R3BSignal*) fArrayNeuLANDVETOSignals->At(ThisMinVETOINdex);
                ThisCluster = (R3BSignalCluster*) fArrayNeuLANDClusters->At(ThisMinClusIndex);
                
                if (VETOSignal->GetTime()<TheScorers->ComputeClusterScoreValue_TOF(ThisCluster))
                {
                    // Then, mark the cluster for VETOing:
                    Cluster_IsLinked.at(ThisMinClusIndex) = kTRUE;
                }
                else
                {
                    // Then, we keep the cluster even though it is a minimum. 
                    // Then, this VETO hit is due to backscattering.
                }
                
                // Then, eliminate this link from the search:
                for (Int_t ksig = 0; ksig<nSignals_VETO; ++ksig)
                {
                    DistanceArray[ksig][ThisMinClusIndex] = 1e99;
                }
                
                for (Int_t kclus = 0; kclus<nClusters_NeuLAND; ++kclus)
                {
                    DistanceArray[ThisMinVETOINdex][kclus] = 1e99;
                }
                
                // Done.
            }
        }
        
        // delete the distance array:
        delete DistanceArray;
    }
    
    // Loop over the clusters from NeuLAND to actually create the primary neutron hits:
    for (Int_t kclus = 0; kclus<nClusters_NeuLAND; ++kclus)
    {
        // Retrieve the current cluster:
        ThisCluster = (R3BSignalCluster*) fArrayNeuLANDClusters->At(kclus);
        
        // Extract the beta of this cluster:
        ThisBeta = TheScorers->ComputeClusterScoreValue_Beta(ThisCluster);
        
        // Extract cluster head:
        ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
        
        // Check that it is in the allowed window:
        if ((ThisBeta>Beta_LowerCut)&&(ThisBeta<Beta_UpperCut))
        {
            // check if there is a VETO:
            if (UseVETO==kFALSE)
            {
                // Then it is easy. Create the primary neutron:
                new ((*fArrayPrimHits_NeuLAND)[PrimHitIndex_NeuLAND]) R3BSignal(ThisSignal);
                PrimHitIndex_NeuLAND = PrimHitIndex_NeuLAND + 1;
            }
            else
            {
                // Then, we must implement the VETO condition:
                if (NeuLAND_VETOCondition=="None")
                {
                    // Then, ignore the VETO:
                    new ((*fArrayPrimHits_NeuLAND)[PrimHitIndex_NeuLAND]) R3BSignal(ThisSignal);
                    PrimHitIndex_NeuLAND = PrimHitIndex_NeuLAND + 1;
                }
                else if (NeuLAND_VETOCondition=="Naive")
                {
                    // Then, only create primary neutrons if the VETO does not contain any signals:
                    if (nSignals_VETO==0)
                    {
                        new ((*fArrayPrimHits_NeuLAND)[PrimHitIndex_NeuLAND]) R3BSignal(ThisSignal);
                        PrimHitIndex_NeuLAND = PrimHitIndex_NeuLAND + 1;
                    }
                }
                else if (NeuLAND_VETOCondition=="TOF")
                {
                    // Then, we only VETO everything if the VETO TOF < NeuLAND TOF:
                    if ((nSignals_VETO==0)||(VETO_TOF>NeuLAND_TOF))
                    {
                        new ((*fArrayPrimHits_NeuLAND)[PrimHitIndex_NeuLAND]) R3BSignal(ThisSignal);
                        PrimHitIndex_NeuLAND = PrimHitIndex_NeuLAND + 1;
                    }
                }
                else if (NeuLAND_VETOCondition=="Advanced")
                {
                    // This is easy here, because it is programmed already:
                    if (Cluster_IsLinked.at(kclus)==kFALSE)
                    {
                        // then it is not VETOed:
                        new ((*fArrayPrimHits_NeuLAND)[PrimHitIndex_NeuLAND]) R3BSignal(ThisSignal);
                        PrimHitIndex_NeuLAND = PrimHitIndex_NeuLAND + 1;
                    }
                }
                else
                {
                    cout << "### R3BBetaReconstructor ERROR: The NeuLAND VETO condition <" << NeuLAND_VETOCondition << "> was not programmed!\n";
                }
            }
        }
    }
                        
    // =========================================================================================
    
    // NOTE: Deal with NEBULA. This is more tricky, since NEBULA contains multiple (not always 2) modules:
    
    if (UseNEBULA==kTRUE)
    {
        Int_t nClusters_NEBULA = fArrayNEBULAClusters->GetEntries();
        Int_t nSignals_NEBVETO = fArrayNEBULAVETOSignals->GetEntries();
        Int_t nSignals_VETO_ThisModule;
        Int_t ThisModule;
    
        // Obtain min. NEBULA TOF (per module):
        Double_t* NEBULA_TOF = new Double_t[NEBULA_nModules];
        for (Int_t kmod = 0; kmod<NEBULA_nModules; ++kmod)
        {
            NEBULA_TOF[kmod] = 1e99;
            for (Int_t kclus = 0; kclus<nClusters_NEBULA; ++kclus)
            {
                ThisCluster = (R3BSignalCluster*) fArrayNEBULAClusters->At(kclus);
                if (GetNEBULAModule(ThisCluster)==kmod)
                {
                    ThisTOF = TheScorers->ComputeClusterScoreValue_TOF(ThisCluster);
                    if (ThisTOF<NEBULA_TOF[kmod]) {NEBULA_TOF[kmod] = ThisTOF;}
                }
            }
        }
    
        // Obtain min. VETO TOF:
        Double_t* NEBVETO_TOF = new Double_t[NEBULA_nModules];
        for (Int_t kmod = 0; kmod<NEBULA_nModules; ++kmod)
        {
            NEBVETO_TOF[kmod] = 1e99;
            if ((UseNEBULAVETO==kTRUE)&&(nSignals_NEBVETO>0))
            {
                for (Int_t ksig = 0; ksig<nSignals_NEBVETO; ++ksig)
                {
                    VETOSignal = (R3BSignal*) fArrayNEBULAVETOSignals->At(ksig);
                    if (GetNEBVETOModule(VETOSignal)==kmod)
                    {
                        ThisTOF = VETOSignal->GetTime();
                        if (ThisTOF<NEBVETO_TOF[kmod]) {NEBVETO_TOF[kmod] = ThisTOF;}
                    }
                }
            }
        }
    
        // Do minimization to link VETO signals to NEBULA clusters for the advanced VETO condition:
        std::vector<Bool_t> NEBULA_Cluster_IsLinked;
        NEBULA_Cluster_IsLinked.resize(nClusters_NEBULA);
        for (Int_t kclus = 0; kclus<nClusters_NEBULA; ++kclus) {NEBULA_Cluster_IsLinked.at(kclus) = kFALSE;}
    
        if ((nClusters_NEBULA>0)&&(nSignals_NEBVETO>0)&&(UseNEBULAVETO==kTRUE))
        {
            // Create a distance array for the minimization:
            Double_t** DistanceArray = new Double_t*[nSignals_NEBVETO];
            for (Int_t ksig = 0; ksig<nSignals_NEBVETO; ++ksig)
            {
                DistanceArray[ksig] = new Double_t[nClusters_NEBULA];
            }
        
            // Then, loop over the entire array to fill it:
            for (Int_t ksig = 0; ksig<nSignals_NEBVETO; ++ksig)
            {   
                // Obtain the VETO signal:
                VETOSignal = (R3BSignal*) fArrayNEBULAVETOSignals->At(ksig);
            
                // loop over the clusters:
                for (Int_t kclus = 0; kclus<nClusters_NEBULA; ++kclus)
                {
                    // Obtain the current cluster:
                    ThisCluster = (R3BSignalCluster*) fArrayNEBULAClusters->At(kclus);
                
                    // And its head:
                    ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
                
                    // If this cluster is not in the beta-window, then exclude it from the search:
                    ThisBeta = TheScorers->ComputeClusterScoreValue_Beta(ThisCluster);
                
                    // Check:
                    if ((ThisBeta<=Beta_LowerCut)||(ThisBeta>=Beta_UpperCut))
                    {
                        DistanceArray[ksig][kclus] = 1e99;
                    }
                    else
                    {
                        // also exclude from the serach if they are not in the same module:
                        if (GetNEBULAModule(ThisCluster)!=GetNEBVETOModule(VETOSignal))
                        {
                            DistanceArray[ksig][kclus] = 1e99;
                        }
                        else
                        {
                            // Fill two vectors:
                            ClusterVect.SetX(ThisSignal->GetPositionX() - Target_X);
                            ClusterVect.SetX(ThisSignal->GetPositionY() - Target_Y);
                            ClusterVect.SetX(ThisSignal->GetPositionZ() - Target_Z);
                    
                            VETOVect.SetX(VETOSignal->GetPositionX() - Target_X);
                            VETOVect.SetX(VETOSignal->GetPositionY() - Target_Y);
                            VETOVect.SetX(VETOSignal->GetPositionZ() - Target_Z);
                    
                            // Rotate the cluster vector along z:
                            Theta = ClusterVect.Theta();
                            Phi = ClusterVect.Phi();
                    
                            VETOVect.RotateZ(-1.0*Phi);
                            VETOVect.RotateY(-1.0*Theta);
                    
                            ClusterVect.RotateZ(-1.0*Phi);
                            ClusterVect.RotateY(-1.0*Theta);
                    
                            // Compute Cluster Shifted Time:
                            Cluster_ShiftedTime = TheScorers->ComputeClusterScoreValue_TOF(ThisCluster);
                            Cluster_ShiftedTime = Cluster_ShiftedTime*VETOVect.Mag()/ClusterVect.Mag();
                    
                            // Compute VETO radius:
                            VETOSignal_Radius = TMath::Sqrt(VETOVect.X()*VETOVect.X() + VETOVect.Y()*VETOVect.Y());
                    
                            // Fill in the distance array:
                            DistanceArray[ksig][kclus] = TMath::Sqrt(VETOSignal_Radius*VETOSignal_Radius + ceff*ceff*(Cluster_ShiftedTime - VETOSignal->GetTime())*(Cluster_ShiftedTime - VETOSignal->GetTime()));
                    
                            // Done.
                        }
                    }
                }
            }
                    
            // Next, do the minimizations:            
            nMinimizations = nSignals_NEBVETO;
            if (nMinimizations>nClusters_NEBULA) {nMinimizations = nClusters_NEBULA;}
            
            for (Int_t time = 0; time<nMinimizations; ++time)
            {    
                // Reset:
                ThisMinimum = 1e98;
                ThisMinClusIndex = -1;
                ThisMinVETOINdex = -1;
            
                // Find the overall minimization (except the 1e99):
                for (Int_t ksig = 0; ksig<nSignals_NEBVETO; ++ksig)
                {
                    for (Int_t kclus = 0; kclus<nClusters_NEBULA; ++kclus)
                    {
                        if (DistanceArray[ksig][kclus]<ThisMinimum)
                        {
                            ThisMinimum = DistanceArray[ksig][kclus];
                            ThisMinClusIndex = kclus;
                            ThisMinVETOINdex = ksig;
                        }
                    }
                }
            
                // Then, if we find a minimum, then mark this cluster:
                if ((ThisMinClusIndex>=0)&&(ThisMinVETOINdex>=0))
                {
                    // Apply TOF condition too:
                    VETOSignal = (R3BSignal*) fArrayNEBULAVETOSignals->At(ThisMinVETOINdex);
                    ThisCluster = (R3BSignalCluster*) fArrayNEBULAClusters->At(ThisMinClusIndex);
                
                    if (VETOSignal->GetTime()<TheScorers->ComputeClusterScoreValue_TOF(ThisCluster))
                    {
                        // Then, mark the cluster for VETOing:
                        NEBULA_Cluster_IsLinked.at(ThisMinClusIndex) = kTRUE;
                    }
                    else
                    {
                        // Then, we keep the cluster even though it is a minimum. 
                        // Then, this VETO hit is due to backscattering.
                    }
                
                    // Then, eliminate this link from the search:
                    for (Int_t ksig = 0; ksig<nSignals_NEBVETO; ++ksig)
                    {
                        DistanceArray[ksig][ThisMinClusIndex] = 1e99;
                    }
                
                    for (Int_t kclus = 0; kclus<nClusters_NEBULA; ++kclus)
                    {
                        DistanceArray[ThisMinVETOINdex][kclus] = 1e99;
                    }
                
                    // Done.
                }
            }
        
            // delete the distance array:
            delete DistanceArray;
        }
    
        // Loop over the clusters from NEBULA to actually create the primary neutron hits:
        for (Int_t kclus = 0; kclus<nClusters_NEBULA; ++kclus)
        {
            // Retrieve the current cluster:
            ThisCluster = (R3BSignalCluster*) fArrayNEBULAClusters->At(kclus);
        
            // Extract the beta of this cluster:
            ThisBeta = TheScorers->ComputeClusterScoreValue_Beta(ThisCluster);
        
            // Extract cluster head:
            ThisSignal = TheScorers->GetPrimaryClusterHit(ThisCluster);
        
            // Check that it is in the allowed window:
            if ((ThisBeta>Beta_LowerCut)&&(ThisBeta<Beta_UpperCut))
            {
                // check if there is a VETO:
                if (UseNEBULAVETO==kFALSE)
                {
                    // Then it is easy. Create the primary neutron:
                    new ((*fArrayPrimHits_NEBULA)[PrimHitIndex_NEBULA]) R3BSignal(ThisSignal);
                    PrimHitIndex_NEBULA = PrimHitIndex_NEBULA + 1;
                }
                else
                {
                    // Then, we must implement the VETO condition:
                    if (NEBULA_VETOCondition=="None")
                    {
                        // Then, ignore the VETO:
                        new ((*fArrayPrimHits_NEBULA)[PrimHitIndex_NEBULA]) R3BSignal(ThisSignal);
                        PrimHitIndex_NEBULA = PrimHitIndex_NEBULA + 1;
                    }
                    else if (NEBULA_VETOCondition=="Naive")
                    {
                        // Then, only create primary neutrons if the VETO of this module does not contain any signals:
                        nSignals_VETO_ThisModule = 0;
                        
                        for (Int_t ksig = 0; ksig<nSignals_NEBVETO; ++ksig)
                        {   
                            VETOSignal = (R3BSignal*) fArrayNEBULAVETOSignals->At(ksig);
                            if (GetNEBVETOModule(VETOSignal)==GetNEBULAModule(ThisCluster))
                            {
                                nSignals_VETO_ThisModule = nSignals_VETO_ThisModule + 1;
                            }
                        }
                        
                        if (nSignals_VETO_ThisModule==0)
                        {
                            new ((*fArrayPrimHits_NEBULA)[PrimHitIndex_NEBULA]) R3BSignal(ThisSignal);
                            PrimHitIndex_NEBULA = PrimHitIndex_NEBULA + 1;
                        }
                    }
                    else if (NEBULA_VETOCondition=="TOF")
                    {
                        // Then, we only VETO everything if the VETO TOF < NEBULA TOF for that module:
                        ThisModule = GetNEBULAModule(ThisCluster);
                        nSignals_VETO_ThisModule = 0;
                        
                        for (Int_t ksig = 0; ksig<nSignals_NEBVETO; ++ksig)
                        {   
                            VETOSignal = (R3BSignal*) fArrayNEBULAVETOSignals->At(ksig);
                            if (GetNEBVETOModule(VETOSignal)==GetNEBULAModule(ThisCluster))
                            {
                                nSignals_VETO_ThisModule = nSignals_VETO_ThisModule + 1;
                            }
                        }
                        
                        if ((nSignals_VETO_ThisModule==0)||(NEBVETO_TOF[ThisModule]<NEBULA_TOF[ThisModule]))
                        {
                            new ((*fArrayPrimHits_NEBULA)[PrimHitIndex_NEBULA]) R3BSignal(ThisSignal);
                            PrimHitIndex_NEBULA = PrimHitIndex_NEBULA + 1;
                        }
                    }
                    else if (NEBULA_VETOCondition=="Advanced")
                    {
                        // This is easy here, because it is programmed already:
                        if (NEBULA_Cluster_IsLinked.at(kclus)==kFALSE)
                        {
                            // then it is not VETOed:
                            new ((*fArrayPrimHits_NEBULA)[PrimHitIndex_NEBULA]) R3BSignal(ThisSignal);
                            PrimHitIndex_NEBULA = PrimHitIndex_NEBULA + 1;
                        }
                    }
                    else
                    {
                        cout << "### R3BBetaReconstructor ERROR: The NEBULA VETO condition <" << NEBULA_VETOCondition << "> was not programmed!\n";
                    }
                }
            }
        }
    }
    
    // ==========================================================================================================
    
    // NOTE: Deal with the combined situation:
    
    for (Int_t k = 0; k<PrimHitIndex_NeuLAND; ++k)
    {
        ThisSignal = (R3BSignal*) fArrayPrimHits_NeuLAND->At(k);
        new ((*fArrayPrimHits_Combined)[PrimHitIndex_Combined]) R3BSignal(ThisSignal);
        PrimHitIndex_Combined = PrimHitIndex_Combined + 1;
    }
    
    for (Int_t k = 0; k<PrimHitIndex_NEBULA; ++k)
    {
        ThisSignal = (R3BSignal*) fArrayPrimHits_NEBULA->At(k);
        new ((*fArrayPrimHits_Combined)[PrimHitIndex_Combined]) R3BSignal(ThisSignal);
        PrimHitIndex_Combined = PrimHitIndex_Combined + 1;
    }
                
    // Log progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "R3BBetaReconstructor Log: We processed " << EventCounter << " events.\n";}
}

Int_t R3BBetaReconstructor::GetNEBULAModule(R3BSignalCluster* ThisCluster)
{
    R3BSignal* Head = TheScorers->GetPrimaryClusterHit(ThisCluster);
    Int_t CrystalIndex = Head->GetCrystalIndex();
    
    Int_t ThisModule = -1;
    while (CrystalIndex>0)
    {
        CrystalIndex = CrystalIndex - NEBULA_nBarsPerModules;
        ThisModule = ThisModule + 1;
    }
    
    return ThisModule;
}

Int_t R3BBetaReconstructor::GetNEBVETOModule(R3BSignal* ThisSignal)
{
    Int_t CrystalIndex = ThisSignal->GetCrystalIndex();
    
    Int_t ThisModule = -1;
    while (CrystalIndex>0)
    {
        CrystalIndex = CrystalIndex - NEBULA_nVETOBarsPerModule;
        ThisModule = ThisModule + 1;
    }
    
    return ThisModule;
}
        
// FairTask Finish function:
void R3BBetaReconstructor::Finish()
{
    // Select proper output file:
    TheOutputFile->cd();
}

// Generate ROOT dictionary:
ClassImp(R3BBetaReconstructor);
