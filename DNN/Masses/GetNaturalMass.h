Double_t Nuclei::GetNaturalMass(Int_t const Z, Int_t const Zab, TString const Unit)
{
    // This function computes the average mass of all isotopes from element Z.
    // No electron masses are included. There is also an input argument Zab.
    // If Zab==Z, a normal average mass is computed. However, if Zab is different, 
    // then the abundance weight of element Zab (& isobar of Z) is used as a weight factor.
    // this method is useful for charge-excange reactions.
       
        Double_t Mass = 0.0;
        Double_t ThisMass = 0.0;
        Double_t Abundance = 0.0;
        Double_t This_Ab = 0.0;
        Int_t A = 0;
        Int_t Size = TheNuclei.size();
        
        for (Int_t k = 0; k<Size; ++k)
        {
            if (TheNuclei.at(k).GetZ()==Z)
            {
                A = TheNuclei.at(k).GetA();
                ThisMass = TheNuclei.at(k).GetMass("clean",Unit);
                This_Ab = 0.0;
                
                for(Int_t n = 0; n<Size; ++n)
                {
                    if ((TheNuclei.at(n).GetA()==A)&&(TheNuclei.at(n).GetZ()==Zab))
                    {
                        This_Ab = TheNuclei.at(n).GetAbundance("Dimless");
                    }
                }
                
                Abundance = Abundance + This_Ab;
                Mass = Mass + This_Ab*ThisMass;
            }
        }
        
        if (TMath::Abs(1.0 - Abundance)>5e-5) {std::cout << "NUCLEAR DATA FAILURE:: Abundance units do not match!!! Abundance = " << Abundance << "\n\n";}
        
        return Mass;
}