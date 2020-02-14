Double_t R3BASCIIFileGenerator::Bisection_Function(Double_t const En, Int_t const TheMultiplicity)
{
    // Computes the proper neutron energy. This function is to be
    // used in a bisection-method.
    Bool_t Display_Output = kFALSE;
    
    // This function also needs the following class members:
    // Bool_t ParticleGun_ExGenerator_DegradeEnergy_MultipleTracks;
    // TString ParticleGun_ExGenerator_BeamIsotope;
    // TString ParticleGun_ExGenerator_TargetIsotope;
    // Double_t ParticleGun_ExGenerator_SingleTheta [rad]
    // std::vector<Double_t> ParticleGun_ExGenerator_PhiVector [rad]
    // std::vector<TLorentzVector*> ParticleGun_ExGenerator_NeutronTracks (elements are created by new!)
    // Int_t ParticleGun_ExGenerator_NeutronMultiplicity (vectors size equals this)
    // Double_t ParticleGun_ExGenerator_BeamEnergy [MeV/u]
    
    // Adjust internal multiplicity:
    Int_t InternalMultiplicity = ParticleGun_ExGenerator_NeutronMultiplicity;
    if (TheMultiplicity<ParticleGun_ExGenerator_NeutronMultiplicity) {InternalMultiplicity = TheMultiplicity;}
    
    // Define all known class members. We begin by defining the beam:
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
    
    // Then, define the target:
    TLorentzVector TargetTrack;
    TargetTrack.SetT(TheNuclei->GetMass(ParticleGun_ExGenerator_TargetIsotope,"MeV"));
    TargetTrack.SetX(0.0);
    TargetTrack.SetY(0.0);
    TargetTrack.SetZ(0.0);
    
    // Next, define the neutron tracks:
    Double_t NeutronMass = TheNuclei->GetMass(ParticleGun_ParticleType,"MeV");
    Double_t NeutronTimeComponent = NeutronMass + En;
    Double_t NeutronSpaceComponent = TMath::Sqrt(NeutronTimeComponent*NeutronTimeComponent - NeutronMass*NeutronMass);
    
    for (Int_t k = 0; k<InternalMultiplicity; ++k)
    {
        ParticleGun_ExGenerator_NeutronTracks.at(k)->SetT(NeutronTimeComponent);
        ParticleGun_ExGenerator_NeutronTracks.at(k)->SetX(NeutronSpaceComponent*TMath::Sin(ParticleGun_ExGenerator_SingleTheta)*TMath::Cos(ParticleGun_ExGenerator_PhiVector.at(k)));
        ParticleGun_ExGenerator_NeutronTracks.at(k)->SetY(NeutronSpaceComponent*TMath::Sin(ParticleGun_ExGenerator_SingleTheta)*TMath::Sin(ParticleGun_ExGenerator_PhiVector.at(k)));
        ParticleGun_ExGenerator_NeutronTracks.at(k)->SetZ(NeutronSpaceComponent*TMath::Cos(ParticleGun_ExGenerator_SingleTheta));
    }
    
    // Then, Define the recoil track:
    Int_t RecoilZ = TheNuclei->GetZ(ParticleGun_ExGenerator_BeamIsotope) + TheNuclei->GetZ(ParticleGun_ExGenerator_TargetIsotope) - InternalMultiplicity*TheNuclei->GetZ(ParticleGun_ParticleType);
    Int_t RecoilA = TheNuclei->GetA(ParticleGun_ExGenerator_BeamIsotope) + TheNuclei->GetA(ParticleGun_ExGenerator_TargetIsotope) - InternalMultiplicity*TheNuclei->GetA(ParticleGun_ParticleType);
    TString RecoilIsotope = TheNuclei->FindNucleus(RecoilZ,RecoilA);
    
    Double_t RecoilMass = TheNuclei->GetMass(RecoilIsotope,"MeV") + ParticleGun_ExGenerator_Recoil_ExcitationEnergy;
    TLorentzVector RecoilTrack;
    RecoilTrack.SetT(0.0);
    RecoilTrack.SetX(0.0);
    RecoilTrack.SetY(0.0);
    RecoilTrack.SetZ(0.0);
    
    for (Int_t k = 0; k<InternalMultiplicity; ++k)
    {
        RecoilTrack.SetX(RecoilTrack.X() - ParticleGun_ExGenerator_NeutronTracks.at(k)->X());
        RecoilTrack.SetY(RecoilTrack.Y() - ParticleGun_ExGenerator_NeutronTracks.at(k)->Y());
        RecoilTrack.SetZ(RecoilTrack.Z() - ParticleGun_ExGenerator_NeutronTracks.at(k)->Z());
    }
    
    RecoilTrack.SetX(RecoilTrack.X() + BeamTrack.X() + TargetTrack.X());
    RecoilTrack.SetY(RecoilTrack.Y() + BeamTrack.Y() + TargetTrack.Y());
    RecoilTrack.SetZ(RecoilTrack.Z() + BeamTrack.Z() + TargetTrack.Z());
    
    Double_t RecoilSpaceComponent = TMath::Sqrt(RecoilTrack.X()*RecoilTrack.X() + RecoilTrack.Y()*RecoilTrack.Y() + RecoilTrack.Z()*RecoilTrack.Z());
    Double_t RecoilTimeComponent = TMath::Sqrt(RecoilMass*RecoilMass + RecoilSpaceComponent*RecoilSpaceComponent);
    RecoilTrack.SetT(RecoilTimeComponent);
    
    // Then, define the function value:
    Double_t Function_Value = BeamTrack.T() + TargetTrack.T() - RecoilTrack.T();
    
    for (Int_t k = 0; k<InternalMultiplicity; ++k)
    {
        Function_Value = Function_Value - ParticleGun_ExGenerator_NeutronTracks.at(k)->T();
    }
    
    if (Display_Output==kTRUE) {cout << " En [MeV] = " << En << " | Function_Value = " << Function_Value << "\n";}
    
    // And, return the answer:
    return Function_Value;
}
