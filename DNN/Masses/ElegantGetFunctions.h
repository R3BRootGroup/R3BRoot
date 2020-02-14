Int_t Nucleus::GetNE()
{
    // returns number of energy levels:
    Int_t Size = (Int_t) Levels.size();
    return Size;
}

void Nucleus::SelectElevel(Int_t const Index)
{
    // Selects an energy level for returning information:
    Int_t Size = (Int_t) Levels.size();
    if ((Index>=0)&&(Index<Size)) {LevelIndex = Index;}
}

void Nucleus::SelectGroundState()
{
    // Makred as the only state with E=0.0:
    Int_t ThisIndex = -1;
    Int_t Counter = 0;
    
    if (Levels.size()>0)
    {
        for (Int_t k = 0; k<Levels.size(); ++k)
        {
            if (TMath::Abs(Levels.at(k).Energy)<1e-6)
            {
                Counter = Counter + 1;
                ThisIndex = k;
            }
        }
    }
    
    if (Counter==0) {cout << "NUCLEAR LEVEL ERROR: You asked for an unspecified ground state!\n\n";}
    else if (Counter>1) {cout << "NUCLEAR LEVEL ERROR: There were multiple ground states specified!\n\n";}
    else
    {
        LevelIndex = ThisIndex;
    }
}

Double_t Nucleus::GetLevelE(TString const Unit)
{
    if (Levels.size()>0)
    {
        if (Unit=="MeV") {return Levels.at(LevelIndex).Energy;}
        else if (Unit=="keV") {return Levels.at(LevelIndex).Energy*1e3;}
        else if (Unit=="eV") {return Levels.at(LevelIndex).Energy*1e6;}
        else if (Unit=="GeV") {return Levels.at(LevelIndex).Energy/1e3;}
        else if (Unit=="J") {return Levels.at(LevelIndex).Energy*PhysConst.q_MeVJ;}
        else
        {
            cout << "NUCLEAR LEVEL ERROR: The unit " << Unit << " was not programmed into Nucleus::GetLevelE !\n\n";
            return -1.0;
        }
    }
    else
    {
        return -1.0;
    }
}
    
Double_t Nucleus::GetLevelLifeTime(TString const Unit) 
{
    if (Levels.size()>0)
    {
        if (Unit=="sec") {return Levels.at(LevelIndex).LifeTime;}    // NOTE: Returned in seconds!
        else if (Unit=="s") {return Levels.at(LevelIndex).LifeTime;} // NOTE: Returned in seconds!
        else if (Unit=="ms") {return Levels.at(LevelIndex).LifeTime*1000.0;}
        else if (Unit=="us") {return Levels.at(LevelIndex).LifeTime*1e6;}
        else if (Unit=="ns") {return Levels.at(LevelIndex).LifeTime*1e9;}
        else if (Unit=="ps") {return Levels.at(LevelIndex).LifeTime*1e12;}
        else if (Unit=="min") {return Levels.at(LevelIndex).LifeTime/60.0;}
        else if (Unit=="hour") {return Levels.at(LevelIndex).LifeTime/(60.0*60.0);}
        else if (Unit=="day") {return Levels.at(LevelIndex).LifeTime/(60.0*60.0*24.0);}
        else if (Unit=="week") {return Levels.at(LevelIndex).LifeTime/(60.0*60.0*24.0*7.0);}
        else if (Unit=="month") {return Levels.at(LevelIndex).LifeTime/(60.0*60.0*24.0*365.0/12.0);}
        else if (Unit=="year") {return Levels.at(LevelIndex).LifeTime/(60.0*60.0*24.0*365.0);}
        else
        {
            cout << "NUCLEAR LEVEL ERROR: The unit " << Unit << " was not supported in Nucleus::GetLevelLifeTime !\n\n";
            return -1.0;
        }
    }
    else 
    {
        return -1.0;
    }
}
    
