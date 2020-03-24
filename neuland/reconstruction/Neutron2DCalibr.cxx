/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "Neutron2DCalibr.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "R3BMCTrack.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron2DPar.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TCutG.h"
#include "TFile.h"
#include "TStyle.h"
#include "TTree.h"

std::unique_ptr<TH2D>& GetOrBuildHist(std::map<unsigned int, std::unique_ptr<TH2D>>& map,
                                      const unsigned int i,
                                      const TString& name,
                                      const TString& title)
{
    if (map.find(i) == map.end())
    {
        map[i] = std::unique_ptr<TH2D>(
            new TH2D(name + std::to_string(i), std::to_string(i) + title, 300, 0, 3000, 50, 0, 50));
        map.at(i)->GetXaxis()->SetTitle("Total Energy [MeV]");
        map.at(i)->GetYaxis()->SetTitle("Number of Clusters");
    }
    return map.at(i);
}

void DoPrint(std::ostream& out,
             const std::map<unsigned int, std::unique_ptr<TH2D>>& map,
             const std::map<unsigned int, TCutG*>& cuts)
{
    out << "\t";
    for (const auto& nh : map)
    {
        out << nh.first << "n\t";
    }
    out << "Purity";
    out << std::endl;

    for (const auto& nc : cuts)
    {
        const unsigned int nOut = nc.first;
        const TCutG* cut = nc.second;

        out << nOut << "n:\t";

        double sum = 0.;
        for (const auto& nh : map)
        {
            sum += ((double)cut->IntegralHist(nh.second.get()) / (double)nh.second->GetEntries());
            out << ((double)cut->IntegralHist(nh.second.get()) / (double)nh.second->GetEntries()) << "\t";
        }
        if (map.find(nOut) != map.end())
        {
            out << (double)cut->IntegralHist(map.at(nOut).get()) / (double)(map.at(nOut)->GetEntries()) / sum;
        }
        out << std::endl;
    }
}

namespace Neuland
{
    Neutron2DCalibr::Neutron2DCalibr(unsigned int nmax)
        : fNMax(nmax)
    {
        TH2::AddDirectory(false);
    }

    void Neutron2DCalibr::AddClusterFile(const TString& filename)
    {
        TFile file(filename, "READ");
        auto tree = (TTree*)file.Get("evt");

        auto clusters = new TClonesArray("R3BNeulandCluster");
        tree->SetBranchAddress("NeulandClusters", &clusters);
        auto primaryPoints = new TClonesArray("R3BNeulandPoint");
        tree->SetBranchAddress("NeulandPrimaryPoints", &primaryPoints);
        auto neutronTracks = new TClonesArray("R3BMCTrack");
        tree->SetBranchAddress("NeulandPrimaryTracks", &neutronTracks);

        const int nEntries = tree->GetEntries();
        for (int ei = 0; ei < nEntries; ei++)
        {
            tree->GetEntry(ei);

            double Etot = 0.;
            int validClusters = 0;

            const int nClusters = clusters->GetEntries();
            for (int ci = 0; ci < nClusters; ci++)
            {
                auto cluster = (R3BNeulandCluster*)clusters->At(ci);
                Etot += cluster->GetE();
                if (fClusterFilters.IsValid(cluster))
                {
                    validClusters++;
                }
            }

            const unsigned int nPrimaryPoints = primaryPoints->GetEntries();
            if (nPrimaryPoints <= fNMax)
            {
                GetOrBuildHist(fHistsNreac, nPrimaryPoints, "hnPP", "n reacted")->Fill(Etot, validClusters);
            }

            const unsigned int nNeutrons = neutronTracks->GetEntries();
            if (nNeutrons <= fNMax)
            {
                GetOrBuildHist(fHistsNreac, nPrimaryPoints, "hnPP", "n reacted")->Fill(Etot, validClusters);
            }
        }

        clusters->Delete();
        delete clusters;
        primaryPoints->Delete();
        delete primaryPoints;
        neutronTracks->Delete();
        delete neutronTracks;
    }

    void Neutron2DCalibr::Optimize(std::vector<double> slope,
                                   std::vector<double> distance,
                                   std::vector<double> dist_off)
    {
        const unsigned int nVars = 3;

        ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
        min->SetMaxFunctionCalls(100000000);
        min->SetMaxIterations(10000000);
        min->SetTolerance(0.05);

        ROOT::Math::Functor f([&](const double* d) { return WastedEfficiency(d); }, nVars);
        min->SetFunction(f);

        /*double step[nVars] = { 0.001, 0.5, 0.5 };
        double variable[nVars] = { 0.04, 10, 3 };
        double lower[nVars] = { 0.001, 1, 3 };
        double upper[nVars] = { 10, 20, 6 };*/
        min->SetLimitedVariable(0, "slope", slope.at(0), slope.at(1), slope.at(2), slope.at(3));
        min->SetLimitedVariable(1, "distance", distance.at(0), distance.at(1), distance.at(2), distance.at(3));
        min->SetLimitedVariable(2, "distance offset", dist_off.at(0), dist_off.at(1), dist_off.at(2), dist_off.at(3));

        min->Minimize();

        std::cout << "Neutron2DCalibr::Optimize done!" << std::endl;
    }

