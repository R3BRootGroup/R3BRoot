void R3BNeulandCADDigitizer::SetParameters(R3BInputClass* inp)
{
    // Calls all functions to set parameters of digitizer at once:
    
    // Link:
    Inputs = inp;
    Total_SetParameters_Worked = kFALSE;
    
    // Check if this is OK:
    if (Inputs->IsLinked()==kTRUE)
    {
        // Then, we can continue.
        
        // Declare local parameters:
        Double_t Local_fPaddleHalfLength;
        Double_t Local_fPMTThresh;
        Double_t Local_fSaturationCoefficient;
        Double_t Local_fTimeRes;
        Double_t Local_fEResRel;
        Double_t Local_fIntegrationTime;
        Double_t Local_fcMedium;
        Double_t Local_fAttenuation;
        Double_t Local_fLambda;
        Bool_t   Local_PrintPar;
        
        // Retrieve parameters:
        if ((MCpoint_BranchName=="R3BNeulandPoint")||(MCpoint_BranchName=="R3BCADNeulandPoint"))
        {
            Local_fPaddleHalfLength = Inputs->GetInputDouble("NeuLAND_Total_BarLength","cm")/2.0 + Inputs->GetInputDouble("NeuLAND_EndPoint_ConeLength","cm")*2.0;
            Local_fPMTThresh = Inputs->GetInputDouble("NeuLAND_Digitizer_PMT_Threshold","MeV");
            Local_fSaturationCoefficient = 1.0/Inputs->GetInputDouble("NeuLAND_Digitizer_SaturationEnergy","MeV");
            Local_fTimeRes = Inputs->GetInputDouble("NeuLAND_Digitizer_Time_Resolution","ns");
            Local_fEResRel = Inputs->GetInputDouble("NeuLAND_Digitizer_Energy_Resolution","MeV");
            Local_fIntegrationTime = Inputs->GetInputDouble("NeuLAND_Digitizer_IntegrationTime","ns");
            Local_fcMedium = Inputs->GetInputDouble("NeuLAND_Digitizer_Effective_LightSpeed","cm/ns");
            Local_fAttenuation = 1.0/Inputs->GetInputDouble("NeuLAND_Digitizer_AttenuationLength","cm");
            Local_fLambda = 1.0/Inputs->GetInputDouble("NeuLAND_Digitizer_PMTPulse_DecayTime","ns");
            Local_PrintPar = Inputs->GetInputBoolian("NeuLAND_Digitizer_PrintParameters");
        }
        else if (MCpoint_BranchName=="R3BCADVETOPoint")
        {
            Local_fPaddleHalfLength = Inputs->GetInputDouble("VETO_Total_BarLength","cm")/2.0 + Inputs->GetInputDouble("VETO_EndPoint_ConeLength","cm")*2.0;
            Local_fPMTThresh = Inputs->GetInputDouble("VETO_Digitizer_PMT_Threshold","MeV");
            Local_fSaturationCoefficient = 1.0/Inputs->GetInputDouble("VETO_Digitizer_SaturationEnergy","MeV");
            Local_fTimeRes = Inputs->GetInputDouble("VETO_Digitizer_Time_Resolution","ns");
            Local_fEResRel = Inputs->GetInputDouble("VETO_Digitizer_Energy_Resolution","MeV");
            Local_fIntegrationTime = Inputs->GetInputDouble("VETO_Digitizer_IntegrationTime","ns");
            Local_fcMedium = Inputs->GetInputDouble("VETO_Digitizer_Effective_LightSpeed","cm/ns");
            Local_fAttenuation = 1.0/Inputs->GetInputDouble("VETO_Digitizer_AttenuationLength","cm");
            Local_fLambda = 1.0/Inputs->GetInputDouble("VETO_Digitizer_PMTPulse_DecayTime","ns");
            Local_PrintPar = Inputs->GetInputBoolian("VETO_Digitizer_PrintParameters");
        }
        else if (MCpoint_BranchName=="R3BCADNEBULAPoint")
        {
            Local_fPaddleHalfLength = Inputs->GetInputDouble("NEBULA_Total_BarLength","cm")/2.0 + Inputs->GetInputDouble("NEBULA_EndPoint_ConeLength","cm")*2.0;
            Local_fPMTThresh = Inputs->GetInputDouble("NEBULA_Digitizer_PMT_Threshold","MeV");
            Local_fSaturationCoefficient = 1.0/Inputs->GetInputDouble("NEBULA_Digitizer_SaturationEnergy","MeV");
            Local_fTimeRes = Inputs->GetInputDouble("NEBULA_Digitizer_Time_Resolution","ns");
            Local_fEResRel = Inputs->GetInputDouble("NEBULA_Digitizer_Energy_Resolution","MeV");
            Local_fIntegrationTime = Inputs->GetInputDouble("NEBULA_Digitizer_IntegrationTime","ns");
            Local_fcMedium = Inputs->GetInputDouble("NEBULA_Digitizer_Effective_LightSpeed","cm/ns");
            Local_fAttenuation = 1.0/Inputs->GetInputDouble("NEBULA_Digitizer_AttenuationLength","cm");
            Local_fLambda = 1.0/Inputs->GetInputDouble("NEBULA_Digitizer_PMTPulse_DecayTime","ns");
            Local_PrintPar = Inputs->GetInputBoolian("NEBULA_Digitizer_PrintParameters");
        }
        else if (MCpoint_BranchName=="R3BCADNEBULAVETOPoint")
        {
            Local_fPaddleHalfLength = Inputs->GetInputDouble("NEBULA_VET_Total_BarLength","cm")/2.0 + Inputs->GetInputDouble("NEBULA_VET_EndPoint_ConeLength","cm")*2.0;
            Local_fPMTThresh = Inputs->GetInputDouble("NEBULA_VET_Digitizer_PMT_Threshold","MeV");
            Local_fSaturationCoefficient = 1.0/Inputs->GetInputDouble("NEBULA_VET_Digitizer_SaturationEnergy","MeV");
            Local_fTimeRes = Inputs->GetInputDouble("NEBULA_VET_Digitizer_Time_Resolution","ns");
            Local_fEResRel = Inputs->GetInputDouble("NEBULA_VET_Digitizer_Energy_Resolution","MeV");
            Local_fIntegrationTime = Inputs->GetInputDouble("NEBULA_VET_Digitizer_IntegrationTime","ns");
            Local_fcMedium = Inputs->GetInputDouble("NEBULA_VET_Digitizer_Effective_LightSpeed","cm/ns");
            Local_fAttenuation = 1.0/Inputs->GetInputDouble("NEBULA_VET_Digitizer_AttenuationLength","cm");
            Local_fLambda = 1.0/Inputs->GetInputDouble("NEBULA_VET_Digitizer_PMTPulse_DecayTime","ns");
            Local_PrintPar = Inputs->GetInputBoolian("NEBULA_VET_Digitizer_PrintParameters");
        }
        else
        {
            Local_fPaddleHalfLength = 125.0;
            Local_fPMTThresh = 1.0;
            Local_fSaturationCoefficient = 0.012;
            Local_fTimeRes = 0.150;
            Local_fEResRel = 0.050;
            Local_fIntegrationTime = 400.0;
            Local_fcMedium = 14.0;
            Local_fAttenuation = 0.008;
            Local_fLambda = 1.0/2.1;
            Local_PrintPar = kTRUE;
        }
        
        // Pass on these parameters:
        AllowManualParameters();
        SetPaddleHalfLength(Local_fPaddleHalfLength);
        SetPMTThreshold(Local_fPMTThresh);
        SetSaturationCoefficient(Local_fSaturationCoefficient);
        SetTimeRes(Local_fTimeRes);
        SetERes(Local_fEResRel);
        SetIntegrationTime(Local_fIntegrationTime);
        SetcMedium(Local_fcMedium);
        SetAttenuation(Local_fAttenuation);
        SetLambda(Local_fLambda);
        if (Local_PrintPar==kTRUE) {SetPrintMode();}
        else {SetBatchMode();}
        
        // Note that it worked:
        Total_SetParameters_Worked = kTRUE;
    }
}