Double_t Nucleus::GetBGT()
{
    // BGT is supposed to be dimensionless.
    if (Levels.size()>0)
    {
        if (Levels.at(LevelIndex).BGT_spec==kTRUE)
        {
            return Levels.at(LevelIndex).BGT;
        }
        else
        {
            cout << "NUCLEAR LEVEL ERROR: You asked for a B(GT)-value that you did not specify previously!\n\n";
            return -1.0;
        }
    }
    else
    {
        return -1.0;
    }
}
    
Double_t Nucleus::GetBF()
{
    // BF is supposed to be dimensionless.
    if (Levels.size()>0)
    {
        if (Levels.at(LevelIndex).BF_spec==kTRUE)
        {
            return Levels.at(LevelIndex).BF;
        }
        else
        {
            cout << "NUCLEAR LEVEL ERROR: You asked for a B(F)-value that you did not specify previously!\n\n";
            return -1.0;
        }
    }
    else
    {
        return -1.0;
    }
}    
    
Int_t Nucleus::GetKey()
{
    // return the key-number of this level:
    if (Levels.size()>0)
    {
        return Levels.at(LevelIndex).Key;
    }
    else
    {
        return -1000000;
    }
}
    
TString Nucleus::GetRefs()
{
    if (Levels.size()>0)
    {
        return Levels.at(LevelIndex).References;
    }
    else
    {
        TString Answer = "There are no levels to ask references for!";
        return Answer;
    }
}
    
void Nucleus::SelectElevel_WithKey(Int_t const ThisKey)
{
    // Select an energy level through its keynumber:
    if (Levels.size()>0)
    {
        Int_t Counter = 0;
        Int_t ThisIndex = 0;
        
        for (Int_t k = 0; k<Levels.size(); ++k)
        {
            if (Levels.at(k).Key==ThisKey)
            {
                Counter = Counter + 1;
                ThisIndex = k;
            }
        }
        
        if (Counter==0) {cout << "NUCLEAR LEVEL ERROR: You wanted to select an energy level that does not exist!\n\n";}
        else if (Counter>1) {cout << "NUCLEAR LEVEL ERROR: There are multiple levels that match this key! Error in the code!!!\n\n";}
        else
        {
            LevelIndex = ThisIndex;
        }
    }
}
    
// ===================================================================================================================

// Now continue with the angular momentum quantities:
Int_t Nucleus::GetNAngMoms()
{
    if (Levels.size()>0)
    {
        return Levels.at(LevelIndex).AngMom.size();
    }
    else
    {
        return -1;
    }
}

void Nucleus::SelectAngMom(Int_t const Index)
{
    // selects a certain angular momentum:
    if (Levels.size()>0)
    {
        Int_t AngSize = Levels.at(LevelIndex).AngMom.size();
        
        if ((Index>=0)&&(Index<AngSize))
        {
            AngIndex = Index;
            // cout << " << AngIndex = " << AngIndex << " >> \n";
        }
    }
}

TString Nucleus::GetParity()
{
    if (Levels.size()>0)
    {
        if (Levels.at(LevelIndex).AngMom.size()>0)
        {
            return Levels.at(LevelIndex).AngMom.at(AngIndex).Parity;
        }
        else
        {
            return "?";
        }
    }
    else
    {
        return "?";
    }
}

Double_t Nucleus::GetJ()
{
    if (Levels.size()>0)
    {
        if (Levels.at(LevelIndex).AngMom.size()>0)
        {
            // cout << "=====================> " << AngIndex << "/" << Levels.at(LevelIndex).AngMom.size() << " || " << LevelIndex << "/" << Levels.size() << "\n\n";
            
            if (Levels.at(LevelIndex).AngMom.at(AngIndex).Jdef==kTRUE)
            {
                return Levels.at(LevelIndex).AngMom.at(AngIndex).J;
            }
            else
            {
                return -1000.0;
            }
        }
        else
        {
            return -1000.0;
        }
    }
    else
    {
        return -1000.0;
    }
}

Bool_t Nucleus::GetJacc()
{
    if (Levels.size()>0)
    {
        if (Levels.at(LevelIndex).AngMom.size()>0)
        {
            return Levels.at(LevelIndex).AngMom.at(AngIndex).Accuracy;
        }
        else
        {
            return kFALSE;
        }
    }
    else
    {
        return kFALSE;
    }
}

