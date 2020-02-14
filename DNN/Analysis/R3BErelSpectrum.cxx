// Every CXX-file includes its own header file:
#include "R3BErelSpectrum.h"

// Default constructor definition:
R3BErelSpectrum::R3BErelSpectrum() : FairTask("R3BErelSpectrum")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create arrays:
    fArrayMCNeutronTracks = new TClonesArray("TLorentzVector");
    fArrayMCRecoilTracks = new TClonesArray("TLorentzVector");
    fArraySignals = new TClonesArray("R3BSignal");
    fArrayLorentzTracks = new TClonesArray("TLorentzVector");
  
    // Control variables:
    EventCounter = 0;
    nEvents = 1;
    NbinsSpectrum = 100;
    EmaxSpectrum = 1.0;
    BranchName = "TheNeutronTracks";
    
    // Input parameters:
    GunType = "NewlyGenerated";
    FileName = "./Output.root";
    BeamType = "neutron";
    BeamEnergy = 600.0;
    BeamEnergyDiff = 1.0;
    ParticleType_ToBeDetected = "neutron";
    ParticleMass = -1.0;
    MaxMultiplicity = 0;
    AngularSpread = 1.0;
    OutputPath = "./";
    IncludeRecoil_ErelSpectrum = kFALSE;
    
    // The spectra:
    ErelSpectrum_GunMultiplicity = 0;
    ErelSpectrum_DetectedMultiplicity = 0;
    ErelSpectrum_ExpMultiplicity = 0;
    GunCounters = 0;
    GunCounters_Limited = 0;
    DetCounters = 0;
    DetCounters_Limited = 0;
    ExpCounters = 0;
    ExpCounters_Limited = 0;
    
    // Auxillary classes:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheOutputFile = 0;
}

// Destructor definition:
R3BErelSpectrum::~R3BErelSpectrum()
{
    // Delete the arrays:
    if(fArraySignals) {fArraySignals->Clear(); delete fArraySignals;}
    if(fArrayMCNeutronTracks) {fArrayMCNeutronTracks->Clear(); delete fArrayMCNeutronTracks;}
    if(fArrayMCRecoilTracks) {fArrayMCRecoilTracks->Clear(); delete fArrayMCRecoilTracks;}
    if(fArrayLorentzTracks) {fArrayLorentzTracks->Clear(); delete fArrayLorentzTracks;}
     
    // Delete other objects:
    delete ErelSpectrum_GunMultiplicity;
    delete ErelSpectrum_DetectedMultiplicity;
    delete ErelSpectrum_ExpMultiplicity;
}

