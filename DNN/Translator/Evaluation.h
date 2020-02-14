void R3BNeuLANDTranslator::InitializeNeutronMatrix()
{
    // Generate the neutron separation matrices:
    MultMatrix = new Double_t*[MaxMultiplicity+2];
    InteractionMatrix = new Double_t*[MaxMultiplicity+2];
    DetectionMatrix = new Double_t*[MaxMultiplicity+2];
    MultNEBULAMatrix = new Double_t*[MaxMultiplicity+2];
    InteractionNEBULAMatrix = new Double_t*[MaxMultiplicity+2];
    DetectionNEBULAMatrix = new Double_t*[MaxMultiplicity+2];
    MultDoubleMatrix = new Double_t*[MaxMultiplicity+2];
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+2); ++krow)
    {
        MultMatrix[krow] = new Double_t[MaxMultiplicity+1];
        InteractionMatrix[krow] = new Double_t[MaxMultiplicity+1];
        DetectionMatrix[krow] = new Double_t[MaxMultiplicity+1];
        MultNEBULAMatrix[krow] = new Double_t[MaxMultiplicity+1];
        InteractionNEBULAMatrix[krow] = new Double_t[MaxMultiplicity+1];
        DetectionNEBULAMatrix[krow] = new Double_t[MaxMultiplicity+1];
        MultDoubleMatrix[krow] = new Double_t[MaxMultiplicity+1];
        
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            MultMatrix[krow][kcol] = 0.0;
            InteractionMatrix[krow][kcol] = 0.0;
            DetectionMatrix[krow][kcol] = 0.0;
            MultNEBULAMatrix[krow][kcol] = 0.0;
            InteractionNEBULAMatrix[krow][kcol] = 0.0;
            DetectionNEBULAMatrix[krow][kcol] = 0.0;
            MultDoubleMatrix[krow][kcol] = 0.0;
        }
    }
    
    // The row index is the determined (possibly wrong) multiplicity.
    // the col index is the true mult. The extra row index, is for the totals.
}