// ===================================================================================================

// Now do the gamma transitions:
Int_t Nucleus::GetNGammas()
{
    if (Levels.size()>0)
    {
        return Levels.at(LevelIndex).Gamma.size();
    }
    else
    {
        return -1;
    }
}

void Nucleus::SelectGamma(Int_t const Index)
{
    // selects a certain angular momentum:
    if (Levels.size()>0)
    {
        Int_t GammaSize = Levels.at(LevelIndex).Gamma.size();
        
        if ((Index>=0)&&(Index<GammaSize))
        {
            GammaIndex = Index;
        }
    }
}

Double_t Nucleus::GetGammaE()
{
    if (Levels.size()>0)
    {
        if (Levels.at(LevelIndex).Gamma.size()>0)
        {
            return Levels.at(LevelIndex).Gamma.at(GammaIndex).E_gamma;
        }
        else
        {
            return -1000.0;
        }
    }
    else
    {
        return -1000.0;
    }
}

Double_t Nucleus::GetGammaI()
{
    if (Levels.size()>0)
    {
        if (Levels.at(LevelIndex).Gamma.size()>0)
        {
            return Levels.at(LevelIndex).Gamma.at(GammaIndex).I_gamma;
        }
        else
        {
            return -1000.0;
        }
    }
    else
    {
        return -1000.0;
    }
}

TString Nucleus::GetGammaTransitionType()
{
    if (Levels.size()>0)
    {
        if (Levels.at(LevelIndex).Gamma.size()>0)
        {
            return Levels.at(LevelIndex).Gamma.at(GammaIndex).GamMult;
        }
        else
        {
            return "??";
        }
    }
    else
    {
        return "??";
    }
}

EnergyLevel Nucleus::GetFinalLevel()
{
    Int_t FinalIndex = -1;
    Int_t Counter = 0;
    
    for (Int_t k = 0; k<Levels.size(); ++k)
    {
        if (TMath::Abs(Levels.at(LevelIndex).Gamma.at(GammaIndex).E_final - Levels.at(k).Energy)<1e-6)
        {
            Counter = Counter + 1;
            FinalIndex = k;
        }
    }
    
    // See if it worked:
    if (Counter==0) {cout << "NUCLEAR LEVEL ERROR: Building the database went wrong: Gamma " << GammaIndex << " form level " << LevelIndex << " does not have a final state!\n\n";}
    else if (Counter>1) {cout << "NUCLEAR LEVEL ERROR: Building the database went wrong: Gamma " << GammaIndex << " form level " << LevelIndex << " has multiple final states!\n\n";}
    else
    {
        return Levels.at(FinalIndex);
    }
}

void Nucleus::SelectFinalLevel()
{
    Int_t FinalIndex = -1;
    Int_t Counter = 0;
    
    for (Int_t k = 0; k<Levels.size(); ++k)
    {
        if (TMath::Abs(Levels.at(LevelIndex).Gamma.at(GammaIndex).E_final - Levels.at(k).Energy)<1e-6)
        {
            Counter = Counter + 1;
            FinalIndex = k;
        }
    }
    
    // See if it worked:
    if (Counter==0) {cout << "NUCLEAR LEVEL ERROR: Building the database went wrong: Gamma " << GammaIndex << " form level " << LevelIndex << " does not have a final state!\n\n";}
    else if (Counter>1) {cout << "NUCLEAR LEVEL ERROR: Building the database went wrong: Gamma " << GammaIndex << " form level " << LevelIndex << " has multiple final states!\n\n";}
    else
    {
        LevelIndex = FinalIndex;
    }
}

Double_t Nucleus::GetGammaEf()
{
    if (Levels.size()>0)
    {
        if (Levels.at(LevelIndex).Gamma.size()>0)
        {
            return Levels.at(LevelIndex).Gamma.at(GammaIndex).E_final;
        }
        else
        {
            return -1000.0;
        }
    }
    else
    {
        return -1000.0;
    }
}