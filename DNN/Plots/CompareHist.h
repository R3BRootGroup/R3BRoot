void R3B_DNN_Plots::CompareHist(TString const File1, TString const File2, TString const HistName, TString const Xlabel, TString const Ylabel, Int_t const Binning, Bool_t const UseLogScale)
{
    // Plots two histograms together for comparison. Begin by testing if the files exist:
    FileStat_t stom;
    Int_t Test1 = gSystem->GetPathInfo(File1.Data(),stom);
    Int_t Test2 = gSystem->GetPathInfo(File2.Data(),stom);
    
    if (!((Test1==0)&&(Test2==0)))
    {
        ErrorMessage("The files you supplied do not exist!");
        if (!(Test1==0)) {ErrorMessage("The first file failed!");}
        if (!(Test2==0)) {ErrorMessage("The second file failed!");}
    }
    else
    {
        // Connect to the files:
        TFile* f1 = new TFile(File1.Data(),"read");
        TFile* f2 = new TFile(File2.Data(),"read");
        
        // Next, retrieve the histograms from both files:
        TH1D* Hist1_file = (TH1D*) f1->Get(HistName.Data());
        TH1D* Hist2_file = (TH1D*) f2->Get(HistName.Data());
        
        TH1D* Hist1 = (TH1D*) Hist1_file;
        TH1D* Hist2 = (TH1D*) Hist2_file;
        
        // Check if both histograms exist:
        if ((Hist1==0)||(Hist1==nullptr))
        {
            ErrorMessage("The histogram is not present in the first file!");
        }
        else
        {
            if ((Hist2==0)||(Hist2==nullptr))
            {
                ErrorMessage("The histogram is not present in the second file!");
            }   
            else
            {
                // Rebin the histograms first:
                if (Binning>0)
                {
                    Hist1 = (TH1D*) Hist1_file->Rebin(Binning,HistName+"Rebin_1");
                    Hist2 = (TH1D*) Hist2_file->Rebin(Binning,HistName+"Rebin_2");
                }
                
                // Now, we can start to make the plot:
                PutErrorsToZero(Hist1);
                Hist1->SetLineColor(4);
                Hist1->SetLineWidth(ScaleFactor);
                PutErrorsToZero(Hist2);
                Hist2->SetLineColor(2);
                Hist2->SetLineWidth(ScaleFactor);
                
                gROOT->SetBatch(kTRUE);
                TCanvas* c1 = DrawHistDouma(Hist1,HistName,Xlabel,Ylabel,4,UseLogScale);
                c1->cd();
                Hist2->Draw("histsame");
                
                // Next, we build the legenda:
                Double_t Xmin = Hist1->GetXaxis()->GetXmin();
                Double_t Xmax = Hist1->GetXaxis()->GetXmax();
                Double_t Ymin = Hist1->GetMinimum();
                Double_t Ymax = Hist1->GetMaximum();
                
                Int_t Length1 = File1.Length();
                Double_t Distance1 = 1.0 - 0.01*((Int_t) Length1);
                Int_t Length2 = File2.Length();
                Double_t Distance2 = 1.0 - 0.01*((Int_t) Length2);
                
                TPaveText* text1 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96,"NB");
                text1->SetFillColor(0); 
                text1->AddText(File1); 
                text1->SetTextColor(4); 
                text1->SetTextSize(0.04); 
                text1->SetTextFont(1);
                text1->SetTextAngle(0.0); 
                text1->Draw("same");
                
                TPaveText* text2;
                if (UseLogScale==kFALSE) {text2 = new TPaveText(Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.90,Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.91,"NB");}
                else {text2 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95*0.2,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96*0.2,"NB");}
                text2->SetFillColor(0); 
                text2->AddText(File2); 
                text2->SetTextColor(2); 
                text2->SetTextSize(0.04); 
                text2->SetTextFont(1);
                text2->SetTextAngle(0.0); 
                text2->Draw("same");
                
                // Save and close:
                c1->SaveAs("./"+HistName+".png");
                c1->Close();
                
                // Done.
            }
        }
    }
}
