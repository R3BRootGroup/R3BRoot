Double_t R3BInputClass::GetAvgBeamEnergy()
{
    // returns Average beam energy in MeV (NOT per u!)
    Double_t MinE = GetInputDouble("ParticleGun_MinimalKineticEnergy","MeV/u");
    Double_t MaxE = GetInputDouble("ParticleGun_MaximalKineticEnergy","MeV/u");
    TString ParticleType = GetInputString("ParticleGun_ParticleType");
    
    Int_t A = TheNuclei->GetA(ParticleType);
    Double_t Energy = ((MinE+MaxE)/2.0)*((Int_t) A);
    
    if (GetInputString("ParticleGun_ASCII_GunChoice")=="ExGenerated")
    {
        Energy = GetInputDouble("ParticleGun_ExGenerator_BeamEnergy","MeV/u")*((Int_t) A);
    }
    
    return Energy;
}

Double_t R3BInputClass::GetAvgBeamEnergyPerNucleon()
{
    // returns Average beam energy in MeV/u.
    Double_t MinE = GetInputDouble("ParticleGun_MinimalKineticEnergy","MeV/u");
    Double_t MaxE = GetInputDouble("ParticleGun_MaximalKineticEnergy","MeV/u");
    Double_t Energy = ((MinE+MaxE)/2.0);
    if (GetInputString("ParticleGun_ASCII_GunChoice")=="ExGenerated")
    {
        Energy = GetInputDouble("ParticleGun_ExGenerator_BeamEnergy","MeV/u");
    }
    return Energy;
}

Double_t R3BInputClass::GetBeamEnergyDiff()
{
    // returns difference in beam energy in MeV (NOT per u!)
    Double_t MinE = GetInputDouble("ParticleGun_MinimalKineticEnergy","MeV/u");
    Double_t MaxE = GetInputDouble("ParticleGun_MaximalKineticEnergy","MeV/u");
    TString ParticleType = GetInputString("ParticleGun_ParticleType");
    
    Int_t A = TheNuclei->GetA(ParticleType);
    Double_t Energy = (TMath::Abs(MinE-MaxE)/2.0)*((Int_t) A);
    return Energy;
}

Double_t R3BInputClass::GetAvgBeamBeta()
{
    // returns beta of the beam.
    
    // Obtain total kinetic energy of the beam (MeV, not MeV/u!)
    Double_t Energy = GetAvgBeamEnergy();
    
    // Obtain total particle mass:
    TString ParticleType = GetInputString("ParticleGun_ParticleType");
    Double_t Mass = TheNuclei->GetMass(ParticleType,"MeV");
    
    // Then, the time component of the 4-momentum is:
    Double_t p0 = Energy + Mass;
    
    // Then, computing gamma is easy:
    Double_t gamma = p0/Mass;
    
    // Then, compute beta:
    Double_t beta = TMath::Sqrt(1.0 - 1.0/(gamma*gamma));
    
    // Done:
    return beta;
}

Double_t R3BInputClass::GetTargetTime()
{
    Double_t BeamMass = TheNuclei->GetMass(GetInputString("ParticleGun_ParticleType"),"MeV");
    Double_t Target_X = GetInputDouble("TARGET_center_x_position","cm");
    Double_t Target_Y = GetInputDouble("TARGET_center_y_position","cm");
    Double_t Target_Z = GetInputDouble("TARGET_center_z_position","cm");
    Double_t Beam_X = GetInputDouble("ParticleGun_x_position","cm");
    Double_t Beam_Y = GetInputDouble("ParticleGun_y_position","cm");
    Double_t Beam_Z = GetInputDouble("ParticleGun_z_position","cm");
    Double_t Dist = TMath::Sqrt((Target_X-Beam_X)*(Target_X-Beam_X) + (Target_Y-Beam_Y)*(Target_Y-Beam_Y) + (Target_Z-Beam_Z)*(Target_Z-Beam_Z));
    Double_t cLight = 29.9792458;
    Double_t Etot = BeamMass + GetAvgBeamEnergy();
    Double_t gamma = Etot/BeamMass;
    Double_t beta = TMath::Sqrt(1.0 - 1.0/(gamma*gamma));
    Double_t Target_T = Dist/(beta*cLight);
    return Target_T;
}

