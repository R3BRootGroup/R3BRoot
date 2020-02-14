void R3BASCIIFileGenerator::Generate4Vector()
{
    // This function generates a single 4-momentum vector 
    // that can then be processed into an ASCII-file. The 
    // invariant mass will match the mass of the particle
    // requiested by the inputs-file.
    
    // Begin by obtaining the proper kinetic energy:
    Double_t Ekin = Generator->Uniform(ParticleGun_MinimalKineticEnergy,ParticleGun_MaximalKineticEnergy);
    
    // Next, reconstruct the total energy (everything is in MeV anyway):
    Double_t Etotal = Ekin*((Int_t) ParticleGun_MassNumber) + ParticleGun_TotalMass;
    
    // Next, we generate spherical coordinates Theta and Phi.
    Double_t Phi = Generator->Uniform(0.0,2.0*TMath::Pi());
    
    // We need different options for the generation of Theta:
    Double_t Theta = 0.0;
    
    if (RandomizeOption=="Uniform")
    {
        // The Sqrt is necessary to ensure uniform distribution in polar coordinates:
        Theta = ParticleGun_AngularSpread*TMath::Sqrt(Generator->Uniform(0.0,1.0));
    }
    else if (RandomizeOption=="Native_Central")
    {
        // Then, Theta is uniform, causing the center to be overpopulated:
        Theta = Generator->Uniform(0.0,ParticleGun_AngularSpread);
    }
    else
    {
        // Default setting is uniform:
        Theta = ParticleGun_AngularSpread*TMath::Sqrt(Generator->Uniform(0.0,1.0));
        ErrorMessage("The RandomizeOption <"+RandomizeOption+"> was NOT supported in Generate4Vector()!");
    }
    
    // Then, convert to radians:
    Theta = Theta*TMath::Pi()/180.0;
    
    // Then, we will generate x, y and z, as if we fire along the z-axis:
    Double_t pLength = TMath::Sqrt(Etotal*Etotal - ParticleGun_TotalMass*ParticleGun_TotalMass);
    Double_t px = pLength*TMath::Cos(Phi)*TMath::Sin(Theta);
    Double_t py = pLength*TMath::Sin(Phi)*TMath::Sin(Theta);
    Double_t pz = pLength*TMath::Cos(Theta);
    
    // Then, fill up the TLorentsVector. Notice that this is MeV!
    ParticleGun_OneVector->SetT(Etotal);
    ParticleGun_OneVector->SetX(px);
    ParticleGun_OneVector->SetY(py);
    ParticleGun_OneVector->SetZ(pz);
    
    // Now, we have to rotate it to the proper direction:
    ParticleGun_OneVector->RotateY(ParticleGun_Direction->Theta());
    ParticleGun_OneVector->RotateZ(ParticleGun_Direction->Phi());

    // And now, the variable ParticleGun_OneVector has a new
    // randomly-generated vector in it.
}