// FairTask initialization function:
InitStatus R3BErelSpectrum::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BErelSpectrum::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    // NOTE: No outputfile is specified here. // TheOutputFile = ioman->GetOutFile();
    
    // Obtain inputs:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: The R3BInputClass is not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Retrieve the required inputs:
    GunType = Inputs->GetInputString("ParticleGun_ASCII_GunChoice");
    ParticleType_ToBeDetected = Inputs->GetInputString("NeuLAND_ParticleType_ToBeDetected");
    ParticleMass = TheNuclei->GetMass(ParticleType_ToBeDetected,"MeV");
    MaxMultiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
    FileName = Inputs->GetInputString("TheOutputPath") + Inputs->GetInputString("NeuLAND_Distributions");
    BeamEnergy = Inputs->GetAvgBeamEnergy();
    AngularSpread = Inputs->GetInputDouble("ParticleGun_AngularSpread","degree");
    BeamEnergyDiff = Inputs->GetBeamEnergyDiff();
    OutputPath = Inputs->GetInputString("TheOutputPath");
    IncludeRecoil_ErelSpectrum = Inputs->GetInputBoolian("IncludeRecoil_ErelSpectrum");
    
    // Compute the histogram range & binning:
    Double_t SpectrumRange = ((Int_t) MaxMultiplicity)*BeamEnergyDiff*6.0*(AngularSpread/4.0)*(AngularSpread/4.0);
    Double_t SpectRes = 0.7*TMath::Sqrt((Int_t) nEvents);
    Int_t SpectrumResolution = (Int_t) SpectRes;
    NbinsSpectrum = SpectrumResolution/2;
    EmaxSpectrum = SpectrumRange;
    if (GunType=="SnData_100keV") {EmaxSpectrum = 0.5; NbinsSpectrum = 100;}
    if (GunType=="SnData_500keV") {EmaxSpectrum = 1.5; NbinsSpectrum = 100;}
    
    // Obtain MC primary tracks:
    if ((TClonesArray*)ioman->GetObject("MCNeutronTracks") == nullptr)
    {
        fArrayMCNeutronTracks = 0;
    }
    else
    {
        fArrayMCNeutronTracks = (TClonesArray*)ioman->GetObject("MCNeutronTracks");
    }
    
    // Obtain the non-neutron primary tracks:
    if ((TClonesArray*)ioman->GetObject("MCRecoilTracks") == nullptr)
    {
        fArrayMCRecoilTracks = 0;
    }
    else
    {
        fArrayMCRecoilTracks = (TClonesArray*)ioman->GetObject("MCRecoilTracks");
    }
    
    // Obtain R3BSignals:
    if ((TClonesArray*)ioman->GetObject("Signals") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BErelSpectrum::Init No R3BSignals!\n\n";
        return kFATAL;
    }
    fArraySignals = (TClonesArray*)ioman->GetObject("Signals");
    
    // Obtain the neutron tracks for which a spectrum has to be generated:
    if ((TClonesArray*)ioman->GetObject(BranchName) == nullptr)
    {
        cout << "I/O-manager FATAL: R3BErelSpectrum::Init No <" << BranchName << "!\n\n";
        return kFATAL;
    }
    fArrayLorentzTracks = (TClonesArray*)ioman->GetObject(BranchName);
        
    // Create Histograms to store the relative energy spectra in:
    TString st = "";
    TString kstr = "";
    TString HistName = "";
    
    ErelSpectrum_GunMultiplicity = new TH1D*[MaxMultiplicity+1];
    ErelSpectrum_DetectedMultiplicity = new TH1D*[MaxMultiplicity+1];
    ErelSpectrum_ExpMultiplicity = new TH1D*[MaxMultiplicity+1];
    GunCounters = new Int_t[MaxMultiplicity+1];
    GunCounters_Limited = new Int_t[MaxMultiplicity+1];
    DetCounters = new Int_t[MaxMultiplicity+1];
    DetCounters_Limited = new Int_t[MaxMultiplicity+1];
    ExpCounters = new Int_t[MaxMultiplicity+1];
    ExpCounters_Limited = new Int_t[MaxMultiplicity+1];
    
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
    {
        kstr = st.Itoa(k,10);
        
        HistName = BranchName + "_Erel_Spectrum_GunMult_n=" + kstr;
        ErelSpectrum_GunMultiplicity[k] = new TH1D(HistName,HistName,NbinsSpectrum,0.0,EmaxSpectrum);
        GunCounters[k] = 0;
        GunCounters_Limited[k] = 0;
        
        HistName = BranchName + "_Erel_Spectrum_DetMult_n=" + kstr;
        ErelSpectrum_DetectedMultiplicity[k] = new TH1D(HistName,HistName,NbinsSpectrum,0.0,EmaxSpectrum);
        DetCounters[k] = 0;
        DetCounters_Limited[k] = 0;
        
        HistName = BranchName + "_Erel_Spectrum_ExpMult_n=" + kstr;
        ErelSpectrum_ExpMultiplicity[k] = new TH1D(HistName,HistName,NbinsSpectrum,0.0,EmaxSpectrum);
        ExpCounters[k] = 0;
        ExpCounters_Limited[k] = 0;
    }
    
    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    return kSUCCESS;
}

