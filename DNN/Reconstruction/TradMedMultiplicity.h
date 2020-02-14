// This header contains the functions to determine the
// multiplicity of the traditional method reconstruction
// using D. Kresans method for multi neutron simulations.
// ==> It uses the traditional cuts.

// Function to read the calibration files:
void R3BScoreApplier::ReadCalibrationFile()
{
    // Reset the boolian tag:
    UseCalibrationCuts = kFALSE;
    
    // Obtain the filename:
    TString FileName = Inputs->GetInputString("TheOutputPath");
    FileName = FileName + "/CutsFile.txt";
    
    // Declare an ifstream to read the file:
    ifstream ThisFile;
  
    // Begin by determining the size of the file:
    Int_t NLines = 0;
    std::string ThisLine;
    ThisFile.open(FileName.Data());
  
    if (!(ThisFile.is_open()))
    {
        std::cout << "### NEUTRON TRACKER ERROR: No calibration file found!\n\n";
        UseCalibrationCuts = kFALSE;
    }
    else
    { 
        while (std::getline(ThisFile,ThisLine)) {NLines = NLines + 1;}
      
        // See if the file is suitable for our situation:
        if (MaxMultiplicity==(NLines-1))
        {
            // Then, we are OK. so generate the array that holds the cuts:
            fCuts = new Double_t[MaxMultiplicity];
      
            // Close and re-open the file:
            ThisFile.close();
            ThisFile.open(FileName.Data());
      
            // Read from the file:
            ThisFile >> fKappa;
      
            for (Int_t kE = 0; kE<MaxMultiplicity; ++kE)
            {
                // Move to the next line:
                std::getline(ThisFile,ThisLine);
          
                // read the next number:
                ThisFile >> fCuts[kE];
            }
      
            // Next, close the file again:
            ThisFile.close();
      
            // give some output:
            cout << "-I- R3BScoreApplier: Max. Multiplicity = " << MaxMultiplicity << "\n"
                 << "                                Kappa             = " << fKappa << "\n";
     
            for (Int_t kE = 0; kE<MaxMultiplicity; ++kE)
            {
                cout << "                                fCuts[" << kE << "]          = " << fCuts[kE] << "\n";
            }
      
            cout << "\n";
       
            // done.
            UseCalibrationCuts = kTRUE;
        }
        else
        {
            cout << "### ERROR: R3BScoreApplier: The file holding the calibration-cuts does not correspond to the current max. multiplicity in the Inputs-file!\n\n";
        }
    }
}

// Function to apply the calibration cuts inside the Exec-function:
Int_t R3BScoreApplier::ApplyCalibrationCuts()
{
    // Declare the answer:
    Int_t TheMultiplicity = -1;
    
    // Next, check that we can actually use our cuts:
    if (UseCalibrationCuts==kTRUE)
    {
        // Begin by obtaining the total amount of clusters:
        Int_t nClusters = fArrayClusters->GetEntries();
        Double_t nClusters_d = ((Int_t) nClusters) + 0.5; // NOTE: Optimization is also done on bin centers, not on the lower part of the bins!
  
        // Next, obtain the total amount of energy:
        R3BSignalCluster* ThisCluster;
        Double_t Etotal = 0.0;
  
        for (Int_t k = 0; k<nClusters; k++) 
        {
            // Retrieve the current cluster:
            ThisCluster = (R3BSignalCluster*) fArrayClusters->At(k);
            
            // And add its energy to the total:
            Etotal = Etotal + ThisCluster->GetEnergy();
        }
        

        // We have nClusters_d and nClusters_d which hold the point in the histogram. now we need to decide
        // between which cuts we are. The cuts are defined as y = fKappa*(fCuts[k] - x).
        // So compute Y for 2 successive cuts and see if nClusters_d is between them. 
        
        if (nClusters_d<(-1.0*TMath::Abs(fKappa)*(Etotal-fCuts[0])))
        {
            // Then we are below the lowest cut:
            TheMultiplicity = 0;
        }
        else if (nClusters_d>(-1.0*TMath::Abs(fKappa)*(Etotal-fCuts[MaxMultiplicity-1])))
        {
            // Then we are above the highest cut:
            TheMultiplicity = MaxMultiplicity;
        }
        else
        {
            if (MaxMultiplicity>=2)
            {
                for (Int_t Range = 1; Range<MaxMultiplicity; ++Range)
                {
                    if ((nClusters_d>(-1.0*TMath::Abs(fKappa)*(Etotal-fCuts[Range-1])))&&(nClusters_d<(-1.0*TMath::Abs(fKappa)*(Etotal-fCuts[Range]))))
                    {
                        TheMultiplicity = Range;
                    }
                }
            }
            else
            {
                std::cout << "### R3BMultMatrix: SERIOUS ERROR IN APPLYING THE CUTS!!!\n\n";
            }
        }
        
        // Adjust the special cases:
        if (nClusters==0) {TheMultiplicity = 0;}
        if (nClusters==1) {TheMultiplicity = 1;}
    }
    
    // Done. Return the answer:
    return TheMultiplicity;
}
    
