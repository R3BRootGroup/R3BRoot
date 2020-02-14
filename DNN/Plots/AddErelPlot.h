void R3B_DNN_Plots::AddErelPlot(TCanvas* ThePlot, TString const HistName, TString const LegendaText, Int_t const TheColor, Double_t const Xmin, Double_t const Xmax, Double_t const Ymin, Double_t const Ymax, Double_t& LegendaHeight, Bool_t const UseLogScale)
{
    // Adds a single Relative Energy Spectrum to the current canvas.
    if (DistFile_Exists==kTRUE)
    {
        // Begin by obtaining the histogram:
        TH1D* TheHistogram = (TH1D*) DistFile->Get(HistName);
        
        if ((TheHistogram!=nullptr)&&(TheHistogram!=0))
        {
            // Then, edit the histogram:        
            Int_t Nbins = TheHistogram->GetNbinsX();
            TheHistogram->SetBinContent(0,0.0);
            TheHistogram->SetBinContent(Nbins+1,0.0);
            TheHistogram->SetLineWidth(ScaleFactor);
            TheHistogram->SetLineColor(TheColor);
    
            // Create the legenda:
            TPaveText* TheText = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*LegendaHeight,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*(LegendaHeight+0.01),"NB");
            TheText->SetFillColor(0); 
            TheText->AddText(LegendaText); 
            TheText->SetTextColor(TheColor); 
            TheText->SetTextSize(0.035); 
            TheText->SetTextFont(1);
            TheText->SetTextAngle(0.0); 
        
            if (UseLogScale==kFALSE) {LegendaHeight = LegendaHeight - 0.043;}
            else {LegendaHeight = LegendaHeight/1.3;}
    
            // Then, make the plots:
            ThePlot->cd();
            TheHistogram->Draw("histsame");
            TheText->Draw("same");
        }
    }
}

Bool_t R3B_DNN_Plots::FindFirstPlot(TH1D* &FirstPlot, TString const HistName)
{
    // This function is to be used to locate the 'primary' Erel-plot. Use it to loop over all 
    // relevant plots in the proper sequence.
    
    Bool_t Answer = kFALSE;
    
    if (DistFile_Exists==kTRUE)
    {
        FirstPlot = (TH1D*) DistFile->Get(HistName);
        if ((FirstPlot!=nullptr)&&(FirstPlot!=0)) 
        {
            Int_t Nbins = FirstPlot->GetNbinsX();
            Double_t Integral = FirstPlot->Integral(1,Nbins);
            if (Integral>1.0)
            {
                Answer = kTRUE;
            }
        }
    }
    
    return Answer;
}
