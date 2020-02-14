void R3B_DNN_Plots::Plot2Graphs(TString const File, TString const GraphName1, TString const GraphName2, TString const Xlabel, TString const Ylabel, Bool_t const UseLogScale, Double_t const Ymini, Double_t const Ymaxi)
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
        TGraphErrors* Graph1 = (TGraphErrors*) f->Get(GraphName1.Data());
        TGraphErrors* Graph2 = (TGraphErrors*) f->Get(GraphName2.Data());
        
        // Check if both histograms exist:
        if ((Graph1==0)||(Graph1==nullptr))
        {
            ErrorMessage("The first graph is not present in the file!");
        }
        else
        {
            if ((Graph2==0)||(Graph2==nullptr))
            {
                ErrorMessage("The second graph is not present in the file!");
            }   
            else
            {                
                // Now, we can start to make the plot:
                Graph1->SetLineColor(4);
                Graph1->SetLineWidth(ScaleFactor);
                Graph2->SetLineColor(2);
                Graph2->SetLineWidth(ScaleFactor);
                
                gROOT->SetBatch(kTRUE);
                TCanvas* c1 = DrawGraphDouma(Graph1,GraphName1,Xlabel,Ylabel,4,UseLogScale,Ymini,Ymaxi);
                c1->cd();
                Graph2->Draw("same");
                
                // Next, we build the legenda:
                Double_t Xmin = Graph1->GetXaxis()->GetXmin();
                Double_t Xmax = Graph1->GetXaxis()->GetXmax();
                Double_t Ymin = Graph1->GetMinimum();
                Double_t Ymax = Graph1->GetMaximum();
                
                Int_t Length1 = GraphName1.Length();
                Double_t Distance1 = 1.0 - 0.01*((Int_t) Length1);
                Int_t Length2 = GraphName2.Length();
                Double_t Distance2 = 1.0 - 0.01*((Int_t) Length2);
                
                TPaveText* text1 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96,"NB");
                text1->SetFillColor(0); 
                text1->AddText(GraphName1); 
                text1->SetTextColor(4); 
                text1->SetTextSize(0.04); 
                text1->SetTextFont(1);
                text1->SetTextAngle(0.0); 
                text1->Draw("same");
                
                TPaveText* text2;
                if (UseLogScale==kFALSE) {text2 = new TPaveText(Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.90,Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*0.91,"NB");}
                else {text2 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.95*0.2,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*0.96*0.2,"NB");}
                text2->SetFillColor(0); 
                text2->AddText(GraphName2); 
                text2->SetTextColor(2); 
                text2->SetTextSize(0.04); 
                text2->SetTextFont(1);
                text2->SetTextAngle(0.0); 
                text2->Draw("same");
                
                // Save and close:
                c1->SaveAs("./"+GraphName1+"_"+GraphName2+".png");
                c1->Close();
                
                // Done.
            }
        }
    }
}
