void R3BASCIIFileGenerator::WriteOneExEvent(Int_t const ThisEvent)
{
    // This function generates and writes a single event with the Ex-generator.
    Bool_t Display_Output = kFALSE;
    Bool_t Display_Bisection = kFALSE;
    
    // This function also needs the following class members:
    // Bool_t ParticleGun_ExGenerator_DegradeEnergy_MultipleTracks;
    // TString ParticleGun_ExGenerator_BeamIsotope;
    // TString ParticleGun_ExGenerator_TargetIsotope;
    // Double_t ParticleGun_ExGenerator_SingleTheta [rad]
    // std::vector<Double_t> ParticleGun_ExGenerator_PhiVector [rad]
    // std::vector<TLorentzVector*> ParticleGun_ExGenerator_NeutronTracks (elements are created by new!)
    // Int_t ParticleGun_ExGenerator_NeutronMultiplicity (vectors size equals this)
    // Double_t ParticleGun_ExGenerator_BeamEnergy [MeV/u]

    
    // Begin with generating a single Theta-angle:
    if (RandomizeOption=="Uniform")
    {
        // The Sqrt is necessary to ensure uniform distribution in polar coordinates:
        ParticleGun_ExGenerator_SingleTheta = ParticleGun_AngularSpread*TMath::Sqrt(Generator->Uniform(0.0,1.0));
    }
    else if (RandomizeOption=="Native_Central")
    {
        // Then, Theta is uniform, causing the center to be overpopulated:
        ParticleGun_ExGenerator_SingleTheta = Generator->Uniform(0.0,ParticleGun_AngularSpread);
    }
    else
    {
        // Default setting is uniform:
        ParticleGun_ExGenerator_SingleTheta = ParticleGun_AngularSpread*TMath::Sqrt(Generator->Uniform(0.0,1.0));
        ErrorMessage("The RandomizeOption <"+RandomizeOption+"> was NOT supported in Generate4Vector()!");
    }
    
    // Conversion to radians:
    ParticleGun_ExGenerator_SingleTheta = ParticleGun_ExGenerator_SingleTheta*TMath::Pi()/180.0;
    
    // Next, generate the multiplicity:
    Int_t ThisMult = 1;
    
    if (ParticleGun_Randomize_Multiplicity==kTRUE)
    {            
        Double_t Rnd_Mult = Generator->Uniform(0.0,1.0);
                
        for (Int_t k = 0; k<ParticleGun_Multiplicity; ++k)
        {
            if ((Rnd_Mult<=Mult_Random_Boundaries[k+1])&&(Rnd_Mult>=Mult_Random_Boundaries[k]))
            {
                ThisMult = k+1;
            }
        }
    }
    else
    {
        ThisMult = ParticleGun_Multiplicity;
    }
    
    ParticleGun_ExGenerator_NeutronMultiplicity = ThisMult;
    
    // Then, generate the phi-angles:
    ParticleGun_ExGenerator_PhiVector.resize(ParticleGun_ExGenerator_NeutronMultiplicity);
    
    for (Int_t k = 0; k<ParticleGun_ExGenerator_NeutronMultiplicity; ++k)
    {
        ParticleGun_ExGenerator_PhiVector.at(k) = Generator->Uniform(0.0,2.0*TMath::Pi());
    }
    
    // Next, resize the vector of neutron tracks:
    ParticleGun_ExGenerator_NeutronTracks.resize(ParticleGun_ExGenerator_NeutronMultiplicity);
    
    for (Int_t k = 0; k<ParticleGun_ExGenerator_NeutronMultiplicity; ++k)
    {
        ParticleGun_ExGenerator_NeutronTracks.at(k) = new TLorentzVector();
    }
    
    // Define multiplicity for the bisection:
    Int_t Bisection_Multiplicity = 1;
    if (ParticleGun_ExGenerator_DegradeEnergy_MultipleTracks==kTRUE) {Bisection_Multiplicity = ParticleGun_ExGenerator_NeutronMultiplicity;}
    else {Bisection_Multiplicity = 1;}
    
    // Then, the next step is to use a bisection to compute the neutron energy. Begin by defining the interval:
    Double_t LeftEnergy = ParticleGun_ExGenerator_BeamEnergy*0.7;
    Double_t LeftFunctionValue = Bisection_Function(LeftEnergy,Bisection_Multiplicity);
    
    Int_t TotalA = TheNuclei->GetA(ParticleGun_ExGenerator_TargetIsotope) + TheNuclei->GetA(ParticleGun_ExGenerator_BeamIsotope);
    Double_t RightEnergy = ParticleGun_ExGenerator_BeamEnergy*1.3;
    Double_t RightFunctionValue = Bisection_Function(RightEnergy,Bisection_Multiplicity);
    
    Double_t MiddleEnergy = 0.0;
    Double_t MiddleFunctionValue = 0.0;
    
    if (Display_Bisection==kTRUE) {cout << "Bisection: Left [MeV]   = " << LeftEnergy << " && Right [MeV]   = " << RightEnergy << "\n";}
    if (Display_Bisection==kTRUE) {cout << "Bisection: Left f-value = " << LeftFunctionValue << " && Right f-value = " << RightFunctionValue << "\n";}
    
    for (Int_t k = 0; k<30; ++k) // 30 Ensures a precision of at least 1 keV
    {
        // Update value:
        MiddleEnergy = (LeftEnergy+RightEnergy)/2.0;
        MiddleFunctionValue = Bisection_Function(MiddleEnergy,Bisection_Multiplicity);
        
        // Decide:
        if ((LeftFunctionValue*MiddleFunctionValue<0.0)&&(RightFunctionValue*MiddleFunctionValue<0.0))
        {
            ErrorMessage("Bisection found multiple answers for the neutron energy!");
        }
        else if ((LeftFunctionValue*MiddleFunctionValue<=0.0)&&(RightFunctionValue*MiddleFunctionValue>=0.0))
        {
            // Then, left part becomes new interval:
            LeftEnergy = LeftEnergy;
            LeftFunctionValue = LeftFunctionValue;
            RightEnergy = MiddleEnergy;
            RightFunctionValue = MiddleFunctionValue;
        }
        else if ((LeftFunctionValue*MiddleFunctionValue>=0.0)&&(RightFunctionValue*MiddleFunctionValue<=0.0))
        {
            // Then, the right interval is the new one:
            LeftEnergy = MiddleEnergy;
            LeftFunctionValue = MiddleFunctionValue;
            RightEnergy = RightEnergy;
            RightFunctionValue = RightFunctionValue;
        }
        else
        {
            ErrorMessage("Bisection found no answers for the neutron energy!");
        }
        
        if (Display_Bisection==kTRUE) {cout << "Step " << k+1 << ": Neutron Energy [MeV] = " << MiddleEnergy << " | Function Value = " << MiddleFunctionValue << "\n";}
    }
    
    if (Display_Bisection==kTRUE) {cout << "Bisection result: Neutron Energy [MeV] = " << MiddleEnergy << " at Theta[deg] = " << ParticleGun_ExGenerator_SingleTheta*180.0/TMath::Pi() << "\n";}
    
    // Now that the energy is known, we can compose the neutron tracks:
    Double_t NeutronMass = TheNuclei->GetMass(ParticleGun_ParticleType,"MeV");
    Double_t NeutronTimeComponent = NeutronMass + MiddleEnergy;
    Double_t NeutronSpaceComponent = TMath::Sqrt(NeutronTimeComponent*NeutronTimeComponent - NeutronMass*NeutronMass);
    
    for (Int_t k = 0; k<ParticleGun_ExGenerator_NeutronMultiplicity; ++k)
    {
        ParticleGun_ExGenerator_NeutronTracks.at(k)->SetT(NeutronTimeComponent);
        ParticleGun_ExGenerator_NeutronTracks.at(k)->SetX(NeutronSpaceComponent*TMath::Sin(ParticleGun_ExGenerator_SingleTheta)*TMath::Cos(ParticleGun_ExGenerator_PhiVector.at(k)));
        ParticleGun_ExGenerator_NeutronTracks.at(k)->SetY(NeutronSpaceComponent*TMath::Sin(ParticleGun_ExGenerator_SingleTheta)*TMath::Sin(ParticleGun_ExGenerator_PhiVector.at(k)));
        ParticleGun_ExGenerator_NeutronTracks.at(k)->SetZ(NeutronSpaceComponent*TMath::Cos(ParticleGun_ExGenerator_SingleTheta));
    }
    
    // Next, compose the recoil track too:
    TVector3 BeamDirection;
    BeamDirection.SetX(ParticleGun_x_direction);
    BeamDirection.SetY(ParticleGun_y_direction);
    BeamDirection.SetZ(ParticleGun_z_direction);
    
    Double_t BeamMass = TheNuclei->GetMass(ParticleGun_ExGenerator_BeamIsotope,"MeV");
    Double_t BeamTimeComponent = BeamMass + ParticleGun_ExGenerator_BeamEnergy*TheNuclei->GetA(ParticleGun_ExGenerator_BeamIsotope);
    Double_t BeamSpaceComponent = TMath::Sqrt(BeamTimeComponent*BeamTimeComponent - BeamMass*BeamMass);
    Double_t BeamTheta = BeamDirection.Theta();
    Double_t BeamPhi = BeamDirection.Phi();
    
    TLorentzVector BeamTrack;
    BeamTrack.SetT(BeamTimeComponent);
    BeamTrack.SetX(BeamSpaceComponent*TMath::Sin(BeamTheta)*TMath::Cos(BeamPhi));
    BeamTrack.SetY(BeamSpaceComponent*TMath::Sin(BeamTheta)*TMath::Sin(BeamPhi));
    BeamTrack.SetZ(BeamSpaceComponent*TMath::Cos(BeamTheta));
    
    TLorentzVector TargetTrack;
    TargetTrack.SetT(TheNuclei->GetMass(ParticleGun_ExGenerator_TargetIsotope,"MeV"));
    TargetTrack.SetX(0.0);
    TargetTrack.SetY(0.0);
    TargetTrack.SetZ(0.0);
    
    Int_t RecoilZ = TheNuclei->GetZ(ParticleGun_ExGenerator_BeamIsotope) + TheNuclei->GetZ(ParticleGun_ExGenerator_TargetIsotope) - ParticleGun_ExGenerator_NeutronMultiplicity*TheNuclei->GetZ(ParticleGun_ParticleType);
    Int_t RecoilA = TheNuclei->GetA(ParticleGun_ExGenerator_BeamIsotope) + TheNuclei->GetA(ParticleGun_ExGenerator_TargetIsotope) - ParticleGun_ExGenerator_NeutronMultiplicity*TheNuclei->GetA(ParticleGun_ParticleType);
    TString RecoilIsotope = TheNuclei->FindNucleus(RecoilZ,RecoilA);
    
    Double_t RecoilMass = TheNuclei->GetMass(RecoilIsotope,"MeV");
    TLorentzVector RecoilTrack;
    
    for (Int_t k = 0; k<ParticleGun_ExGenerator_NeutronMultiplicity; ++k)
    {
        RecoilTrack.SetT(RecoilTrack.T() - ParticleGun_ExGenerator_NeutronTracks.at(k)->T());
        RecoilTrack.SetX(RecoilTrack.X() - ParticleGun_ExGenerator_NeutronTracks.at(k)->X());
        RecoilTrack.SetY(RecoilTrack.Y() - ParticleGun_ExGenerator_NeutronTracks.at(k)->Y());
        RecoilTrack.SetZ(RecoilTrack.Z() - ParticleGun_ExGenerator_NeutronTracks.at(k)->Z());
    }
    
    RecoilTrack.SetT(RecoilTrack.T() + BeamTrack.T() + TargetTrack.T());
    RecoilTrack.SetX(RecoilTrack.X() + BeamTrack.X() + TargetTrack.X());
    RecoilTrack.SetY(RecoilTrack.Y() + BeamTrack.Y() + TargetTrack.Y());
    RecoilTrack.SetZ(RecoilTrack.Z() + BeamTrack.Z() + TargetTrack.Z());
    
    // Then, write our event to the file. Start with the first line:
    Double_t pBeam = 0.0;
    Double_t bpar = 0.0;
    
    if (Include_SnNucleus==kTRUE)
    {
        ASCII_File << "    " << ThisEvent << "    " << ParticleGun_ExGenerator_NeutronMultiplicity+1 << "    " << pBeam << "    " << bpar << "\n";
    }
    else
    {
        ASCII_File << "    " << ThisEvent << "    " << ParticleGun_ExGenerator_NeutronMultiplicity+0 << "    " << pBeam << "    " << bpar << "\n";
    }
            
    // Then, loop over multiplicity:
    Int_t Ion_ID;
    Int_t Ion_Z;
    Int_t Ion_A;
    Int_t PDGCode = TheNuclei->GetPDGCode(ParticleGun_ParticleType);
    
    if (TMath::Abs(PDGCode)>1000000000)
    {
        // Then, it is an ion:
        Ion_ID = -1;
        Ion_Z = TheNuclei->GetZ(ParticleGun_ParticleType);
        Ion_A = TheNuclei->GetA(ParticleGun_ParticleType);
    }
    else
    {
        Ion_ID = 1;
        Ion_Z = 0;
        Ion_A = PDGCode;
    }
    
    for (Int_t k = 0; k<ParticleGun_ExGenerator_NeutronMultiplicity; ++k)
    {
        // Write to the file: The 1e-3 is conversion to GeV:
        ASCII_File << "    " << Ion_ID << "    " << Ion_Z << "    " << Ion_A << "    " << 1e-3*ParticleGun_ExGenerator_NeutronTracks.at(k)->X() << "    " << 1e-3*ParticleGun_ExGenerator_NeutronTracks.at(k)->Y() << "    " << 1e-3*ParticleGun_ExGenerator_NeutronTracks.at(k)->Z() << "    " << ParticleGun_x_position << "    " << ParticleGun_y_position << "    " << ParticleGun_z_position << "    " << 1e-3*TheNuclei->GetMass(ParticleGun_ParticleType,"MeV") << "\n";
    }
    
    // Finally, include the recoil track if wanted:
    if ((Include_SnNucleus==kTRUE)&&(ParticleGun_ExGenerator_DegradeEnergy_MultipleTracks==kTRUE))
    {
        PDGCode = TheNuclei->GetPDGCode(RecoilIsotope);
        
        if (TMath::Abs(PDGCode)>1000000000)
        {
            // Then, it is an ion:
            Ion_ID = -1;
            Ion_Z = TheNuclei->GetZ(RecoilIsotope);
            Ion_A = TheNuclei->GetA(RecoilIsotope);
        }
        else
        {
            Ion_ID = 1;
            Ion_Z = 0;
            Ion_A = PDGCode;
        }
        
        ASCII_File << "    " << Ion_ID << "    " << Ion_Z << "    " << Ion_A << "    " << 1e-3*RecoilTrack.X() << "    " << 1e-3*RecoilTrack.Y() << "    " << 1e-3*RecoilTrack.Z() << "    " << ParticleGun_x_position << "    " << ParticleGun_y_position << "    " << ParticleGun_z_position << "    " << 1e-3*TheNuclei->GetMass(RecoilIsotope,"MeV") << "\n";
    }
    
    // Done. Decide if we want to give outputs:
    if (Display_Output==kTRUE)
    {
        cout << "Multiplicity = " << ParticleGun_ExGenerator_NeutronMultiplicity << " | " << " Beam = " << ParticleGun_ExGenerator_BeamIsotope << " && Target = " << ParticleGun_ExGenerator_TargetIsotope << " && Beam E [MeV/u] = " << ParticleGun_ExGenerator_BeamEnergy << " | (type,E,theta,phi) = ";
        
        for (Int_t k = 0; k<ParticleGun_ExGenerator_NeutronMultiplicity; ++k)
        {
            cout << "(" << ParticleGun_ParticleType << "," << MiddleEnergy << "," << ParticleGun_ExGenerator_SingleTheta*180.0/TMath::Pi() << "," << ParticleGun_ExGenerator_PhiVector.at(k)*180.0/TMath::Pi() << ") ";
        }
        
        cout << "(" << RecoilIsotope << "," << RecoilTrack.T() - RecoilMass << "," << RecoilTrack.Theta()*180.0/TMath::Pi() << "," << RecoilTrack.Phi()*180.0/TMath::Pi() << ")\n";
    }
    
    // In the end, delete & destroy all vectors:
    for (Int_t k = 0; k<ParticleGun_ExGenerator_NeutronMultiplicity; ++k)
    {
        delete ParticleGun_ExGenerator_NeutronTracks.at(k);
    }
    ParticleGun_ExGenerator_NeutronTracks.clear();
    ParticleGun_ExGenerator_PhiVector.clear();
   
    if ((ThisEvent+1)%1000==0) {cout << "R3BASCIIFileGenerator Generated " << ThisEvent+1 << " events.\n";}
}
