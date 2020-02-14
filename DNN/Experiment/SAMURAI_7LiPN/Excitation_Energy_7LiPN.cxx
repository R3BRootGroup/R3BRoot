// Every CXX-file includes its own header file:
#include "Excitation_Energy_7LiPN.h"

// Default constructor definition:
Excitation_Energy_7LiPN::Excitation_Energy_7LiPN() : FairTask("Excitation_Energy_7LiPN")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create arrays:
    fArrayNeuLANDSignals = new TClonesArray("R3BSignal");
    fArrayNeuLANDClusters = new TClonesArray("R3BSignalCluster");
    fArrayNeuLANDVETOSignals = new TClonesArray("R3BSignal");
    fArrayNeutronTracks = new TClonesArray("TLorentzVector"); 
    fArrayEx = new TClonesArray("ExValue");
    fArrayEx_MultipleClusters = new TClonesArray("ExValue");
    fArrayMCNeutronTracks = new TClonesArray("TLorentzVector");
        
    // Other variables:
    UseVETO = kTRUE;
    NeuLAND_VETOCondition = "TOF";
    BranchTitle = "TheNeutronTracks";
    OutputPath = "./";
    EventCounter = 0;         
    nEvents = 1;
    Inputs = 0;
    AvgBeamEnergy = 250.0;
    SimulationData_IsAvailable = kFALSE;
    LimitToMC = kFALSE;
    Lower_EdepCut = 10.0;
    
    // Initialize Nuclear dataBase:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheOutputFile = 0;
    
    // Histograms:
    NScatterBins = 5;
    ScatterStep = 1.0;
    NExBins = 1000;
    ExHistograms = 0;
    ExHistograms_MultClus = 0;
}

// Destructor definition:
Excitation_Energy_7LiPN::~Excitation_Energy_7LiPN()
{
    // Delete the arrays:
    if (fArrayNeuLANDSignals) {fArrayNeuLANDSignals->Clear(); delete fArrayNeuLANDSignals;}
    if (fArrayNeuLANDClusters) {fArrayNeuLANDClusters->Clear(); delete fArrayNeuLANDClusters;}
    if (fArrayNeuLANDVETOSignals) {fArrayNeuLANDVETOSignals->Clear(); delete fArrayNeuLANDVETOSignals;}
    if (fArrayNeutronTracks) {fArrayNeutronTracks->Clear(); delete fArrayNeutronTracks;}
    if (fArrayEx) {fArrayEx->Clear(); delete fArrayEx;}
    if (fArrayEx_MultipleClusters) {fArrayEx_MultipleClusters->Clear(); delete fArrayEx_MultipleClusters;}
    if (fArrayMCNeutronTracks) {fArrayMCNeutronTracks->Clear(); delete fArrayMCNeutronTracks;}

    // Delete other objects:
    delete TheNuclei;
    delete ExHistograms;
    delete ExHistograms_MultClus;
}

