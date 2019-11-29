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
#include "FairMCPoint.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "R3BMCTrack.h"
#include "R3BNeulandNeutron2DPar.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TColor.h"
#include "TCutG.h"
#include "TFile.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TTree.h"

unsigned int GetNin(const TClonesArray* tracks)
{
    unsigned int n = 0;

    const Int_t nTracks = tracks->GetEntries();
    for (Int_t i = 0; i < nTracks; i++)
    {
        auto track = (R3BMCTrack*)tracks->At(i);
        if (track->GetMotherId() == -1 && track->GetPdgCode() == 2112)
        {
            n++;
        }
    }
    return n;
}

TH2D* GetOrBuildHist(std::map<UInt_t, TH2D*>& map, const unsigned int i, const TString& name, const TString& title)
{
    if (map.find(i) == map.end())
    {
        map[i] = new TH2D(name + TString::UItoa(i, 10), TString::UItoa(i, 10) + title, 300, 0, 3000, 50, 0, 50);
        map.at(i)->GetXaxis()->SetTitle("Total Energy [MeV]");
        map.at(i)->GetYaxis()->SetTitle("Number of Clusters");
    }
    return map.at(i);
}

void DoPrint(std::ostream& out, const std::map<UInt_t, TH2D*>& map, const std::map<UInt_t, TCutG*>& cuts)
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
        const UInt_t nOut = nc.first;
        const TCutG* cut = nc.second;

        out << nOut << "n:\t";

        Double_t sum = 0.;
        for (const auto& nh : map)
        {
            sum += ((Double_t)cut->IntegralHist(nh.second) / (Double_t)nh.second->GetEntries());
            out << ((Double_t)cut->IntegralHist(nh.second) / (Double_t)nh.second->GetEntries()) << "\t";
        }
        if (map.find(nOut) != map.end())
        {
            out << (Double_t)cut->IntegralHist(map.at(nOut)) / (Double_t)(map.at(nOut)->GetEntries()) / sum;
        }
        out << std::endl;
    }
}

namespace Neuland
{
    Neutron2DCalibr::Neutron2DCalibr(UInt_t nmax)
        : fNMax(nmax)
    {
    }

    void Neutron2DCalibr::AddClusterFile(const TString& filename)
    {
        auto file = new TFile(filename, "READ");
        auto tree = (TTree*)file->Get("evt");
        tree->AddFriend("evt2=evt", TString(filename).ReplaceAll(".digi.root", ".simu.root"));

        TBranch* branch = tree->GetBranch("NeulandClusters");
        auto clusters = new TClonesArray("R3BNeulandCluster");
        branch->SetAddress(&clusters);

        TBranch* branch2 = tree->GetBranch("NeulandPrimaryPoints");
        auto npnips = new TClonesArray("FairMCPoint");
        branch2->SetAddress(&npnips);

        TBranch* branch3 = tree->GetBranch("MCTrack");
        auto tracks = new TClonesArray("R3BMCTrack");
        branch3->SetAddress(&tracks);

        const Int_t nEntries = tree->GetEntries();
        for (Int_t ei = 0; ei < nEntries; ei++)
        {
            branch->GetEntry(ei);
            branch2->GetEntry(ei);
            branch3->GetEntry(ei);

            Double_t Etot = 0.;
            Int_t validClusters = 0;
            const Int_t nClusters = clusters->GetEntries();

            for (Int_t ci = 0; ci < nClusters; ci++)
            {
                auto cluster = (R3BNeulandCluster*)clusters->At(ci);
                Etot += cluster->GetE();
                if (fClusterFilters.IsValid(cluster))
                {
                    validClusters++;
                }
            }

            const unsigned int nNPNIPs = npnips->GetEntries();
            if (nNPNIPs <= fNMax)
            {
                auto h = GetOrBuildHist(fHistsNreac, nNPNIPs, "hEC", "n reacted");
                h->Fill(Etot, validClusters);
            }

            const unsigned int nNin = GetNin(tracks);
            if (nNin <= fNMax)
            {
                auto h = GetOrBuildHist(fHistsNin, nNin, "hNin", "n incoming");
                h->Fill(Etot, validClusters);
            }
        }
    }

