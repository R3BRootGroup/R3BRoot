// Every CXX-file includes its own header file:
#include "R3BPhysList.h"

// Default constructor definition:
R3BPhysList::R3BPhysList() : FairTask("R3BPhysList")
{
    // Set default verbosity level:
    fVerbose = 1;

    // Create arrays:
    fArrayTracks = new TClonesArray("R3BMCTrack");
    fArrayPoints = new TClonesArray("R3BNeulandPoint");
    fArrayDigis = new TClonesArray("R3BNeulandDigi");
  
    // Set other class values:
    nEvents = 1;
    EventCounter = 0;
    InteractionCounter = 0;
    ProtonCounter = 0;
    DetectionCounter = 0;
    FileName = "./Histograms.root";
    InteractionRate = new ObjDouble("InteractionRate");
    ProtonRate = new ObjDouble("ProtonRate");
    DetectionRate = new ObjDouble("DetectionRate");
    
    // Build nuclear database:
    TheNuclei = new Nuclei();
    TheNuclei->BuildDataBase();
    TheOutputFile = 0;
}

// Destructor definition:
R3BPhysList::~R3BPhysList()
{
    // Delete the arrays:
    if(fArrayTracks) {fArrayTracks->Clear(); delete fArrayTracks;}
    if(fArrayPoints) {fArrayPoints->Clear(); delete fArrayPoints;}
    if(fArrayDigis)  {fArrayDigis->Clear(); delete fArrayDigis;}
    delete InteractionRate;
    delete ProtonRate;
    delete DetectionRate;
}

// FairTask initialization function:
InitStatus R3BPhysList::Init()
{
    // Obtain the I/O manager:
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        cout << "I/O-manager FATAL: R3BPhysList::Init: No FairRootManager\n\n";
        return kFATAL;
    }
    // NOTE: No output file is specified! // TheOutputFile = ioman->GetOutFile();
    
    // Obtain inputs:
    if (Inputs->IsLinked()==kFALSE)
    {
        cout << "I/O-manager FATAL: R3BInputClass: not linked to a file!\n\n";
        return kFATAL;
    }
    
    // Retrieve inputs:
    FileName = Inputs->GetInputString("TheOutputPath") + Inputs->GetInputString("NeuLAND_Distributions");
    
    // Obtain R3BNeulandPoints:
    if ((TClonesArray*)ioman->GetObject("NeulandPoints") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BPhysList::Init No R3BNeulandPoints!\n\n";
        return kFATAL;
    }
    fArrayPoints = (TClonesArray*)ioman->GetObject("NeulandPoints");
        
    // MC tracks:
    if ((TClonesArray*)ioman->GetObject("MCTrack") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BPhysList::Init No MCTrack!\n\n";
        return kFATAL;
    }
    fArrayTracks = (TClonesArray*)ioman->GetObject("MCTrack");
    
    // NeuLAND Digis:
    if ((TClonesArray*)ioman->GetObject("NeulandDigis") == nullptr)
    {
        cout << "I/O-manager FATAL: R3BPhysList::Init No NeulandDigis!\n\n";
        return kFATAL;
    }
    fArrayDigis = (TClonesArray*)ioman->GetObject("NeulandDigis");
    
    // Build histograms & other tags:
    Nparticles = 14;
    Hist_ParticleID = new TH1D("Hist_ParticleID","Particle ID in showers",Nparticles,0.0,1.0);
    ParticleID = new TString[Nparticles];
    ParticleID[0]  = "all";
    ParticleID[1]  = "neutron";
    ParticleID[2]  = "proton";
    ParticleID[3]  = "electron";
    ParticleID[4]  = "gamma";
    ParticleID[5]  = "neutrino (ve, vnu, vtau)";
    ParticleID[6]  = "pion (pi+, pi- & pi0)";
    ParticleID[7]  = "muon (+ & -)";
    ParticleID[8]  = "positron";
    ParticleID[9]  = "deuteron";
    ParticleID[10] = "tritium";
    ParticleID[11] = "alpha";
    ParticleID[12] = "C-ion";
    ParticleID[13] = "any other ion.";
    
    Hist_Edep = new TH1D*[Nparticles];
    Hist_AlphaScatter = new TH1D*[Nparticles];
    TString st = "";
    TString kstr = "";
    
    for (Int_t k = 0; k<Nparticles; ++k)
    {
        kstr = st.Itoa(k,10);
        Hist_Edep[k] = new TH1D("Hist_Edep"+kstr,"Energy Deposition in Scintillators [MeV] "+ParticleID[k],300,0.0,500.0);
        Hist_AlphaScatter[k] = new TH1D("Hist_AlphaScatter"+kstr,"Scattering angle of secondary particles [deg] "+ParticleID[k],500,0.0,180.0);
    }
    
    // Then, return the succes statement:
    if (Inputs->ContainsNoErrors()==kFALSE) {Inputs->PrintAllErrors(); return kFATAL;}
    EventCounter = 0;
    InteractionCounter = 0;
    ProtonCounter = 0;
    DetectionCounter = 0;
    return kSUCCESS;
}