// FairTask initialization function:
InitStatus Excitation_Energy_7LiPN::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: Excitation_Energy_7LiPN::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    TheOutputFile = ioman->GetOutFile();
    
    // Check the inputs class:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: Excitation_Energy_7LiPN: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Obtain all required inputs:
    OutputPath = Inputs->GetInputString("TheOutputPath");
    UseVETO = Inputs->GetInputBoolian("VETO_Include_in_SETUP");
    NeuLAND_VETOCondition = Inputs->GetInputString("BetaReconstruction_NeuLAND_Select_VETOCondition");
    SimulationData_IsAvailable = Inputs->GetInputBoolian("NeuLAND_TranslateToSignals_Mark_TruePrimarySignals");
    AvgBeamEnergy = Inputs->GetAvgBeamEnergyPerNucleon();
    Lower_EdepCut = Inputs->GetInputDouble("NeuLAND_Lower_EdepCut_Ex_Spectra","MeV");
    
    // Adapt VETO condition. Advanced is pointless and should be TOF:
    if (NeuLAND_VETOCondition=="Advanced") {NeuLAND_VETOCondition = "TOF";}
    
    // Obtain the reconstructed Neutron Tracks:
    if ((TClonesArray*)ioman->GetObject(BranchTitle.Data()) == nullptr)
    {
        cout << "I/O-manager FATAL: Excitation_Energy_7LiPN::Init() ==> Could not locate neutron tracks <" + BranchTitle + ">!\n\n";
        return kFATAL;
    }
    fArrayNeutronTracks = (TClonesArray*) ioman->GetObject(BranchTitle.Data());
    
    // Obtain NeuLAND signals:
    if ((TClonesArray*)ioman->GetObject("Signals") == nullptr)
    {
        cout << "I/O-manager FATAL: Excitation_Energy_7LiPN::Init No Signals!\n\n";
        return kFATAL;
    }
    fArrayNeuLANDSignals = (TClonesArray*)ioman->GetObject("Signals");
    
    // Also retrieve the clusters:
    if ((TClonesArray*)ioman->GetObject("Clusters") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BMultReader::Init No R3BSignalClusters!\n\n";
        return kFATAL;
    }
    fArrayNeuLANDClusters = (TClonesArray*)ioman->GetObject("Clusters");
    
    if (UseVETO==kTRUE)
    {
        if ((TClonesArray*)ioman->GetObject("VETOSignals") == nullptr)
        {
            cout << "I/O-manager FATAL: Excitation_Energy_7LiPN::Init No VETOSignals!\n\n";
            return kFATAL;
        }
        fArrayNeuLANDVETOSignals = (TClonesArray*)ioman->GetObject("VETOSignals");
    }
    
    // Obtain the MC neutron tracks if they exist:
    if (SimulationData_IsAvailable==kTRUE)
    {
        // Retrieve the MC neutron tracks:
        if ((TClonesArray*)ioman->GetObject("MCNeutronTracks") == nullptr)
        {
            cout << "I/O-manager FATAL: Excitation_Energy_7LiPN::Init No MCNeutronTracks!\n\n";
            return kFATAL;
        }
        fArrayMCNeutronTracks = (TClonesArray*)ioman->GetObject("MCNeutronTracks");
    }
    
    // Register the output:
    ioman->Register(BranchTitle+"_ExcitationEnergy","ExValue",fArrayEx,kTRUE);
    ioman->Register(BranchTitle+"_Ex_MultipleClusters","ExValue",fArrayEx_MultipleClusters,kTRUE);
    
    // Create histograms:
    if (TMath::Abs(AvgBeamEnergy-252.7)<10.0) {NExBins = 500;}
    if (TMath::Abs(AvgBeamEnergy-109.1)<10.0) {NExBins = 300;}
    ExHistograms = new TH1D*[NScatterBins+1];
    ExHistograms_MultClus = new TH1D*[NScatterBins+1];
    TString st = "";
    TString kstr = "";
    
    for (Int_t k = 0; k<(NScatterBins+1); ++k)
    {
        kstr = st.Itoa(k,10);
        ExHistograms[k] = new TH1D(BranchTitle+"ExHistogram_"+kstr,BranchTitle+"ExHistogram_"+kstr,NExBins,-1.0*AvgBeamEnergy,1.0*AvgBeamEnergy);
        ExHistograms_MultClus[k] = new TH1D(BranchTitle+"ExHistogram_MultClus"+kstr,BranchTitle+"ExHistogram_MultClus_"+kstr,NExBins,-1.0*AvgBeamEnergy,1.0*AvgBeamEnergy);
    }
    
    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    
    return kSUCCESS;
}

