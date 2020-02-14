Double_t SquaredWasted_EqualDist(Double_t const *TheArray)
{
    // The function that is supposed to be minimized
    // for optimizing the cuts. It is NOT a class member,
    // and, therefore, it works only with the global variables.
    
    // The input needs to be a single array. Its dimension
    // will be 2, where Kappa is the first element and the distance
    // between the cuts is the second.

    // adjust the cuts to the inputs:
    GLOBAL_Kappa = TheArray[0];
    
    if (GLOBAL_Max_Multiplicity>0) {GLOBAL_Ethresholds[0] = 1.0;}
    
    if (GLOBAL_Max_Multiplicity>1)
    {
        for (Int_t k = 2; k<(GLOBAL_Max_Multiplicity+1); ++k)
        {
            GLOBAL_Ethresholds[k-1] = GLOBAL_Ethresholds[k-2] + TheArray[1];
        }
    }
    
    // Next, compute the squared wasted efficiency:
    Double_t TotalSum = 0.0;
    Double_t ThisEff = 0.0;
    
    for (Int_t k = 0; k<(GLOBAL_Max_Multiplicity+1); ++k)
    {
        ThisEff = GLOBAL_HistIntegral(k,k);
        TotalSum = TotalSum + (1.0-ThisEff)*(1.0-ThisEff);
    }
        
    // And Finally, return the answer:
    return TotalSum;
}