Double_t R3BInputClass::GetNeuLANDStartTime()
{
    Double_t BeamMass = TheNuclei->GetMass(GetInputString("ParticleGun_ParticleType"),"MeV");
    Double_t Target_X = GetInputDouble("TARGET_center_x_position","cm");
    Double_t Target_Y = GetInputDouble("TARGET_center_y_position","cm");
    Double_t Target_Z = GetInputDouble("TARGET_center_z_position","cm");
    Double_t NeuLAND_X = GetInputDouble("NeuLAND_center_x_position","cm");
    Double_t NeuLAND_Y = GetInputDouble("NeuLAND_center_y_position","cm");
    Double_t NeuLAND_Z = GetInputDouble("NeuLAND_front_z_position","cm");
    Double_t Dist = TMath::Sqrt((Target_X-NeuLAND_X)*(Target_X-NeuLAND_X) + (Target_Y-NeuLAND_Y)*(Target_Y-NeuLAND_Y) + (Target_Z-NeuLAND_Z)*(Target_Z-NeuLAND_Z));
    Double_t cLight = 29.9792458;
    Double_t Etot = BeamMass + GetAvgBeamEnergy();
    Double_t gamma = Etot/BeamMass;
    Double_t beta = TMath::Sqrt(1.0 - 1.0/(gamma*gamma));
    Double_t NeuLAND_Time = Dist/(beta*cLight) + GetTargetTime();
    return NeuLAND_Time;
}

Double_t R3BInputClass::GetNEBULAStartTime()
{
    Double_t BeamMass = TheNuclei->GetMass(GetInputString("ParticleGun_ParticleType"),"MeV");
    Double_t Target_X = GetInputDouble("TARGET_center_x_position","cm");
    Double_t Target_Y = GetInputDouble("TARGET_center_y_position","cm");
    Double_t Target_Z = GetInputDouble("TARGET_center_z_position","cm");
    Double_t NEBULA_X = GetInputDouble("NEBULA_center_x_position","cm");
    Double_t NEBULA_Y = GetInputDouble("NEBULA_center_y_position","cm");
    Double_t NEBULA_Z = GetInputDouble("NEBULA_front_z_position","cm");
    Double_t Dist = TMath::Sqrt((Target_X-NEBULA_X)*(Target_X-NEBULA_X) + (Target_Y-NEBULA_Y)*(Target_Y-NEBULA_Y) + (Target_Z-NEBULA_Z)*(Target_Z-NEBULA_Z));
    Double_t cLight = 29.9792458;
    Double_t Etot = BeamMass + GetAvgBeamEnergy();
    Double_t gamma = Etot/BeamMass;
    Double_t beta = TMath::Sqrt(1.0 - 1.0/(gamma*gamma));
    Double_t NEBULA_Time = Dist/(beta*cLight) + GetTargetTime();
    return NEBULA_Time;
}

