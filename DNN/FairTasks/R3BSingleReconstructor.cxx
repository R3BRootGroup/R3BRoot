// Every CXX-file includes its own header file:
#include "R3BSingleReconstructor.h"

// Default constructor definition:
R3BSingleReconstructor::R3BSingleReconstructor() : FairTask("R3BSingleReconstructor")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create arrays:
    fArrayNeuLANDSignals = new TClonesArray("R3BSignal");
    fArrayNEBULASignals = new TClonesArray("R3BSignal");
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
    
    // Other parameters:
    EventCounter = 0;
    nEvents = 1;
    
    // Auxillary classes:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
}

// Destructor definition:
R3BSingleReconstructor::~R3BSingleReconstructor()
{
    if (fArrayNeuLANDSignals) {fArrayNeuLANDSignals->Clear(); delete fArrayNeuLANDSignals;}
    if (fArrayNEBULASignals) {fArrayNEBULASignals->Clear(); delete fArrayNEBULASignals;}
    if (fArrayPrimHits_NeuLAND) {fArrayPrimHits_NeuLAND->Clear(); delete fArrayPrimHits_NeuLAND;}
    if (fArrayPrimHits_NEBULA) {fArrayPrimHits_NEBULA->Clear(); delete fArrayPrimHits_NEBULA;}
    if (fArrayPrimHits_Combined) {fArrayPrimHits_Combined->Clear(); delete fArrayPrimHits_Combined;}
    if (fArrayNeuLANDVETOSignals) {fArrayNeuLANDVETOSignals->Clear(); delete fArrayNeuLANDVETOSignals;}
    if (fArrayNEBULAVETOSignals) {fArrayNEBULAVETOSignals->Clear(); delete fArrayNEBULAVETOSignals;}
    
    // Delete other objects:
    delete TheNuclei;
}

