void R3B_DNN_Plots::Compare3Graphs(TString const File1, TString const File2, TString const File3, TString const GraphName, TString const Xlabel, TString const Ylabel, Bool_t const UseLogScale, Double_t const Ymini, Double_t const Ymaxi)
{
    // Plots two histograms together for comparison. Begin by testing if the files exist:
    FileStat_t stom;
    Int_t Test1 = gSystem->GetPathInfo(File1.Data(),stom);
    Int_t Test2 = gSystem->GetPathInfo(File2.Data(),stom);
    Int_t Test3 = gSystem->GetPathInfo(File3.Data(),stom);
    
    if (!((Test1==0)&&(Test2==0)&&(Test3==0)))
    {
        ErrorMessage("The files you supplied do not exist!");
        if (!(Test1==0)) {ErrorMessage("The first file failed!");}
        if (!(Test2==0)) {ErrorMessage("The second file failed!");}
        if (!(Test3==0)) {ErrorMessage("The third file failed!");}
    }
    else
    {
        // Connect to the files:
        TFile* f1 = new TFile(File1.Data(),"read");
        TFile* f2 = new TFile(File2.Data(),"read");
        TFile* f3 = new TFile(File3.Data(),"read");
        
        // Next, retrieve the histograms from both files:
        TGraphErrors* Graph1 = (TGraphErrors*) f1->Get(GraphName.Data());
        TGraphErrors* Graph2 = (TGraphErrors*) f2->Get(GraphName.Data());
        TGraphErrors* Graph3 = (TGraphErrors*) f3->Get(GraphName.Data());
        
        // Check if both histograms exist:
        if ((Graph1==0)||(Graph1==nullptr)||(Graph2==0)||(Graph2==nullptr)||(Graph3==0)||(Graph3==nullptr))
        {
            ErrorMessage("The graphs is not present in the files!");
        }
        else
        {           
                // Now, we can start to make the plot:
                gROOT->SetBatch(kTRUE);
                TCanvas* c1 = DrawGraphDouma(Graph1,GraphName,Xlabel,Ylabel,4,UseLogScale,Ymini,Ymaxi);
                c1->cd();
                
                Graph1->SetLineColor(4);
                Graph1->SetLineWidth(ScaleFactor);
                Graph2->SetLineColor(2);
                Graph2->SetLineWidth(ScaleFactor);
                Graph3->SetLineColor(8);
                Graph3->SetLineWidth(ScaleFactor);
                
                Graph1->Draw("same");
                Graph2->Draw("same");
                Graph3->Draw("same");
                
                // Next, we build the legenda:
                Double_t Xmin = Graph1->GetXaxis()->GetXmin();
                Double_t Xmax = Graph1->GetXaxis()->GetXmax();
                Double_t Ymin = Graph1->GetMinimum();
                Double_t Ymax = Graph1->GetMaximum();
                Double_t Ypos = 0.95;
                
                Int_t Length1 = File1.Length();
                Double_t Distance1 = 1.0 - 0.01*((Int_t) Length1);
                Int_t Length2 = File2.Length();
                Double_t Distance2 = 1.0 - 0.01*((Int_t) Length2);
                Int_t Length3 = File3.Length();
                Double_t Distance3 = 1.0 - 0.01*((Int_t) Length3);
                
                TPaveText* text1 = new TPaveText(Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*Ypos,Xmin+(Xmax-Xmin)*Distance1,Ymin+(Ymax-Ymin)*Ypos*1.01,"NB");
                text1->SetFillColor(0); 
                text1->AddText(File1); 
                text1->SetTextColor(4); 
                text1->SetTextSize(0.04); 
                text1->SetTextFont(1);
                text1->SetTextAngle(0.0); 
                text1->Draw("same");
                
                if (UseLogScale==kTRUE) {Ypos = Ypos/2.0;}
                else {Ypos = Ypos - 0.05;}
                
                TPaveText* text2 = new TPaveText(Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*Ypos,Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*Ypos*1.01,"NB");
                text2->SetFillColor(0); 
                text2->AddText(File2); 
                text2->SetTextColor(2); 
                text2->SetTextSize(0.04); 
                text2->SetTextFont(1);
                text2->SetTextAngle(0.0); 
                text2->Draw("same");
                
                if (UseLogScale==kTRUE) {Ypos = Ypos/2.0;}
                else {Ypos = Ypos - 0.05;}
                
                TPaveText* text3 = new TPaveText(Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*Ypos,Xmin+(Xmax-Xmin)*Distance2,Ymin+(Ymax-Ymin)*Ypos*1.01,"NB");
                text3->SetFillColor(0); 
                text3->AddText(File3); 
                text3->SetTextColor(8); 
                text3->SetTextSize(0.04); 
                text3->SetTextFont(1);
                text3->SetTextAngle(0.0); 
                text3->Draw("same");
                
                // Save and close:
                c1->SaveAs("./"+GraphName+"_3.png");
                c1->Close();
                
                // Done.
        }
    }
}