// FairTask Execute function:
void Excitation_Energy_7LiPN::Exec(Option_t *option)
{
    // Reset the output array:
    fArrayEx->Clear("C");
    
    // Begin by testing if the multiplicity is 1.
    // Otherwise, the data is unsuited for 7Li(p,n)7Be charge-exchange reaction.
    Int_t Multiplicity = fArrayNeutronTracks->GetEntries();
    Int_t CreationIndex = 0;
    
    Int_t TrueMultiplicity = -1;
    if (SimulationData_IsAvailable==kTRUE) {TrueMultiplicity = fArrayMCNeutronTracks->GetEntries();}
    
    // Do the multiplicity test:
    Bool_t MultiplicityTest = kTRUE;
    
    if ((LimitToMC==kFALSE)||((LimitToMC==kTRUE)&&(SimulationData_IsAvailable==kFALSE)))
    {
        if (Multiplicity==1) {MultiplicityTest = kTRUE;}
        else {MultiplicityTest = kFALSE;}
    }
    
    if ((SimulationData_IsAvailable==kTRUE)&&(LimitToMC==kTRUE))
    {
        if ((Multiplicity==1)&&(TrueMultiplicity==1)) {MultiplicityTest = kTRUE;}
        else {MultiplicityTest = kFALSE;}
    }
    
    // Also obtain the number of clusters:
    Int_t nClusters = fArrayNeuLANDClusters->GetEntries();
    Double_t Edep = 0.0;
    
    // Also obtain TOf for VETO condition (advanced is meaningless here, since multiplicity sould be 1):
    Double_t NeuLAND_TOF = 1e99;
    R3BSignal* ThisSignal;
    Int_t nSignals = fArrayNeuLANDSignals->GetEntries();
    for (Int_t k = 0; k<nSignals; ++k)
    {
        ThisSignal = (R3BSignal*) fArrayNeuLANDSignals->At(k);
        Edep = Edep + ThisSignal->GetEnergy();
        if (ThisSignal->GetTime()<NeuLAND_TOF) {NeuLAND_TOF = ThisSignal->GetTime();}
    }
    
    // And, obtain the VETO TOF:
    Double_t VETO_TOF = 1e99;
    Int_t nSignals_VETO = 0;
    if (UseVETO==kTRUE)
    {
        nSignals_VETO = fArrayNeuLANDVETOSignals->GetEntries();
        for (Int_t k = 0; k<nSignals_VETO; ++k)
        {
            ThisSignal = (R3BSignal*) fArrayNeuLANDVETOSignals->At(k);
            if (ThisSignal->GetTime()<VETO_TOF) {VETO_TOF = ThisSignal->GetTime();}
        }
    }
    
    // Define VETO condition:
    Bool_t VETO_Event = kFALSE;
    
    if (NeuLAND_VETOCondition=="Naive")
    {
        if (nSignals_VETO>0) {VETO_Event = kTRUE;}
    }
    else if (NeuLAND_VETOCondition=="TOF")
    {
        if ((nSignals_VETO>0)&&(nSignals>0))
        {
            if (VETO_TOF<NeuLAND_TOF)
            {
                VETO_Event = kTRUE;
            }
        }
    }
    else if (NeuLAND_VETOCondition=="None")
    {
        VETO_Event = kFALSE;
    }
    else
    {
        VETO_Event = kFALSE;
        cout << "Excitation_Energy_7LiPN ERROR: The VETO condition <" << NeuLAND_VETOCondition << "> was not supported!\n";
    }
    
    // NOTE: Do not use VETO condition for the perfect reconstruction, so that
    // when we gauche the peaks against this, we take the VETO effects along properly:
    if (BranchTitle=="NeutronTracks_PerfectMethod_Signals") {VETO_Event = kFALSE;}
        
    // Now, decide whether we can use the event.
    // NOTE: Edep cut: there is no reasonable argument that this is background.
    // if it comes in the primary neutron peak, but is a low-energy particle,
    // then it must be produced BEFORE the target reaction. Hence, it is instrumental
    // background, which is already subtracted. Besides, we also use the VETO on top of that.
    if ((MultiplicityTest==kTRUE)&&(VETO_Event==kFALSE))
    {
        // Then, we can move on. Extract the neutron track:
        TLorentzVector* NeutronTrack = (TLorentzVector*) fArrayNeutronTracks->At(0);
        Double_t ENeutronTotal = NeutronTrack->T();
        Double_t ENeutron = ENeutronTotal - TheNuclei->GetMass("neutron","MeV");
        Double_t GammaNeutron = ENeutronTotal/TheNuclei->GetMass("neutron","MeV");
        Double_t BetaNeutron = TMath::Sqrt(1.0 - 1.0/(GammaNeutron*GammaNeutron));
        Double_t Alpha_LAB = NeutronTrack->Theta()*(180.0/TMath::Pi());
        
        // Then, we can now extract the quantities that we want:
        Double_t Ex = GetEx(NeutronTrack,1);
        Double_t Alpha = GetEx(NeutronTrack,2);
        Double_t Phi = GetEx(NeutronTrack,3);
        
        // Compute first Energy Derivative too:
        TLorentzVector* NeutronPlus = new TLorentzVector();
        NeutronPlus->SetT(NeutronTrack->T());
        NeutronPlus->SetX(NeutronTrack->X());
        NeutronPlus->SetY(NeutronTrack->Y());
        NeutronPlus->SetZ(NeutronTrack->Z());
        
        TLorentzVector* NeutronMin = new TLorentzVector();
        NeutronMin->SetT(NeutronTrack->T());
        NeutronMin->SetX(NeutronTrack->X());
        NeutronMin->SetY(NeutronTrack->Y());
        NeutronMin->SetZ(NeutronTrack->Z());
        
        AdjustNeutronTrack(NeutronPlus,ENeutron*1.001);
        AdjustNeutronTrack(NeutronMin,ENeutron*0.999);
        
        Double_t Energy_Derivative = (GetEx(NeutronPlus,1) - GetEx(NeutronMin,1))/(0.002*ENeutron);
        
        // Write values to the output:
        new ((*fArrayEx)[CreationIndex]) ExValue(ENeutron,Ex,Alpha,Alpha_LAB,Phi,Energy_Derivative,Edep,nClusters);
        if (nClusters>1) {new ((*fArrayEx_MultipleClusters)[CreationIndex]) ExValue(ENeutron,Ex,Alpha,Alpha_LAB,Phi,Energy_Derivative,Edep,nClusters);}
        CreationIndex = CreationIndex + 1;
        
        // And fill them in in the histograms:
        Double_t TempAlpha = Alpha;
        Int_t CurrentScatterBin = 0;
        
        while (TempAlpha>0.0)
        {
            CurrentScatterBin = CurrentScatterBin + 1;
            TempAlpha = TempAlpha - ScatterStep;
        }
        
        ExHistograms[0]->Fill(Ex);
        if (CurrentScatterBin<(NScatterBins+1)) {ExHistograms[CurrentScatterBin]->Fill(Ex);}
        
        if (nClusters>1)
        {
            ExHistograms_MultClus[0]->Fill(Ex);
            if (CurrentScatterBin<(NScatterBins+1)) {ExHistograms_MultClus[CurrentScatterBin]->Fill(Ex);}
        }
        
        // Done. Now delete everything:
        delete NeutronPlus;
        delete NeutronMin;
    }
    
    // Log progress:
    EventCounter = EventCounter + 1;
    if (EventCounter%1000==0) {cout << "Excitation_Energy_7LiPN Log: We processed " << EventCounter << " events.\n";}
}

