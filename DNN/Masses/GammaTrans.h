void Nucleus::AddLevelGamma(Int_t const Key, Double_t const EE, Double_t const II, Double_t const Ef, TString const GamMult, TString const Unit)
{
    // Adds a gamma transition to an energy level of a certain nucleus. 
    // NOTE: All energies HAVE to be in the unit specified in Unit.
    // Check that it actually exists:    
    Int_t ComparisonCounter = 0;
    Int_t Size = (Int_t) Levels.size();
    Int_t Index = -1;
    
    if (Size>0)
    {
        for (Int_t k = 0; k<Size; ++k)
        {
            if (Levels.at(k).Key==Key) 
            {
                ComparisonCounter = ComparisonCounter + 1;
                Index = k;
            }
        }
    }
    
    if (ComparisonCounter!=1)
    {
        cout << "NUCEAR LEVEL ERROR: This energy level does not exist! Error in Key = " << Key << "\n\n";
    }
    else
    {
        // Then we can create the state:
        GammaTransition NewGamma;
        NewGamma.I_gamma = II/100.0; // NOTE: So that we can input it as in the nndc database & still match our own units too.
        NewGamma.GamMult = GamMult;
        
        // Do the units:
        Bool_t UnitSucces = kFALSE;
        
        if (Unit=="MeV")
        {
            NewGamma.E_final = Ef;
            NewGamma.E_gamma = EE;
            UnitSucces = kTRUE;
        }
        else if (Unit=="keV")
        {
            NewGamma.E_final = Ef/1000.0;
            NewGamma.E_gamma = EE/1000.0;
            UnitSucces = kTRUE;
        }
        else if (Unit=="J")
        {
            NewGamma.E_final = Ef/PhysConst.q_MeVJ;
            NewGamma.E_gamma = EE/PhysConst.q_MeVJ;
            UnitSucces = kTRUE;
        }
        else
        {
            UnitSucces = kFALSE;
            cout << "NUCEAR LEVEL ERROR: The unit <"<<Unit<<"> was not defined in Nucleus.AddLevelGamma()! Error in Key = " << Levels.at(Index).Key << "\n\n";
        }
            
        // Next, check that this state does not yet exist:
        Int_t GamSize = (Int_t) Levels.at(Index).Gamma.size();
        Int_t GamCounter = 0;
            
        if (GamSize>0)
        {
            for (Int_t k = 0; k<GamSize; ++k)
            {
                if (TMath::Abs(Levels.at(Index).Gamma.at(k).E_gamma-NewGamma.E_gamma)<1e-6)
                {
                    // NOTE: We only test for the energy of the gamma. Then we define it as existing.
                    GamCounter = GamCounter + 1;
                }
            }
        }
            
        if (GamCounter!=0)
        {
            cout << "NUCEAR LEVEL ERROR: Level " << Levels.at(Index).Key << " already has a gamma transition with E_gamma=" << NewGamma.E_gamma << " defined!\n\n";
        }
        else
        {
            // The next test that we have to apply is whether the transition is valid.
            // Check that the final state is defined:
            Int_t EfCounter = 0;
            
            if (Size>0)
            {
                for (Int_t k = 0; k<Size; ++k)
                {
                    if (k!=Index) // Ef sould not be the same level as the current one!
                    {
                        if (TMath::Abs((Levels.at(k).Energy-NewGamma.E_final))<1e-6)
                        {
                            EfCounter = EfCounter + 1;
                        }
                    }
                }
            }
            
            if (EfCounter!=1)
            {
                cout << "NUCEAR LEVEL ERROR: You specified a Gamma-transition to a non-defined final energy state! Error in Key = " << Levels.at(Index).Key << " | E(level) = " << Levels.at(Index).Energy << " | Diff = " << TMath::Abs(Levels.at(Index).Energy - NewGamma.E_final) << "\n\n";
            }
            else
            {
                // Test that the transition makes sense:
                if (TMath::Abs(Levels.at(Index).Energy - NewGamma.E_gamma - NewGamma.E_final)>1e-3)
                {
                    cout << "NUCEAR LEVEL ERROR: The gamma-energy of your transition does not correspond to Ei-Ef! Error in Key = " << Levels.at(Index).Key << " | E(level) = " << Levels.at(Index).Energy << " | Ei-Ef-Egam = " 
                         << TMath::Abs(Levels.at(Index).Energy - NewGamma.E_gamma - NewGamma.E_final) << "\n\n";
                }
                else
                {
                    // Then, we can finally add our gamma-transition, provided that we have not encountered
                    // problems with the units:
                    if (UnitSucces==kTRUE)
                    {
                        Levels.at(Index).Gamma.push_back(NewGamma);
                        
                        // Then close all blocks:
                    }
                }
            }
        }
    }
}

// ====================================================================================================================

void Nucleus::ClearLevelGamma(Int_t const Key)
{
    // Clears all Gamma-transitions of an existing energy level of this nucleus.
    // Check that it actually exists:
    Int_t ComparisonCounter = 0;
    Int_t Size = (Int_t) Levels.size();
    Int_t Index = -1;
    
    if (Size>0)
    {
        for (Int_t k = 0; k<Size; ++k)
        {
            if (Levels.at(k).Key==Key) 
            {
                ComparisonCounter = ComparisonCounter + 1;
                Index = k;
            }
        }
    }
    
    if (ComparisonCounter!=1)
    {
        cout << "NUCEAR LEVEL ERROR: This energy level does not exist!\n\n";
    }
    else
    {
        Levels.at(Index).Gamma.clear();
    }
}