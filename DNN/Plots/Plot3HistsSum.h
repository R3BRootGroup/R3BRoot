void R3B_DNN_Plots::Plot3HistsSum(TString const File, TString const HistName1, TString const HistName2, TString const HistName3, TString const Xlabel, TString const Ylabel, Int_t const Binning, Bool_t const UseLogScale)
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
        TH1D* Hist3_file = (TH1D*) f->Get(HistName3.Data());
        
        TH1D* Hist1 = (TH1D*) Hist1_file;
        TH1D* Hist2 = (TH1D*) Hist2_file;
        TH1D* Hist3 = (TH1D*) Hist3_file;
        
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
                if ((Hist3==0)||(Hist2==nullptr))
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
                    Hist3 = (TH1D*) Hist3_file->Rebin(Binning,HistName3+"Rebin_3");
                }
                
                // Compute the sum of 2 and 3:
                Int_t Nbins = Hist1->GetNbinsX();
                Double_t Xmin = Hist1->GetXaxis()->GetXmin();
                Double_t Xmax = Hist1->GetXaxis()->GetXmax();
                Double_t Ymin = Hist1->GetMinimum();
                Double_t Ymax = Hist1->GetMaximum();
                
                TH1D* Hist4 = new TH1D("Hist4","Sum_2&3",Nbins,Xmin,Xmax);
                
                for (Int_t k = 0; k<(Nbins+2); ++k)
                {
                    Hist4->SetBinContent(k,Hist2->GetBinContent(k)+Hist3->GetBinContent(k));
                    Hist4->SetBinError(k,TMath::Sqrt(Hist4->GetBinContent(k)));
                }                
                
                // Now, we can start to make the plot:
                PutErrorsToZero(Hist1);
                Hist1->SetLineColor(1);
                Hist1->SetLineWidth(ScaleFactor);
                PutErrorsToZero(Hist2);
                Hist2->SetLineColor(2);
                Hist2->SetLineWidth(ScaleFactor);
                PutErrorsToZero(Hist3);
                Hist3->SetLineColor(3);
                Hist3->SetLineWidth(ScaleFactor);
                PutErrorsToZero(Hist4);
                Hist4->SetLineColor(4);
                Hist4->SetLineWidth(ScaleFactor);
                
                gROOT->SetBatch(kTRUE);
                TCanvas* c1 = DrawHistDouma(Hist1,HistName1,Xlabel,Ylabel,1,UseLogScale);
                c1->cd();
                Hist2->Draw("histsame");
                Hist3->Draw("histsame");
                Hist4->Draw("histsame");
                
                // Next, we build the legenda:
                Int_t Length1 = HistName1.Length();
                Double_t Distance1 = 1.0 - 0.01*((Int_t) Length1);
                Int_t Length2 = HistName2.Length();
                Double_t Distance2 = 1.0 - 0.01*((Int_t) Length2);
                Int_t Length3 = HistName3.Length();
                Double_t Distance3 = 1.0 - 0.01*((Int_t) Length3);
                
                TPaveText* text1 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96,"NB");
                text1->SetFillColor(0); 
                text1->AddText(HistName1); 
                text1->SetTextColor(1); 
                text1->SetTextSize(0.04); 
                text1->SetTextFont(1);
                text1->SetTextAngle(0.0); 
                text1->Draw("same");
                
                TPaveText* text2;
                if (UseLogScale==kFALSE) {text2 = new TPaveText(Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.90,Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.91,"NB");}
                else {text2 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95*0.5,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96*0.5,"NB");}
                text2->SetFillColor(0); 
                text2->AddText(HistName2); 
                text2->SetTextColor(2); 
                text2->SetTextSize(0.04); 
                text2->SetTextFont(1);
                text2->SetTextAngle(0.0); 
                text2->Draw("same");
                
                TPaveText* text3;
                if (UseLogScale==kFALSE) {text3 = new TPaveText(Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.85,Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.86,"NB");}
                else {text3 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95*0.25,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96*0.25,"NB");}
                text3->SetFillColor(0); 
                text3->AddText(HistName3); 
                text3->SetTextColor(3); 
                text3->SetTextSize(0.04); 
                text3->SetTextFont(1);
                text3->SetTextAngle(0.0); 
                text3->Draw("same");
                
                TPaveText* text4;
                if (UseLogScale==kFALSE) {text4 = new TPaveText(Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.80,Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.81,"NB");}
                else {text4 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95*0.125,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96*0.125,"NB");}
                text4->SetFillColor(0); 
                text4->AddText("Sum of 2 & 3"); 
                text4->SetTextColor(4); 
                text4->SetTextSize(0.04); 
                text4->SetTextFont(1);
                text4->SetTextAngle(0.0); 
                text4->Draw("same");
                
                // Save and close:
                c1->SaveAs("./"+HistName1+"_"+HistName2+"_"+HistName3+".png");
                c1->Close();
                
                // Done.
                }
            }
        }
    }
}
