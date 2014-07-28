
{
    ifstream ifile("neuland_sync_159.txt");
    
    char str1[20], str2[20], str3[20], str4[20];
    
    ifile >> str1 >> str2 >> str3 >> str4;
    
    Int_t barId;
    Double_t tdiff;
    Double_t toff;
    Double_t veff;
    
    TFile *file = new TFile("igor.root", "RECREATE");
    TH1F *h_tdiff = new TH1F("h_tdiff_igor", "", 100, 0.5, 100.5);
    TH1F *h_toff = new TH1F("h_toff_igor", "", 100, 0.5, 100.5);
    TH1F *h_veff = new TH1F("h_veff_igor", "", 100, 0.5, 100.5);
    
    while(!ifile.eof())
    {
        ifile >> barId >> tdiff >> toff >> veff;
        if(barId > 0 && barId <= 100)
        {
            h_tdiff->SetBinContent(barId, tdiff);
            h_toff->SetBinContent(barId, toff);
            h_veff->SetBinContent(barId, veff);
        }
    }
    
    ifile.close();
    h_tdiff->Write();
    h_toff->Write();
    h_veff->Write();
    file->Close();
}