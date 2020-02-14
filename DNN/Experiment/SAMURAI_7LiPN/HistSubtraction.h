TH1D* HistSubtraction(TString const NewHistName, TH1D* SignalHist, TH1D* BackHist, Double_t const BackScale, Double_t const LowerBoundary, Double_t const UpperBoundary)
{
    // Performs the instrumental background subtraction. Begin by doing some tests:
    Int_t Nbins = SignalHist->GetNbinsX();
    Double_t Xmin = SignalHist->GetXaxis()->GetXmin();
    Double_t Xmax = SignalHist->GetXaxis()->GetXmax();
    
    Int_t Nbins_back;
    Double_t Xmin_back;
    Double_t Xmax_back;
    
    if (BackHist!=0)
    {
        Nbins_back = BackHist->GetNbinsX();
        Xmin_back = BackHist->GetXaxis()->GetXmin();
        Xmax_back = BackHist->GetXaxis()->GetXmax();
    }
    else
    {
        Nbins_back = Nbins;
        Xmin_back = Xmin;
        Xmax_back = Xmax;
    }
    
    Bool_t Test = kTRUE;
    
    if ((LowerBoundary<Xmin)||(LowerBoundary>Xmax)) {cout << "### Your specified LowerBoundary is not within the histogram interval!\n"; Test = kFALSE;}
    if ((UpperBoundary<Xmin)||(UpperBoundary>Xmax)) {cout << "### Your specified UpperBoundary is not within the histogram interval!\n"; Test = kFALSE;}
    if (Nbins!=Nbins_back) {cout << "### Number of bins of Signal & Background histograms are not equal!\n"; Test = kFALSE;}
    if (Xmin!=Xmin_back) {cout << "### Xmin of Signal & Background histograms are not equal!\n"; Test = kFALSE;}
    if (Xmax!=Xmax_back) {cout << "### Xmax of Signal & Background histograms are not equal!\n"; Test = kFALSE;}
    if (BackScale<=0.0) {cout << "### Background can only be scaled by a positive number!\n"; Test = kFALSE;}
    if (UpperBoundary<=LowerBoundary) {cout << "### LowerBoundary should be strictly lower than UpperBoundary!\n"; Test = kFALSE;}
    
    // Declare the answer:
    TH1D* SubtractedHist = 0;
    
    if (Test==kTRUE)
    {
        // Then, we can start working.
        // Define the step size:
        Double_t StepSize = (Xmax-Xmin)/((Int_t) Nbins);
        
        // Compute the number of skipped bins:
        Double_t Nbins_Skipped_d = (LowerBoundary-Xmin)/StepSize;
        Int_t Nbins_Skipped = (Int_t) Nbins_Skipped_d;
        
        // Adjust lower boundary:
        Double_t RealLowerBoundary = Xmin + ((Int_t) Nbins_Skipped)*StepSize;
        
        // Then, adjust upperboundary:
        Double_t Nbins_New_d = (UpperBoundary-RealLowerBoundary)/StepSize;
        Int_t Nbins_New = (Int_t) Nbins_New_d;
        Double_t RealUpperBoundary = RealLowerBoundary + ((Int_t) Nbins_New)*StepSize;
        
        // Next, we can then declare the new histogram:
        SubtractedHist = new TH1D(NewHistName,NewHistName,Nbins_New,RealLowerBoundary,RealUpperBoundary);
        
        // Then, loop over it to perform the subtraction:
        Double_t SignalContent;
        Double_t BackgroundContent;
        Double_t BinError;
        Double_t BinContent;
        
        for (Int_t ThisBin = 1; ThisBin<(Nbins_New+1); ++ThisBin)
        {
            SignalContent = SignalHist->GetBinContent(ThisBin+Nbins_Skipped);
            
            if (BackHist!=0) {BackgroundContent = BackHist->GetBinContent(ThisBin+Nbins_Skipped);}
            else {BackgroundContent = 0.0;}
            
            BinContent = SignalContent - BackScale*BackgroundContent;
            BinError = TMath::Sqrt(SignalContent + BackScale*BackScale*BackgroundContent);
            
            SubtractedHist->SetBinContent(ThisBin,BinContent);
            SubtractedHist->SetBinError(ThisBin,BinError);
        }
        
        // Done.
    }
    
    // Return the naswer:
    return SubtractedHist;
}
