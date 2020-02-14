TCanvas* R3B_DNN_Plots::OneScatterHist(TString const NewFileName, Bool_t const ThisLogScale, TString const particle, Int_t const Rebinning)
{
    // Plots all energy deposition histograms & some combinations.
    if (NewFileName!="")
    {
        FileStat_t stom;
        Int_t NewTest = gSystem->GetPathInfo(NewFileName.Data(),stom);
        if (NewTest==0)
        {
            DistFile = new TFile(NewFileName.Data(),"read");
        }
        else
        {
            ErrorMessage("The new file does not exists!");
            CheckFiles();
        }
    }
    
    // Determine the index:
    Int_t Index = 0;
    if (particle=="All")       {Index = 0;}
    if (particle=="Neutrons")  {Index = 1;}
    if (particle=="Protons")   {Index = 2;}
    if (particle=="Electrons") {Index = 3;}
    if (particle=="Gammas")    {Index = 4;}
    if (particle=="Neutrinos") {Index = 5;}
    if (particle=="Pions")     {Index = 6;}
    if (particle=="Muons")     {Index = 7;}
    if (particle=="Positrons") {Index = 8;}
    if (particle=="Deuterons") {Index = 9;}
    if (particle=="Tritons")   {Index = 10;}
    if (particle=="Alphas")    {Index = 11;}
    if (particle=="C-ions")    {Index = 12;}
    if (particle=="Fragments") {Index = 13;}
    
    // Begin by retrieving all relevant histograms:
    TH1D* Hist_AlphaScatter;
    TString st = "";
    TString kstr = "";
    TString HistName = "";
    Bool_t HistCheck = kTRUE;
    
    kstr = st.Itoa(Index,10);
    HistName = "Hist_AlphaScatter"+kstr;
    Hist_AlphaScatter = (TH1D*) DistFile->Get(HistName.Data());
    if ((Hist_AlphaScatter==0)||(Hist_AlphaScatter==nullptr)) {HistCheck = kFALSE;}
    
    if (HistCheck==kFALSE)
    {
        ErrorMessage("The scatter histogram could not be found!");
    }
    else
    {
        // Begin by creating the separate plots:
        gROOT->SetBatch(kTRUE);
        
        Hist_AlphaScatter->Rebin(Rebinning);
        TCanvas* c1 = DrawHistDouma(Hist_AlphaScatter,"Scattering Angle Distribution","Scattering Angle [deg]","Counts",0,ThisLogScale);
          
        Int_t Nbins = Hist_AlphaScatter->GetNbinsX();
        Double_t Xmin = Hist_AlphaScatter->GetXaxis()->GetXmin();
        Double_t Xmax = Hist_AlphaScatter->GetXaxis()->GetXmax();
        Double_t Ymin = Hist_AlphaScatter->GetMinimum();
        Double_t Ymax = Hist_AlphaScatter->GetMaximum();
        Double_t Ypos = 0.95;
        
        TPaveText* text0 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*Ypos,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*Ypos*1.01,"NB");
        text0->SetFillColor(0); 
        text0->AddText("Scattering Angle "+particle); 
        text0->SetTextColor(kBlack); 
        text0->SetTextSize(0.04); 
        text0->SetTextFont(1);
        text0->SetTextAngle(0.0); 
        // text0->Draw("same");
         
        if (ThisLogScale==kTRUE) {Ypos = Ypos/2.0;}
        else {Ypos = Ypos - 0.05;}
        
        // return the picture:
        return c1;
        // Done.
    }
}
