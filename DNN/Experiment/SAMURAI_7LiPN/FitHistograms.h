void CrossSections_7LiPN::FitHistograms()
{
    // This function fits the subtracted histograms with a pure Gaussian.
    
    // Test that they exist:
    if (SubtractedHists.size()>0)
    {
        // Now, we can move on. Define parameters:
        Generator = new TRandom3(0);
        Double_t LowerFitBoundary = -20.0;
        Double_t UpperFitBoundary = 10.0;
        if (Experiment_IsValidation==kTRUE) {UpperFitBoundary = 5.0;}
        
        if ((Experiment_Is110MeV==kTRUE)&&(Experiment_IsValidation==kTRUE)) {LowerFitBoundary = -3.0;}
        if ((Experiment_Is110MeV==kTRUE)&&(Experiment_IsValidation==kTRUE)) {UpperFitBoundary = 1.0;}
        
        if ((Experiment_Is110MeV==kTRUE)&&(Experiment_IsValidation==kFALSE)) {LowerFitBoundary = -15.0;}
        if ((Experiment_Is110MeV==kTRUE)&&(Experiment_IsValidation==kFALSE)) {UpperFitBoundary = 5.0;}
        
        Double_t PeakPosition = 0.0;
        
        // Resize vectors:
        PeakAreas.resize(SubtractedHists.size());
        PeakAreaErrors.resize(SubtractedHists.size());
        
        // Define arrays:
        Int_t nHistograms = SubtractedHists.size();
        TF1** FitFunctions = new TF1*[nHistograms];
        TF1** PlotFunctions = new TF1*[nHistograms];
        TFitResultPtr* FitStatus = new TFitResultPtr[nHistograms];
        Int_t* FitStatus_int = new Int_t[nHistograms];
        
        // Declare other things we need:
        Int_t nBins;
        Double_t Xmin;
        Double_t Xmax;
        Double_t Ymin;
        Double_t Ymax;
        Double_t Ystart;
        Double_t BinSize;
        Int_t PeakBin;
        TString st = "";
        TString kfitstr = "";
        TString Text = "";
        Double_t GuessedSigma;
        Double_t PeakHeight;
        Bool_t FitSucces;
        
        // loop over all histograms to do the fitting:
        for (Int_t kfit = 0; kfit<nHistograms; ++kfit)
        {
            // Enumeration:
            kfitstr = st.Itoa(kfit,10);
            FitSucces = kFALSE;
            
            // Begin by extracting the histogram properties:
            nBins = SubtractedHists.at(kfit)->GetNbinsX();
            Xmin = SubtractedHists.at(kfit)->GetXaxis()->GetXmin();
            Xmax = SubtractedHists.at(kfit)->GetXaxis()->GetXmax();
            BinSize = TMath::Abs(Xmax-Xmin)/((Int_t) nBins);
            
            // Extract the position of the peak bin:
            for (Int_t k = 0; k<nBins; ++k)
            {
                if (((Xmin + BinSize*((Int_t) k))<=PeakPosition)&&
                    ((Xmin + BinSize*((Int_t) (k+1)))>=PeakPosition))
                {
                    PeakBin = k+1;
                }
            }
            
            // Define the fitting & plotting function:
            FitFunctions[kfit] = new TF1("FitFunction"+kfitstr,"([0]/[2])*TMath::Exp(-0.5*((x-[1])/[2])*((x-[1])/[2]))",LowerFitBoundary,UpperFitBoundary);
            PlotFunctions[kfit] = new TF1("FitFunction"+kfitstr,"([0]/[2])*TMath::Exp(-0.5*((x-[1])/[2])*((x-[1])/[2]))",Xmin,Xmax);
            
            // Guess fitting parameters:
            PeakHeight = SubtractedHists.at(kfit)->GetBinContent(PeakBin);
            GuessedSigma = TMath::Abs(PeakPosition-LowerFitBoundary);
            if (TMath::Abs(PeakPosition-UpperFitBoundary)<GuessedSigma) {GuessedSigma = TMath::Abs(PeakPosition-UpperFitBoundary);}
            
            FitFunctions[kfit]->SetParameter(0,PeakHeight*GuessedSigma);
            FitFunctions[kfit]->SetParameter(1,PeakPosition + Generator->Uniform(1.0) - 0.5);
            FitFunctions[kfit]->SetParameter(2,GuessedSigma*(1.0 + Generator->Uniform(0.2) - 0.1));
            
            // Set limits:
            FitFunctions[kfit]->SetParLimits(0,0.0,10.0*TMath::Abs(FitFunctions[kfit]->GetParameter(0)));
            FitFunctions[kfit]->SetParLimits(1,LowerFitBoundary,UpperFitBoundary);
            FitFunctions[kfit]->SetParLimits(2,0.0,10.0*TMath::Abs(FitFunctions[kfit]->GetParameter(2)));
            
            // Make the fit:
            if (SubtractedHists.at(kfit)->Integral()<0.9)
            {
                ErrorMessage("Histogram "+kfitstr+" was empty! No fit could be made!");
                PeakAreas.at(kfit) = 0.0;
                PeakAreaErrors.at(kfit) = 0.0;
                
                PlotFunctions[kfit]->SetParameter(0,0.0);
                PlotFunctions[kfit]->SetParameter(1,0.0);
                PlotFunctions[kfit]->SetParameter(2,1.0);
            }
            else
            {
                FitStatus[kfit] = SubtractedHists.at(kfit)->Fit(FitFunctions[kfit],"RSBQ0");
                
                // Check that fitting worked:
                if (FitStatus[kfit]->IsEmpty()==kTRUE)
                {
                    ErrorMessage("Fit result of bin "+kfitstr+" was empty!");
                    PeakAreas.at(kfit) = 0.0;
                    PeakAreaErrors.at(kfit) = 0.0;
                    
                    PlotFunctions[kfit]->SetParameter(0,0.0);
                    PlotFunctions[kfit]->SetParameter(1,0.0);
                    PlotFunctions[kfit]->SetParameter(2,1.0);
                }
                else
                {
                    FitStatus_int[kfit] = (Int_t) FitStatus[kfit];
            
                    // Transport parameters:
                    PlotFunctions[kfit]->SetParameter(0,FitStatus[kfit]->Parameter(0));
                    PlotFunctions[kfit]->SetParameter(1,FitStatus[kfit]->Parameter(1));
                    PlotFunctions[kfit]->SetParameter(2,FitStatus[kfit]->Parameter(2));
                
                    PlotFunctions[kfit]->SetParError(0,FitStatus[kfit]->ParError(0));
                    PlotFunctions[kfit]->SetParError(1,FitStatus[kfit]->ParError(1));
                    PlotFunctions[kfit]->SetParError(2,FitStatus[kfit]->ParError(2));
                
                    // Restyle:
                    PlotFunctions[kfit]->SetLineWidth(4.0);
                    PlotFunctions[kfit]->SetLineColor(2);
                    PlotFunctions[kfit]->SetNpx(nBins);
                
                    // Compute area:
                    PeakAreas.at(kfit) = TMath::Sqrt(2.0*TMath::Pi())*FitStatus[kfit]->Parameter(0);
                    PeakAreaErrors.at(kfit) = TMath::Sqrt(2.0*TMath::Pi())*FitStatus[kfit]->ParError(0);
                
                    // That concludes the histogram loop.
                    FitSucces = kTRUE;
                }
            }
        }
            
        // Next, we will need to make a fitting picture. Begin by making a histogram & title array:
        TH1D** TheHists = new TH1D*[nHistograms];
        TString* TheTitles = new TString[nHistograms];
        
        for (Int_t kfit = 0; kfit<nHistograms; ++kfit) 
        {
            TheHists[kfit] = SubtractedHists.at(kfit);
            TheTitles[kfit] = SubtractedHists.at(kfit)->GetName();
        }
        
        // Then, determine the canvas size:
        Int_t Nhist = nHistograms;
        Int_t Nhor  = 1;
        Int_t Nvert = 1;
        
        while (Nhist>(Nhor*Nvert))
        {
            if (Nhor==Nvert) {Nhor = Nhor + 1;}
            else {Nvert = Nvert + 1;}
        }
        
        // Determine labels:
        TString xlabel = "Excitation Energy [MeV]";
        TString ylabel = "Counts";
        Int_t HistColor = 1;
        Double_t ScaleFactor = 4.0;
        gROOT->SetBatch(kTRUE);
        Bool_t UseGlobalMax = kFALSE;
        
        // Create the histogram plot:
        TCanvas** c1 = DrawHistDoumaArray(Nhist,Nhor,Nvert,TheHists,TheTitles,xlabel,ylabel,HistColor,ScaleFactor,UseGlobalMax);
        
        // Create Text labels:
        TPaveText** Test_status = new TPaveText*[nHistograms];
        TPaveText** Test_chi2 = new TPaveText*[nHistograms];
        TPaveText** Test_NDF = new TPaveText*[nHistograms];
        TPaveText** Test_chiprob = new TPaveText*[nHistograms];
        TPaveText** Test_area = new TPaveText*[nHistograms];
        
        // Next, we must plot the functions on top:
        for (Int_t kfit = 0; kfit<nHistograms; ++kfit) 
        {
            // Plot the function:
            c1[0]->cd(kfit+1);
            PlotFunctions[kfit]->Draw("csame");
            
            // Make a legenda:
            Xmin = SubtractedHists.at(kfit)->GetXaxis()->GetXmin();
            Xmax = SubtractedHists.at(kfit)->GetXaxis()->GetXmax();
            Ymin = SubtractedHists.at(kfit)->GetMinimum();
            Ymax = SubtractedHists.at(kfit)->GetMaximum();
            if (FitSucces==kFALSE) {Ymin = 0.0; Ymax = 1.0;}
            Ystart = Ymax + 0.05*(Ymax-Ymin);
            
            Test_status[kfit] = new TPaveText(Xmin+(Xmax-Xmin)*0.70,Ystart,Xmin+(Xmax-Xmin)*0.71,Ystart+0.00001,"NB");
            Test_status[kfit]->SetFillColor(0); 
            Text = "Fit Status = ";
            if (FitSucces==kTRUE) {Text = Text + st.Itoa(((Int_t) FitStatus[kfit]),10);} else {Text = Text + "--";}
            Test_status[kfit]->AddText(Text); 
            Test_status[kfit]->SetTextColor(4); 
            Test_status[kfit]->SetTextSize(0.04); 
            Test_status[kfit]->SetTextFont(1);
            Test_status[kfit]->SetTextAngle(0.0); 
            Test_status[kfit]->Draw("same");
            Ystart = Ystart - 0.04*(Ymax-Ymin);
        
            Test_chi2[kfit] = new TPaveText(Xmin+(Xmax-Xmin)*0.70,Ystart,Xmin+(Xmax-Xmin)*0.71,Ystart+0.00001,"NB");
            Test_chi2[kfit]->SetFillColor(0); 
            Text = "Abs chi^2 = ";
            if (FitSucces==kTRUE) {Text = Text + RoundOff(FitStatus[kfit]->Chi2(),3);} else {Text = Text + "--";}
            Test_chi2[kfit]->AddText(Text); 
            Test_chi2[kfit]->SetTextColor(4); 
            Test_chi2[kfit]->SetTextSize(0.04); 
            Test_chi2[kfit]->SetTextFont(1);
            Test_chi2[kfit]->SetTextAngle(0.0); 
            Test_chi2[kfit]->Draw("same");
            Ystart = Ystart - 0.04*(Ymax-Ymin);
        
            Test_NDF[kfit] = new TPaveText(Xmin+(Xmax-Xmin)*0.70,Ystart,Xmin+(Xmax-Xmin)*0.71,Ystart+0.00001,"NB");
            Test_NDF[kfit]->SetFillColor(0); 
            Text = "Fit NDF = ";
            if (FitSucces==kTRUE) {Text = Text + st.Itoa(FitFunctions[kfit]->GetNDF(),10);} else {Text = Text + "--";}
            Test_NDF[kfit]->AddText(Text); 
            Test_NDF[kfit]->SetTextColor(4); 
            Test_NDF[kfit]->SetTextSize(0.04); 
            Test_NDF[kfit]->SetTextFont(1);
            Test_NDF[kfit]->SetTextAngle(0.0); 
            Test_NDF[kfit]->Draw("same");
            Ystart = Ystart - 0.04*(Ymax-Ymin);
        
            Test_chiprob[kfit] = new TPaveText(Xmin+(Xmax-Xmin)*0.70,Ystart,Xmin+(Xmax-Xmin)*0.71,Ystart+0.00001,"NB");
            Test_chiprob[kfit]->SetFillColor(0); 
            Text = "Chi^2 Prob = ";
            if (FitSucces==kTRUE) {Text = Text + RoundOff(TMath::Prob(FitStatus[kfit]->Chi2(),FitFunctions[kfit]->GetNDF()),3);} else {Text = Text + "--";}
            Test_chiprob[kfit]->AddText(Text); 
            Test_chiprob[kfit]->SetTextColor(4); 
            Test_chiprob[kfit]->SetTextSize(0.04); 
            Test_chiprob[kfit]->SetTextFont(1);
            Test_chiprob[kfit]->SetTextAngle(0.0); 
            Test_chiprob[kfit]->Draw("same");
            Ystart = Ystart - 0.04*(Ymax-Ymin);
            
            Test_area[kfit] = new TPaveText(Xmin+(Xmax-Xmin)*0.70,Ystart,Xmin+(Xmax-Xmin)*0.71,Ystart+0.00001,"NB");
            Test_area[kfit]->SetFillColor(0); 
            Text = "Peak Area = ";
            if (FitSucces==kTRUE) {Text = Text + RoundOff(PeakAreas.at(kfit),3) + " +/- " + RoundOff(PeakAreaErrors.at(kfit),3);} else {Text = Text + "--";}
            Test_area[kfit]->AddText(Text); 
            Test_area[kfit]->SetTextColor(4); 
            Test_area[kfit]->SetTextSize(0.04); 
            Test_area[kfit]->SetTextFont(1);
            Test_area[kfit]->SetTextAngle(0.0); 
            Test_area[kfit]->Draw("same");
            Ystart = Ystart - 0.04*(Ymax-Ymin);
            
            // Done.
        }
        
        // Next, store the picture:
        TString PictureName = "PeakFitting_" + BranchTitle;
        
        if (BranchTitle.Contains("Perfect")==kTRUE) {PictureName = PictureName + "_MC";}
        
        if (Experiment_Is110MeV==kTRUE)     {PictureName = PictureName + "_110MeV";}
        if (Experiment_Is250MeV==kTRUE)     {PictureName = PictureName + "_250MeV";}
        if (Experiment_IsValidation==kTRUE) {PictureName = PictureName + "_Validation";}
        else {PictureName = PictureName + "_Experiment";}
        PictureName = PictureName + ".png";
        
        c1[0]->SaveAs(OutputPath + PictureName);
        c1[0]->Close();
        
        // So now all areas have been computed (with errors) & we have a nice picture.
        // All that is left then, is to delete all arrays:
        
        delete FitFunctions;
        delete PlotFunctions;
        delete FitStatus_int;
        delete Test_status;
        delete Test_chi2;
        delete Test_NDF;
        delete Test_chiprob;
        delete Test_area;
        delete c1;
        // no deleting TheHists, since those histograms should be kept inside the vectors.
        
        // Done.
    }
}
    
    
    
    
    
    
    
        
