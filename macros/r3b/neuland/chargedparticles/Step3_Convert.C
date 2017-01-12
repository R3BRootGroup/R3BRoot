void SingleExportAscii(TH1* hist, ofstream& out, const TString separator = "\t")
{
    if (!hist)
    {
        // throw;
        cout << "nope" << endl;
        return;
    }

    out << "# Output " << hist->ClassName() << ": " << hist->GetName() << " (" << hist->GetTitle() << ")\n";
    if (hist->GetDimension() == 1)
    {
        out << "# BinCenter" << separator << "Content" << separator << "BinHalfWidth" << separator << "Error\n";
        for (Int_t i = 1; i <= hist->GetNbinsX(); i++)
        {
            out << hist->GetBinCenter(i) << separator << hist->GetBinContent(i) << separator << hist->GetBinWidth(i) / 2
                << separator << hist->GetBinError(i) << endl;
        }
    }
    else if (hist->GetDimension() == 2)
    {
        out << "# xBinCenter" << separator << "yBinCenter" << separator << "Content" << separator << "xBinHalfWidth"
            << separator << "yBinHalfWidth" << separator << "Error" << endl;
        for (Int_t i = 1; i <= hist->GetNbinsX(); i++)
        {
            Double_t xcenter = hist->GetXaxis()->GetBinCenter(i);
            Double_t xwidth = hist->GetXaxis()->GetBinWidth(i) / 2;
            for (Int_t j = 1; j <= hist->GetNbinsY(); j++)
            {
                out << xcenter << separator << hist->GetYaxis()->GetBinCenter(j) << separator
                    << hist->GetBinContent(i, j) << separator << xwidth << separator
                    << hist->GetYaxis()->GetBinWidth(j) / 2 << separator << hist->GetBinError(i, j) << endl;
            }
            /*if (j > 1)
            {
                out << endl; // produce a blank line after each set of Ybins for a certain Xbin. Gnuplot likes this.
            }*/
        }
    }
}

void Step3_Convert(const TString filename)
{
    TFile* file = new TFile(filename, "READ");

    std::vector<TString> hists;
    // No bracket enclosed initializer list in root 5
    hists.push_back("NeulandDigiMon/hDepth");
    hists.push_back("NeulandDigiMon/hForemostEnergy");
    hists.push_back("NeulandDigiMon/hEtot");
    hists.push_back("NeulandClusterMon/ClusterEnergyMoment");
    hists.push_back("NeulandClusterMon/ClusterMaxEnergyDigiMinusFirstDigiMag");


    // for (const TString& hist : hists)
    for (Int_t i = 0; i < hists.size(); i++)
    {
        const TString hist = hists.at(i);
        std::cout << hist << std::endl;
        TH1D* h = (TH1D*)file->GetObjectChecked(hist, "TH1D");

        const TString outfile = (TString(filename).ReplaceAll(
            "digi.root",
            TString(hist).ReplaceAll("NeulandDigiMon/", "").ReplaceAll("NeulandClusterMon/", "") + ".dat"));
        ofstream outstream(outfile);

        SingleExportAscii(h, outstream);

        outstream.close();
    }
}