Double_t Excitation_Energy_7LiPN::GetEx(TLorentzVector* const Track, Int_t const Option)
{
    // Extracts excitation-energy from the neutron vector:
    TLorentzVector* NeutronTrack = new TLorentzVector();
    NeutronTrack->SetT(Track->T());
    NeutronTrack->SetX(Track->X());
    NeutronTrack->SetY(Track->Y());
    NeutronTrack->SetZ(Track->Z());
    
    //TODO: Use beam vector from detectors instead of hard-code.
        
    // Use normal kinematics:
    Double_t Ekin_proton = 0.0;
    if (TMath::Abs(AvgBeamEnergy-252.7)<10.0) {Ekin_proton = 252.7;}
    if (TMath::Abs(AvgBeamEnergy-109.1)<10.0) {Ekin_proton = 109.1;}
    Double_t p0_proton = Ekin_proton + TheNuclei->GetMass("proton","MeV");
    Double_t pz_proton = TMath::Sqrt(p0_proton*p0_proton - TheNuclei->GetMass("proton","MeV")*TheNuclei->GetMass("proton","MeV"));
    TLorentzVector* ProtonBeam = new TLorentzVector();
    ProtonBeam->SetT(p0_proton);
    ProtonBeam->SetX(0.0);
    ProtonBeam->SetY(0.0);
    ProtonBeam->SetZ(pz_proton);
    
    TLorentzVector* LiTarget = new TLorentzVector();
    LiTarget->SetT(TheNuclei->GetMass("Li7","MeV"));
    LiTarget->SetX(0.0);
    LiTarget->SetY(0.0);
    LiTarget->SetZ(0.0);
      
    // Compute the recoil-track:
    TLorentzVector* RecoilTrack = new TLorentzVector();
    RecoilTrack->SetT(ProtonBeam->T() + LiTarget->T() - NeutronTrack->T());
    RecoilTrack->SetX(ProtonBeam->X() + LiTarget->X() - NeutronTrack->X());
    RecoilTrack->SetY(ProtonBeam->Y() + LiTarget->Y() - NeutronTrack->Y());
    RecoilTrack->SetZ(ProtonBeam->Z() + LiTarget->Z() - NeutronTrack->Z());
    
    // Next, boost to the CM-frame:
    TLorentzVector* CMTrack = new TLorentzVector();
    CMTrack->SetT(ProtonBeam->T() + LiTarget->T());
    CMTrack->SetX(ProtonBeam->X() + LiTarget->X());
    CMTrack->SetY(ProtonBeam->Y() + LiTarget->Y());
    CMTrack->SetZ(ProtonBeam->Z() + LiTarget->Z());
    
    // Next, extract the vector of relativistic betas:
    Double_t CM_Mass = CMTrack->M();
    Double_t CM_gamma = CMTrack->T()/CM_Mass;
    Double_t c_Light = 29.9792458;
    Double_t CM_beta_x = CMTrack->X()/(CM_Mass*CM_gamma); //*c_Light);
    Double_t CM_beta_y = CMTrack->Y()/(CM_Mass*CM_gamma); //*c_Light);
    Double_t CM_beta_z = CMTrack->Z()/(CM_Mass*CM_gamma); //*c_Light);
    
    // Duplicate the CM-track to test the boost:
    TLorentzVector* CMTrack_Boosted = new TLorentzVector();
    CMTrack_Boosted->SetT(CMTrack->T());
    CMTrack_Boosted->SetX(CMTrack->X());
    CMTrack_Boosted->SetY(CMTrack->Y());
    CMTrack_Boosted->SetZ(CMTrack->Z());
   
    // Execute the boost:
    ProtonBeam->Boost(-1.0*CM_beta_x,-1.0*CM_beta_y,-1.0*CM_beta_z);
    LiTarget->Boost(-1.0*CM_beta_x,-1.0*CM_beta_y,-1.0*CM_beta_z);
    NeutronTrack->Boost(-1.0*CM_beta_x,-1.0*CM_beta_y,-1.0*CM_beta_z);
    RecoilTrack->Boost(-1.0*CM_beta_x,-1.0*CM_beta_y,-1.0*CM_beta_z);
    CMTrack_Boosted->Boost(-1.0*CM_beta_x,-1.0*CM_beta_y,-1.0*CM_beta_z);
    
    // Check that everything worked:
    if (TMath::Abs(CMTrack_Boosted->T()-CM_Mass)>1e-3) {cout << "### ERROR: Boost to CM-frame failed in T-direction! Before = " << CMTrack->T() << " && After = " << CMTrack_Boosted->T() << " && Mass = " << CM_Mass << "\n";}
    if (TMath::Abs(CMTrack_Boosted->X())>1e-3)     {cout << "### ERROR: Boost to CM-frame failed in X-direction! Before = " << CMTrack->X() << " && After = " << CMTrack_Boosted->X() << "\n";}
    if (TMath::Abs(CMTrack_Boosted->Y())>1e-3)     {cout << "### ERROR: Boost to CM-frame failed in Y-direction! Before = " << CMTrack->Y() << " && After = " << CMTrack_Boosted->Y() << "\n";}
    if (TMath::Abs(CMTrack_Boosted->Z())>1e-3)     {cout << "### ERROR: Boost to CM-frame failed in Z-direction! Before = " << CMTrack->Z() << " && After = " << CMTrack_Boosted->Z() << "\n";}
    
    // Extract excitation-energy:
    Double_t Ex = RecoilTrack->M() - TheNuclei->GetMass("Be7","MeV");
    
    // Extract scattering angles too:
    Double_t Alpha = NeutronTrack->Theta()*180.0/TMath::Pi();
    Double_t Phi = NeutronTrack->Phi()*180.0/TMath::Pi();
    
    // Delete all tracks:
    delete LiTarget;
    delete ProtonBeam;
    delete RecoilTrack;
    delete CMTrack;
    delete CMTrack_Boosted;
    delete NeutronTrack;
    
    // Return Answer:
         if (Option==1) {return Ex;}
    else if (Option==2) {return Alpha;} 
    else if (Option==3) {return Phi;}
    else {return -1000.0;}
}

