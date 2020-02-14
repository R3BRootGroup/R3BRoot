// Function to plot the effects of this scorer:
TCanvas* ScoringClass::PlotScorer()
{
    // Set to batch mode:
    gROOT->SetBatch(kTRUE);
    
    // Generate the picture of the full histogram:
    TString Title = ScorerName + " Scorer Illustration";
    TString xlabel = ScorerName + " value";
    TString ylabel = "Counts";
    Int_t Color = 1;
    
    TCanvas* ThePicture = DrawHistDouma(FullSpectrum,Title,xlabel,ylabel,Color,Plot_UseLogScale);
    
    // Next, plot the primary hits on top:
    ThePicture->cd();
    PrimarySpectrum->UseCurrentStyle();
    PrimarySpectrum->SetLineWidth(ScaleFactor);
    PrimarySpectrum->SetLineColor(2);
    PrimarySpectrum->Draw("histsame");
    
    // And then, plot the legenda:
    Double_t Xmin = FullSpectrum->GetXaxis()->GetXmin();
    Double_t Xmax = FullSpectrum->GetXaxis()->GetXmax();
    Double_t Ymin = FullSpectrum->GetMinimum();
    Double_t Ymax = FullSpectrum->GetMaximum();
    Double_t RelYPos = 0.95;
            
    TPaveText* text1 = new TPaveText(Xmin+(Xmax-Xmin)*0.80,Ymin+(Ymax-Ymin)*RelYPos,Xmin+(Xmax-Xmin)*0.81,Ymin+(Ymax-Ymin)*(RelYPos+0.01),"NB");
    text1->SetFillColor(0); 
    text1->AddText("All hits"); 
    text1->SetTextColor(1); 
    text1->SetTextSize(0.04); 
    text1->SetTextFont(1);
    text1->SetTextAngle(0.0); 
    text1->Draw("same");
    RelYPos = RelYPos - 0.05;
    
    TPaveText* text2 = new TPaveText(Xmin+(Xmax-Xmin)*0.80,Ymin+(Ymax-Ymin)*RelYPos,Xmin+(Xmax-Xmin)*0.81,Ymin+(Ymax-Ymin)*(RelYPos+0.01),"NB");
    text2->SetFillColor(0); 
    text2->AddText("Primary hits"); 
    text2->SetTextColor(2); 
    text2->SetTextSize(0.04); 
    text2->SetTextFont(1);
    text2->SetTextAngle(0.0); 
    text2->Draw("same");
    RelYPos = RelYPos - 0.05;
    
    // Done.
    return ThePicture;
}
