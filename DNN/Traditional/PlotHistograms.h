void R3BTradMethClass::PlotHistograms()
{
    // Generates a picture of the calibration histograms.
    // gROOT->SetBatch(kTRUE);
    
    // Check that there is sufficient information to proceed:
    if (Histograms_Loaded==kFALSE)
    {
        ErrorMessage("You need to load the histograms before calling the PlotHistograms-function!");
    }
    else
    {
        // Begin by defining the size of the picture:
        Int_t Nplots = Max_Multiplicity + 1;
        Int_t Nhor = 1;
        Int_t Nvert = 1;
    
        while ((Nhor*Nvert)<Nplots)
        {
            if (Nhor==Nvert) {Nhor = Nhor + 1;}
            else {Nvert = Nvert + 1;}
        }
    
        // Next, generate the titles:
        TString* Titles = new TString[Nplots];
        TString st = "";
        TString kstr = "";
    
        for (Int_t k = 0; k<Nplots; ++k)
        {
            kstr = st.Itoa(k,10);
            Titles[k] = "Multiplicity Histogram n="+kstr;
        }
        
        // Then, determine the overall scale of the plots:
        Double_t PlotMin = 1.0;
        Double_t PlotMax = 2.0;
        
        // Determine histogram maxima:
        Double_t* HistMax = new Double_t[Max_Multiplicity+1];
        
        for (Int_t k = 0; k<(Max_Multiplicity+1); ++k)
        {
            HistMax[k] = HistIntegral(k,-2);
            if ((PlotMax<HistMax[k])&&(k!=0)) {PlotMax = HistMax[k];}
        }
    
        // Then, generate the canvas:
        TCanvas* TheCanvas = DrawHistDouma_2D_flatArray(Nplots,Nhor,Nvert,MultHistograms,Titles,"Energy Deposition [MeV]","#Clusters [dim. less]","Counts",PlotMin,PlotMax);
    
        // Next, generate the lines and the efficiency piece-by-piece:
    
        // Get histogram information:
        Int_t NbinsX = MultHistograms[0]->GetNbinsX();
        Int_t NbinsY = MultHistograms[0]->GetNbinsY();
        Double_t Xmin = MultHistograms[0]->GetXaxis()->GetXmin();
        Double_t Xmax = MultHistograms[0]->GetXaxis()->GetXmax();
        Double_t Ymin = MultHistograms[0]->GetYaxis()->GetXmin();
        Double_t Ymax = MultHistograms[0]->GetYaxis()->GetXmax();
    
        // Declare lines & TPaveText:
        TPaveText** TextEff = new TPaveText*[Nplots];
        TLine** LowerLines = new TLine*[Nplots];
        TLine** UpperLines = new TLine*[Nplots];
        Double_t EffDouble = 0.0;
        TString EffString = "";
    
        // loop over all pictures:
        for (Int_t k = 0; k<Nplots; ++k)
        {
            // Generate the text:
            TextEff[k] = new TPaveText(Xmin+(Xmax-Xmin)*0.22,Ymin+(Ymax-Ymin)*0.93,Xmin+(Xmax-Xmin)*0.23,Ymin+(Ymax-Ymin)*0.94,"NB");
            EffDouble = HistIntegral(k,k);
            if ((EffDouble>=0.0)&&(EffDouble<=1.0)) {EffString = RoundOff(EffDouble*100.0,3) + "%";} else {EffString = "0%";}
            TextEff[k]->SetFillColor(0); 
            TextEff[k]->AddText(EffString); 
            TextEff[k]->SetTextColor(2); 
            TextEff[k]->SetTextSize(0.1); 
            TextEff[k]->SetTextFont(1);
            TextEff[k]->SetTextAngle(0.0); 
        
            // Generate the lower lines:
            if (k==0)
            {
                LowerLines[k] = 0;
            }
            else
            {
                LowerLines[k] = new TLine(0.0,-1.0*Kappa*Ethresholds[k-1],Ethresholds[k-1],0.0);
                LowerLines[k]->SetLineWidth(2.0);
                LowerLines[k]->SetLineColor(1);
            }
        
            // generate the upper lines:
            if (k==Max_Multiplicity)
            {
                UpperLines[k] = 0;
            }
            else
            {
                UpperLines[k] = new TLine(0.0,-1.0*Kappa*Ethresholds[k],Ethresholds[k],0.0);
                UpperLines[k]->SetLineWidth(2.0);
                UpperLines[k]->SetLineColor(1);
            }
        
            // Draw:
            TheCanvas->cd(k+1);
            TextEff[k]->Draw("same");
            if (LowerLines[k]!=0) {LowerLines[k]->Draw("same");}
            if (UpperLines[k]!=0) {UpperLines[k]->Draw("same");}
        
            // Done.
        }
        
        // Save & close the picture:
        TString PATH = Inputs->GetInputString("TheOutputPath");
        PATH = PATH + "TraditionalMethod.png";
        TheCanvas->SaveAs(PATH);
        TheCanvas->Close();
        
        // delete commands:
        delete HistMax;
        
        // Finish blocks:
    }
}