void R3BNeuLANDTranslator::CreateControlHistograms()
{
    // Initialize histograms:
    Double_t Distance = TMath::Sqrt(TMath::Power(TMath::Abs(ParticleGun_x_position - TargetX),2.0) + 
                                    TMath::Power(TMath::Abs(ParticleGun_y_position - TargetY),2.0) + 
                                    TMath::Power(TMath::Abs(ParticleGun_z_position - TargetZ),2.0));
    
    Distance = Distance + TMath::Sqrt(TMath::Power(TMath::Abs(NeuLAND_Center_X - TargetX),2.0) + 
                                      TMath::Power(TMath::Abs(NeuLAND_Center_Y - TargetY),2.0) + 
                                      TMath::Power(TMath::Abs(NeuLAND_Front_Z  - TargetZ),2.0));
    
    Double_t cLight = 29.9792458;
    Double_t TravelTime = Distance/(cLight*BeamBeta);
    
    Multiplicity_Gun = new TH1I("Multiplicity_Gun"+OutputNameTag,"ParticleGun Fired Neutron Multiplicity",MaxMultiplicity+1,0,MaxMultiplicity+1);
    Multiplicity_Prim = new TH1I("Multiplicity_Prim"+OutputNameTag,"Incoming particle Multiplicity from NeuLAND Primaries",MaxMultiplicity+1,0,MaxMultiplicity+1);
    Multiplicity_Signal = new TH1I("Multiplicity_Signal"+OutputNameTag,"Incoming particle Multiplicity from R3BSignals",MaxMultiplicity+1,0,MaxMultiplicity+1);
    PrimToSignal_Distance = new TH1D("PrimToSignal_Distance"+OutputNameTag,"PrimToSignal_Distance"+OutputNameTag,200,0.0,NeuLAND_Total_Bar_Length);
    PrimToSignal_Distance_Conditional = new TH1D("PrimToSignal_Distance_Conditional"+OutputNameTag,"PrimToSignal_Distance_Conditional"+OutputNameTag,200,0.0,NeuLAND_Total_Bar_Length);
    AllTimes = new TH1D("AllTimes"+OutputNameTag,"AllTimes"+OutputNameTag,1000,0.0,2.0*TravelTime);
    PrimTimes = new TH1D("PrimTimes"+OutputNameTag,"PrimTimes"+OutputNameTag,1000,0.0,2.0*TravelTime);
    
    if (DistanceCheck==kTRUE)
    {
        Double_t MaxLength = NeuLAND_Total_Bar_Length;
        if (MaxLength<((Int_t) nPaddlesPerPlane)*NeuLAND_TotalBarThicknessXY) {MaxLength = ((Int_t) nPaddlesPerPlane)*NeuLAND_TotalBarThicknessXY;}
    
        MCPrint = new TH3D("MCPrint"+OutputNameTag,"MCPrint"+OutputNameTag,nPaddlesPerPlane,-0.5*MaxLength,0.5*MaxLength,nPaddlesPerPlane,-0.5*MaxLength,0.5*MaxLength,nDoublePlanes*2,NeuLAND_Front_Z,NeuLAND_Front_Z+NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes)*2.0);
        SignalPrint = new TH3D("MCPrint"+OutputNameTag,"MCPrint"+OutputNameTag,nPaddlesPerPlane,-0.5*MaxLength,0.5*MaxLength,nPaddlesPerPlane,-0.5*MaxLength,0.5*MaxLength,nDoublePlanes*2,NeuLAND_Front_Z,NeuLAND_Front_Z+NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes)*2.0);

        MCPrint_Indu = new TH3D*[MaxMultiplicity];
        SignalPrint_Indu = new TH3D*[MaxMultiplicity];
        TString st = "";
        TString kstr = "";
    
        for (Int_t k = 0; k<MaxMultiplicity; ++k)
        {
            kstr = st.Itoa(k,10);
            MCPrint_Indu[k] = new TH3D("MCPrint_track"+kstr+OutputNameTag,"MCPrint_track"+kstr+OutputNameTag,nPaddlesPerPlane,-0.5*MaxLength,0.5*MaxLength,nPaddlesPerPlane,-0.5*MaxLength,0.5*MaxLength,nDoublePlanes*2,NeuLAND_Front_Z,NeuLAND_Front_Z+NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes)*2.0);
            SignalPrint_Indu[k] = new TH3D("SignalPrint_track"+kstr+OutputNameTag,"SignalPrint_track"+kstr+OutputNameTag,nPaddlesPerPlane,-0.5*MaxLength,0.5*MaxLength,nPaddlesPerPlane,-0.5*MaxLength,0.5*MaxLength,nDoublePlanes*2,NeuLAND_Front_Z,NeuLAND_Front_Z+NeuLAND_TotalBarThicknessZ*((Int_t) nDoublePlanes)*2.0);
        }
    }
}

    
void R3BNeuLANDTranslator::FillControlHistograms()
{
    // Fill in the control histograms:
    Multiplicity_Gun->Fill(ThisGunMultiplicity);
    if (ThisNeuLANDMultiplicity==ThisGunMultiplicity) {Multiplicity_Prim->Fill(ThisNeuLANDMultiplicity);}
    if (ThisDetectedMultiplicity==ThisGunMultiplicity) {Multiplicity_Signal->Fill(ThisDetectedMultiplicity);}
    if (FillTheDistanceHistograms==kTRUE) {FillDistanceHistograms();}
    
    Int_t nSignals = fArraySignals->GetEntries();
    R3BSignal* ThisSignal;
    
    for (Int_t k = 0; k<nSignals; ++k)
    {
        ThisSignal = (R3BSignal*) fArraySignals->At(k);
        AllTimes->Fill(ThisSignal->GetTime());
        if (ThisSignal->IsPrimarySim()==kTRUE) {PrimTimes->Fill(ThisSignal->GetTime());}
    }
}

void R3BNeuLANDTranslator::UpdateSingleCounters()
{
    // Update:
    if (ThisGunMultiplicity==1) {GunMult_Counter = GunMult_Counter + 1;}
    if ((ThisNeuLANDMultiplicity==1)&&(ThisGunMultiplicity==1)) {NeuLANDMult_Counter = NeuLANDMult_Counter + 1;}
    if ((ThisDetectedMultiplicity==1)&&(ThisGunMultiplicity==1)) {DetectedMult_Counter = DetectedMult_Counter + 1;}
}

