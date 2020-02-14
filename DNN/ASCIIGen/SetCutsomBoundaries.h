void R3BASCIIFileGenerator::SetCutsomBoundaries(std::vector<Double_t> MultProbabilities)
{
    // Defines custom boundaries for multiplicity randomization.
    // The user has to define these boundaries inside MCtransport.cpp!
    CustomBoundaries_AreSpecified = kFALSE;
    
    // Begin by testing the appropriate size:
    if (MultProbabilities.size()!=ParticleGun_Multiplicity)
    {
        ErrorMessage("Your custom multiplicity-probabilities vector did not have the appropriate size!");
    }
    else
    {
        // Test if the sum of the elements equals unity:
        Double_t Sum = 0.0;
        Bool_t PosTest = kTRUE;
        
        for (Int_t k = 0; k<MultProbabilities.size(); ++k)
        {
            Sum = Sum + MultProbabilities.at(k);
            
            if (MultProbabilities.at(k)<=0.0)
            {
                ErrorMessage("Each element of your custom multiplicity-probabilities vector must be strictly positive!");
                PosTest = kFALSE;
            }
        }
        
        if (TMath::Abs(Sum-1.0)>1e-4)
        {
            ErrorMessage("The sum of your custom multiplicity-probabilities vector was not equal to unity!");
        }
        else
        {
            if (PosTest==kTRUE)
            {
                // Then, we are OK. do NOT sort our vector first, because we specify probabilities, not boundaries!
                // Hence, it is enough to know that each element is strictly positive.
                
                // Then, create our boundaries:
                CustomBoundaries.resize(ParticleGun_Multiplicity+1);
                CustomBoundaries.at(0) = 0.0;
                
                for (Int_t k = 1; k<(ParticleGun_Multiplicity+1); ++k)
                {
                    CustomBoundaries.at(k) = CustomBoundaries.at(k-1) + MultProbabilities.at(k-1);
                }
                
                // Done.
                CustomBoundaries_AreSpecified = kTRUE;
            }
        }
    }
}
