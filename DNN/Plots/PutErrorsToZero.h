void R3B_DNN_Plots::PutErrorsToZero(TH1D* hist)
{
    // Puts all error bars to zero in the histogram:
    Int_t Nbins = hist->GetNbinsX();
    
    for (Int_t k = 0; k<(Nbins+2); ++k)
    {
        hist->SetBinError(k,0.0);
    }
    
    hist->SetBinContent(0,0.0);
    hist->SetBinContent(Nbins+1,0.0);
}