// FairTask initialization function:
InitStatus R3BSingleReconstructor::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BSingleReconstructor::Init: No FairRootManager\n\n";
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

    // Retrieve the clusters:
    if ((TClonesArray*)ioman->GetObject("Signals") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BSingleReconstructor::Init No R3BSignalClusters!\n\n";
        return kFATAL;
    }
    fArrayNeuLANDSignals = (TClonesArray*)ioman->GetObject("Signals");
    
    if (UseNEBULA==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("NEBULASignals") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BSingleReconstructor::Init No NEBULA R3BSignalClusters!\n\n";
            return kFATAL;
        }
        fArrayNEBULASignals = (TClonesArray*)ioman->GetObject("NEBULASignals");
    }
    
    // Obtain VETO signals:
    if (UseVETO==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("VETOSignals") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BSingleReconstructor::Init No VETOSignals!\n\n";
            return kFATAL;
        }
        fArrayNeuLANDVETOSignals = (TClonesArray*)ioman->GetObject("VETOSignals");
    }
    
    if (UseNEBULAVETO==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("NEBULAVETOSignals") == nullptr)
        {
            cout << "I/O-manager FATAL: R3BSingleReconstructor::Init No NEBULAVETOSignals!\n\n";
            return kFATAL;
        }
        fArrayNeuLANDVETOSignals = (TClonesArray*)ioman->GetObject("NEBULAVETOSignals");
    }
    
    // Register Output arrays:
    ioman->Register("SingleReconstructed_PrimHits_NeuLAND","R3BSignal",fArrayPrimHits_NeuLAND,kTRUE);
    
    if (UseNEBULA==kTRUE)
    {
        ioman->Register("SingleReconstructed_PrimHits_NEBULA","R3BSignal",fArrayPrimHits_NEBULA,kTRUE);
        ioman->Register("SingleReconstructed_PrimHits_Combined","R3BSignal",fArrayPrimHits_Combined,kTRUE);
    }
    
    // Then, we can return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BSingleReconstructor::Exec(Option_t *option)
{
    // Clean output arrays. Inputs will be handled automatically.
    fArrayPrimHits_NeuLAND->Clear("C");
    fArrayPrimHits_NEBULA->Clear("C");
    fArrayPrimHits_Combined->Clear("C");
    
    // Next, find the minimum signal for each of the detectors:
    R3BSignal* NeuLANDMinSignal = new R3BSignal(); NeuLANDMinSignal->SetTime(1e99);
    R3BSignal* VETOMinSignal = new R3BSignal(); VETOMinSignal->SetTime(1e99);
    R3BSignal* NEBULAMinSignal = new R3BSignal(); NEBULAMinSignal->SetTime(1e99);
    R3BSignal* NEBVETOMinSignal = new R3BSignal(); NEBVETOMinSignal->SetTime(1e99);
    R3BSignal* ThisSignal;

    for (Int_t k = 0; k<fArrayNeuLANDSignals->GetEntries(); ++k)
    {
        ThisSignal = (R3BSignal*) fArrayNeuLANDSignals->At(k);
        
        if (ThisSignal->GetTime()<NeuLANDMinSignal->GetTime())
        {
            NeuLANDMinSignal->Duplicate(ThisSignal);
        }
    }
    
    if (UseVETO==kTRUE)
    {
        for (Int_t k = 0; k<fArrayNeuLANDVETOSignals->GetEntries(); ++k)
        {
            ThisSignal = (R3BSignal*) fArrayNeuLANDVETOSignals->At(k);
        
            if (ThisSignal->GetTime()<VETOMinSignal->GetTime())
            {
                VETOMinSignal->Duplicate(ThisSignal);
            }
        }
    }
    
    if (UseNEBULA==kTRUE)
    {
        for (Int_t k = 0; k<fArrayNEBULASignals->GetEntries(); ++k)
        {
            ThisSignal = (R3BSignal*) fArrayNEBULASignals->At(k);
        
            if (ThisSignal->GetTime()<NEBULAMinSignal->GetTime())
            {
                NEBULAMinSignal->Duplicate(ThisSignal);
            }
        }
    }
    
    if (UseNEBULAVETO==kTRUE)
    {
        for (Int_t k = 0; k<fArrayNEBULAVETOSignals->GetEntries(); ++k)
        {
            ThisSignal = (R3BSignal*) fArrayNEBULAVETOSignals->At(k);
        
            if (ThisSignal->GetTime()<NEBVETOMinSignal->GetTime())
            {
                NEBVETOMinSignal->Duplicate(ThisSignal);
            }
        }
    }
    
    // Then, do the reconstruction. NeuLAND first:
    if ((UseVETO==kFALSE)||((UseVETO==kTRUE)&&(NeuLAND_VETOCondition=="None")))
    {
        if (fArrayNeuLANDSignals->GetEntries()>0) {new ((*fArrayPrimHits_NeuLAND)[0]) R3BSignal(NeuLANDMinSignal);}
    }
    
    if ((UseVETO==kTRUE)&&((NeuLAND_VETOCondition=="TOF")||(NeuLAND_VETOCondition=="Advanced")))
    {
        if ((fArrayNeuLANDVETOSignals->GetEntries()==0)||(NeuLANDMinSignal->GetTime()<VETOMinSignal->GetTime()))
        {
            if (fArrayNeuLANDSignals->GetEntries()>0) {new ((*fArrayPrimHits_NeuLAND)[0]) R3BSignal(NeuLANDMinSignal);}
        }
    }
    
    if ((UseVETO==kTRUE)&&(NeuLAND_VETOCondition=="Naive"))
    {
        if (fArrayNeuLANDVETOSignals->GetEntries()==0)
        {
            if (fArrayNeuLANDSignals->GetEntries()>0) {new ((*fArrayPrimHits_NeuLAND)[0]) R3BSignal(NeuLANDMinSignal);}
        }
    }
    
    // Then, NEBULA:
    if (UseNEBULA==kTRUE)
    {
        if ((UseNEBULAVETO==kFALSE)||((UseNEBULAVETO==kTRUE)&&(NEBULA_VETOCondition=="None")))
        {
            if (fArrayNEBULASignals->GetEntries()>0) {new ((*fArrayPrimHits_NEBULA)[0]) R3BSignal(NEBULAMinSignal);}
        }
        
        if ((UseNEBULAVETO==kTRUE)&&((NEBULA_VETOCondition=="TOF")||(NEBULA_VETOCondition=="Advanced")))
        {
            if ((fArrayNEBULAVETOSignals->GetEntries()==0)||(NEBULAMinSignal->GetTime()<NEBVETOMinSignal->GetTime()))
            {
                if (fArrayNEBULASignals->GetEntries()>0) {new ((*fArrayPrimHits_NEBULA)[0]) R3BSignal(NEBULAMinSignal);}
            }
        }
    
        if ((UseNEBULAVETO==kTRUE)&&(NEBULA_VETOCondition=="Naive"))
        {
            if (fArrayNEBULAVETOSignals->GetEntries()==0)
            {
                if (fArrayNEBULASignals->GetEntries()>0) {new ((*fArrayPrimHits_NEBULA)[0]) R3BSignal(NEBULAMinSignal);}
            }
        }
    }
    
    // And finally, the combination:
    if (UseNEBULA==kTRUE)
    {
        if ((fArrayPrimHits_NeuLAND->GetEntries()==1)&&(fArrayPrimHits_NEBULA->GetEntries()==0))
        {   
            ThisSignal = (R3BSignal*) fArrayPrimHits_NeuLAND->At(0);
            new ((*fArrayPrimHits_Combined)[0]) R3BSignal(ThisSignal);
        }
        
        if ((fArrayPrimHits_NeuLAND->GetEntries()==0)&&(fArrayPrimHits_NEBULA->GetEntries()==1))
        {   
            ThisSignal = (R3BSignal*) fArrayPrimHits_NEBULA->At(0);
            new ((*fArrayPrimHits_Combined)[0]) R3BSignal(ThisSignal);
        }
        
        if ((fArrayPrimHits_NeuLAND->GetEntries()==1)&&(fArrayPrimHits_NEBULA->GetEntries()==1))
        {   
            if ((((R3BSignal*) fArrayPrimHits_NeuLAND->At(0))->GetTime())<(((R3BSignal*) fArrayPrimHits_NEBULA->At(0))->GetTime()))
            {
                ThisSignal = (R3BSignal*) fArrayPrimHits_NeuLAND->At(0);
                new ((*fArrayPrimHits_Combined)[0]) R3BSignal(ThisSignal);
            }
            else
            {
                ThisSignal = (R3BSignal*) fArrayPrimHits_NEBULA->At(0);
                new ((*fArrayPrimHits_Combined)[0]) R3BSignal(ThisSignal);
            }
        }
    }
                
    // Log progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "R3BSingleReconstructor Log: We processed " << EventCounter << " events.\n";}
    
    // delete all objects:
    delete NeuLANDMinSignal;
    delete VETOMinSignal;
    delete NEBULAMinSignal;
    delete NEBVETOMinSignal;
}
        
// FairTask Finish function:
void R3BSingleReconstructor::Finish()
{
    // Select proper output file:
    TheOutputFile->cd();
}

// Generate ROOT dictionary:
ClassImp(R3BSingleReconstructor);
