void CrossSections_7LiPN::ComputeTheCrossSection()
{
    // Begin by obtaining the amount of theta-bins & the size:
    Excitation_Energy_7LiPN* Ex = new Excitation_Energy_7LiPN();
    Int_t NThetaBins = Ex->GetNScatterBins();
    Double_t ThetaStepDeg = Ex->GetScatterStep();
    Double_t ThetaStepRad = ThetaStepDeg*TMath::Pi()/180.0;
    delete Ex;
    
    TString st = "";
    TString kstr = "";
    
    // Then, check that all vectors have the proper size:
    Bool_t SizeCheck = kTRUE;
    if (NThetaBins!=PeakAreas.size()) {SizeCheck = kFALSE; ErrorMessage("PeakAreas did not contain an area for each Theta-bin");}
    if (NThetaBins!=PeakAreaErrors.size()) {SizeCheck = kFALSE; ErrorMessage("PeakAreaErrors did not contain an area for each Theta-bin");}
    if (NThetaBins!=Epsilon.size()) {SizeCheck = kFALSE; ErrorMessage("Epsilon did not contain an area for each Theta-bin");}
    if (NThetaBins!=Epsilon_Error.size()) {SizeCheck = kFALSE; ErrorMessage("Epsilon_Error did not contain an area for each Theta-bin");}
    
    // Check that all of them are positive numbers:
    if (SizeCheck==kTRUE)
    {
        for (Int_t k = 0; k<NThetaBins; ++k)
        {
            kstr = st.Itoa(k,10);
            if (PeakAreas.at(k)<=0.0) {SizeCheck = kFALSE; ErrorMessage("PeakAreas.at("+kstr+") was <=0!");}
            if (PeakAreaErrors.at(k)<=0.0) {SizeCheck = kFALSE; ErrorMessage("PeakAreaErrors.at("+kstr+") was <=0!");}
            if (Epsilon.at(k)<=0.0) {SizeCheck = kFALSE; ErrorMessage("Epsilon.at("+kstr+") was <=0!");}
            if (Epsilon_Error.at(k)<=0.0) {SizeCheck = kFALSE; ErrorMessage("Epsilon_Error.at("+kstr+") was <=0!");}
        }
    }
    
    // Check again:
    if (SizeCheck==kTRUE)
    {
        // Then, we can start computing cross sections in counts/sterad:
        Double_t* Xvalue = new Double_t[NThetaBins];
        Double_t* Xerror = new Double_t[NThetaBins];
        Double_t* Yvalue = new Double_t[NThetaBins];
        Double_t* Yerror = new Double_t[NThetaBins];
        Double_t* Ysyste = new Double_t[NThetaBins];
        Double_t InnerRadius;
        Double_t OuterRadius;
        Double_t ThisAcceptance;
        Double_t ThisYavg = 0.0;
        
        for (Int_t k = 0; k<NThetaBins; ++k)
        {
            // Start with computing the acceptance. 
            // NOTE: It is assumed that the acceptance is complete and contains no errors.
            // This is only OK if the max. Theta cone from the target falls inside NeuLAND.
            InnerRadius = ThetaStepRad*((Int_t) k);
            OuterRadius = ThetaStepRad*((Int_t) (k+1));
            ThisAcceptance = TMath::Pi()*(OuterRadius*OuterRadius - InnerRadius*InnerRadius);
            
            // Then, assign values:
            Xvalue[k] = ThetaStepDeg*((Int_t) k);
            Xvalue[k] = Xvalue[k] + 0.5*ThetaStepDeg;
            
            Xerror[k] = ThetaStepDeg/(2.0*TMath::Sqrt(3.0));
            
            Yvalue[k] = PeakAreas.at(k)/(ThisAcceptance*Epsilon.at(k));
            
            Yerror[k] = (PeakAreaErrors.at(k)*PeakAreaErrors.at(k))/(ThisAcceptance*ThisAcceptance*Epsilon.at(k)*Epsilon.at(k));
            Yerror[k] = Yerror[k] + (PeakAreas.at(k)*PeakAreas.at(k)*Epsilon_Error.at(k)*Epsilon_Error.at(k))/
                                    (ThisAcceptance*ThisAcceptance*Epsilon.at(k)*Epsilon.at(k)*Epsilon.at(k)*Epsilon.at(k));
            Yerror[k] = TMath::Sqrt(Yerror[k]);
            
            Ysyste[k] = TMath::Abs((PeakAreas.at(k)*Epsilon_Systematic.at(k))/(ThisAcceptance*Epsilon.at(k)*Epsilon.at(k)));
            
            ThisYavg = ThisYavg + Yvalue[k];
        }
        ThisYavg = ThisYavg/((Int_t) NThetaBins);
        
        // Next, compose the TGraphErrors:
        TheCrossSection = new TGraphErrors(NThetaBins,&Xvalue[0],&Yvalue[0],&Xerror[0],&Yerror[0]);
        
        // Also, compose the systematic error band:
        TheSystErrorBand = new TH1D("Systematic_ErrorBand_"+BranchTitle,"Systematic_ErrorBand_"+BranchTitle,NThetaBins,0.0,((Int_t) NThetaBins)*ThetaStepDeg);
        TheSystErrorPlot = new TH1D("Systematic_ErrorPlot_"+BranchTitle,"Systematic_ErrorPlot_"+BranchTitle,NThetaBins,0.0,((Int_t) NThetaBins)*ThetaStepDeg);
        
        for (Int_t k = 0; k<NThetaBins; ++k)
        {
            TheSystErrorBand->SetBinContent(k+1,ThisYavg);
            TheSystErrorBand->SetBinError(k+1,Ysyste[k]);
            
            TheSystErrorPlot->SetBinContent(k+1,ThisYavg*BandPosition);
            if (UseCrossLogScale==kTRUE) {TheSystErrorPlot->SetBinError(k+1,Ysyste[k]*BandPosition);}
            else {TheSystErrorPlot->SetBinError(k+1,Ysyste[k]*1.0);}
        }
        
        // Then, write it to an output file:
        TString CrossFileName = OutputPath + "/TheCrossSections";
        if (Experiment_Is110MeV==kTRUE) {CrossFileName = CrossFileName + "_110MeV";}
        if (Experiment_Is250MeV==kTRUE) {CrossFileName = CrossFileName + "_250MeV";}
        if (Experiment_IsValidation==kTRUE) {CrossFileName = CrossFileName + "_Validation";}
        else {CrossFileName = CrossFileName + "_Experiment";}
        CrossFileName = CrossFileName + ".root";
        
        // Check if it already exists:
        FileStat_t stom;
        Int_t test = gSystem->GetPathInfo(CrossFileName.Data(),stom);
        TFile* f;
    
        if (test==0)
        {
            // Then the file already exists:
            f = new TFile(CrossFileName.Data(),"update");
        }
        else
        {
            // recreate the file:
            f = new TFile(CrossFileName.Data(),"recreate");
        }
    
        // Write the histogram to the file:
        TString TheKey = BranchTitle + "_CrossSection";
        TString ErrorKey = BranchTitle + "_SystErrors";
        TString EPlotKey = BranchTitle + "_SystPlots";
        f->cd();
        
        TheCrossSection->Write(TheKey,2);
        TheSystErrorBand->Write(ErrorKey,2);
        TheSystErrorPlot->Write(EPlotKey,2);
        f->Close();
        
        // Next, generate a picture:
        gROOT->SetBatch(kTRUE);
        if (Experiment_Is110MeV==kTRUE) {TheKey = TheKey + "_110MeV";}
        if (Experiment_Is250MeV==kTRUE) {TheKey = TheKey + "_250MeV";}
        if (Experiment_IsValidation==kTRUE) {TheKey = TheKey + "_Validation";}
        else {TheKey = TheKey + "_Experiment";}
        
        TCanvas* c1 = DrawGraphDouma(TheCrossSection,
                                     TheKey,
                                     "Scattering Angle [deg]",
                                     "Diff. Cross Section [Counts/sterad]",
                                     1, 
                                     4.0,
                                     UseCrossLogScale,
                                     "AP");
        
        TheSystErrorPlot->SetFillColor(kGray+2);
        TheSystErrorPlot->SetLineColor(kGray+2);
        TheSystErrorPlot->SetLineWidth(8.0);
        TheSystErrorPlot->Draw("E3same");
        
        TheCrossSection->UseCurrentStyle();
        TheCrossSection->SetLineWidth(6.0);
        TheCrossSection->SetLineColor(1);
        TheCrossSection->Draw("Psame");
        
        // And save the picture too:
        c1->SaveAs(OutputPath + "/" + TheKey + ".png");
        c1->Close();
        
        // Done.
    }
}
