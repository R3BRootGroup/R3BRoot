void R3BNeuLANDTranslator::WriteMultMatrix()
{
    // Begin by generating the proper filename:
    TString st = "";
    TString kstr = "";
    TString nTstr = st.Itoa(TotalNumberOfThreads,10);
    TString cTstr = st.Itoa(CurrentThread,10);
    TString FileName = TheOutputPath + "/Neutron_Separation_Matrix_Maximum";
    if (TotalNumberOfThreads>1)
    {
        FileName = FileName + "_nT" + nTstr + "_cT" + cTstr + ".txt";
    }
    else
    {
        FileName = FileName + ".txt";
    }
    std::ofstream WriteTextFile (FileName.Data(), std::ofstream::out);
    
    // Write down the 1-interaction counters:
    WriteTextFile << "\n\n";
    WriteTextFile << "Amount of events with 1 prim. track fired                       = " << GunMult_Counter << "\n";
    WriteTextFile << "Amount of events with 1 prim. track interaction with NeuLAND    = " << NeuLANDMult_Counter << "\n";
    WriteTextFile << "Amount of events with 1 prim. track detected by NeuLAND signals = " << DetectedMult_Counter << "\n";
    
    // --------------------------------------------------------------------------------------------------------------------
    
    // NeuLAND:
    
    // Write down the neutron separation matrix gun/detected:
    WriteTextFile << "\n\n";
    WriteTextFile << "NeuLAND Neutron Separation Matrix for perfect reconstruction (Gun/Detected):\n";
    WriteTextFile << "        Particle Gun Multiplicities:\n";
    WriteTextFile << "                 ";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k) {kstr = st.Itoa(k,10); WriteTextFile << kstr << "n:      ";}
    WriteTextFile << "\n";
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (MultMatrix[MaxMultiplicity+1][kcol]>0.0)
            {
                MultMatrix[krow][kcol] = MultMatrix[krow][kcol]/MultMatrix[MaxMultiplicity+1][kcol];
            }
            else
            {
                MultMatrix[krow][kcol] = 0.0;
            }
            
            if (kcol==0) 
            {
                kstr = st.Itoa(krow,10);
                WriteTextFile << "Det. mult. " << kstr << "n: ";
            }
            
            WriteTextFile << RoundOff(100.0*MultMatrix[krow][kcol],4) << "%  ";
        }
        WriteTextFile << "\n";
    }
    
    // Write down the neutron separation matrix gun/NeuLAND:
    WriteTextFile << "\n\n";
    WriteTextFile << "NeuLAND Neutron Separation Matrix for perfect reconstruction (Gun/Interaction):\n";
    WriteTextFile << "        Particle Gun Multiplicities:\n";
    WriteTextFile << "                 ";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k) {kstr = st.Itoa(k,10); WriteTextFile << kstr << "n:      ";}
    WriteTextFile << "\n";
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (InteractionMatrix[MaxMultiplicity+1][kcol]>0.0)
            {
                InteractionMatrix[krow][kcol] = InteractionMatrix[krow][kcol]/InteractionMatrix[MaxMultiplicity+1][kcol];
            }
            else
            {
                InteractionMatrix[krow][kcol] = 0.0;
            }
            
            if (kcol==0) 
            {
                kstr = st.Itoa(krow,10);
                WriteTextFile << "Int. mult. " << kstr << "n: ";
            }
            
            WriteTextFile << RoundOff(100.0*InteractionMatrix[krow][kcol],4) << "%  ";
        }
        WriteTextFile << "\n";
    }
    
    // Write down the neutron separation matrix neuland/detected:
    WriteTextFile << "\n\n";
    WriteTextFile << "NeuLAND Neutron Separation Matrix for perfect reconstruction (NeuLAND/Detected):\n";
    WriteTextFile << "        NeuLAND Interaction Multiplicities:\n";
    WriteTextFile << "                 ";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k) {kstr = st.Itoa(k,10); WriteTextFile << kstr << "n:      ";}
    WriteTextFile << "\n";
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (DetectionMatrix[MaxMultiplicity+1][kcol]>0.0)
            {
                DetectionMatrix[krow][kcol] = DetectionMatrix[krow][kcol]/DetectionMatrix[MaxMultiplicity+1][kcol];
            }
            else
            {
                DetectionMatrix[krow][kcol] = 0.0;
            }
            
            if (kcol==0) 
            {
                kstr = st.Itoa(krow,10);
                WriteTextFile << "Det. mult. " << kstr << "n: ";
            }
            
            WriteTextFile << RoundOff(100.0*DetectionMatrix[krow][kcol],4) << "%  ";
        }
        WriteTextFile << "\n";
    }
    
    // --------------------------------------------------------------------------------------------------------------------
    
    // NEBULA:
    if (UseNEBULA==kTRUE)
    {
    WriteTextFile << "\n-----------------------------------------------------------------------------------\n";
    
    // Write down the neutron separation matrix gun/detected:
    WriteTextFile << "\n\n";
    WriteTextFile << "NEBULA Neutron Separation Matrix for perfect reconstruction (Gun/Detected):\n";
    WriteTextFile << "        Particle Gun Multiplicities:\n";
    WriteTextFile << "                 ";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k) {kstr = st.Itoa(k,10); WriteTextFile << kstr << "n:      ";}
    WriteTextFile << "\n";
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (MultNEBULAMatrix[MaxMultiplicity+1][kcol]>0.0)
            {
                MultNEBULAMatrix[krow][kcol] = MultNEBULAMatrix[krow][kcol]/MultNEBULAMatrix[MaxMultiplicity+1][kcol];
            }
            else
            {
                MultNEBULAMatrix[krow][kcol] = 0.0;
            }
            
            if (kcol==0) 
            {
                kstr = st.Itoa(krow,10);
                WriteTextFile << "Det. mult. " << kstr << "n: ";
            }
            
            WriteTextFile << RoundOff(100.0*MultNEBULAMatrix[krow][kcol],4) << "%  ";
        }
        WriteTextFile << "\n";
    }
    
    // Write down the neutron separation matrix gun/NeuLAND:
    WriteTextFile << "\n\n";
    WriteTextFile << "NEBULA Neutron Separation Matrix for perfect reconstruction (Gun/Interaction):\n";
    WriteTextFile << "        Particle Gun Multiplicities:\n";
    WriteTextFile << "                 ";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k) {kstr = st.Itoa(k,10); WriteTextFile << kstr << "n:      ";}
    WriteTextFile << "\n";
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (InteractionNEBULAMatrix[MaxMultiplicity+1][kcol]>0.0)
            {
                InteractionNEBULAMatrix[krow][kcol] = InteractionNEBULAMatrix[krow][kcol]/InteractionNEBULAMatrix[MaxMultiplicity+1][kcol];
            }
            else
            {
                InteractionNEBULAMatrix[krow][kcol] = 0.0;
            }
            
            if (kcol==0) 
            {
                kstr = st.Itoa(krow,10);
                WriteTextFile << "Int. mult. " << kstr << "n: ";
            }
            
            WriteTextFile << RoundOff(100.0*InteractionNEBULAMatrix[krow][kcol],4) << "%  ";
        }
        WriteTextFile << "\n";
    }
    
    // Write down the neutron separation matrix neuland/detected:
    WriteTextFile << "\n\n";
    WriteTextFile << "NEBULA Neutron Separation Matrix for perfect reconstruction (NeuLAND/Detected):\n";
    WriteTextFile << "        NeuLAND Interaction Multiplicities:\n";
    WriteTextFile << "                 ";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k) {kstr = st.Itoa(k,10); WriteTextFile << kstr << "n:      ";}
    WriteTextFile << "\n";
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (DetectionNEBULAMatrix[MaxMultiplicity+1][kcol]>0.0)
            {
                DetectionNEBULAMatrix[krow][kcol] = DetectionNEBULAMatrix[krow][kcol]/DetectionNEBULAMatrix[MaxMultiplicity+1][kcol];
            }
            else
            {
                DetectionNEBULAMatrix[krow][kcol] = 0.0;
            }
            
            if (kcol==0) 
            {
                kstr = st.Itoa(krow,10);
                WriteTextFile << "Det. mult. " << kstr << "n: ";
            }
            
            WriteTextFile << RoundOff(100.0*DetectionNEBULAMatrix[krow][kcol],4) << "%  ";
        }
        WriteTextFile << "\n";
    }
    
    // --------------------------------------------------------------------------------------------------------------------
    
    // Combo:
    WriteTextFile << "\n-----------------------------------------------------------------------------------\n";
    
    // Write down the neutron separation matrix gun/detected:
    WriteTextFile << "\n\n";
    WriteTextFile << "Combined Neutron Separation Matrix for perfect reconstruction (Gun/Detected):\n";
    WriteTextFile << "        Particle Gun Multiplicities:\n";
    WriteTextFile << "                 ";
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k) {kstr = st.Itoa(k,10); WriteTextFile << kstr << "n:      ";}
    WriteTextFile << "\n";
    
    for (Int_t krow = 0; krow<(MaxMultiplicity+1); ++krow)
    {
        for (Int_t kcol = 0; kcol<(MaxMultiplicity+1); ++kcol)
        {
            if (MultDoubleMatrix[MaxMultiplicity+1][kcol]>0.0)
            {
                MultDoubleMatrix[krow][kcol] = MultDoubleMatrix[krow][kcol]/MultDoubleMatrix[MaxMultiplicity+1][kcol];
            }
            else
            {
                MultDoubleMatrix[krow][kcol] = 0.0;
            }
            
            if (kcol==0) 
            {
                kstr = st.Itoa(krow,10);
                WriteTextFile << "Det. mult. " << kstr << "n: ";
            }
            
            WriteTextFile << RoundOff(100.0*MultDoubleMatrix[krow][kcol],4) << "%  ";
        }
        WriteTextFile << "\n";
    }
    
    } // UseNEBULA==kTRUE
    
    // Finish off the TextFile:
    WriteTextFile << "\n\n";  
    WriteTextFile.close();
    
    // Next, create an efficiency file:
    TString EffFileName = TheOutputPath + "/Detection_Efficiency";
    if (TotalNumberOfThreads>1)
    {
        EffFileName = EffFileName + "_nT" + nTstr + "_cT" + cTstr + ".txt";
    }
    else
    {
        EffFileName = EffFileName + ".txt";
    }
    std::ofstream EffWriteTextFile (EffFileName.Data(), std::ofstream::out);
    
    for (Int_t k = 0; k<(MaxMultiplicity+1); ++k)
    {
        EffWriteTextFile << MultMatrix[k][k] << "\n";
    }
    
    EffWriteTextFile.close();
}
