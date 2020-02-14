std::vector<Double_t> R3BASCIIFileGenerator::DefineMultBoundaries()
{
    // Creates the boundaries for randomization of the neutron multiplicities.
    // Begin by a uniform distribution:
    std::vector<Double_t> Boundaries;
    Boundaries.resize(ParticleGun_Multiplicity+1);
    
    for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
    {
        Boundaries.at(k) = 1.0/((Int_t) ParticleGun_Multiplicity);
        Boundaries.at(k) = Boundaries.at(k)*((Int_t) k);
    }
    
    // Start by looking if the file of our choice exists:
    if (ParticleGun_Uniformize_Detected_Multiplicity==kTRUE)
    {
        // First, try our modeling function:
        TF1* DetEff = new TF1("DetectionEfficiency","[0]*(1.0+[1]*TMath::Exp(-1.0*[2]*x))",0.0,((Int_t) (ParticleGun_Multiplicity+1)));
        DetEff->SetParameter(0,ParticleGun_InfiniteDetectionRate);
        DetEff->SetParameter(1,(1.0/ParticleGun_InfiniteDetectionRate)-1.0);
        DetEff->SetParameter(2,1.0-ParticleGun_SingleDetectionRate);
        
        for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
        {
            if (k==0) {Boundaries.at(k) = 0.0;}
            else
            {
                Boundaries.at(k) = Boundaries.at(k-1) + 1.0/DetEff->Eval((Int_t) k);
            }
        }
        
        // Renormalize:
        Double_t MaxBound = Boundaries.at(ParticleGun_Multiplicity);
    
        for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
        {
            Boundaries.at(k) = Boundaries.at(k)/MaxBound;
        }
        
        // Next, see if we can do better with previous data:
        FileStat_t stom;
        Int_t test = gSystem->GetPathInfo(FileName.Data(),stom);
        TFile* f;
    
        if (test==0)
        {
            // Then the file already exists:
            f = new TFile(FileName.Data(),"read");
            
            // Next, try to retrieve the histograms:
            TH1I* GunMult = (TH1I*) f->Get("Multiplicity_FromParticleGun");
            TH1I* DetMult = (TH1I*) f->Get("Multiplicity_FromR3BSignals");
            
            // check if both histograms exist:
            if ((GunMult!=nullptr)&&(GunMult!=0)&&(DetMult!=nullptr)&&(DetMult!=0))
            {
                // Retrieve the number of bins:
                Int_t NBinsG = GunMult->GetNbinsX();
                Int_t NBinsD = DetMult->GetNbinsX();
                
                // Check if they match:
                if ((NBinsG==(ParticleGun_Multiplicity+1))&&(NBinsD==(ParticleGun_Multiplicity+1)))
                {
                    // then, we can define the probabilities for each multiplicity:
                    std::vector<Double_t> Probabilities;
                    Probabilities.resize(ParticleGun_Multiplicity);
                    
                    for (Int_t k = 0; k<ParticleGun_Multiplicity; ++k)
                    {
                        Probabilities.at(k) = (Int_t) DetMult->GetBinContent(k+2);
                        Probabilities.at(k) = Probabilities.at(k)/((Int_t) GunMult->GetBinContent(k+2));
                    }
                    
                    // Then, fill up the boundaries:
                    for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
                    {
                        if (k==0) {Boundaries.at(k) = 0.0;}
                        else
                        {
                            Boundaries.at(k) = Boundaries.at(k-1) + 1.0/Probabilities.at(k-1);
                        }
                    }
                    
                    // Renormalize:
                    MaxBound = Boundaries.at(ParticleGun_Multiplicity);
    
                    for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
                    {
                        Boundaries.at(k) = Boundaries.at(k)/MaxBound;
                    }
                    
                    // Done. Close all blocks:
                }
            }
        }
        
        // If we need to use Elena's boundaries, then simply overwrite:
        if (ParticleGun_Multiplicity_Distribution=="Elena")
        {
            // Define numbers:
            std::vector<Double_t> ElenaNumbers;
            ElenaNumbers.resize(5);
            ElenaNumbers.at(0) = 0.58;
            ElenaNumbers.at(1) = 0.65;
            ElenaNumbers.at(2) = 0.70;
            ElenaNumbers.at(3) = 0.76;
            ElenaNumbers.at(4) = 1.00;
            
            // Transfer them to boundaries:
            if (ParticleGun_Multiplicity<=5)
            {
                for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
                {
                    if (k==0) {Boundaries.at(k) = 0.0;}
                    else
                    {
                        Boundaries.at(k) = Boundaries.at(k-1) + ElenaNumbers.at(k-1);
                    }
                }
                
                // Renormalize:
                MaxBound = Boundaries.at(ParticleGun_Multiplicity);
    
                for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
                {
                    Boundaries.at(k) = Boundaries.at(k)/MaxBound;
                }
                
                // Done.
            }
        }
        
        // Use our own version of Elena's boundaries:
        if (ParticleGun_Multiplicity_Distribution=="AdaptedElena")
        {
            // Define numbers:
            std::vector<Double_t> ElenaNumbers;
            ElenaNumbers.resize(5);
            ElenaNumbers.at(0) = 0.18609;
            ElenaNumbers.at(1) = 0.19455;
            ElenaNumbers.at(2) = 0.19608;
            ElenaNumbers.at(3) = 0.20535;
            ElenaNumbers.at(4) = 0.21793;
            
            // Transfer them to boundaries:
            if (ParticleGun_Multiplicity<=5)
            {
                for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
                {
                    if (k==0) {Boundaries.at(k) = 0.0;}
                    else
                    {
                        Boundaries.at(k) = Boundaries.at(k-1) + ElenaNumbers.at(k-1);
                    }
                }
                
                // Renormalize:
                MaxBound = Boundaries.at(ParticleGun_Multiplicity);
    
                for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
                {
                    Boundaries.at(k) = Boundaries.at(k)/MaxBound;
                }
                
                // Done.
            }
        }
        
        // Manual multiplicity specification:
        if (ParticleGun_Multiplicity_Distribution=="Custom")
        {
            if (CustomBoundaries_AreSpecified==kTRUE)
            {
                for (Int_t k = 0; k<(ParticleGun_Multiplicity+1); ++k)
                {
                    Boundaries.at(k) = CustomBoundaries.at(k);
                }
            }
            else
            {
                ErrorMessage("You cannot choose the Custom-option for multiplicity randomization without specifiying the boundaries in MCtransport.cpp!");
            }
        }
    }
    
    // return the answer:
    return Boundaries;
}