void R3BNeuLANDTranslator::UpdateMultMatrix()
{
    // NeuLAND:
    
    // Update the gun/detected matrix:
    if ((ThisDetectedMultiplicity>=0)&&(ThisDetectedMultiplicity<=MaxMultiplicity)&&(ThisGunMultiplicity>=0)&&(ThisGunMultiplicity<=MaxMultiplicity))
    {
        MultMatrix[ThisDetectedMultiplicity][ThisGunMultiplicity] = MultMatrix[ThisDetectedMultiplicity][ThisGunMultiplicity] + 1.0;
        MultMatrix[MaxMultiplicity+1][ThisGunMultiplicity] = MultMatrix[MaxMultiplicity+1][ThisGunMultiplicity] + 1.0;
    }
    
    // Same for the gun/neuland matrix:
    if ((ThisNeuLANDMultiplicity>=0)&&(ThisNeuLANDMultiplicity<=MaxMultiplicity)&&(ThisGunMultiplicity>=0)&&(ThisGunMultiplicity<=MaxMultiplicity))
    {
        InteractionMatrix[ThisNeuLANDMultiplicity][ThisGunMultiplicity] = InteractionMatrix[ThisNeuLANDMultiplicity][ThisGunMultiplicity] + 1.0;
        InteractionMatrix[MaxMultiplicity+1][ThisGunMultiplicity] = InteractionMatrix[MaxMultiplicity+1][ThisGunMultiplicity] + 1.0;
    }
    
    // Update and for neuland/detected:
    if ((ThisDetectedMultiplicity>=0)&&(ThisDetectedMultiplicity<=MaxMultiplicity)&&(ThisNeuLANDMultiplicity>=0)&&(ThisNeuLANDMultiplicity<=MaxMultiplicity))
    {
        DetectionMatrix[ThisDetectedMultiplicity][ThisNeuLANDMultiplicity] = DetectionMatrix[ThisDetectedMultiplicity][ThisNeuLANDMultiplicity] + 1.0;
        DetectionMatrix[MaxMultiplicity+1][ThisNeuLANDMultiplicity] = DetectionMatrix[MaxMultiplicity+1][ThisNeuLANDMultiplicity] + 1.0;
    }
    
    // -------------------------------------------------------------------------------------------------
    
    // NEBULA:
    if (UseNEBULA==kTRUE)
    {
        // Update the gun/detected matrix:
        if ((ThisNEBDetectedMultiplicity>=0)&&(ThisNEBDetectedMultiplicity<=MaxMultiplicity)&&(ThisGunMultiplicity>=0)&&(ThisGunMultiplicity<=MaxMultiplicity))
        {
            MultNEBULAMatrix[ThisNEBDetectedMultiplicity][ThisGunMultiplicity] = MultNEBULAMatrix[ThisNEBDetectedMultiplicity][ThisGunMultiplicity] + 1.0;
            MultNEBULAMatrix[MaxMultiplicity+1][ThisGunMultiplicity] = MultNEBULAMatrix[MaxMultiplicity+1][ThisGunMultiplicity] + 1.0;
        }
    
        // Same for the gun/neuland matrix:
        if ((ThisNEBULAMultiplicity>=0)&&(ThisNEBULAMultiplicity<=MaxMultiplicity)&&(ThisGunMultiplicity>=0)&&(ThisGunMultiplicity<=MaxMultiplicity))
        {
            InteractionNEBULAMatrix[ThisNEBULAMultiplicity][ThisGunMultiplicity] = InteractionNEBULAMatrix[ThisNEBULAMultiplicity][ThisGunMultiplicity] + 1.0;
            InteractionNEBULAMatrix[MaxMultiplicity+1][ThisGunMultiplicity] = InteractionNEBULAMatrix[MaxMultiplicity+1][ThisGunMultiplicity] + 1.0;
        }
    
        // Update and for neuland/detected:
        if ((ThisNEBDetectedMultiplicity>=0)&&(ThisNEBDetectedMultiplicity<=MaxMultiplicity)&&(ThisNEBULAMultiplicity>=0)&&(ThisNEBULAMultiplicity<=MaxMultiplicity))
        {
            DetectionNEBULAMatrix[ThisNEBDetectedMultiplicity][ThisNEBULAMultiplicity] = DetectionNEBULAMatrix[ThisNEBDetectedMultiplicity][ThisNEBULAMultiplicity] + 1.0;
            DetectionNEBULAMatrix[MaxMultiplicity+1][ThisNEBULAMultiplicity] = DetectionNEBULAMatrix[MaxMultiplicity+1][ThisNEBULAMultiplicity] + 1.0;
        }
    
        // -------------------------------------------------------------------------------------------------
    
        // Combined:
        if (((ThisDetectedMultiplicity+ThisNEBDetectedMultiplicity)>=0)&&((ThisDetectedMultiplicity+ThisNEBDetectedMultiplicity)<=MaxMultiplicity)&&(ThisGunMultiplicity>=0)&&(ThisGunMultiplicity<=MaxMultiplicity))
        {
            MultDoubleMatrix[ThisDetectedMultiplicity+ThisNEBDetectedMultiplicity][ThisGunMultiplicity] = MultDoubleMatrix[ThisDetectedMultiplicity+ThisNEBDetectedMultiplicity][ThisGunMultiplicity] + 1.0;
            MultDoubleMatrix[MaxMultiplicity+1][ThisGunMultiplicity] = MultDoubleMatrix[MaxMultiplicity+1][ThisGunMultiplicity] + 1.0;
        }
    }
}



