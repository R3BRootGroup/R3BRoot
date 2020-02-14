Double_t GLOBAL_HistIntegral(Int_t const Index, Int_t const Range)
{
    // Integrates the histogram Index over a certain range and then divides by the total.
    // This function uses the global variables instead of the class variables. However,
    // it does not use any guard for checking whether these variables exist or not.
    
    // Define the answer:
    Double_t Answer = 0.0;
    
    // check that we ask for a valid index:
    if ((Index<0)||(Index>GLOBAL_Max_Multiplicity))
    {
        cout << "### ERROR: You can only ask for a histogram ranging from 0 to the max. multiplicity!\n";
    }
    else
    {
        // Check that we ask for a valid range:
        if ((Range<-2)||(Range>GLOBAL_Max_Multiplicity))
        {
                cout << "### ERROR: you may only ask for a range from -1 till mult.+1\n";
                // -1: integrate full histogram:
                //  0: integrate below the lowest line.
                //  1-to-mult. integrate between k-th line & k-1-th line.
                //  mult+1: integrate above highest line.
                // -2: Ask for histogram maximum instead.
        }
        else
        {
            // Now we can start with the integration. Request histogram information:
            Int_t NbinsX = GLOBAL_MultHistograms[Index]->GetNbinsX();
            Int_t NbinsY = GLOBAL_MultHistograms[Index]->GetNbinsY();
            Double_t Xmin = GLOBAL_MultHistograms[Index]->GetXaxis()->GetXmin();
            Double_t Xmax = GLOBAL_MultHistograms[Index]->GetXaxis()->GetXmax();
            Double_t Ymin = GLOBAL_MultHistograms[Index]->GetYaxis()->GetXmin();
            Double_t Ymax = GLOBAL_MultHistograms[Index]->GetYaxis()->GetXmax();
                
            // Define integral sums:
            Double_t TotalSum = 0.0;
            Double_t PartialSum = 0.0;
            Double_t Content = 0.0;
            Double_t Max = 0.0;
                
            // Define bin centers:
            Double_t BinCenterX = 0.0;
            Double_t BinCenterY = 0.0;
                
            // start integrating. Loop over the histogram. negate overflow & underflow:
            for (Int_t kx = 1; kx<(NbinsX+1); ++kx)
            {
                // Define X-center:
                BinCenterX = ((Int_t) kx) - 0.5;
                BinCenterX = BinCenterX/((Int_t) NbinsX);
                BinCenterX = Xmin + (Xmax-Xmin)*BinCenterX;
                        
                for (Int_t ky = 1; ky<(NbinsY+1); ++ky)
                {
                    // Define Y-center:
                    BinCenterY = ((Int_t) ky) - 0.5;
                    BinCenterY = BinCenterY/((Int_t) NbinsY);
                    BinCenterY = Ymin + (Ymax-Ymin)*BinCenterY;
                            
                    // Request bin content & add to the total:
                    Content = GLOBAL_MultHistograms[Index]->GetBinContent(kx,ky);
                    TotalSum = TotalSum + Content;
                    
                    // Determine the maximum too:
                    if (Max<Content) {Max = Content;}
                            
                    // Decide whether it should be added to the partial sum yes/no:
                    if ((Range>=1)&&(Range<=(GLOBAL_Max_Multiplicity-1)))
                    {
                        // Then, we integrate between the lines:
                        if ((BinCenterY>GLOBAL_Kappa*(BinCenterX - GLOBAL_Ethresholds[Range-1]))&&(BinCenterY<GLOBAL_Kappa*(BinCenterX - GLOBAL_Ethresholds[Range])))
                        {
                            PartialSum = PartialSum + Content;
                        }
                    }
                    else if (Range==0)
                    {
                        // Then, integrate below the lowest line:
                        if (BinCenterY<GLOBAL_Kappa*(BinCenterX - GLOBAL_Ethresholds[0]))
                        {
                            PartialSum = PartialSum + Content;
                        }
                    }
                    else if (Range==GLOBAL_Max_Multiplicity)
                    {
                        // Then, integrate above the highest line:
                        if (BinCenterY>GLOBAL_Kappa*(BinCenterX - GLOBAL_Ethresholds[GLOBAL_Max_Multiplicity-1]))
                        {
                            PartialSum = PartialSum + Content;
                        }
                    }
                    else if (Range==-1)
                    {
                        // Then, integrate everything:
                        PartialSum = PartialSum + Content;
                    }
                    else if (Range==-2)
                    {
                        // Do nothing, because this is OK.
                    }
                    else
                    {
                        cout << "### ERROR: Somehow an unidentified integration option was called. VERY BAD!!!\n";
                    }
                        
                    // Close off:
                }
            }
                
            // Next, define the efficiency:
            Double_t Efficiency = PartialSum/TotalSum;
                
            // Then, define the answer:
            if (Range==-1)
            {
                if (PartialSum==TotalSum) {Answer = TotalSum;}
                else {cout << "### ERROR: Full integration option did not work!\n";}
            }
            else if (Range==-2)
            {
                Answer = Max;
            }
            else
            {
                Answer = Efficiency;
            }
                    
            // Done. close all blocks:
        }
    }
                            
    // return the answer:
    return Answer;
}
