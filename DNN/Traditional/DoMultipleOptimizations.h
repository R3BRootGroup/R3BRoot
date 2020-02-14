void R3BTradMethClass::DoMultipleOptimizations(Int_t const nTimes)
{
    // This function repeats the initialization and optimization 
    // of the cuts nTimes, so that a more robust average is obtained.
    
    // First, see if the histograms are loaded:
    if (Histograms_Loaded==kFALSE)
    {
        cout << "### TradMed ERROR: Yoe can only call DoMultipleOptimizations(Int_t const nTimes) after loading the histograms!\n\n";
    }
    else
    {
        // Then, reset the average values:
        Avg_Kappa = 0.0;
        for (Int_t k = 0; k<Max_Multiplicity; ++k) {Avg_Ethresholds[k] = 0.0;}
        Int_t Take_Along = 0;
        
        // Next, perform optimizations:
        Int_t kTime = 0;
        
        while (Take_Along<nTimes)
        {
            // Do the optimization:
            InitializeCuts();
            Optimization_IncludeZero();
            OptimizeCuts();
        
            // Then, update the averages:
            if ((MinimizerStatus==0)||(MinimizerStatus==5))
            {
                Avg_Kappa = Avg_Kappa + Kappa;
                for (Int_t k = 0; k<Max_Multiplicity; ++k) {Avg_Ethresholds[k] = Avg_Ethresholds[k] + Ethresholds[k];}
                Take_Along = Take_Along + 1;
            }
            
            // Give some output:
            cout << "Optimzation " << kTime+1 << "/" << nTimes << " was done. Status = " << MinimizerStatus << "\n";
            ++kTime;
            
            // Done.
        }
        
        // Finish computation of the averages:
        if (Take_Along>0)
        {
            Avg_Kappa = Avg_Kappa/((Int_t) Take_Along);
            for (Int_t k = 0; k<Max_Multiplicity; ++k) {Avg_Ethresholds[k] = Avg_Ethresholds[k]/((Int_t) Take_Along);}
        }
        else
        {
            Avg_Kappa = Kappa;
            for (Int_t k = 0; k<Max_Multiplicity; ++k) {Avg_Ethresholds[k] = Ethresholds[k];}
        }
        
        // Pass average values to standard-values:
        Kappa = Avg_Kappa;
        GLOBAL_Kappa = Avg_Kappa;
        for (Int_t k = 0; k<Max_Multiplicity; ++k) {Ethresholds[k] = Avg_Ethresholds[k];}
        for (Int_t k = 0; k<Max_Multiplicity; ++k) {GLOBAL_Ethresholds[k] = Avg_Ethresholds[k];}
        
        // Done.
    }
}
