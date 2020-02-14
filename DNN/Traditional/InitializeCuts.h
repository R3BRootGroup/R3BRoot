void R3BTradMethClass::InitializeCuts()
{
    // Call this class after you loaded the histograms.
    // It will provide initial guesses of the cuts and Kappa
    // based on the loaded histograms:
    
    if (Histograms_Loaded==kTRUE)
    {
        // Obtain inputs:
        Int_t nDoublePlanes = Inputs->GetInputInteger("NeuLAND_Number_of_DoublePlanes");
        Double_t DoublePlaneScale = ((Int_t) nDoublePlanes)/30.0;
        
        // Obtain histogram-data:
        Int_t NbinsX = MultHistograms[0]->GetNbinsX();
        Int_t NbinsY = MultHistograms[0]->GetNbinsY();
        Double_t Xmin = MultHistograms[0]->GetXaxis()->GetXmin();
        Double_t Xmax = MultHistograms[0]->GetXaxis()->GetXmax();
        Double_t Ymin = MultHistograms[0]->GetYaxis()->GetXmin();
        Double_t Ymax = MultHistograms[0]->GetYaxis()->GetXmax();
        
        TRandom3* Generator = new TRandom3(0); // seed with time.
        Double_t RandomNumber = 0.95 + Generator->Uniform(0.1);
        delete Generator;
        
        // Guess Kappa:
        Kappa = -1.0*(Ymax-Ymin)/(Xmax-Xmin)*(1.0 + 2.0*DoublePlaneScale)/3.0;
        Kappa = Kappa*RandomNumber;
        
        // Guess stepsize:
        Double_t StepSize = 1.0*(Xmax-Xmin)/((Int_t) Max_Multiplicity);
        StepSize = StepSize*(4.0 + 1.0*DoublePlaneScale)/5.0;
        StepSize = StepSize*RandomNumber;
        
        // Guess values:
        for (Int_t k = 0; k<Max_Multiplicity; ++k)
        {
            Ethresholds[k] = ((Int_t) k)*StepSize;
        }
        
        Ethresholds[0] = 1.0;
        
        // Done:
        Cuts_Initialized = kTRUE;
        
        // Pass to global variables:
        GLOBAL_Max_Multiplicity = Max_Multiplicity;
        GLOBAL_MultHistograms = MultHistograms;
        GLOBAL_Ethresholds = Ethresholds;
        GLOBAL_Kappa = Kappa;
    }
}
