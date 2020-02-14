// Include own header:
#include "Nucleus.h"

// Default constructor:
Nucleus::Nucleus() 
    {
        Name = "-";
        A = -1;
        Z = -1;
        Mass = -1.0;
        E_Bind = -1.0;
        LifeTime = -1.0;
        Abundance = -1.0;
        Sn = -1.0;
        Sp = -1.0;
        PhysConst = DefineConstants();
        
        Levels.clear();
        LevelIndex = 0;
        AngIndex = 0;
        GammaIndex = 0;
    }
    
    // Destructor:
Nucleus::~Nucleus()
    {
        Levels.clear();
    }
    
    // ===============================================================================================================
    
    // Get-functions:
    
    TString Nucleus::GetName() 
    {
        return Name;
    }
    
    Int_t Nucleus::GetA() 
    {
        return A;
    }
    
    Int_t Nucleus::GetZ() 
    {
        return Z;
    }
    
    Double_t Nucleus::GetLifeTime(TString const Unit) 
    {
        if (Unit=="sec") {return LifeTime;}    // NOTE: Returned in seconds!
        else if (Unit=="s") {return LifeTime;} // NOTE: Returned in seconds!
        else if (Unit=="ms") {return LifeTime*1000.0;}
        else if (Unit=="us") {return LifeTime*1e6;}
        else if (Unit=="ns") {return LifeTime*1e9;}
        else if (Unit=="ps") {return LifeTime*1e12;}
        else if (Unit=="min") {return LifeTime/60.0;}
        else if (Unit=="hour") {return LifeTime/(60.0*60.0);}
        else if (Unit=="day") {return LifeTime/(60.0*60.0*24.0);}
        else if (Unit=="week") {return LifeTime/(60.0*60.0*24.0*7.0);}
        else if (Unit=="month") {return LifeTime/(60.0*60.0*24.0*365.0/12.0);}
        else if (Unit=="year") {return LifeTime/(60.0*60.0*24.0*365.0);}
        else
        {
            cout << "NUCLEAR DATA FAILURE: The unit <"<<Unit<<"> was not supported in Nucleus.GetLifeTime()\n\n";
            return -1.0;
        }
    }
     
    Double_t Nucleus::GetAbundance(TString const mode) 
    {
        if (mode=="Dimless") {return Abundance;}      // NOTE: Returns the abundance as a dimensionless number between 0 & 1
        else if (mode=="%") {return Abundance*100.0;} // NOTE: returns the abundance as a percentage.
        else
        {
            cout << "NUCLEAR DATA FAILURE: The mode <"<<mode<<"> was not supported in Nucleus.GetAbundance()\n\n";
            return -1.0;
        }
    }
    
    Double_t Nucleus::GetBindingEnergy(TString const mode, TString const Unit)
    {
        if ((mode=="Total")&&(Unit=="MeV")) {return E_Bind*((Int_t) A);}                          // NOTE: Return the total amount of binding energy in MeV.
        else if ((mode=="Total")&&(Unit=="J")) {return (E_Bind*((Int_t) A))*PhysConst.q_eVJ*1e6;} // NOTE: Return the total amount of binding energy in J.
        else if ((mode=="PerN")&&(Unit=="MeV")) {return E_Bind;}                                  // NOTE: Return the binding energy per nucleon in MeV.
        else if ((mode=="PerN")&&(Unit=="J")) {return E_Bind*PhysConst.q_eVJ*1e6;}                // NOTE: Return the binding energy per nucleon in J.
        else
        {
            cout << "NUCLEAR DATA FAILURE: The combination of mode="<<mode<<" & unit="<<Unit<<" was not supported in Nucleus.GetBindingEnergy()\n\n";
            return -1.0;
        }
    }
    
    Double_t Nucleus::GetSp(TString const Unit)
    {
        if (Unit=="MeV") {return Sp;}
        else if (Unit=="J") {return Sp*PhysConst.q_eVJ*1e6;}
        else
        {
            cout << "NUCLEAR DATA FAILURE: The unit="<<Unit<<" was not supported in Nucleus.GetSp()\n\n";
            return -1.0;
        }
    }
    
    Double_t Nucleus::GetSn(TString const Unit)
    {
        if (Unit=="MeV") {return Sn;}
        else if (Unit=="J") {return Sn*PhysConst.q_eVJ*1e6;}
        else
        {
            cout << "NUCLEAR DATA FAILURE: The unit="<<Unit<<" was not supported in Nucleus.GetSn()\n\n";
            return -1.0;
        }
    }
    
    Double_t Nucleus::GetMass(TString const mode, TString const Unit)
    {
        if ((mode=="clean")&&(Unit=="MeV")) {return Mass;}                                                                                                           // NOTE: Returns the total nuclear mass in MeV.
        else if ((mode=="clean")&&(Unit=="u")) {return Mass*PhysConst.q_MeVJ/(PhysConst.Mu_kg*PhysConst.cc_ms*PhysConst.cc_ms);}                                     // NOTE: Returns the total nuclear mass in u.
        else if ((mode=="clean")&&(Unit=="kg")) {return Mass*PhysConst.q_MeVJ/(PhysConst.cc_ms*PhysConst.cc_ms);}                                                    // NOTE: Returns the total nuclear mass in kg.
        else if ((mode=="e-")&&(Unit=="MeV")) {return Mass+(((Int_t) Z)*PhysConst.Me_MeV);}                                                                          // NOTE: Returns the total atomic mass in MeV.
        else if ((mode=="e-")&&(Unit=="u")) {return (Mass+(((Int_t) Z)*PhysConst.Me_MeV))*PhysConst.q_MeVJ/(PhysConst.Mu_kg*PhysConst.cc_ms*PhysConst.cc_ms);}       // NOTE: Returns the total atomic mass in u.
        else if ((mode=="e-")&&(Unit=="kg")) {return (Mass+(((Int_t) Z)*PhysConst.Me_MeV))*PhysConst.q_MeVJ/(PhysConst.cc_ms*PhysConst.cc_ms);}                      // NOTE: Returns the total atomic mass in kg.
        else
        {
            cout << "NUCLEAR DATA FAILURE: The combination of mode="<<mode<<" & unit="<<Unit<<" was not supported in Nucleus.GetMass()\n\n";
            return -1.0;
        }
    }
    
    Int_t Nucleus::GetPDGCode()
    {
        return PDGCode;
    }
    
    // ===============================================================================================================
    
    // Set-functions:
    void Nucleus::SetName(TString const str) 
    {
        Name = str;
    }
    
    void Nucleus::SetA(Int_t const n) 
    {
        A = n;
    }
    
    void Nucleus::SetZ(Int_t const n) 
    {
        Z = n;
    }
    
    void Nucleus::SetLifeTime(TString const Unit, Double_t const t) 
    {
        if (Unit=="sec") {LifeTime = t;}    // NOTE: LifeTime was set in seconds.
        else if (Unit=="s") {LifeTime = t;} // NOTE: LifeTime was set in seconds.
        else if (Unit=="ms") {LifeTime = t/1000.0;}
        else if (Unit=="us") {LifeTime = t/1e6;}
        else if (Unit=="ns") {LifeTime = t/1e9;}
        else if (Unit=="ps") {LifeTime = t/1e12;}
        else if (Unit=="min") {LifeTime = t*60.0;}
        else if (Unit=="hour") {LifeTime = t*60.0*60.0;}
        else if (Unit=="day") {LifeTime = t*60.0*60.0*24.0;}
        else if (Unit=="week") {LifeTime = t*60.0*60.0*24.0*7.0;}
        else if (Unit=="month") {LifeTime = t*60.0*60.0*24.0*365.0/12.0;}
        else if (Unit=="year") {LifeTime = t*60.0*60.0*24.0*365.0;}
        else
        {
            cout << "NUCLEAR DATA FAILURE: The unit <"<<Unit<<"> was not supported in Nucleus.SetLifeTime()\n\n";
            LifeTime = -1.0;
        }
    }
    
    void Nucleus::SetAbundance(TString const mode, Double_t const a)
    {
        if (mode=="Dimless") {Abundance = a;}      // NOTE: Sets the abundance as a dimensionless number between 0 & 1.
        else if (mode=="%") {Abundance = a/100.0;} // NOTE: Sets the abundance specified as a percentage.
        else
        {
            cout << "NUCLEAR DATA FAILURE: The mode <"<<mode<<"> was not supported in Nucleus.SetAbundance()\n\n";
            Abundance = -1.0;
        }
    }
    
    void Nucleus::SetMass(TString const mode, TString const Unit, Double_t const m)
    {
        if (A<0)
        {
            cout << "NUCLEAR DATA FAILURE: You cannot call Nucleus.SetMass() without defining both A & Z first!\n\n";
        }
        else
        {
        
        // Go through all options:
        if ((Unit=="MeV")&&(mode=="clean"))
        {
            // this is easy, we want to specify the mass in MeV:
            Mass = m;
        }
        else if ((Unit=="MeV")&&(mode=="e-"))
        {
            // Then we have to subtract the electron mass first (in MeV):
            Mass = m - ((Int_t) TMath::Abs(Z))*PhysConst.Me_MeV;
        }
        else if ((Unit=="u")&&(mode=="clean"))
        {
            // Then we have to convert the mass to MeV:
            Mass = m*PhysConst.Mu_kg*PhysConst.cc_ms*PhysConst.cc_ms/PhysConst.q_MeVJ;
        }
        else if ((Unit=="u")&&(mode=="e-"))
        {
            // Then we have to convert the mass to MeV & subtract the electron mass:
            Mass = m*PhysConst.Mu_kg*PhysConst.cc_ms*PhysConst.cc_ms/PhysConst.q_MeVJ - ((Int_t) TMath::Abs(Z))*PhysConst.Me_MeV;
        }
        else if ((Unit=="kg")&&(mode=="clean"))
        {
            // Then we have to convert the mass to MeV:
            Mass = m*PhysConst.cc_ms*PhysConst.cc_ms/PhysConst.q_MeVJ;
        } 
        else if ((Unit=="kg")&&(mode=="e-"))
        {
            // Then we have to convert the mass to MeV & subtract the electron mass:
            Mass = m*PhysConst.cc_ms*PhysConst.cc_ms/PhysConst.q_MeVJ - ((Int_t) TMath::Abs(Z))*PhysConst.Me_MeV;
        }
        else
        {
            // Give an error message:
            cout << "NUCLEAR DATA FAILURE: The combination of mode="<<mode<<" & unit="<<Unit<<" was not supported in Nucleus.SetMass()\n\n";
            Mass = -1.0;
        }
        
        // And then we have to compute the binding energy per nucleon as well:
        if (Mass>=0.0)
        {
            if (A>0)
            {
                E_Bind = ((Int_t) TMath::Abs(Z))*PhysConst.Mp_MeV + ((Int_t) (A-TMath::Abs(Z)))*PhysConst.Mn_MeV - Mass;
                E_Bind = E_Bind/((Int_t) A);
            }
            else
            {
                E_Bind = 0.0;
            }
        }
        else
        {
            E_Bind = -1.0;
        }
        }
    }
    
    void Nucleus::SetBindingEnergy(TString const mode, TString const Unit, Double_t const E)
    {
        if (A<0)
        {
            cout << "NUCLEAR DATA FAILURE: You cannot call Nucleus.SetBindingEnergy() without defining both A & Z first!\n\n";
        }
        else
        {
        
        // Go through all options:
        if ((mode=="PerN")&&(Unit=="MeV"))
        {
            // This is easy:
            E_Bind = E;
        }
        else if ((mode=="Total")&&(Unit=="MeV"))
        {
            // Then divide by A:
            E_Bind = E/((Int_t) A);
        }
        else if ((mode=="PerN")&&(Unit=="keV"))
        {
            // This is easy:
            E_Bind = E/1000.0;
        }
        else if ((mode=="Total")&&(Unit=="MeV"))
        {
            // Then divide by A:
            E_Bind = (E/1000.0)/((Int_t) A);
        }
        else if ((mode=="PerN")&&(Unit=="J"))
        {
            // Then we must convert to MeV first:
            E_Bind = E/PhysConst.q_MeVJ;
        }
        else if ((mode=="Total")&&(Unit=="J"))
        {
            // Then we must convert to MeV & divide by A:
            E_Bind = (E/PhysConst.q_MeVJ)/((Int_t) A);
        }
        else
        {
            cout << "NUCLEAR DATA FAILURE: The combination of mode="<<mode<<" & unit="<<Unit<<" was not supported in Nucleus.SetBindingEnergy()\n\n";
            E_Bind = -1.0;
        }
        
        // Now we must compute the nuclear mass:
        if (TMath::Abs(E_Bind+1.0)>1e-6)
        {
            Mass = ((Int_t) TMath::Abs(Z))*PhysConst.Mp_MeV + ((Int_t) (A-TMath::Abs(Z)))*PhysConst.Mn_MeV - (E_Bind*((Int_t) A));
        }
        }
    }
    
    void Nucleus::SetSp(TString const Unit, Double_t const E)
    {
        if (Unit=="MeV") {Sp = E;}
        else if (Unit=="J") {Sp = E/(PhysConst.q_eVJ*1e6);}
        else
        {
            cout << "NUCLEAR DATA FAILURE: The unit="<<Unit<<" was not supported in Nucleus.SetSp()\n\n";
            Sp = -1.0;
        }
    }
    
    void Nucleus::SetSn(TString const Unit, Double_t const E)
    {
        if (Unit=="MeV") {Sn = E;}
        else if (Unit=="J") {Sn = E/(PhysConst.q_eVJ*1e6);}
        else
        {
            cout << "NUCLEAR DATA FAILURE: The unit="<<Unit<<" was not supported in Nucleus.SetSn()\n\n";
            Sp = -1.0;
        }
    }
    
    void Nucleus::ComputePDGCode()
    {
        if (A>1)
        {
            // reconstrcut the code:
            PDGCode = 1000000000 + 10*A + 10000*Z;
        }
    }
     
    void Nucleus::SetPDGCode(Int_t const PDG)
    {
        PDGCode = PDG;
    }
   
    // =======================================================================================================
    
    // Print-function:
    void Nucleus::Print(Bool_t const PrintGammas, Int_t const JBoundary)
    {
        // Print basic information:
        cout << "Nucleus Name                     = " << Name << "\n"
             << "Nucleus Z                        = " << Z << "\n"
             << "Nucleus A                        = " << A << "\n"
             << "Nuclear Mass [MeV]               = " << Mass << "\n"
             << "Binding energy per nucleon [MeV] = " << E_Bind << "\n"
             << "LifeTime [sec]                   = " << LifeTime << "\n"
             << "Natural Abundance [Dim. less]    = " << Abundance << "\n"
             << "\n";
             
        // Print energy levels:
        Int_t Size = (Int_t) Levels.size();
        Int_t AngSize = 0;
        Int_t GamSize = 0;
        Bool_t PrintThisLevel = kFALSE;
        
        if (Size>0) 
        {
            for (Int_t k = 0; k<Size; ++k)
            {
                AngSize = (Int_t) Levels.at(k).AngMom.size();
                GamSize = (Int_t) Levels.at(k).Gamma.size();
            
                PrintThisLevel = kFALSE;
                if (AngSize>0)
                {
                    for (Int_t n = 0; n<AngSize; ++n)
                    {
                        if (Levels.at(k).AngMom.at(n).Jdef==kTRUE)
                        {
                            if (Levels.at(k).AngMom.at(n).J<=JBoundary)
                            {
                                PrintThisLevel = kTRUE;
                            }
                        }
                    }
                }
                if (JBoundary<0) {PrintThisLevel = kTRUE;}
                
                if (PrintThisLevel==kTRUE)
                {                
                
                cout << Levels.at(k).Key << ": ";
                if (k<10) {cout << " ";}
                cout << "E = " << Levels.at(k).Energy << " MeV | Ref = " << Levels.at(k).References.Data() << " | J/pi = ";
            
                if (AngSize>0) 
                {
                    for (Int_t n = 0; n<AngSize; ++n)
                    {
                        if (Levels.at(k).AngMom.at(n).Accuracy==kFALSE) {cout << "(";}
                        if (Levels.at(k).AngMom.at(n).Jdef==kTRUE) {cout << Levels.at(k).AngMom.at(n).J;}
                        else {cout << "?";}
                        if (Levels.at(k).AngMom.at(n).Accuracy==kFALSE) {cout << ")";}
                        cout << Levels.at(k).AngMom.at(n).Parity.Data();
                        if (n!=(AngSize-1)) {cout << ",";}
                    }
                }
                else
                {
                    cout << "??";
                }
            
                if (Levels.at(k).LifeTime>0.0) {cout << " | T1/2 = " << Levels.at(k).LifeTime << " sec\n";}
                else {cout << " | T1/2 = ?? sec\n";}
            
                // Now we must see if we print the Gamma transitions along with it:
                if ((PrintGammas==kTRUE)&&(GamSize>0))
                {
                    for (Int_t n = 0; n<GamSize; ++n)
                    {
                        cout << "    ==> " << "E(gamma) = " << Levels.at(k).Gamma.at(n).E_gamma << " MeV | I(gamma) = " << Levels.at(k).Gamma.at(n).I_gamma*100.0 << "% | E(final) = " << Levels.at(k).Gamma.at(n).E_final << " MeV | Transistion Type = " << Levels.at(k).Gamma.at(n).GamMult.Data() << "\n";
                    }
                }
                
                }            
                // Done.
            }
        }
        
        // insert a new line:
        cout << "\n\n";
        
        // Done.
    }
   
// Include other member functions:
#include "AddEnergyLevel.h"
#include "SetLevelLifeTime.h"
#include "SetLevelBGT.h"
#include "SetLevelBF.h"
#include "SortEnergyLevels.h"
#include "SetLevelReference.h"
#include "AngMom.h"
#include "GetEnergyLevel.h"
#include "GammaTrans.h"
#include "ElegantGetFunctions.h"

// Generate ROOT dictionary:
ClassImp(Nucleus);