// FairTask Initialization function:
void R3BPhysList::Exec(Option_t *option)
{
    // NOTE: FairTask will automatically load the inputs into the TClonesArrays.
    
    // Declare what we need:
    Int_t Npoints = fArrayPoints->GetEntries();
    Int_t Ntracks = fArrayTracks->GetEntries();
    Int_t Ndigis = fArrayDigis->GetEntries();
    
    R3BNeulandPoint* ThisPoint;
    R3BMCTrack* ThisTrack;
    R3BMCTrack* MotherTrack;
    FairMCPoint* ThisPrimary;
    TLorentzVector ThisMomentum;
    TLorentzVector MotherMomentum;
    
    Int_t ThisTrackID;
    Int_t ThisMotherID;
    Int_t SecondMotherID;
    Int_t ThisPDGCode;
    Double_t p0; 
    Double_t ThisMass;
    Double_t MotherMass;
    Double_t MotherTheta;
    Double_t MotherPhi;
    Double_t ThisTheta;
    Double_t ThisPhi;
    Double_t ThisScatterAngle;
    Double_t ThisDoubleIndex;
    
    // Declare the arrays we need to keep track of everything:
    Bool_t* Track_IsComputed = new Bool_t[Ntracks];
    Bool_t* Track_IsSecondary = new Bool_t[Ntracks];
    Double_t* Track_Edep = new Double_t[Ntracks];
    Double_t* Track_AlphaScatter = new Double_t[Ntracks];
    Int_t* ParticleID_Index = new Int_t[Ntracks];
    
    for (Int_t n = 0; n<Ntracks; ++n)
    {
        Track_IsComputed[n] = kFALSE;
        Track_IsSecondary[n] = kFALSE;
        Track_Edep[n] = 0.0;
        Track_AlphaScatter[n] = 0.0;
        ParticleID_Index[n] = -1000;
    }
    
    // Loop over all NeuLAND points:
    for (Int_t k = 0; k<Npoints; ++k)
    {
        // Retrieve the current point:
        ThisPoint = (R3BNeulandPoint*) fArrayPoints->At(k);
        
        // Then, obtain the corresponding MC track:
        ThisTrackID = ThisPoint->GetTrackID();
        ThisTrack = (R3BMCTrack*) fArrayTracks->At(ThisTrackID);
        
        // Accumulate energy deposition per track:
        Track_Edep[ThisTrackID] = Track_Edep[ThisTrackID] + ThisPoint->GetEnergyLoss()*1000.0; // NOTE: Conversion to MeV!
        
        // Check if already have done this calculation:
        if (Track_IsComputed[ThisTrackID]==kFALSE)
        {
            // Obtain the tracks 4-momentum:
            ThisMomentum.SetX(ThisTrack->GetPx()*1000.0); // Conversion to MeV.
            ThisMomentum.SetY(ThisTrack->GetPy()*1000.0); // Conversion to MeV.
            ThisMomentum.SetZ(ThisTrack->GetPz()*1000.0); // Conversion to MeV.
            ThisMass = ThisTrack->GetMass()*1000.0;         // Conversion to MeV.
            p0 = ThisMomentum.X()*ThisMomentum.X() + ThisMomentum.Y()*ThisMomentum.Y() + ThisMomentum.Z()*ThisMomentum.Z() + ThisMass*ThisMass;
            p0 = TMath::Sqrt(p0);
            ThisMomentum.SetT(p0);
            
            // Cathegorise ParticleID:
            ThisPDGCode = ThisTrack->GetPdgCode();
            ParticleID_Index[ThisTrackID] = 0;
            if (ThisPDGCode==2112) {ParticleID_Index[ThisTrackID] = 1;} // neutron
            if (ThisPDGCode==2212) {ParticleID_Index[ThisTrackID] = 2;} // proton
            if (ThisPDGCode==11)   {ParticleID_Index[ThisTrackID] = 3;} // electron
            if (ThisPDGCode==22)   {ParticleID_Index[ThisTrackID] = 4;} // gamma
            if ((ThisPDGCode==12)||(ThisPDGCode==-12)||(ThisPDGCode==14)||(ThisPDGCode==-14)||(ThisPDGCode==16)||(ThisPDGCode==-16)) {ParticleID_Index[ThisTrackID] = 5;} // all neutrinoes
            if ((ThisPDGCode==111)||(ThisPDGCode==211)||(ThisPDGCode==-211)) {ParticleID_Index[ThisTrackID] = 6;} // all pions
            if ((ThisPDGCode==13)||(ThisPDGCode==-13)) {ParticleID_Index[ThisTrackID] = 7;} // all muons
            if (ThisPDGCode==-11)   {ParticleID_Index[ThisTrackID] = 8;} // positron
            if (ThisPDGCode==1000010020) {ParticleID_Index[ThisTrackID] = 9;} // deuteron
            if (ThisPDGCode==1000010030) {ParticleID_Index[ThisTrackID] = 10;} // triton
            if (ThisPDGCode==1000020040) {ParticleID_Index[ThisTrackID] = 11;} // alpha
            if ((ThisPDGCode>1000060010)&&(ThisPDGCode<1000060990)) {ParticleID_Index[ThisTrackID] = 12;} // C-ion
            if ((ThisPDGCode>1000000000)&&(ParticleID_Index[ThisTrackID]==0)) {ParticleID_Index[ThisTrackID] = 13;} // any other ion.
        
            // Get information about the mother track:
            ThisMotherID = ThisTrack->GetMotherId();
        
            if (ThisMotherID!=-1)
            {
                // Obtain the mother track:
                MotherTrack = (R3BMCTrack*) fArrayTracks->At(ThisMotherID);
            
                // Ontain mother 4-momentum:
                MotherMomentum.SetX(MotherTrack->GetPx()*1000.0); // Conversion to MeV.
                MotherMomentum.SetY(MotherTrack->GetPy()*1000.0); // Conversion to MeV.
                MotherMomentum.SetZ(MotherTrack->GetPz()*1000.0); // Conversion to MeV.
                MotherMass = MotherTrack->GetMass()*1000.0;         // Conversion to MeV.
                p0 = MotherMomentum.X()*MotherMomentum.X() + MotherMomentum.Y()*MotherMomentum.Y() + MotherMomentum.Z()*MotherMomentum.Z() + MotherMass*MotherMass;
                p0 = TMath::Sqrt(p0);
                MotherMomentum.SetT(p0);
            
                // Obtain motherID of mother:
                SecondMotherID = MotherTrack->GetMotherId();
        
                // Rotate ThisMomentum untill MotherMomentum is along z-axis:
                ThisPhi = ThisMomentum.Phi();
                MotherPhi = MotherMomentum.Phi();
                ThisTheta = ThisMomentum.Theta();
                MotherTheta = MotherMomentum.Theta();
                MotherMass = MotherMomentum.M();
                 
                ThisMomentum.RotateZ(-1.0*MotherPhi);
                MotherMomentum.RotateZ(-1.0*MotherPhi);
                
                ThisMomentum.RotateY(-1.0*MotherTheta);
                MotherMomentum.RotateY(-1.0*MotherTheta);
                
                // Check that the rotation worked:
                if (TMath::Abs(MotherMomentum.X()/MotherMomentum.Z())>1e-6) {cout << "### R3BPhysList ERROR: Rotation of 4-momenta failed: " << MotherMomentum.X()/MotherMomentum.Z() << "\n";}
                if (TMath::Abs(MotherMomentum.Y()/MotherMomentum.Z())>1e-6) {cout << "### R3BPhysList ERROR: Rotation of 4-momenta failed: " << MotherMomentum.Y()/MotherMomentum.Z() << "\n";}
                
                // next, obtain the scattering angle:
                ThisScatterAngle = ThisMomentum.Theta()*180.0/TMath::Pi(); // NOTE: Conversion to degrees.
                Track_AlphaScatter[ThisTrackID] = ThisScatterAngle;
           
                // See then, if ThisParticle = secondary, hence, mother of mother == -1:
                if (SecondMotherID==-1)
                {
                    // Then, mark it as a strictly secondary particle:
                    Track_IsSecondary[ThisTrackID] = kTRUE;
                }
                
                // Close all blocks. Note that we finished the computation:
                Track_IsComputed[ThisTrackID] = kTRUE;
            }
        }
    }
    
    // Fill in the histograms:
    Bool_t ContainsProtons = kFALSE;
    
    for (Int_t i = 0; i<Ntracks; ++i)
    {
        // Only fill in computed secondary tracks:
        if ((Track_IsComputed[i]==kTRUE)&&(Track_IsSecondary[i]==kTRUE))
        {
            if (ParticleID_Index[i]==2) {ContainsProtons = kTRUE;}
            
            ThisDoubleIndex = (Int_t) ParticleID_Index[i];
            ThisDoubleIndex = (ThisDoubleIndex + 0.5)/((Int_t) Nparticles);
            
            Hist_ParticleID->Fill(ThisDoubleIndex); // Particle ID
            Hist_ParticleID->Fill(0.0); // to consistently fill the all-column.
            
            // Fill energy deposition per track:
            Hist_Edep[0]->Fill(Track_Edep[i]);
            Hist_Edep[ParticleID_Index[i]]->Fill(Track_Edep[i]);
            
            // Fill scattering angle per track:
            Hist_AlphaScatter[0]->Fill(Track_AlphaScatter[i]);
            Hist_AlphaScatter[ParticleID_Index[i]]->Fill(Track_AlphaScatter[i]);
        }
    }
            
    // Log progress:
    EventCounter = EventCounter + 1;
    if (Npoints>0) {InteractionCounter = InteractionCounter + 1;}
    if (ContainsProtons==kTRUE) {ProtonCounter = ProtonCounter + 1;}
    if (Ndigis>0) {DetectionCounter = DetectionCounter + 1;}
    if (EventCounter%1000==0) {cout << "NeuLANDTranslator Log: We processed " << EventCounter << " events.\n";}
}
        
