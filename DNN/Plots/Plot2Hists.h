void R3B_DNN_Plots::Plot2Hists(TString const File, TString const HistName1, TString const HistName2, TString const Xlabel, TString const Ylabel, Int_t const Binning, Bool_t const UseLogScale)
{
    // Plots two histograms together for comparison. Begin by testing if the files exist:
    FileStat_t stom;
    Int_t Test = gSystem->GetPathInfo(File.Data(),stom);
    
    if (!(Test==0))
    {
        ErrorMessage("The file you supplied do not exist!");
    }
    else
    {
        // Connect to the file:
        TFile* f = new TFile(File.Data(),"read");
        f->cd();
        
        // Next, retrieve the histograms from both files:
        TH1D* Hist1_file = (TH1D*) f->Get(HistName1.Data());
        TH1D* Hist2_file = (TH1D*) f->Get(HistName2.Data());
        
        TH1D* Hist1 = (TH1D*) Hist1_file;
        TH1D* Hist2 = (TH1D*) Hist2_file;
        
        // Check if both histograms exist:
        if ((Hist1==0)||(Hist1==nullptr))
        {
            ErrorMessage("The first histogram is not present in the file!");
        }
        else
        {
            if ((Hist2==0)||(Hist2==nullptr))
            {
                ErrorMessage("The second histogram is not present in the file!");
            }   
            else
            {
                // Rebin the histograms first:
                if (Binning>0)
                {
                    Hist1 = (TH1D*) Hist1_file->Rebin(Binning,HistName1+"Rebin_1");
                    Hist2 = (TH1D*) Hist2_file->Rebin(Binning,HistName2+"Rebin_2");
                }
                
                // Now, we can start to make the plot:
                PutErrorsToZero(Hist1);
                Hist1->SetLineColor(4);
                Hist1->SetLineWidth(2.0);
                PutErrorsToZero(Hist2);
                Hist2->SetLineColor(2);
                Hist2->SetLineWidth(2.0);
                
                gROOT->SetBatch(kTRUE);
                TCanvas* c1 = DrawHistDouma(Hist1,HistName1,Xlabel,Ylabel,4,UseLogScale);
                c1->cd();
                Hist2->Draw("histsame");
                
                // Next, we build the legenda:
                Double_t Xmin = Hist1->GetXaxis()->GetXmin();
                Double_t Xmax = Hist1->GetXaxis()->GetXmax();
                Double_t Ymin = Hist1->GetMinimum();
                Double_t Ymax = Hist1->GetMaximum();
                
                Int_t Length1 = HistName1.Length();
                Double_t Distance1 = 1.0 - 0.01*((Int_t) Length1);
                Int_t Length2 = HistName2.Length();
                Double_t Distance2 = 1.0 - 0.01*((Int_t) Length2);
                
                TPaveText* text1 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96,"NB");
                text1->SetFillColor(0); 
                text1->AddText(HistName1); 
                text1->SetTextColor(4); 
                text1->SetTextSize(0.04); 
                text1->SetTextFont(1);
                text1->SetTextAngle(0.0); 
                text1->Draw("same");
                
                TPaveText* text2;
                if (UseLogScale==kFALSE) {text2 = new TPaveText(Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.90,Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.91,"NB");}
                else {text2 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95*0.2,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96*0.2,"NB");}
                text2->SetFillColor(0); 
                text2->AddText(HistName2); 
                text2->SetTextColor(2); 
                text2->SetTextSize(0.04); 
                text2->SetTextFont(1);
                text2->SetTextAngle(0.0); 
                text2->Draw("same");
                
                // Save and close:
                c1->SaveAs("./"+HistName1+"_"+HistName2+".png");
                c1->Close();
                
                // Done.
            }
        }
    }
}