// FairTask Execute function:
void R3BErelSpectrum::Exec(Option_t *option)
{
    // NOTE: FairTask will automatically load the inputs into the TClonesArrays.
    
    // Obtain particle gun multiplicity:
    Int_t GunMultiplicity = 0;
    if (fArrayMCNeutronTracks!=0) {GunMultiplicity = fArrayMCNeutronTracks->GetEntries();}
    
    // Obtain the detected multiplicity:
    Int_t DetectedMultiplicity = 0;
    Int_t nSignals = fArraySignals->GetEntries();
    R3BSignal* ThisSignal;
    
    for (Int_t k = 0; k<nSignals; ++k)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        if (ThisSignal->IsPrimarySim()==kTRUE) {DetectedMultiplicity = DetectedMultiplicity + 1;}
    }
    
    // Obtain the experimental multiplicity:
    Int_t ExpMultiplicity = fArrayLorentzTracks->GetEntries();
    
    // Provide limitations to the other multiplicities, just in case:
    if (ExpMultiplicity>MaxMultiplicity) {ExpMultiplicity = MaxMultiplicity;}
    if (DetectedMultiplicity>MaxMultiplicity) {DetectedMultiplicity = MaxMultiplicity;}
    if (GunMultiplicity>MaxMultiplicity) {GunMultiplicity = MaxMultiplicity;}
    
    // Next, compute the relative energy:
    TLorentzVector* SumVector = new TLorentzVector();
    TLorentzVector* ThisVector;
    Double_t SumMass;
    Double_t InvMass;
    
    SumVector->SetT(0.0);
    SumVector->SetX(0.0);
    SumVector->SetY(0.0);
    SumVector->SetZ(0.0);
    SumMass = 0.0;
    
    for (Int_t k = 0; k<ExpMultiplicity; ++k)
    {
        // retrieve current LorentzVector:
        ThisVector = (TLorentzVector*) fArrayLorentzTracks->At(k);
        
        // Add it to the sum-vector:
        SumVector->SetT(SumVector->T() + ThisVector->T());
        SumVector->SetX(SumVector->X() + ThisVector->X());
        SumVector->SetY(SumVector->Y() + ThisVector->Y());
        SumVector->SetZ(SumVector->Z() + ThisVector->Z());
        
        // Add the sum of the masses:
        SumMass = SumMass + ParticleMass;
    }
    
    // Add the recoil nuclei, if this is wanted:
    if ((IncludeRecoil_ErelSpectrum==kTRUE)&&(fArrayMCRecoilTracks!=0))
    {
        // loop over all recoil vectors:
        for (Int_t k = 0; k<fArrayMCRecoilTracks->GetEntries(); ++k)
        {
            // Retrieve the vector:
            ThisVector = (TLorentzVector*) fArrayMCRecoilTracks->At(k);
            
            // Add it to the sum:
            SumVector->SetT(SumVector->T() + ThisVector->T());
            SumVector->SetX(SumVector->X() + ThisVector->X());
            SumVector->SetY(SumVector->Y() + ThisVector->Y());
            SumVector->SetZ(SumVector->Z() + ThisVector->Z());
            
            // And also to the mass:
            SumMass = SumMass + ThisVector->M();
            
            // Done.
        }
    }
    
    InvMass = SumVector->M();
    Double_t Erel = TMath::Abs(InvMass - SumMass);
     
    // Then, fill it into the proper histograms:
    ErelSpectrum_ExpMultiplicity[ExpMultiplicity]->Fill(Erel); 
    ExpCounters[ExpMultiplicity] = ExpCounters[ExpMultiplicity] + 1;
    if ((Erel>=0)&&(Erel<EmaxSpectrum)) {ExpCounters_Limited[ExpMultiplicity] = ExpCounters_Limited[ExpMultiplicity] + 1;}
    
    if (ExpMultiplicity==DetectedMultiplicity)
    {
        ErelSpectrum_DetectedMultiplicity[DetectedMultiplicity]->Fill(Erel); 
        DetCounters[DetectedMultiplicity] = DetCounters[DetectedMultiplicity] + 1;
        if ((Erel>=0)&&(Erel<EmaxSpectrum)) {DetCounters_Limited[DetectedMultiplicity] = DetCounters_Limited[DetectedMultiplicity] + 1;}
    }
    
    if (ExpMultiplicity==GunMultiplicity)
    {
        ErelSpectrum_GunMultiplicity[GunMultiplicity]->Fill(Erel); 
        GunCounters[GunMultiplicity] = GunCounters[GunMultiplicity] + 1;
        if ((Erel>=0)&&(Erel<EmaxSpectrum)) {GunCounters_Limited[GunMultiplicity] = GunCounters_Limited[GunMultiplicity] + 1;}
    }    
    
    // Log progress:
    EventCounter = EventCounter + 1;    
    if ((EventCounter%1000==0)&&(BranchName=="TheNeutronTracks")) {cout << "R3BErelSpectrum Log: We processed " << EventCounter << " events.\n";}
    
    // delete all new stuff:
    delete SumVector;
}
        
