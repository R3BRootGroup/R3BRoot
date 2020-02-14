// Includsion of our own header:
#include "Nuclei.h"

    // Constructor:
    Nuclei::Nuclei()
    {
        TheNuclei.clear();
    }
    
    // Destructor:
    Nuclei::~Nuclei()
    {
        TheNuclei.clear();
    }
    
    // ============================================================================================================================================
    // Standard Get-functions:
    
    // NOTE: returns the A of a nucleus.
    Int_t Nuclei::GetA(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetA();}
        }
        
        return -1;
    }
    
    // NOTE: Returns the name of a nucleus, given A & Z:
    TString Nuclei::FindNucleus(Int_t const Z,Int_t const A)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if ((TheNuclei.at(k).GetA()==A)&&(TheNuclei.at(k).GetZ()==Z)) {return TheNuclei.at(k).GetName();}
        }
        
        return "";
    }
    
    // NOTE: returns the Z of a nucleus.
    Int_t Nuclei::GetZ(TString const Name) 
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetZ();}
        }
        
        return -1;
    }
    
    // NOTE: returns the PDG code of a nucleus.
    Int_t Nuclei::GetPDGCode(TString const Name) 
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetPDGCode();}
        }
        
        return -1;
    }
    
    // NOTE: returns the lifetime of a given nucleus in the unit of your choice.
    Double_t Nuclei::GetLifeTime(TString const Name, TString const Unit) 
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetLifeTime(Unit);}
        }
        
        return -1.0;
    }
    
    // NOTE: returns the 1p-separation energy in the unit of your choice.
    Double_t Nuclei::GetSp(TString const Name, TString const Unit) 
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetSp(Unit);}
        }
        
        return -1.0;
    }
    
    // NOTE: returns the 1n-separation energy in the unit of your choice.
    Double_t Nuclei::GetSn(TString const Name, TString const Unit) 
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetSn(Unit);}
        }
        
        return -1.0;
    }
    
    // NOTE: returns the Abundance of a given nucleus in the mode of your choice.
    Double_t Nuclei::GetAbundance(TString const Name, TString const mode) 
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetAbundance(mode);}
        }
        
        return -1.0;
    }
    
    // NOTE: returns the binding energy of a given nucleus in the mode & unit of your choice.
    Double_t Nuclei::GetBindingEnergy(TString const Name, TString const Unit, TString const mode)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetBindingEnergy(mode,Unit);}
        }
        
        return -1.0;
    }
    
    // NOTE: returns the mass of a given nucleus in the unit of your choice. Mass is WITHOUT electrons!
    Double_t Nuclei::GetMass(TString const Name, TString const Unit)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetMass("clean",Unit);}
        }
        
        return -1.0;
    }
    
    // NOTE: returns the mass of a given nucleus in the unit of your choice. Mass is WITH electrons!
    Double_t Nuclei::GetAtomicMass(TString const Name, TString const Unit)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetMass("e-",Unit);}
        }
        
        return -1.0;
    }
    
    // NOTE: Returns the name of the recoil nucleus after a charge-exchange reaction with
    // isospin change DT. DT>0 means Z goes up.
    TString Nuclei::GetRecoil_DT(TString const input_Name, Int_t const DT)
    {
        // Begin by obtaining Z & A:
        Int_t Target_Z = GetZ(input_Name);
        Int_t Target_A = GetA(input_Name);
        
        // Then, loop again over the database and obtain the right nucleus:
        Int_t Size = TheNuclei.size();
        Int_t Counter = 0;
        TString output_Name = "";
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if ((TheNuclei.at(k).GetA()==Target_A)&&(TheNuclei.at(k).GetZ()==(Target_Z+DT)))
            {
                Counter = Counter + 1;
                output_Name = TheNuclei.at(k).GetName();
            }
        }
        
        if (Counter==0) {output_Name = "This recoil nucleus is NOT in the database!";}
        if (Counter>1) {output_Name = "There are multiple recoils in the database! Re-check your code!!!";}
        
        return output_Name;
    }        
    
    Double_t Nuclei::GetIAS(TString const Name_Initial,TString const Name_Final, TString const Unit)
    {
        // computes IAS excitation energy according to Nuclear Physics A 916 (2013) 219–240.
        
        // Declare tha answer:
        Double_t Answer = 0.0;
        
        // Get constants of physics:
        PhysicsConstants PhysConst = DefineConstants();
        
        // So begin with finding the indices of both nuclei involved:
        Int_t Size = TheNuclei.size();
        Int_t Initial_Index = -1;
        Int_t Final_Index = -1;
        Int_t Initial_Counter = 0;
        Int_t Final_Counter = 0;
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name_Initial) {Initial_Counter = Initial_Counter + 1; Initial_Index = k;}
            if (TheNuclei.at(k).GetName()==Name_Final) {Final_Counter = Final_Counter + 1; Final_Index = k;}
        }
        
        if ((Initial_Counter==1)&&(Final_Counter==1)&&(Initial_Index!=Final_Index))
        {
            // then, we can proceed:
            Double_t A_Initial = (Int_t) TheNuclei.at(Initial_Index).GetA();
            Double_t Z_Initial = (Int_t) TheNuclei.at(Initial_Index).GetZ();
            Double_t Mass_Initial = TheNuclei.at(Initial_Index).GetMass("clean","MeV");
            
            Double_t A_Final = (Int_t) TheNuclei.at(Final_Index).GetA();
            Double_t Z_Final = (Int_t) TheNuclei.at(Final_Index).GetZ();
            Double_t Mass_Final = TheNuclei.at(Final_Index).GetMass("clean","MeV");
            
            // Check that we indeed have a valid case:
            if ((A_Initial==A_Final)&&(Z_Initial==(Z_Final-1)))
            {
                // The coulomb energy difference is:
                Double_t DEcoul = 1.44*(Z_Initial + 0.5)*TMath::Power(A_Initial,-1.0/3.0) - 1.13;
                
                // cout << "DEcoul = " << DEcoul << "\n";
                
                // Then excitation-energy is then:
                Double_t ExIAS = DEcoul + Mass_Initial - Mass_Final + PhysConst.Mp_MeV - PhysConst.Mn_MeV;
                
                // So propagate this to the answer:
                Answer = ExIAS;
            }
            else
            {
                cout << "### ERROR: you specified two nuclei that cannot be related by Initial(3He,t)Final!\n"
                     << "### ERROR: Your nuclei: " << Name_Initial << ", A = " << A_Initial << ", Z = " << Z_Initial << " | " << Name_Final << ", A = " << A_Final << ", Z = " << Z_Final << "\n\n";
            }
        }
        else
        {
            cout << "### ERROR: You did not specify two nuclei that were added to the database!\n"
                 << "### ERROR: Initial_Counter = " << Initial_Counter << " | Final_Counter = " << Final_Counter << " | Initial_Index = " << Initial_Index << " |  Final_Index = " << Final_Index << "\n\n";
        }
        
        // Then, implement the units:
        if (Unit=="MeV") {Answer = Answer*1.0;}
        else if (Unit=="keV") {Answer = Answer*1000.0;}
        else if (Unit=="eV") {Answer = Answer*1e6;}
        else if (Unit=="J") {Answer = Answer*PhysConst.q_eVJ*1e6;}
        else {cout << "### ERROR: The unit " << Unit << " was not supported in the code!\n\n";}
        
        // Then, return the answer in the end:
        return Answer;
    }            
    
    // ============================================================================================================================================
    // Get-functions for obtaining information from the energy levels:
    
    Int_t Nuclei::GetNElevels(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetNElevels();}
        }
        
        return -1;
    }
    
    EnergyLevel Nuclei::GetEnergyLevel(TString const Name, Int_t const Index)
    {
        Int_t Size = TheNuclei.size();
        EnergyLevel Answer;
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {Answer = TheNuclei.at(k).GetEnergyLevel(Index);}
        }
        
        return Answer;
    }
    
    Int_t Nuclei::GetNE(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetNE();}
        }
        
        return -1;
    }
    
    Int_t Nuclei::GetNAngMoms(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetNAngMoms();}
        }
        
        return -1;
    }
    
    Int_t Nuclei::GetNGammas(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetNGammas();}
        }
        
        return -1;
    }
    
    void Nuclei::SelectElevel(TString const Name, Int_t const Index)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {TheNuclei.at(k).SelectElevel(Index);}
        }
    }
    
    void Nuclei::SelectGroundState(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {TheNuclei.at(k).SelectGroundState();}
        }
    }
    
    void Nuclei::SelectElevel_WithKey(TString const Name, Int_t const Key)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {TheNuclei.at(k).SelectElevel_WithKey(Key);}
        }
    }
    
    void Nuclei::SelectFinalLevel(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {TheNuclei.at(k).SelectFinalLevel();}
        }
    }
    
    void Nuclei::SelectAngMom(TString const Name, Int_t const Index)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {TheNuclei.at(k).SelectAngMom(Index);}
        }
    }
    
    void Nuclei::SelectGamma(TString const Name, Int_t const Index)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {TheNuclei.at(k).SelectGamma(Index);}
        }
    }
    
    Int_t Nuclei::GetKey(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetKey();}
        }
        
        return -1;
    }
    
    Double_t Nuclei::GetLevelE(TString const Name, TString const Unit)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetLevelE(Unit);}
        }
        
        return -1.0;
    }
    
    Double_t Nuclei::GetLevelLifeTime(TString const Name, TString const Unit)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetLevelLifeTime(Unit);}
        }
        
        return -1.0;
    }
    
    Double_t Nuclei::GetBGT(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetBGT();}
        }
        
        return -1.0;
    }
    
    Double_t Nuclei::GetBF(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetBF();}
        }
        
        return -1.0;
    }
    
    Double_t Nuclei::GetJ(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetJ();}
        }
        
        return -1.0;
    }
    
    Double_t Nuclei::GetGammaE(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetGammaE();}
        }
        
        return -1.0;
    }
    
    Double_t Nuclei::GetGammaI(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetGammaI();}
        }
        
        return -1.0;
    }
    
    Double_t Nuclei::GetGammaEf(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetGammaEf();}
        }
        
        return -1.0;
    }
    
    TString Nuclei::GetRefs(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        TString Answer = "";
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {Answer = TheNuclei.at(k).GetRefs();}
        }
        
        return Answer;
    }
    
    TString Nuclei::GetParity(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        TString Answer = "?";
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {Answer = TheNuclei.at(k).GetParity();}
        }
        
        return Answer;
    }
    
    TString Nuclei::GetGammaTransitionType(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        TString Answer = "??";
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {Answer = TheNuclei.at(k).GetGammaTransitionType();}
        }
        
        return Answer;
    }
    
    Bool_t Nuclei::GetJacc(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {return TheNuclei.at(k).GetJacc();}
        }
        
        return kFALSE;
    }

    EnergyLevel Nuclei::GetFinalLevel(TString const Name)
    {
        Int_t Size = TheNuclei.size();
        EnergyLevel Answer;
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetName()==Name) {Answer = TheNuclei.at(k).GetFinalLevel();}
        }
        
        return Answer;
    }
    
    // ============================================================================================================================================
    
    // Printing function:
    
    void Nuclei::Print(Bool_t const PrintGammas, TString const NucName, Int_t const JBoundary)
    {
        Int_t Size = TheNuclei.size();
        cout << "\n\n"
             << "=============== PRINT NUCLEAR DATABASE =============="
             << "\n\n";
        
        if (NucName=="ALL")
        {
            // Then we print everything:
            for (Int_t k = 0; k<Size; ++k)
            {
                TheNuclei.at(k).Print(PrintGammas,JBoundary);
            }
        }
        else
        {
            // Then we print only what we asked for:
            for (Int_t k = 0; k<Size; ++k)
            {
                if (TheNuclei.at(k).GetName()==NucName) {TheNuclei.at(k).Print(PrintGammas,JBoundary);}
            }
        }
    }
    
    // ============================================================================================================================================
    
// Inclusion of other member functions:
#include "GetNaturalMass.h"
#include "BuildDataBase.h"
#include "FindNucleusFromPDG.h"

// Generate ROOT dictionary:
ClassImp(Nuclei);
