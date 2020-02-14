void R3BTradMethClass::Illustrate_EqualDist(Int_t const Resol)
{
    // Illustrates the minimization function of with equal distance
    // between the cuts.
    
    // Begin by extracting histogram information:
    Int_t NbinsX = MultHistograms[0]->GetNbinsX();
    Int_t NbinsY = MultHistograms[0]->GetNbinsY();
    Double_t Xmin = MultHistograms[0]->GetXaxis()->GetXmin();
    Double_t Xmax = MultHistograms[0]->GetXaxis()->GetXmax();
    Double_t Ymin = MultHistograms[0]->GetYaxis()->GetXmin();
    Double_t Ymax = MultHistograms[0]->GetYaxis()->GetXmax();
    
    // Next, create a new histogram to hold the illustration:
    
    // Compute quatities:
    Double_t Kappa_Diagonal = -1.0*TMath::Abs((Ymax-Ymin)/(Xmax-Xmin));
    Double_t Difference_Estimate = (Xmax-Xmin)/((Int_t) Max_Multiplicity);
    Int_t Resolution = Resol;
    
    // Define X as distance & Y as Kappa:
    TH2D* Illustration = new TH2D("Minimization_EqualDist","Minimization_EqualDist",Resolution,0.0,2.5*Difference_Estimate,Resolution,6.0*Kappa_Diagonal,0.01*Kappa_Diagonal);
    
    // Define an input Array for the function:
    Double_t* InputArray = new Double_t[2];
    Double_t Answer = 0.0;
    
    // Update histogram information:
    NbinsX = Illustration->GetNbinsX();
    NbinsY = Illustration->GetNbinsY();
    Xmin = Illustration->GetXaxis()->GetXmin();
    Xmax = Illustration->GetXaxis()->GetXmax();
    Ymin = Illustration->GetYaxis()->GetXmin();
    Ymax = Illustration->GetYaxis()->GetXmax();
    
    // Then, loop over the full histogram & fill it up:
    for (Int_t kx = 1; kx<(NbinsX+1); ++kx)
    {
        for (Int_t ky = 1; ky<(NbinsY+1); ++ky)
        {
            // Fill up the array. [0]=Kappa=Y:
            InputArray[0] = (((Int_t) ky) - 0.5);
            InputArray[0] = InputArray[0]/((Int_t) NbinsY);
            InputArray[0] = InputArray[0]*(Ymax-Ymin) + Ymin;
            
            // [1]=Difference=X:
            InputArray[1] = (((Int_t) kx) - 0.5);
            InputArray[1] = InputArray[1]/((Int_t) NbinsX);
            InputArray[1] = InputArray[1]*(Xmax-Xmin) + Xmin;
            
            // Evaluate the function:
            Answer = SquaredWasted_EqualDist(InputArray);
            
            // Put it is the histogram:
            Illustration->SetBinContent(kx,ky,Answer);
            Illustration->SetBinError(kx,ky,0.0);
        }
        
        // Give some output:
        cout << "Progress (X-bins): " << kx << "/" << NbinsX << "\n";
    }
    
    // Next, save this histogram into a file:
    TString PATH = Inputs->GetInputString("TheOutputPath");
    PATH = PATH + "/Illustrations.root";
    
    TFile* f = new TFile(PATH.Data(),"recreate");
    Illustration->Write("Minimization_Function",2);
    
    TObjString* Xlabel = new TObjString("Distance between the cuts [MeV]");
    TObjString* Ylabel = new TObjString("Kappa [#clusters/MeV]");
    TObjString* Zlabel = new TObjString("Squared Wasted Efficiency [%^2]");
    
    Xlabel->Write("Xlabel",2);
    Xlabel->Write("Ylabel",2);
    Xlabel->Write("Zlabel",2);
    
    f->Close();
    
    // delete all objects:
    delete f;
    delete InputArray;
    delete Illustration;
    
    // Done.
}