    TCutG* Neutron2DCalibr::GetCut(const unsigned int nNeutrons, const double k, const double k0, const double m)
    {
        if (!fCuts[nNeutrons])
        {
            fCuts[nNeutrons] = new TCutG(TString(std::to_string(nNeutrons)), 4);
            fCuts.at(nNeutrons)->SetVarX("Total Energy [MeV]");
            fCuts.at(nNeutrons)->SetVarY("Number of Clusters");
        }

        double y0;
        if (nNeutrons != 0)
        {
            y0 = (nNeutrons - 1) * k + k0;
        }
        else
        {
            y0 = -1;
        }

        const double y3 = nNeutrons * k + k0;
        const double x1 = y0 / m;
        const double x2 = y3 / m;
        // std::cout << "(0, " << y0 << ") (" << x1 << ", 0) (" << x2 << ", 0) ( 0, " << y3 << ")" << std::endl;

        TCutG* cut = fCuts[nNeutrons];
        cut->SetPoint(0, -1, y0);
        cut->SetPoint(1, x1, -1);
        if (nNeutrons == fNMax)
        {
            cut->SetPoint(2, 100000, -1);
            cut->SetPoint(3, -1, 100000);
        }
        else
        {
            cut->SetPoint(2, x2, -1);
            cut->SetPoint(3, -1, y3);
        }
        return cut;
    }

    double Neutron2DCalibr::WastedEfficiency(const double* d)
    {
        double m = d[0];
        double k = d[1];
        double k0 = d[2];
        GetCut(0, k, k0, m);

        double wasted_efficiency = 0;
        for (auto& nh : fHistsNreac)
        {
            const unsigned int nNeutrons = nh.first;
            wasted_efficiency += 1. - ((double)GetCut(nNeutrons, k, k0, m)->IntegralHist(nh.second.get()) /
                                       (double)nh.second->GetEntries());
        }
        return wasted_efficiency;
    }

    void Neutron2DCalibr::Draw(const TString& img) const
    {
        gStyle->SetOptStat(0);
        gStyle->SetPalette(-1);
        gStyle->SetPalette(kViridis);
        TColor::InvertPalette();

        std::unique_ptr<TCanvas> c(new TCanvas("Neutron2DCalibr", "Neuland Neutron2D Calibr", 500 * fNMax, 500));
        c->Divide(fNMax, 1);

        for (auto& nh : fHistsNreac)
        {
            if (nh.first == 0)
            {
                continue;
            }
            c->cd(nh.first);

            gPad->SetTopMargin(0.03);
            gPad->SetBottomMargin(0.15);
            gPad->SetLeftMargin(0.06);
            gPad->SetRightMargin(0.13);

            auto& hist = nh.second;
            hist->SetLineWidth(1);
            hist->SetTitle("");
            hist->SetXTitle("");
            hist->GetXaxis()->SetRangeUser(0, 2500);
            hist->SetYTitle("");
            hist->GetXaxis()->SetLabelFont(134);
            hist->GetYaxis()->SetLabelFont(134);
            hist->GetZaxis()->SetLabelFont(134);
            hist->GetXaxis()->SetLabelSize(25);
            hist->GetYaxis()->SetLabelSize(25);
            hist->GetZaxis()->SetLabelSize(25);

            hist->Draw("colz");
            fCuts.at(nh.first)->Draw("same");
        }

        if (!(img == ""))
        {
            c->Print(img);
        }
    }

    void Neutron2DCalibr::Print(std::ostream& out) const
    {
        out << "REAC" << std::endl;
        DoPrint(out, fHistsNreac, fCuts);
        out << std::endl;

        out << "IN" << std::endl;
        DoPrint(out, fHistsNin, fCuts);
        out << std::endl;
    }

    void Neutron2DCalibr::WriteParameterFile(const TString& parFile) const
    {
        std::unique_ptr<FairRuntimeDb> rtdb(FairRuntimeDb::instance());
        FairParRootFileIo io(kTRUE);
        io.open(parFile);
        rtdb->setOutput(&io);

        auto par = (R3BNeulandNeutron2DPar*)rtdb->getContainer("R3BNeulandNeutron2DPar");

        rtdb->addRun(1);
        par->SetNeutronCuts(fCuts);
        par->setChanged();
        rtdb->writeContainers();

        for (auto& nh : fHistsNreac)
        {
            nh.second->Write();
        }

        for (auto& nh : fHistsNin)
        {
            nh.second->Write();
        }

        rtdb->saveOutput();
        rtdb->print();
        rtdb->closeOutput();
    }

    Neutron2DCalibr::~Neutron2DCalibr()
    {
        for (auto& nc : fCuts)
        {
            delete nc.second;
        }
    }

}; // namespace Neuland

ClassImp(Neuland::Neutron2DCalibr);