// Definition of Finish-function:
void R3BPhysList::Finish()
{   
    // try to find first if the file already exists:
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
        // recreate the file:
        f = new TFile(FileName.Data(),"recreate");
    }
    
    // write the histogram to the file:
    f->cd();
    
    TString WriteKey = Hist_ParticleID->GetName();
    Hist_ParticleID->Write(WriteKey.Data(),2);
    
    for (Int_t k = 0; k<Nparticles; ++k)
    {
        WriteKey = Hist_Edep[k]->GetName();
        Hist_Edep[k]->Write(WriteKey.Data(),2);
        
        WriteKey = Hist_AlphaScatter[k]->GetName();
        Hist_AlphaScatter[k]->Write(WriteKey.Data(),2);
    }
    
    // Compute overall rates & write it to the file:
    cout << "\n\n";
    
    Double_t IntRate = 1.0/((Int_t) EventCounter);
    IntRate = IntRate*((Int_t) InteractionCounter);
    InteractionRate->SetNumber(IntRate);
    InteractionRate->WriteToFile(f);
    cout << "==> Overall Interaction Rate = " << IntRate << "\n";
    
    Double_t ProtRate = 1.0/((Int_t) EventCounter);
    ProtRate = ProtRate*((Int_t) ProtonCounter);
    ProtonRate->SetNumber(ProtRate);
    ProtonRate->WriteToFile(f);
    cout << "==> Overall DetectionRate Rate = " << ProtRate << "\n";
    
    Double_t DetRate = 1.0/((Int_t) EventCounter);
    DetRate = DetRate*((Int_t) DetectionCounter);
    DetectionRate->SetNumber(DetRate);
    DetectionRate->WriteToFile(f);
    cout << "==> Overall DetectionRate Rate = " << DetRate << "\n";
    
    cout << "\n\n";
    
    // Save & close:
    f->Close();
    delete f;
    
    // Done.
}

// Generate ROOT dictionary:
ClassImp(R3BPhysList);