void Excitation_Energy_7LiPN::AdjustNeutronTrack(TLorentzVector* Track, Double_t NewEkin)
{
    Double_t Gamma = Track->T()/Track->M();
    Double_t BetaTotal = TMath::Sqrt(1.0 - 1.0/(Gamma*Gamma));
    Double_t Beta_x = Track->X()/(Track->M()*Gamma);
    Double_t Beta_y = Track->Y()/(Track->M()*Gamma);
    Double_t Beta_z = Track->Z()/(Track->M()*Gamma);
    
    // Determine direction:
    Beta_x = Beta_x/BetaTotal;
    Beta_y = Beta_y/BetaTotal;
    Beta_z = Beta_z/BetaTotal;
    
    // Now, we do the new job:
    Double_t GammaNew = (Track->M()+NewEkin)/Track->M();
    Double_t BetaNew = TMath::Sqrt(1.0 - 1.0/(GammaNew*GammaNew));
    Beta_x = Beta_x*BetaNew*Track->M()*GammaNew;
    Beta_y = Beta_y*BetaNew*Track->M()*GammaNew;
    Beta_z = Beta_z*BetaNew*Track->M()*GammaNew;
    Double_t Tnew = GammaNew*Track->M();
    
    Track->SetT(Tnew);
    Track->SetX(Beta_x);
    Track->SetY(Beta_y);
    Track->SetZ(Beta_z);
}

// Definition of Finish-function:
void Excitation_Energy_7LiPN::Finish()
{
    // Select OutputFile:
    TheOutputFile->cd();
    
    // write histograms:
    TString st = "";
    TString kstr = "";
    
    for (Int_t k = 0; k<(NScatterBins+1); ++k)
    {
        kstr = st.Itoa(k,10);
        if (k==0) {ExHistograms[k]->Write(BranchTitle+"_Excitation_Energy_Total",2);}
        else {ExHistograms[k]->Write(BranchTitle+"_Excitation_Energy_Bin_"+kstr,2);}
        if (k==0) {ExHistograms_MultClus[k]->Write(BranchTitle+"_Excitation_Energy_MultClus_Total",2);}
        else {ExHistograms_MultClus[k]->Write(BranchTitle+"_Excitation_Energy_MultClus_Bin_"+kstr,2);}
    }
}

// Generate ROOT dictionary:
ClassImp(Excitation_Energy_7LiPN);
