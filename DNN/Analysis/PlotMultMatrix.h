void R3BMultMatrix::PlotMultMatrix(Double_t** const ThisMatrix, TString const ThisTitle, TString const IntMult)
{
    // This functions draws a graphical representation of the provided neutron separation matrix.
    gROOT->SetBatch(kTRUE);
    
    // Begin with the declaration of a histogram:
    TH2D* MultHist = new TH2D(ThisTitle,ThisTitle,MaxMultiplicity+1,-0.5,((Int_t) (MaxMultiplicity+1))-0.5,MaxMultiplicity+1,-0.5,((Int_t) (MaxMultiplicity+1))-0.5);
    
    // Next, fill this histogram with the matrix:
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (ThisMatrix[kcol][krow]>0.0001)
            {
                MultHist->SetBinContent(krow+1,kcol+1,100.0*ThisMatrix[kcol][krow]);
                MultHist->SetBinError(krow+1,kcol+1,0.0);
            }
            else
            {
                MultHist->SetBinContent(krow+1,kcol+1,-1.0);
                MultHist->SetBinError(krow+1,kcol+1,0.0);
            }
        }
    }
    
    // Then, plot the representation:
    TCanvas* c1 = DrawHistDouma_2D_flat(MultHist,ThisTitle,IntMult,"Determined Multiplicity","[%]");
    
    // Next, add the numbers into the picture:
    TPaveText*** TheNumbers = new TPaveText**[MaxMultiplicity+1];
    Double_t Xpos;
    Double_t Ypos;
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        TheNumbers[krow] = new TPaveText*[MaxMultiplicity+1];
        
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (ThisMatrix[kcol][krow]>0.001)
            {
                Xpos = (Int_t) krow;
                Ypos = (Int_t) kcol;
            
                TheNumbers[krow][kcol] = new TPaveText(Xpos-0.001,Ypos-0.001,Xpos+0.001,Ypos+0.001,"NB");
                TheNumbers[krow][kcol]->SetFillColor(0);
                TheNumbers[krow][kcol]->AddText(RoundOff(100.0*ThisMatrix[kcol][krow],3) + "%");
                TheNumbers[krow][kcol]->SetTextColor(1);
                TheNumbers[krow][kcol]->SetTextSize(0.03);
                TheNumbers[krow][kcol]->SetTextFont(1);
                TheNumbers[krow][kcol]->SetTextAngle(0.0);
                
                if (kcol==krow) 
                {
                    TheNumbers[krow][kcol]->SetTextSize(0.057);
                    TheNumbers[krow][kcol]->SetTextColor(1);
                }
                
                TheNumbers[krow][kcol]->Draw("same");
            }
        }
    }
    
    // Save the representation:
    c1->SaveAs(TheOutputPath + ThisTitle + ".png");
    c1->Close();
    
    // delete our objects:
    delete MultHist;
    delete c1;
    delete TheNumbers;
    
    // Done.
}
