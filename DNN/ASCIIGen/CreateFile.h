void R3BASCIIFileGenerator::CreateFile()
{
    // This function creates the full ASCII-file as it can be used 
    // by the ASCII-generator in an R3B macro.
    
    // Test if we actually can create a new file:
    if (ParticleGun_Loaded==kFALSE)
    {
        ErrorMessage("You did not load the particle-gun before calling CreateFile()!");
    }
    else
    {
        // Before we can do anything, we must rebuild the random number generator
        // in case that we do not want time dependency:
        Bool_t TimeDep = Inputs->GetInputBoolian("R3BRoot_RandomSeed_IsTimeDependent");
        if (TimeDep==kFALSE) {Generator = new TRandom3(335566);}
        
        // Begin by recreating a new file. Be sure to use the correct naming convention:
        TString FileName_True = FileName;
        if (TotalNumberOfThreads>1)
        {
            TString nst = "";
            TString nT_tot_str = nst.Itoa(TotalNumberOfThreads,10);
            TString nT_cur_str = nst.Itoa(CurrentThread,10);
            FileName_True = FileName_True.ReplaceAll(".dat","_nT"+nT_tot_str+"_cT"+nT_cur_str+".dat");
        }
        ASCII_File = std::ofstream(FileName_True.Data(), std::ofstream::out);
    
        // Next, define the variables for the initial line:
        Int_t nEvents = R3BRoot_nEvents;
        if (TotalNumberOfThreads>1) {nEvents = nEvents/TotalNumberOfThreads;}
            
        Int_t ThisMult = ParticleGun_Multiplicity;
        Double_t pBeam = 0.0;
        Double_t bpar = 0.0;
        Double_t Rnd_Mult = 0.0;
        
        // Then, define what we need for the second line(s):
        Int_t Ion_ID = 1; // should be -1 for nuclei (A>1), else 1.
        Int_t Ion_Z = 0;  // Should be zero, unless A>1
        Int_t Ion_A = 0;  // Should be A for nuclei and PDCCode else.
        
        // Next, begin looping over the event:
        for (Int_t Event = 0; Event<nEvents; ++Event)
        {
            // Then, decide upon multiplicity:
            if (ParticleGun_Randomize_Multiplicity==kTRUE)
            {            
                Rnd_Mult = Generator->Uniform(0.0,1.0);
                
                for (Int_t k = 0; k<ParticleGun_Multiplicity; ++k)
                {
                    if ((Rnd_Mult<=Mult_Random_Boundaries[k+1])&&(Rnd_Mult>=Mult_Random_Boundaries[k]))
                    {
                        ThisMult = k+1;
                    }
                }
            }
            else
            {
                ThisMult = ParticleGun_Multiplicity;
            }
            
            // Decide whether we should write newly generated events, our read out events:
            if (ParticleCun_GunType.Contains("SnData")==kTRUE)
            {
                // Then, read out an event:
                ReadandWriteOneSnEvent(Event,ThisMult);
            }
            else if (ParticleCun_GunType=="ExGenerated")
            {
                // Then, read out an event:
                WriteOneExEvent(Event);
            }
            else
            {
                // Then, create a new event. Write the first line:
                ASCII_File << "    " << Event << "    " << ThisMult << "    " << pBeam << "    " << bpar << "\n";
            
                // Then, loop over multiplicity:
                for (Int_t k = 0; k<ThisMult; ++k)
                {
                    // Generate a new particle vector:
                    Generate4Vector();
                
                    // Next, decide if this is an ion or not:
                    if (TMath::Abs(ParticleGun_PDGCode)>1000000000)
                    {
                        // Then, it is an ion:
                        Ion_ID = -1;
                        Ion_Z = ParticleGun_ChargeNumber;
                        Ion_A = ParticleGun_MassNumber;
                    }
                    else
                    {
                        Ion_ID = 1;
                        Ion_Z = 0;
                        Ion_A = ParticleGun_PDGCode;
                    }
                
                    // Then, write it to the file. The 1e-3 is conversion to GeV:
                    ASCII_File << "    " << Ion_ID << "    " << Ion_Z << "    " << Ion_A << "    " << 1e-3*ParticleGun_OneVector->X() << "    " << 1e-3*ParticleGun_OneVector->Y() << "    " << 1e-3*ParticleGun_OneVector->Z() << "    " << ParticleGun_x_position << "    " << ParticleGun_y_position << "    " << ParticleGun_z_position << "    " << 1e-3*ParticleGun_TotalMass << "\n";
                
                    // And that is it. Close all blocks:
                }
            }
        }
        
        // Then, close the file:
        ASCII_File.close();
        
        // And finish the function:
    }
}
