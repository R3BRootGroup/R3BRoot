TString Nuclei::FindNucleusFromPDG(Int_t const PDG)
{
    // Finds the name of a nucleus in the database,
    // given its PDG code.
    
    Int_t Abeam = -1;
    Int_t Zbeam = -1;
    
    if (TMath::Abs(PDG)>1000000000)
    {
        Int_t PDGsmall = 0;
        if (PDG>0) {PDGsmall = PDG - 1000000000;}
        if (PDG<0) {PDGsmall = PDG + 1000000000;}
        Double_t PDG_d = (Int_t) TMath::Abs(PDGsmall);

        Double_t Z_d = PDG_d/10.0;
        Double_t R_d = PDG_d/10000.0;
    
        Double_t z_Z_d = PDG_d/10000.0;
        Double_t z_R_d = PDG_d/10000000.0;
    
        Int_t Z = (Double_t) Z_d;
        Int_t R = (Double_t) R_d;
        Z_d = Z_d - 1000.0*((Int_t) R);
        Z = (Double_t) Z_d;
        Z_d = (Int_t) Z;
    
        Int_t z_Z = (Double_t) z_Z_d;
        Int_t z_R = (Double_t) z_R_d;
        z_Z_d = z_Z_d - 1000.0*((Int_t) z_R);
        z_Z = (Double_t) z_Z_d;
        z_Z_d = (Int_t) z_Z;
    
        Abeam = Z_d;
        Zbeam = z_Z_d;
    }
    else
    {
        cout << "### ERROR: an ion-PDG must be larger than 1000000000!\n\n";
    }
    
    // Next, find the right nucleus:
    TString Answer = "";
    Int_t Size = TheNuclei.size();
        
    for (Int_t k = 0; k<Size; ++k)
    {
        if ((TheNuclei.at(k).GetA()==Abeam)&&(TheNuclei.at(k).GetZ()==Zbeam)) {Answer = TheNuclei.at(k).GetName();}
    }
    
    return Answer;
}
