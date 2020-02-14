void R3BASCIIFileGenerator::ReadandWriteOneSnEvent(Int_t const ThisEvent, Int_t const CurrentMultiplicity)
{
    // This function reads one event from the Sn datafiles and copies it
    // into ASCII_File: the file we write to. Begin by testing that
    // vector with Sn-files is big enough:
    TString st = "";
    TString CurMultStr = st.Itoa(CurrentMultiplicity,10);
    
    if ((CurrentMultiplicity>nReadedFiles)||(CurrentMultiplicity<=0)||(SnFiles_Initialized==kFALSE))
    {
        if (CurrentMultiplicity>nReadedFiles) {ErrorMessage("Not enough Sn datafiles are available to read an event with multiplicity "+CurMultStr);}
        if (CurrentMultiplicity<=0) {ErrorMessage("You should supply a strictly positive number as the current multiplicity for this event!");}
        if (SnFiles_Initialized==kFALSE) {ErrorMessage("You should initialize the Sn datafiles first!");}
    }
    else
    {        
        // cout << "==> Event = " << ThisEvent 
        //      << " | CurrentCounters[CurrentMultiplicity=" << CurrentMultiplicity << "] = " << CurrentCounters.at(CurrentMultiplicity-1) << " | "
        //      << " |   StartCounters[CurrentMultiplicity=" << CurrentMultiplicity << "] = " <<   StartCounters.at(CurrentMultiplicity-1) << " | "
        //      << " |   FinalCounters[CurrentMultiplicity=" << CurrentMultiplicity << "] = " <<   FinalCounters.at(CurrentMultiplicity-1) << "\n";
        
        // Then, continue reading from the current file. Declare what we need:
        Int_t Event;
        Int_t ThisMult;
        Int_t ThisStartPoint;
        Double_t pBeam;
        Double_t bpar;
        std::string ThisLine;
        TVector3 ThisVector;
        TString* TheLines = new TString[CurrentMultiplicity+2];
        
        // Declare for random rotation along the z-axis:
        Double_t RandomPhiAngle = Generator->Uniform(0.0,2.0*TMath::Pi());
        Int_t* Ion_ID = new Int_t[CurrentMultiplicity+2];
        Int_t* Ion_Z = new Int_t[CurrentMultiplicity+2];
        Int_t* PDG = new Int_t[CurrentMultiplicity+2];
        Double_t* Momentum_X = new Double_t[CurrentMultiplicity+2];
        Double_t* Momentum_Y = new Double_t[CurrentMultiplicity+2];
        Double_t* Momentum_Z = new Double_t[CurrentMultiplicity+2];
        Double_t* Position_X = new Double_t[CurrentMultiplicity+2];
        Double_t* Position_Y = new Double_t[CurrentMultiplicity+2];
        Double_t* Position_Z = new Double_t[CurrentMultiplicity+2];
        Double_t* Mass = new Double_t[CurrentMultiplicity+2];
        
        // before we start reading, see if we may have to skip a few events:
        Int_t ThisCurrentStart = 0;
        if (TotalNumberOfThreads>1) {ThisCurrentStart = CurrentThread;}
        
        // Then, see if we have to skip:
        if (CurrentCounters.at(CurrentMultiplicity-1)<(StartCounters.at(CurrentMultiplicity-1)+(2*ThisCurrentStart)))
        {
            // Skip the first part of the events:
            for (Int_t n = 0; n<(StartCounters.at(CurrentMultiplicity-1)+(2*ThisCurrentStart)-CurrentCounters.at(CurrentMultiplicity-1)); ++n)
            {   
                for (Int_t k = 0; k<(CurrentMultiplicity+2); ++k)
                {
                    std::getline(SnDataFiles[CurrentMultiplicity-1],ThisLine);
                }
            }
            
            // Adapt the CurrentCounters appropriately:
            CurrentCounters.at(CurrentMultiplicity-1) = StartCounters.at(CurrentMultiplicity-1) + ThisCurrentStart;
        }
                 
        // Next, obtain the first line of the new event:
        SnDataFiles[CurrentMultiplicity-1] >> Event;
        SnDataFiles[CurrentMultiplicity-1] >> ThisMult;
        SnDataFiles[CurrentMultiplicity-1] >> pBeam;
        SnDataFiles[CurrentMultiplicity-1] >> bpar;
        
        // Obtain all other lines of that event:
        for (Int_t k = 0; k<(CurrentMultiplicity+2); ++k)
        {
            std::getline(SnDataFiles[CurrentMultiplicity-1],ThisLine);
            TheLines[k] = ThisLine;
            
            // After the last getline, we are back at an event-line, which should not be flushed!
            if (k!=(CurrentMultiplicity+1))
            {
                SnDataFiles[CurrentMultiplicity-1] >> Ion_ID[k];
                SnDataFiles[CurrentMultiplicity-1] >> Ion_Z[k];
                SnDataFiles[CurrentMultiplicity-1] >> PDG[k];
                SnDataFiles[CurrentMultiplicity-1] >> Momentum_X[k];
                SnDataFiles[CurrentMultiplicity-1] >> Momentum_Y[k];
                SnDataFiles[CurrentMultiplicity-1] >> Momentum_Z[k];
                SnDataFiles[CurrentMultiplicity-1] >> Position_X[k];
                SnDataFiles[CurrentMultiplicity-1] >> Position_Y[k];
                SnDataFiles[CurrentMultiplicity-1] >> Position_Z[k];
                SnDataFiles[CurrentMultiplicity-1] >> Mass[k];
            }
        }
        
        // Then, the current file counter should be updated:
        if (TotalNumberOfThreads>1)
        {
            // Then, things are a bit more difficult. Updating the counter is easy:
            CurrentCounters.at(CurrentMultiplicity-1) = CurrentCounters.at(CurrentMultiplicity-1) + TotalNumberOfThreads;
            
            // However, then we should also skip the corresponding events:
            for (Int_t n = 0; n<(TotalNumberOfThreads-1); ++n)
            {
                for (Int_t k = 0; k<(CurrentMultiplicity+2); ++k)
                {
                    std::getline(SnDataFiles[CurrentMultiplicity-1],ThisLine);
                }
            }
        }
        else
        {
            // Then, things are easy:
            CurrentCounters.at(CurrentMultiplicity-1) = CurrentCounters.at(CurrentMultiplicity-1) + 1;
        }
        
        // Next, check if we should re-open this file, so we can start again:
        if (CurrentCounters.at(CurrentMultiplicity-1)>FinalCounters.at(CurrentMultiplicity-1))
        {
            SnDataFiles[CurrentMultiplicity-1].close();
            SnDataFiles[CurrentMultiplicity-1].open(SnDataFileName.at(CurrentMultiplicity-1));
            if (TotalNumberOfThreads>1) {CurrentCounters.at(CurrentMultiplicity-1) = CurrentThread;} // NOTE: This starts at zero!
            else {CurrentCounters.at(CurrentMultiplicity-1) = 0;}
        }
        
        // Next, write to the new file:
        if (ThisMult!=(CurrentMultiplicity+1))
        {
            ErrorMessage("Something went wrong in reading from the file! We got the wrong line!");
        }
        else
        {
            // Check if we should include the nucleus:
            if (Include_SnNucleus==kTRUE)
            {
                ThisStartPoint = 1;
            }
            else
            {
                ThisStartPoint = 2;
                ThisMult = ThisMult - 1;
            }
            
            // Then, process the first line of the event:
            ASCII_File << "    " << ThisEvent << "    " << ThisMult << "    ";
            if (pBeam==0.0) {ASCII_File << "0.0";} else {ASCII_File << pBeam;}
            ASCII_File << "    ";
            if (bpar==0.0) {ASCII_File << "0.0";} else {ASCII_File << bpar;}
            ASCII_File << "\n";
            
            // Next, process the particles:
            for (Int_t k = ThisStartPoint; k<(CurrentMultiplicity+2); ++k)
            {
                // NOTE: We should here refer to [k-1]!
                    
                // Then, update the ParticleGun-position:
                Position_X[k-1] = ParticleGun_x_position;
                Position_Y[k-1] = ParticleGun_y_position;
                Position_Z[k-1] = ParticleGun_z_position;
                    
                // Also, update the ParticleGun-direction:
                ThisVector.SetX(Momentum_X[k-1]);
                ThisVector.SetY(Momentum_Y[k-1]);
                ThisVector.SetZ(Momentum_Z[k-1]);
                        
                // randomized rotation:
                if (RandomRotateNeutrons==kTRUE)
                {
                    ThisVector.RotateZ(RandomPhiAngle);
                }                        
                    
                // ParticleGun-direction:
                ThisVector.RotateY(ParticleGun_Direction->Theta());
                ThisVector.RotateZ(ParticleGun_Direction->Phi());
                    
                Momentum_X[k-1] = ThisVector.X();
                Momentum_Y[k-1] = ThisVector.Y();
                Momentum_Z[k-1] = ThisVector.Z();
                    
                // Then, print the line:
                ASCII_File << "    " << Ion_ID[k-1] << "    " << Ion_Z[k-1] << "    " << PDG[k-1] << "    " << Momentum_X[k-1] << "    " << Momentum_Y[k-1] << "    " << Momentum_Z[k-1] << "    " << Position_X[k-1] << "    " << Position_Y[k-1] << "    " << Position_Z[k-1] << "    " << Mass[k-1] << "\n";
            }
        }
        
        // next, delete-commands:
        delete[] TheLines;
        delete[] Ion_ID;
        delete[] Ion_Z;
        delete[] PDG;
        delete[] Momentum_X;
        delete[] Momentum_Y;
        delete[] Momentum_Z;
        delete[] Position_X;
        delete[] Position_Y;
        delete[] Position_Z;
        delete[] Mass;
    }
}