// FairTask Finish function:
void R3BErelSpectrum::Finish()
{
    // First, see if our output-file already exists or not:
    TString st = "";
    TString kstr = "";
    FileStat_t stom;
    Int_t test = gSystem->GetPathInfo(FileName.Data(),stom);
    TFile* f;
    
    if (test==0)
    {
        // Then the file already exists:
        f = new TFile(FileName.Data(),"update");
    }
    else
    {
        // Then, recreate the file:
        f = new TFile(FileName.Data(),"recreate");
    }
    
    // Next, write all histograms to the file:
    f->cd();
    TString WriteName = "";
    
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
    {
        WriteName = ErelSpectrum_GunMultiplicity[k]->GetName();
        ErelSpectrum_GunMultiplicity[k]->Write(WriteName,2);
        
        WriteName = ErelSpectrum_DetectedMultiplicity[k]->GetName();
        ErelSpectrum_DetectedMultiplicity[k]->Write(WriteName,2);
        
        WriteName = ErelSpectrum_ExpMultiplicity[k]->GetName();
        ErelSpectrum_ExpMultiplicity[k]->Write(WriteName,2);
    }
    
    // Then, finish up:
    f->Close();
    delete f;
    
    // Next, write the Histogram integrals to a .txt-file:
    TString TextFileName = OutputPath + "/ErelSpectrum_Integrals.txt";
    
    ifstream Test;
    Test.open(TextFileName.Data());
    
    if (!Test.is_open())
    {
        // Then, recreate it:
        std::ofstream Creation (TextFileName.Data(), std::ofstream::out);
        Creation.close();
    }
    else
    {
        Test.close();
    }
    
    // Next, append to the .txt-file:
    Int_t ExpTotCounter = 0;
    Int_t ExpTotCounter_Limited = 0;
    Int_t DetTotCounter = 0;
    Int_t DetTotCounter_Limited = 0;
    Int_t GunTotCounter = 0;
    Int_t GunTotCounter_Limited = 0;
    
    std::ofstream WriteTextFile;
    WriteTextFile.open(TextFileName.Data(), std::ios::app);
    
    WriteTextFile << "Histogram Integrals of " << BranchName << "\n";
    WriteTextFile << "Histogram Boundaries are " << 0.0 << " & " << EmaxSpectrum << "\n";
    
    WriteTextFile << "\n\nUnLimited Integrals:\n";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
    {
        kstr = st.Itoa(k,10);
        WriteTextFile << "Multiplicity: " << kstr << " | Exp. int. = " << ExpCounters[k] << " | Det. int. = " << DetCounters[k] << " | Gun int. = " << GunCounters[k] << "\n";
        ExpTotCounter = ExpTotCounter + ExpCounters[k];
        DetTotCounter = DetTotCounter + DetCounters[k];
        GunTotCounter = GunTotCounter + GunCounters[k];
    }
        WriteTextFile << " ### Total:   " << kstr << " | Exp. int. = " << ExpTotCounter << " | Det. int. = " << DetTotCounter << " | Gun int. = " << GunTotCounter << "\n";
        
    WriteTextFile << "\n\nLimited Integrals to Histogram Range:\n";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
    {
        kstr = st.Itoa(k,10);
        WriteTextFile << "Multiplicity: " << kstr << " | Exp. int. = " << ExpCounters_Limited[k] << " | Det. int. = " << DetCounters_Limited[k] << " | Gun int. = " << GunCounters_Limited[k] << "\n";
        ExpTotCounter_Limited = ExpTotCounter_Limited + ExpCounters_Limited[k];
        DetTotCounter_Limited = DetTotCounter_Limited + DetCounters_Limited[k];
        GunTotCounter_Limited = GunTotCounter_Limited + GunCounters_Limited[k];
    }
        WriteTextFile << " ### Total:   " << kstr << " | Exp. int. = " << ExpTotCounter_Limited << " | Det. int. = " << DetTotCounter_Limited << " | Gun int. = " << GunTotCounter_Limited << "\n";
        
    WriteTextFile << "\n\n---------------------------------------------------------------------------\n\n";
    
    WriteTextFile.close();
    
    // Done.
}

// Generate ROOT dictionary:
ClassImp(R3BErelSpectrum);

// NOTE: if a method uses a perfect multiplicity reconstruction method, then the histogram integral
// should be identical to the perfect signal reconstruction. After all, we have always exactly one Erel
// per event, meaning that if we only enforce conditions on the multiplicity during histogram filling, all reconstruction 
// methods with the same multiplicity will have the same histogram integral by definition. That the Scoring+
// histogram is above the perfect signal reconstruction histogram, is because the Erel spectra have very very long tails.
// The difference in the peak is compensated gradually in the long tail. However, this, we do not see.
// And as a result, the histogram integrals limited to the plotting ranges can be larger for Scoring+.
// Same is true for TDR under perfect multiplicity and for cluster/signal perfect reconstruction.
// all have exactly the same multiplicity (except for those very very few events where there are not enough clusters),
// so differences in the peak are compensated by the long tails (compare histogram integrals in .txt-file).
// This is not an error, it is just a result of cutting off the spectrum's tail. 
// NOTE: This tail problem has been confirmed. see the .txt-files of the histogram integrals.

// And also remember that the labelling (hencem, the perfect signal reconstruction) can also carry errors...
// we know that labelling is an issue, which Scoring+ and TDR are not bothered by much, but DNN is.