Double_t R3BInputClass::GetTotalPaddleThickness()
{
    Double_t gAlumniniumStrip_Thickness = GetInputDouble("NeuLAND_AluminiumFrame_Thickness","cm");
    Double_t gBC408Thickness = GetInputDouble("NeuLAND_BC408_BarThickness","cm")/2.0;
    Double_t gAlThickness = GetInputDouble("NeuLAND_ReflectionFoil_Thickness","cm");
    Double_t gTapeThickness = GetInputDouble("NeuLAND_WrappingTape_Thickness","cm");
    Double_t gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness/2.0;
    if ((gAlumniniumStrip_Thickness/2.0)<0.03) {gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness;
    if (gAlumniniumStrip_Thickness<0.03) {gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleThickNess = gPaddleDistance_xy;
    if (gPaddleDistance_z>gPaddleDistance_xy) {gPaddleThickNess = gPaddleDistance_z;}
    return gPaddleThickNess*2.0;
}

Double_t R3BInputClass::GetNEBULATotalPaddleThickness()
{
    Double_t gAlumniniumStrip_Thickness = GetInputDouble("NEBULA_AluminiumFrame_Thickness","cm");
    Double_t gBC408Thickness = GetInputDouble("NEBULA_BC408_BarThickness","cm")/2.0;
    Double_t gAlThickness = GetInputDouble("NEBULA_ReflectionFoil_Thickness","cm");
    Double_t gTapeThickness = GetInputDouble("NEBULA_WrappingTape_Thickness","cm");
    Double_t gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness/2.0;
    if ((gAlumniniumStrip_Thickness/2.0)<0.03) {gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness;
    if (gAlumniniumStrip_Thickness<0.03) {gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleThickNess = gPaddleDistance_xy;
    if (gPaddleDistance_z>gPaddleDistance_xy) {gPaddleThickNess = gPaddleDistance_z;}
    return gPaddleThickNess*2.0;
}

Double_t R3BInputClass::GetTotalPaddleThicknessXY()
{
    Double_t gAlumniniumStrip_Thickness = GetInputDouble("NeuLAND_AluminiumFrame_Thickness","cm");
    Double_t gBC408Thickness = GetInputDouble("NeuLAND_BC408_BarThickness","cm")/2.0;
    Double_t gAlThickness = GetInputDouble("NeuLAND_ReflectionFoil_Thickness","cm");
    Double_t gTapeThickness = GetInputDouble("NeuLAND_WrappingTape_Thickness","cm");
    Double_t gAirGabThickness = GetInputDouble("NeuLAND_Bars_AirGab","cm");
    Double_t gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness/2.0;
    if ((gAlumniniumStrip_Thickness/2.0)<gAirGabThickness) {gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAirGabThickness;}
    Double_t gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness;
    if (gAlumniniumStrip_Thickness<gAirGabThickness) {gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAirGabThickness;}
    Double_t gPaddleThickNess = gPaddleDistance_xy;
    if (gPaddleDistance_z>gPaddleDistance_xy) {gPaddleThickNess = gPaddleDistance_z;}
    return gPaddleDistance_xy*2.0;
}

Double_t R3BInputClass::GetTotalPaddleThicknessZ()
{
    Double_t gAlumniniumStrip_Thickness = GetInputDouble("NeuLAND_AluminiumFrame_Thickness","cm");
    Double_t gBC408Thickness = GetInputDouble("NeuLAND_BC408_BarThickness","cm")/2.0;
    Double_t gAlThickness = GetInputDouble("NeuLAND_ReflectionFoil_Thickness","cm");
    Double_t gTapeThickness = GetInputDouble("NeuLAND_WrappingTape_Thickness","cm");
    Double_t gAirGabThickness = GetInputDouble("NeuLAND_Bars_AirGab","cm");
    Double_t gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness/2.0;
    if ((gAlumniniumStrip_Thickness/2.0)<gAirGabThickness) {gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAirGabThickness;}
    Double_t gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness;
    if (gAlumniniumStrip_Thickness<gAirGabThickness) {gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAirGabThickness;}
    Double_t gPaddleThickNess = gPaddleDistance_xy;
    if (gPaddleDistance_z>gPaddleDistance_xy) {gPaddleThickNess = gPaddleDistance_z;}
    return gPaddleDistance_z*2.0;
}

Double_t R3BInputClass::GetVETOTotalPaddleThicknessZ()
{
    Double_t gAlumniniumStrip_Thickness = GetInputDouble("VETO_AluminiumFrame_Thickness","cm");
    Double_t gBC408Thickness = GetInputDouble("VETO_BC408_BarThickness_Z","cm")/2.0;
    Double_t gAlThickness = GetInputDouble("VETO_ReflectionFoil_Thickness","cm");
    Double_t gTapeThickness = GetInputDouble("VETO_WrappingTape_Thickness","cm");
    Double_t gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness/2.0;
    if ((gAlumniniumStrip_Thickness/2.0)<0.03) {gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness;
    if (gAlumniniumStrip_Thickness<0.03) {gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleThickNess = gPaddleDistance_xy;
    if (gPaddleDistance_z>gPaddleDistance_xy) {gPaddleThickNess = gPaddleDistance_z;}
    return gPaddleDistance_z*2.0;
}

Double_t R3BInputClass::GetVETOTotalPaddleThicknessXY()
{
    Double_t gAlumniniumStrip_Thickness = GetInputDouble("VETO_AluminiumFrame_Thickness","cm");
    Double_t gBC408Thickness = GetInputDouble("VETO_BC408_BarThickness_XY","cm")/2.0;
    Double_t gAlThickness = GetInputDouble("VETO_ReflectionFoil_Thickness","cm");
    Double_t gTapeThickness = GetInputDouble("VETO_WrappingTape_Thickness","cm");
    Double_t gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness/2.0;
    if ((gAlumniniumStrip_Thickness/2.0)<0.03) {gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness;
    if (gAlumniniumStrip_Thickness<0.03) {gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleThickNess = gPaddleDistance_xy;
    if (gPaddleDistance_z>gPaddleDistance_xy) {gPaddleThickNess = gPaddleDistance_z;}
    return gPaddleDistance_xy*2.0;
}

Double_t R3BInputClass::GetNEBULAVETOTotalPaddleThicknessZ()
{
    Double_t gAlumniniumStrip_Thickness = GetInputDouble("NEBULA_VET_AluminiumFrame_Thickness","cm");
    Double_t gBC408Thickness = GetInputDouble("NEBULA_VET_BC408_BarThickness_Z","cm")/2.0;
    Double_t gAlThickness = GetInputDouble("NEBULA_VET_ReflectionFoil_Thickness","cm");
    Double_t gTapeThickness = GetInputDouble("NEBULA_VET_WrappingTape_Thickness","cm");
    Double_t gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness/2.0;
    if ((gAlumniniumStrip_Thickness/2.0)<0.03) {gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness;
    if (gAlumniniumStrip_Thickness<0.03) {gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleThickNess = gPaddleDistance_xy;
    if (gPaddleDistance_z>gPaddleDistance_xy) {gPaddleThickNess = gPaddleDistance_z;}
    return gPaddleDistance_z*2.0;
}


Double_t R3BInputClass::GetNEBULAVETOTotalPaddleThicknessXY()
{
    Double_t gAlumniniumStrip_Thickness = GetInputDouble("NEBULA_VET_AluminiumFrame_Thickness","cm");
    Double_t gBC408Thickness = GetInputDouble("NEBULA_VET_BC408_BarThickness_XY","cm")/2.0;
    Double_t gAlThickness = GetInputDouble("NEBULA_VET_ReflectionFoil_Thickness","cm");
    Double_t gTapeThickness = GetInputDouble("NEBULA_VET_WrappingTape_Thickness","cm");
    Double_t gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness/2.0;
    if ((gAlumniniumStrip_Thickness/2.0)<0.03) {gPaddleDistance_xy = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + gAlumniniumStrip_Thickness;
    if (gAlumniniumStrip_Thickness<0.03) {gPaddleDistance_z = gBC408Thickness + gAlThickness + gTapeThickness + 0.03;}
    Double_t gPaddleThickNess = gPaddleDistance_xy;
    if (gPaddleDistance_z>gPaddleDistance_xy) {gPaddleThickNess = gPaddleDistance_z;}
    return gPaddleDistance_xy*2.0;
}
