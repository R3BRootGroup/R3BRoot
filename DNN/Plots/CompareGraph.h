void R3B_DNN_Plots::CompareGraph(TString const File1, TString const File2, TString const GraphName, TString const Xlabel, TString const Ylabel, Bool_t const UseLogScale, Double_t const Ymini, Double_t const Ymaxi)
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
        TGraphErrors* Graph1 = (TGraphErrors*) f1->Get(GraphName.Data());
        TGraphErrors* Graph2 = (TGraphErrors*) f2->Get(GraphName.Data());
        
        // Check if both histograms exist:
        if ((Graph1==0)||(Graph1==nullptr))
        {
            ErrorMessage("The histogram is not present in the first file!");
        }
        else
        {
            if ((Graph2==0)||(Graph2==nullptr))
            {
                ErrorMessage("The histogram is not present in the second file!");
            }   
            else
            {                
                // Now, we can start to make the plot:
                Graph1->SetLineColor(4);
                Graph1->SetLineWidth(ScaleFactor);
                Graph2->SetLineColor(2);
                Graph2->SetLineWidth(ScaleFactor);
                
                gROOT->SetBatch(kTRUE);
                TCanvas* c1 = DrawGraphDouma(Graph1,GraphName,Xlabel,Ylabel,4,UseLogScale,Ymini,Ymaxi);
                c1->cd();
                Graph2->Draw("same");
                
                // Next, we build the legenda:
                Double_t Xmin = Graph1->GetXaxis()->GetXmin();
                Double_t Xmax = Graph1->GetXaxis()->GetXmax();
                Double_t Ymin = Graph1->GetMinimum();
                Double_t Ymax = Graph1->GetMaximum();
                
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
                c1->SaveAs("./"+GraphName+".png");
                c1->Close();
                
                // Done.
            }
        }
    }
}
