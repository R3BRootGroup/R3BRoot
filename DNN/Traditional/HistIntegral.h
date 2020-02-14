Double_t R3BTradMethClass::HistIntegral(Int_t const Index, Int_t const Range)
{
    // Integrates the histogram Index over a certain range and then divides by the total.
    
    // Define the answer:
    Double_t Answer = 0.0;
    
    // Check whether the histograms are loaded:
    if (Histograms_Loaded==kFALSE)
    {
        ErrorMessage("You cannot call the HistIntegral-function before loading the histograms!");
    }
    else
    {
        // check that we ask for a valid index:
        if ((Index<0)||(Index>Max_Multiplicity))
        {
            ErrorMessage("You can only ask for a histogram ranging from 0 to the max. multiplicity!");
        }
        else
        {
            // Check that we ask for a valid range:
            if ((Range<-2)||(Range>Max_Multiplicity))
            {
                ErrorMessage("you may only ask for a range from -1 till mult.+1");
                // -1: integrate full histogram:
                //  0: integrate below the lowest line.
                //  1-to-mult. integrate between k-th line & k-1-th line.
                //  mult+1: integrate above highest line.
                // -2: Ask for histogram maximum instead.
            }
            else
            {
                // Now we can start with the integration. Use the global function for that purpose, so that
                // we do not need to duplicate code.
                GLOBAL_Max_Multiplicity = Max_Multiplicity;
                GLOBAL_MultHistograms = MultHistograms;
                GLOBAL_Ethresholds = Ethresholds;
                GLOBAL_Kappa = Kappa;
                
                Answer = GLOBAL_HistIntegral(Index,Range);
                    
                // Done. close all blocks:
            }
        }
    }
                            
    // return the answer:
    return Answer;
}
