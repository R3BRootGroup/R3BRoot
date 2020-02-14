void R3B_DNN_Plots::ScatterHists(TString const NewFileName, Bool_t const ThisLogScale)
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
    
    // Begin by retrieving all relevant histograms:
    TH1D** Hist_AlphaScatter = new TH1D*[14];
    TString st = "";
    TString kstr = "";
    TString HistName = "";
    Bool_t HistCheck = kTRUE;
    
    for (Int_t k = 0; k<14; ++k)
    {
        kstr = st.Itoa(k,10);
        HistName = "Hist_AlphaScatter"+kstr;
        Hist_AlphaScatter[k] = (TH1D*) DistFile->Get(HistName.Data());
        if ((Hist_AlphaScatter[k]==0)||(Hist_AlphaScatter[k]==nullptr)) {HistCheck = kFALSE;}
    }
    
    if (HistCheck==kFALSE)
    {
        ErrorMessage("Some of the Edep Histograms could not be found!");
        
        for (Int_t k = 0; k<14; ++k)
        {
            if ((Hist_AlphaScatter[k]==0)||(Hist_AlphaScatter[k]==nullptr))
            {
                kstr = st.Itoa(k,10);
                ErrorMessage("Histogram k="+kstr+" could not be found!");
            }
        }
    }
    else
    {
        // Begin by creating the separate plots:
        gROOT->SetBatch(kTRUE);
        
        Int_t Nbins = Hist_AlphaScatter[0]->GetNbinsX();
        Double_t Xmin = Hist_AlphaScatter[0]->GetXaxis()->GetXmin();
        Double_t Xmax = Hist_AlphaScatter[0]->GetXaxis()->GetXmax();
        Double_t Ymin = Hist_AlphaScatter[0]->GetMinimum();
        Double_t Ymax = Hist_AlphaScatter[0]->GetMaximum();
        Double_t Ypos = 0.95;
        
        TPaveText* text0 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*Ypos,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*Ypos*1.01,"NB");
        text0->SetFillColor(0); 
        text0->AddText("Scattering Angle all tracks"); 
        text0->SetTextColor(kBlack); 
        text0->SetTextSize(0.04); 
        text0->SetTextFont(1);
        text0->SetTextAngle(0.0); 
        
        if (ThisLogScale==kTRUE) {Ypos = Ypos/2.0;}
        else {Ypos = Ypos - 0.05;}
        
        TPaveText* text1 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*Ypos,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*Ypos*1.01,"NB");
        text1->SetFillColor(0);  
        text1->SetTextSize(0.04); 
        text1->SetTextFont(1);
        text1->SetTextAngle(0.0); 
        
        TCanvas* c1 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[1]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[1]->SetLineColor(kGreen+2);
        Hist_AlphaScatter[1]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle neutrons");
        text1->SetTextColor(kGreen+2);
        text1->Draw("same");
        c1->SaveAs(OutPutPath+"/AlphaScat_01neutrons.png");
        c1->Close();
        
        TCanvas* c2 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[2]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[2]->SetLineColor(kRed);
        Hist_AlphaScatter[2]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle protons");
        text1->SetTextColor(kRed);
        text1->Draw("same");
        c2->SaveAs(OutPutPath+"/AlphaScat_02protons.png");
        c2->Close();
        
        TCanvas* c3 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[3]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[3]->SetLineColor(kCyan+1);
        Hist_AlphaScatter[3]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle electrons");
        text1->SetTextColor(kCyan+1);
        text1->Draw("same");
        c3->SaveAs(OutPutPath+"/AlphaScat_03electrons.png");
        c3->Close();
        
        TCanvas* c4 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[4]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[4]->SetLineColor(kGreen);
        Hist_AlphaScatter[4]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle gammas");
        text1->SetTextColor(kGreen);
        text1->Draw("same");
        c4->SaveAs(OutPutPath+"/AlphaScat_04gammas.png");
        c4->Close();
        
        TCanvas* c5 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[5]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[5]->SetLineColor(kYellow+1);
        Hist_AlphaScatter[5]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle neutrinos");
        text1->SetTextColor(kYellow+1);
        text1->Draw("same");
        c5->SaveAs(OutPutPath+"/AlphaScat_05neutrinos.png");
        c5->Close();
        
        TCanvas* c6 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[6]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[6]->SetLineColor(kTeal);
        Hist_AlphaScatter[6]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle pions");
        text1->SetTextColor(kTeal);
        text1->Draw("same");
        c6->SaveAs(OutPutPath+"/AlphaScat_06pions.png");
        c6->Close();
        
        TCanvas* c7 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[7]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[7]->SetLineColor(kSpring-10);
        Hist_AlphaScatter[7]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle muons");
        text1->SetTextColor(kSpring-10);
        text1->Draw("same");
        c7->SaveAs(OutPutPath+"/AlphaScat_07muons.png");
        c7->Close();
        
        TCanvas* c8 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[8]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[8]->SetLineColor(kOrange);
        Hist_AlphaScatter[8]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle positrons");
        text1->SetTextColor(kOrange);
        text1->Draw("same");
        c8->SaveAs(OutPutPath+"/AlphaScat_08positrons.png");
        c8->Close();
        
        TCanvas* c9 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[9]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[9]->SetLineColor(kBlue);
        Hist_AlphaScatter[9]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle deuterons");
        text1->SetTextColor(kBlue);
        text1->Draw("same");
        c9->SaveAs(OutPutPath+"/AlphaScat_09deuterons.png");
        c9->Close();
        
        TCanvas* c10 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[10]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[10]->SetLineColor(kMagenta);
        Hist_AlphaScatter[10]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle tritons");
        text1->SetTextColor(kMagenta);
        text1->Draw("same");
        c10->SaveAs(OutPutPath+"/AlphaScat_10tritons.png");
        c10->Close();
        
        TCanvas* c11 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[11]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[11]->SetLineColor(kViolet+1);
        Hist_AlphaScatter[11]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle alphas");
        text1->SetTextColor(kViolet+1);
        text1->Draw("same");
        c11->SaveAs(OutPutPath+"/AlphaScat_11alphas.png");
        c11->Close();
        
        TCanvas* c12 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[12]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[12]->SetLineColor(kOrange+3);
        Hist_AlphaScatter[12]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle C-ions");
        text1->SetTextColor(kOrange+3);
        text1->Draw("same");
        c12->SaveAs(OutPutPath+"/AlphaScat_12Cions.png");
        c12->Close();
        
        TCanvas* c13 = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        text0->Draw("same");
        Hist_AlphaScatter[13]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[13]->SetLineColor(kAzure+3);
        Hist_AlphaScatter[13]->Draw("HistSame");
        text1->Clear();
        text1->AddText("Scattering Angle fragments");
        text1->SetTextColor(kAzure+3);
        text1->Draw("same");
        c13->SaveAs(OutPutPath+"/AlphaScat_13fragments.png");
        c13->Close();
        
        // Next, make a sumplot:
        TH1D* ProtonDeuteron = new TH1D("ProtonDeuteron_alpha","ProtonDeuteron_alpha",Nbins,Xmin,Xmax);
        TH1D* ProtonDeuteronTriton = new TH1D("ProtonDeuteronTriton_alpha","ProtonDeuteronTriton_alpha",Nbins,Xmin,Xmax);
        TH1D* ProtonDeuteronTritonAlpha = new TH1D("ProtonDeuteronTritonAlpha_alpha","ProtonDeuteronTritonAlpha_alpha",Nbins,Xmin,Xmax);
        
        for (Int_t k = 0; k<(Nbins+2); ++k)
        {
            ProtonDeuteron->SetBinContent(k,Hist_AlphaScatter[2]->GetBinContent(k)+Hist_AlphaScatter[9]->GetBinContent(k));
            ProtonDeuteron->SetBinError(k,TMath::Sqrt(ProtonDeuteron->GetBinContent(k)));
            
            ProtonDeuteronTriton->SetBinContent(k,Hist_AlphaScatter[2]->GetBinContent(k)+Hist_AlphaScatter[9]->GetBinContent(k)+Hist_AlphaScatter[10]->GetBinContent(k));
            ProtonDeuteronTriton->SetBinError(k,TMath::Sqrt(ProtonDeuteronTriton->GetBinContent(k)));
            
            ProtonDeuteronTritonAlpha->SetBinContent(k,Hist_AlphaScatter[2]->GetBinContent(k)+Hist_AlphaScatter[9]->GetBinContent(k)+Hist_AlphaScatter[10]->GetBinContent(k)+Hist_AlphaScatter[11]->GetBinContent(k));
            ProtonDeuteronTritonAlpha->SetBinError(k,TMath::Sqrt(ProtonDeuteronTritonAlpha->GetBinContent(k)));
        }
        
        TCanvas* cSum = DrawHistDouma(Hist_AlphaScatter[0],"Scattering Angle Distribution","Deposited Energy [keV]","Counts",1,ThisLogScale);
        Hist_AlphaScatter[0]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[0]->SetLineColor(kBlack);
        Hist_AlphaScatter[0]->SetFillColor(kBlack);
        Hist_AlphaScatter[0]->Draw("histsame");
        ProtonDeuteronTritonAlpha->SetLineWidth(ScaleFactor);
        ProtonDeuteronTritonAlpha->SetLineColor(kMagenta);
        ProtonDeuteronTritonAlpha->SetFillColor(kMagenta);
        ProtonDeuteronTritonAlpha->Draw("histsame");
        ProtonDeuteronTriton->SetLineWidth(ScaleFactor);
        ProtonDeuteronTriton->SetLineColor(kBlue);
        ProtonDeuteronTriton->SetFillColor(kBlue);
        ProtonDeuteronTriton->Draw("histsame");
        ProtonDeuteron->SetLineWidth(ScaleFactor);
        ProtonDeuteron->SetLineColor(kGreen);
        ProtonDeuteron->SetFillColor(kGreen);
        ProtonDeuteron->Draw("histsame");
        Hist_AlphaScatter[2]->SetLineWidth(ScaleFactor);
        Hist_AlphaScatter[2]->SetLineColor(kRed);
        Hist_AlphaScatter[2]->SetFillColor(kRed);
        Hist_AlphaScatter[2]->Draw("histsame");
        
        text0->Clear();
        text0->AddText("All particles");
        text0->Draw("same");
        
        text1->Clear();
        text1->AddText("Protons");
        text1->SetTextColor(kRed);
        text1->Draw("same");
        
        if (ThisLogScale==kTRUE) {Ypos = Ypos/2.0;}
        else {Ypos = Ypos - 0.05;}
        
        TPaveText* text2 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*Ypos,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*Ypos*1.01,"NB");
        text2->SetFillColor(0);  
        text2->SetTextSize(0.04); 
        text2->SetTextFont(1);
        text2->SetTextAngle(0.0);
        text2->AddText("Deuterons");
        text2->SetTextColor(kGreen);
        text2->Draw("same");
        
        if (ThisLogScale==kTRUE) {Ypos = Ypos/2.0;}
        else {Ypos = Ypos - 0.05;}
        
        TPaveText* text3 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*Ypos,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*Ypos*1.01,"NB");
        text3->SetFillColor(0);  
        text3->SetTextSize(0.04); 
        text3->SetTextFont(1);
        text3->SetTextAngle(0.0);
        text3->AddText("Tritons");
        text3->SetTextColor(kBlue);
        text3->Draw("same");
        
        if (ThisLogScale==kTRUE) {Ypos = Ypos/2.0;}
        else {Ypos = Ypos - 0.05;}
        
        TPaveText* text4 = new TPaveText(Xmin+(Xmax-Xmin)*0.75,Ymin+(Ymax-Ymin)*Ypos,Xmin+(Xmax-Xmin)*0.76,Ymin+(Ymax-Ymin)*Ypos*1.01,"NB");
        text4->SetFillColor(0);  
        text4->SetTextSize(0.04); 
        text4->SetTextFont(1);
        text4->SetTextAngle(0.0);
        text4->AddText("Alphas");
        text4->SetTextColor(kMagenta);
        text4->Draw("same");
        
        if (ThisLogScale==kTRUE) {Ypos = Ypos/2.0;}
        else {Ypos = Ypos - 0.05;}
        
        cSum->SaveAs(OutPutPath + "/AlphaScat_Overview.png");
        cSum->Close();
        
        // Done.
    }
}
