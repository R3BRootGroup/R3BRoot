void R3BTradMethClass::OptimizeCuts()
{
    // Optimizes the current cuts of the traditional methods
    // by minimizing the squared wasted efficiency.
    
    // Check if we can actually optimize:
    if ((Cuts_Initialized==kTRUE)&&(Histograms_Loaded==kTRUE))
    {
        // Declare the ROOT minimizer:  
        ROOT::Math::Minimizer* Minimizer = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
    
        // Set the properties of the Minimizer:
        Minimizer->SetMaxFunctionCalls(10000);
        Minimizer->SetMaxIterations(10000);
        Minimizer->SetTolerance(0.001);
    
        // Declare the ROOT functor to be minimized. This has to be an
        // independent function; it cannot be a member function.
        ROOT::Math::Functor f(&SquaredWasted,Max_Multiplicity+1);
        
        // And link the functor to the minimizer:
        Minimizer->SetFunction(f);
    
        // Next, declare the steps, initial values & limits of the 
        // input variables of the minimization function:
        Double_t* Steps = new Double_t[Max_Multiplicity+1];
        Double_t* Initials = new Double_t[Max_Multiplicity+1];
        Double_t* Lowers = new Double_t[Max_Multiplicity+1];
        Double_t* Uppers = new Double_t[Max_Multiplicity+1];
    
        // Extract histogram information to supply the values:
        Int_t NbinsX = MultHistograms[0]->GetNbinsX();
        Int_t NbinsY = MultHistograms[0]->GetNbinsY();
        Double_t Xmin = MultHistograms[0]->GetXaxis()->GetXmin();
        Double_t Xmax = MultHistograms[0]->GetXaxis()->GetXmax();
        Double_t Ymin = MultHistograms[0]->GetYaxis()->GetXmin();
        Double_t Ymax = MultHistograms[0]->GetYaxis()->GetXmax();
    
        // Supply Kappa-values (NB: Kappa<0):
        Initials[0] = Kappa;
        Steps[0] = TMath::Abs(Kappa*0.01);
        Uppers[0] = 0.0;
        Lowers[0] = 3.0*Kappa;
    
        // Supply other values:
        if (Max_Multiplicity>0)
        {
            for (Int_t k = 1; k<(Max_Multiplicity+1); ++k)
            {
                if (k==1) {Initials[k] = Ethresholds[k-1];}
                else {Initials[k] = Ethresholds[k-1]-Ethresholds[k-2];}
            
                Steps[k] = (Xmax-Xmin)/((Int_t) NbinsX);
                Uppers[k] = (Xmax-Xmin)/2.0;
                Lowers[k] = 0.0;
            }
            
            // remove n=0 from optimization if wanted:
            if (Include_Zero==kFALSE)
            {
                Steps[1] = 0.0000001;
                Initials[1] = 1.0;
                Lowers[1] = 0.99999;
                Uppers[1] = 1.00001;
            }
        }
    
        // supply these variables to the minimizer:
        Minimizer->SetLimitedVariable(0,"Kappa",Initials[0],Steps[0],Lowers[0],Uppers[0]);
    
        TString st = "";
        TString kstr = "";
    
        if (Max_Multiplicity>0)
        {
            for (Int_t k = 1; k<(Max_Multiplicity+1); ++k)
            {
                kstr = st.Itoa(k,10);
                kstr = "Ethreshold_"+kstr;
                Minimizer->SetLimitedVariable(k,kstr.Data(),Initials[k],Steps[k],Lowers[k],Uppers[k]);
            }
        }
    
    
        // Now everything is set. Execute the minimization process: 
        Minimizer->Minimize();
        
        // Store the status:
        MinimizerStatus = (Int_t) Minimizer->Status();
    
        // Then extract the results:
        Double_t const *Results = Minimizer->X();
    
        Kappa = Results[0];
        
        for (Int_t k = 1; k<(Max_Multiplicity+1); ++k)
        {
            if (k==1) {Ethresholds[k-1] = Results[k];}
            else {Ethresholds[k-1] = Ethresholds[k-2] + Results[k];}
        }
        
        // And also pass to the global variables:
        GLOBAL_Max_Multiplicity = Max_Multiplicity;
        GLOBAL_MultHistograms = MultHistograms;
        GLOBAL_Ethresholds = Ethresholds;
        GLOBAL_Kappa = Kappa;
    }
    else
    {
        ErrorMessage("You need valid cuts AND loaded histograms for an optimization!");
    }
}
