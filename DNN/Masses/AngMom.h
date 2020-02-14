void Nucleus::AddLevelAngMom(Int_t const Key, Int_t const JJ, TString const Par, Bool_t const Acc)
{
    AddLevelAngMomBetween(Key,(Int_t) JJ,Par,Acc);
}

void Nucleus::AddLevelAngMom(Int_t const Key, Double_t const JJ, TString const Par, Bool_t const Acc)
{
    AddLevelAngMomBetween(Key,JJ,Par,Acc);
}

void Nucleus::AddLevelAngMomBetween(Int_t const Key, Double_t const JJ, TString const Par, Bool_t const Acc)
{
    // Adds an angular momentum state to an existing energy level of this nucleus.
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
        // Check that parity was specified properly:
        if (!((Par=="+")||(Par=="-")||(Par=="?")))
        {
            cout << "NUCEAR LEVEL ERROR: You did not specify a valid parity!\n\n";
        }
        else
        {
            // Test if the angular momentum is either an integer or half-integer:
            Int_t Rem_i = (Int_t) JJ;
            Double_t Rem_d = (Int_t) Rem_i;
            Double_t Rem_Diff = JJ - Rem_d;
            
            if (!((TMath::Abs(Rem_Diff)<1e-6)||(TMath::Abs(Rem_Diff-0.5)<1e-6)))
            {
                cout << "NUCEAR LEVEL ERROR: You can only specify integer of half-integer parity! Key = " << Key << "\n\n";
            }
            else
            {
            
                // Then we can create the state:
                AngularMomentum NewAng;
                NewAng.Parity = Par;
                NewAng.Accuracy = Acc;
                NewAng.Jdef = kTRUE;

                if (TMath::Abs(Rem_Diff)<1e-6) {NewAng.J = Rem_d;}
                else {NewAng.J = Rem_d + 0.5;}
            
                // However, is J is negative, we assume that J is actually fully unknown:
                if (JJ<0.0)
                {
                    NewAng.J = -1.0;
                    NewAng.Jdef = kFALSE;
                }
            
                // Next, check that this state does not yet exist:
                Int_t AngSize = (Int_t) Levels.at(Index).AngMom.size();
                Int_t AngIndexx = -1;
                Int_t AngCounter = 0;
            
                if (AngSize>0)
                {
                    for (Int_t k = 0; k<AngSize; ++k)
                    {
                        if (Levels.at(Index).AngMom.at(k).J==JJ)
                        {
                            // NOTE: We only test for J. If J is already there, we do not accept it, even if Parity could be different.
                            AngCounter = AngCounter + 1;
                            AngIndexx = k;
                        }
                    }
                }
            
                if (AngCounter!=0)
                {
                    cout << "NUCEAR LEVEL ERROR: Level "<<Levels.at(Index).Key<<" already has an angular momentum state J="<<JJ<<" defined!\n\n";
                }
                else
                {
                    // add the state:
                    Levels.at(Index).AngMom.push_back(NewAng);
                
                    // close all blocks:
                }
            }
        }
    }
}

// ====================================================================================================================

void Nucleus::ClearLevelAngMom(Int_t const Key)
{
    // Clears all angular momentum states of an existing energy level of this nucleus.
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
        Levels.at(Index).AngMom.clear();
    }
}