    void Neutron2DCalibr::Optimize(std::vector<Double_t> slope,
                                   std::vector<Double_t> distance,
                                   std::vector<Double_t> dist_off)
    {
        const UInt_t nVars = 3;

        ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
        min->SetMaxFunctionCalls(100000000);
        min->SetMaxIterations(10000000);
        min->SetTolerance(0.05);

        ROOT::Math::Functor f([&](const Double_t* d) { return WastedEfficiency(d); }, nVars);
        min->SetFunction(f);

        /*Double_t step[nVars] = { 0.001, 0.5, 0.5 };
        Double_t variable[nVars] = { 0.04, 10, 3 };
        Double_t lower[nVars] = { 0.001, 1, 3 };
        Double_t upper[nVars] = { 10, 20, 6 };*/
        min->SetLimitedVariable(0, "slope", slope.at(0), slope.at(1), slope.at(2), slope.at(3));
        min->SetLimitedVariable(1, "distance", distance.at(0), distance.at(1), distance.at(2), distance.at(3));
        min->SetLimitedVariable(2, "distance offset", dist_off.at(0), dist_off.at(1), dist_off.at(2), dist_off.at(3));

        min->Minimize();

        std::cout << "Neutron2DCalibr::Optimize done!" << std::endl;
    }

    TCutG* Neutron2DCalibr::GetCut(const UInt_t nNeutrons, const Double_t k, const Double_t k0, const Double_t m)
    {
        if (!fCuts[nNeutrons])
        {
            fCuts[nNeutrons] = new TCutG(TString::UItoa(nNeutrons, 10), 4);
            fCuts.at(nNeutrons)->SetVarX("Total Energy [MeV]");
            fCuts.at(nNeutrons)->SetVarY("Number of Clusters");
        }

        Double_t y0;
        if (nNeutrons != 0)
        {
            y0 = (nNeutrons - 1) * k + k0;
        }
        else
        {
            y0 = -1;
        }

        const Double_t y3 = nNeutrons * k + k0;
        const Double_t x1 = y0 / m;
        const Double_t x2 = y3 / m;
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

    Double_t Neutron2DCalibr::WastedEfficiency(const Double_t* d)
    {
        Double_t m = d[0];
        Double_t k = d[1];
        Double_t k0 = d[2];
        GetCut(0, k, k0, m);

        Double_t wasted_efficiency = 0;
        for (auto& nh : fHistsNreac)
        {
            const UInt_t nNeutrons = nh.first;
            wasted_efficiency += 1. - ((Double_t)GetCut(nNeutrons, k, k0, m)->IntegralHist(nh.second) /
                                       (Double_t)nh.second->GetEntries());
        }
        return wasted_efficiency;
    }

    void Neutron2DCalibr::Draw(const TString& img) const
    {
        gStyle->SetOptStat(0);
        // reverse viridis
        Double_t stops[9] = { 0.0000, 0.1250, 0.2500, 0.3750, 0.5000, 0.6250, 0.7500, 0.8750, 1.0000 };
        Double_t red[9] = { 246. / 255., 144. / 255., 74. / 255., 35. / 255., 28. / 255.,
                            33. / 255.,  43. / 255.,  51. / 255., 26. / 255. };
        Double_t green[9] = { 222. / 255., 200. / 255., 180. / 255., 150. / 255., 118. / 255.,
                              87. / 255.,  55. / 255.,  24. / 255.,  9. / 255. };
        Double_t blue[9] = { 0. / 255.,   35. / 255.,  72. / 255., 101. / 255., 112. / 255.,
                             114. / 255., 112. / 255., 96. / 255., 30. / 255. };
        TColor::CreateGradientColorTable(9, stops, red, green, blue, 255, 1);

        auto c = new TCanvas("Neutron2DCalibr", "Neuland Neutron2D Calibr", 500 * fNMax, 500);
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

            auto hist = nh.second;
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

        c->Draw();
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
        FairRuntimeDb* rtdb = FairRuntimeDb::instance();

        FairParRootFileIo* io = new FairParRootFileIo(kTRUE);
        io->open(parFile);
        rtdb->setOutput(io);

        R3BNeulandNeutron2DPar* par = (R3BNeulandNeutron2DPar*)rtdb->getContainer("R3BNeulandNeutron2DPar");

        rtdb->addRun(1);
        par->SetNeutronCuts(fCuts);
        par->setChanged();
        rtdb->writeContainers();

        rtdb->saveOutput();
        rtdb->print();

        for (auto& nh : fHistsNreac)
        {
            nh.second->Write();
        }

        for (auto& nh : fHistsNin)
        {
            nh.second->Write();
        }
    }
}; // namespace Neuland
