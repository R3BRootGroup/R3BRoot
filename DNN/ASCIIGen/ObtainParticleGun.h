void R3BASCIIFileGenerator::ObtainParticleGun()
{
    // Loads the proper inputs into the class.
    ParticleGun_Loaded = kFALSE;
    SnFiles_Initialized = kFALSE;
    
    // We already know that the inputs class is properly linked. Hence:
    if (Inputs==0)
    {
        ErrorMessage("You should provide an inputs class to this class before calling ObtainParticleGun()!");
    }
    else
    {
        // Retrieve all inputs:
        Inputs->ReadFile();
        
        // Then, define the output filename:
        FileName = Inputs->GetInputString("TheOutputPath") + Inputs->GetInputString("ParticleGun_ASCII_FileName");
        
        // Next, obtain the proper inputs:
        TheVMCFolder = Inputs->GetInputString("TheVMCFolder");
        R3BRoot_nEvents = Inputs->GetInputInteger("R3BRoot_nEvents");
        ParticleCun_GunType = Inputs->GetInputString("ParticleGun_ASCII_GunChoice");
        ParticleGun_ParticleType = Inputs->GetInputString("ParticleGun_ParticleType");
        ParticleGun_Multiplicity = Inputs->GetInputInteger("ParticleGun_Multiplicity");
        ParticleGun_Randomize_Multiplicity = Inputs->GetInputBoolian("ParticleGun_Randomize_Multiplicity");
        ParticleGun_Uniformize_Detected_Multiplicity = Inputs->GetInputBoolian("ParticleGun_Uniformize_Detected_Multiplicity");
        ParticleGun_SingleDetectionRate = Inputs->GetInputDouble("ParticleGun_SingleDetectionRate","dimless");
        ParticleGun_InfiniteDetectionRate = Inputs->GetInputDouble("ParticleGun_InfiniteDetectionRate","dimless");
        ParticleGun_x_position = Inputs->GetInputDouble("ParticleGun_x_position","cm");
        ParticleGun_y_position = Inputs->GetInputDouble("ParticleGun_y_position","cm");
        ParticleGun_z_position = Inputs->GetInputDouble("ParticleGun_z_position","cm");
        ParticleGun_x_direction = Inputs->GetInputDouble("ParticleGun_x_direction","cm");
        ParticleGun_y_direction = Inputs->GetInputDouble("ParticleGun_y_direction","cm");
        ParticleGun_z_direction = Inputs->GetInputDouble("ParticleGun_z_direction","cm");
        ParticleGun_AngularSpread = Inputs->GetInputDouble("ParticleGun_AngularSpread","degree");
        ParticleGun_MinimalKineticEnergy = Inputs->GetInputDouble("ParticleGun_MinimalKineticEnergy","MeV/u");
        ParticleGun_MaximalKineticEnergy = Inputs->GetInputDouble("ParticleGun_MaximalKineticEnergy","MeV/u");
        ParticleGun_Multiplicity_Distribution = Inputs->GetInputString("ParticleGun_MultDistribution");
        ParticleGun_ExGenerator_DegradeEnergy_MultipleTracks = Inputs->GetInputBoolian("ParticleGun_ExGenerator_DegradeEnergy_MultipleTracks");
        ParticleGun_ExGenerator_BeamIsotope = Inputs->GetInputString("ParticleGun_ExGenerator_BeamIsotope");
        ParticleGun_ExGenerator_TargetIsotope = Inputs->GetInputString("ParticleGun_ExGenerator_TargetIsotope");
        ParticleGun_ExGenerator_BeamEnergy = Inputs->GetInputDouble("ParticleGun_ExGenerator_BeamEnergy","MeV/u");
        ParticleGun_ExGenerator_Recoil_ExcitationEnergy = Inputs->GetInputDouble("ParticleGun_ExGenerator_Recoil_ExcitationEnergy","MeV");
        RandomizeOption = Inputs->GetInputString("ParticleGun_AngularSpread_Pattern");
        Include_SnNucleus = Inputs->GetInputBoolian("ParticleGun_IncludeSnNucleus_SnDataFiles");
        RandomRotateNeutrons = Inputs->GetInputBoolian("ParticleGun_RandomRotateNeutrons_SnDataFiles");
        
        // Next, obtain the other variables from the Nuclear DataBase:
        ParticleGun_PDGCode = TheNuclei->GetPDGCode(ParticleGun_ParticleType);
        ParticleGun_TotalMass = TheNuclei->GetMass(ParticleGun_ParticleType,"MeV");
        ParticleGun_Direction->SetX(ParticleGun_x_direction);
        ParticleGun_Direction->SetY(ParticleGun_y_direction);
        ParticleGun_Direction->SetZ(ParticleGun_z_direction);
        
        // These ones are for conversion between MeV/u and MeV-total. Hence, it should
        // be positive and >=1. They are A and Z of the nucleus.
        ParticleGun_ChargeNumber = TheNuclei->GetZ(ParticleGun_ParticleType);
        ParticleGun_ChargeNumber = TMath::Abs(ParticleGun_ChargeNumber);
        if (ParticleGun_ChargeNumber<1) {ParticleGun_ChargeNumber = 1;}
        
        ParticleGun_MassNumber = TheNuclei->GetA(ParticleGun_ParticleType);
        ParticleGun_MassNumber = TMath::Abs(ParticleGun_MassNumber);
        if (ParticleGun_MassNumber<1) {ParticleGun_MassNumber = 1;}
        
        // Deal with randomization of multiplicities:
        std::vector<Double_t> TheBoundaries = DefineMultBoundaries();
        Mult_Random_Boundaries = new Double_t[ParticleGun_Multiplicity+1];
        Mult_Random_Boundaries[0] = 0.0;
        
        for (Int_t k = 1; k<(ParticleGun_Multiplicity+1); ++k)
        {
            Mult_Random_Boundaries[k] = TheBoundaries.at(k);
        }
        
        if (ParticleGun_Randomize_Multiplicity==kTRUE)
        {
            cout << "\n\nParticleGun Multiplicity shall be randomized with the following distribution:\n";
            TString st = "";
            TString kstr = "";
            
            for (Int_t k = 0; k<ParticleGun_Multiplicity; ++k)
            {
                kstr = st.Itoa(k+1,10);
                cout << kstr + "n: rnd in [ " << Mult_Random_Boundaries[k] << " ; " << Mult_Random_Boundaries[k+1] << " ] ==> Prob. = " << Mult_Random_Boundaries[k+1] - Mult_Random_Boundaries[k] << "\n";
            }
            
            cout << "\n\n";
        }
        
        // Initialize the SnDataFiles:
        if (ParticleCun_GunType.Contains("SnData")==kTRUE)
        {
            InitializeSnFiles();
        }
        
        // Check that this nucleus indeed exists in the database:
        if (ParticleGun_TotalMass<0.0)
        {
            ErrorMessage("The particle <"+ParticleGun_ParticleType+"> was not added to the Database in R3BRoot/DNN/Masses/NuclearData/");
            ErrorMessage("The particle <"+ParticleGun_ParticleType+"> was not added to the BuildDatabase()-function in R3BRoot/DNN/Masses/");
            ParticleGun_PDGCode = 22;
            ParticleGun_TotalMass = 0.0;
        }
        else
        {
            // check that the FileName is indeed a .dat and that this file can be created:
            if (FileName.Contains(".dat")==kFALSE)
            {
                ErrorMessage("The ASCII-filename does not contain <.dat>, which is the obligated file-extension!");
            }
            else
            {
                // Test that this file indeed can be created:
                ifstream TestFile_1;
                TestFile_1.open(FileName.Data());
                
                if (TestFile_1.is_open()==kTRUE) 
                {
                    // Then, this file exists. So it is a valid path-name.
                    TestFile_1.close();
                    ParticleGun_Loaded = kTRUE;
                }
                else
                {
                    // Then, try to create the file, to see if it is a valid path name:
                    std::ofstream TestFile_2 (FileName.Data(), std::ofstream::out);
                    TestFile_2.close();
                    
                    ifstream TestFile_3;
                    TestFile_3.open(FileName.Data());
                    if (TestFile_3.is_open()==kTRUE)
                    {
                        // Then, we are still OK:
                        TestFile_3.close();
                        ParticleGun_Loaded = kTRUE;
                    }
                    else
                    {
                        // Then, FileName is NOT a valid path name:
                        ErrorMessage("The Inputs <TheOutputPath+ParticleGun_ASCII_FileName> containing the TString <"+FileName+"> does NOT contain a valid path+filename!");
                    }
                }
            }
        }
    }
}
