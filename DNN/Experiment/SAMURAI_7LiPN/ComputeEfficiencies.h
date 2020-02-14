void CrossSections_7LiPN::ComputeEfficiencies()
{
    // Begin by checking that all vectors have the proper size:
    Int_t Size = PeakAreas.size();
    Bool_t SizeCheck = kTRUE;
    TString st = "";
    TString kstr = "";
    
    if (Size<=0) {SizeCheck = kFALSE; ErrorMessage("We cannot compute efficiencies if the size of the PeakAreas-vector is <=0!");}
    if (Size!=PeakAreaErrors.size()) {SizeCheck = kFALSE; ErrorMessage("The size of PeakAreas & PeakAreaErrors has to be equal!");}
    if (Size!=MonteCarlo_PeakAreas.size()) {SizeCheck = kFALSE; ErrorMessage("The size of PeakAreas & MonteCarlo_PeakAreas has to be equal!");}
    if (Size!=MonteCarlo_PeakAreaErrors.size()) {SizeCheck = kFALSE; ErrorMessage("The size of PeakAreas & MonteCarlo_PeakAreaErrors has to be equal!");}
    if (MC_Detection_Efficiency<=0.0) {SizeCheck = kFALSE; ErrorMessage("The MC detection efficiency has to be >0!");}
    if (MC_Detection_Efficiency>1.0) {SizeCheck = kFALSE; ErrorMessage("The MC detection efficiency has to be <=1!");}
    
    // Next, check that the MC areas are positive:
    if (SizeCheck==kTRUE)
    {
        for (Int_t k = 0; k<Size; ++k)
        {
            kstr = st.Itoa(k,10);
            if (MonteCarlo_PeakAreas.at(k)<1.0) {SizeCheck = kFALSE; ErrorMessage("The MC peak area in bin "+kstr+" is not >1.0!");}
            if (MonteCarlo_PeakAreas.at(k)<=MonteCarlo_PeakAreaErrors.at(k)) {SizeCheck = kFALSE; ErrorMessage("The MC peak area in bin "+kstr+" is <= its error!");}
            // if (MonteCarlo_PeakAreas.at(k)<PeakAreas.at(k)) {SizeCheck = kFALSE; ErrorMessage("The MC area is smaller than the peak area for bin "+kstr+"!");}
        }
    }
    
    // Then, we can proceed:
    if (SizeCheck==kTRUE)
    {
        // Resize the efficiency vectors:
        Epsilon.resize(Size);
        Epsilon_Error.resize(Size);
        Epsilon_Systematic.resize(Size);
        
        // Then, divide peak areas to compute epsilon:
        for (Int_t k = 0; k<Size; ++k)
        {
            // Execute division:
            Epsilon.at(k) = PeakAreas.at(k)/MonteCarlo_PeakAreas.at(k);
            Epsilon_Error.at(k) = TMath::Sqrt((PeakAreaErrors.at(k)*PeakAreaErrors.at(k))/(MonteCarlo_PeakAreas.at(k)*MonteCarlo_PeakAreas.at(k)) +
                                              (PeakAreas.at(k)*PeakAreas.at(k)*MonteCarlo_PeakAreaErrors.at(k)*MonteCarlo_PeakAreaErrors.at(k))/
                                              (MonteCarlo_PeakAreas.at(k)*MonteCarlo_PeakAreas.at(k)*MonteCarlo_PeakAreas.at(k)*MonteCarlo_PeakAreas.at(k)));
            
            // Execute overall normalization:
            Epsilon.at(k) = Epsilon.at(k)*MC_Detection_Efficiency;
            Epsilon_Error.at(k) = TMath::Sqrt(Epsilon_Error.at(k)*Epsilon_Error.at(k)*MC_Detection_Efficiency*MC_Detection_Efficiency + 
                                              Epsilon.at(k)*Epsilon.at(k)*MC_Detection_Efficiency_Error*MC_Detection_Efficiency_Error);
            
            // Take care of boundary cases:
            if (Epsilon.at(k)-Epsilon_Error.at(k)<0.0)
            {
                Epsilon_Error.at(k) = Epsilon.at(k);
            }
            
            if (Epsilon.at(k)+Epsilon_Error.at(k)>1.0)
            {
                Epsilon_Error.at(k) = 1.0 - Epsilon.at(k);
            }
            
            // Systematic errors cannot be computed from the data straight forward.
            // the user has to manually provide them. Hence, we will write zeros here
            // so that between writing & reading, the user can provide the proper inputs:
            Epsilon_Systematic.at(k) = 0.0;
        }
        
        // Next, we will generate output:
        TString TextFileName = OutputPath + "/CrossSection_Efficiencies_" + BranchTitle;
        if (Experiment_Is110MeV==kTRUE) {TextFileName = TextFileName + "_110MeV";}
        if (Experiment_Is250MeV==kTRUE) {TextFileName = TextFileName + "_250MeV";}
        if (Experiment_IsValidation==kTRUE) {TextFileName = TextFileName + "_Validation";}
        else {TextFileName = TextFileName + "_Experiment";}
        TextFileName = TextFileName + ".txt";
        
        std::ofstream EffTextFile (TextFileName.Data(), std::ofstream::out);
        
        for (Int_t k = 0; k<Size; ++k)
        {
            EffTextFile << Epsilon.at(k) << " " << Epsilon_Error.at(k) << " " << Epsilon_Systematic.at(k) << "\n";
        }
        
        EffTextFile.close();
        
        // Done.
    }
}
