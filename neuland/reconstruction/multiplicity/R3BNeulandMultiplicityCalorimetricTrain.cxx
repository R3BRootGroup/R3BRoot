#include "R3BNeulandMultiplicityCalorimetricTrain.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRuntimeDb.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "TDirectory.h"
#include <iostream>
#include <numeric>

/*
 *      ^
 *      |
 * n    |
 * c    3
 * l    X X
 * u    X   X
 * s    X     X
 * t    X       X
 * e    0         X
 * r    | X         X
 *      |---1 X X X X 2----->
 *                   edep
 */

R3BNeulandMultiplicityCalorimetricTrain::R3BNeulandMultiplicityCalorimetricTrain(TString clusters, TString tracks)
    : FairTask("R3BNeulandMultiplicityCalorimetricTrain")
    , fClusters(std::move(clusters))
    , fTracks(std::move(tracks))
    , fPar(nullptr)
    , fEdepOpt({ 200, 25, 50, 1500 })
    , fNclusterOpt({ 10, 5, 5, 50 })
    , fWeight(0)
{
}

R3BNeulandMultiplicityCalorimetricTrain::~R3BNeulandMultiplicityCalorimetricTrain()
{
    for (auto& nc : fCuts)
    {
        delete nc.second;
    }
}

InitStatus R3BNeulandMultiplicityCalorimetricTrain::Init()
{
    // Input
    fClusters.Init();
    fTracks.Init();

    // Output Parameter Container
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(FATAL) << "R3BNeulandMultiplicityCalorimetricTrain:Init: No FairRootManager";
        return kFATAL;
    }

    auto rtdb = FairRuntimeDb::instance();
    if (rtdb == nullptr)
    {
        LOG(FATAL) << "R3BNeulandMultiplicityCalorimetricTrain::Init: No FairRuntimeDb!";
        return kFATAL;
    }
    fPar = (R3BNeulandMultiplicityCalorimetricPar*)rtdb->getContainer("R3BNeulandMultiplicityCalorimetricPar");
    if (fPar == nullptr)
    {
        LOG(FATAL) << "R3BNeulandMultiplicityCalorimetricTrain::Init: No R3BNeulandMultiplicityCalorimetricPar!";
        return kFATAL;
    }
    // FIXME: FairRuntimeDB needs to be forced to load the Data from the second file with Run Id 1
    // rtdb->initContainers(rtdb->getCurrentRun()->getRunId(), 1);

    return kSUCCESS;
}

void R3BNeulandMultiplicityCalorimetricTrain::Exec(Option_t*)
{
    const int nPN = fTracks.Retrieve().size();

    const auto clusters = fClusters.Retrieve();
    const int nClusters = clusters.size();

    if (nClusters == 0)
    {
        return;
    }

    const int Edep = (int)std::accumulate(
        clusters.cbegin(), clusters.cend(), 0., [](Double_t s, const R3BNeulandCluster* c) { return s + c->GetE(); });

    GetOrBuildHist(nPN)->Fill(Edep, nClusters);
}

void R3BNeulandMultiplicityCalorimetricTrain::FinishTask()
{
    Optimize();

    if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug))
    {
        Print();
    }

    auto rtdb = FairRuntimeDb::instance();
    rtdb->addRun(1);
    fPar->SetNeutronCuts(fCuts);
    fPar->setChanged();
    rtdb->writeContainer(fPar, rtdb->getRun(1), nullptr);

    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir("NeulandMultiplicityCalorimetricTrain");
    gDirectory->cd("NeulandMultiplicityCalorimetricTrain");

    for (auto& nh : fHists)
    {
        nh.second->Write();
    }

    gDirectory = tmp;
}

void R3BNeulandMultiplicityCalorimetricTrain::Optimize()
{
    ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Simplex");
    // min->SetMaxFunctionCalls(100000);
    // min->SetMaxIterations(100000);
    // min->SetTolerance(0.05);

    ROOT::Math::Functor f([&](const double* d) { return WastedEfficiency(d); }, 2);
    min->SetFunction(f);

    min->SetLimitedVariable(0, "edep", fEdepOpt.at(0), fEdepOpt.at(1), fEdepOpt.at(2), fEdepOpt.at(3));
    min->SetLimitedVariable(
        1, "ncluster", fNclusterOpt.at(0), fNclusterOpt.at(1), fNclusterOpt.at(2), fNclusterOpt.at(3));

    min->Minimize();

    LOG(INFO) << "R3BNeulandMultiplicityCalorimetricTrain::Optimize done!";
}

TCutG* R3BNeulandMultiplicityCalorimetricTrain::GetCut(const unsigned int nNeutrons,
                                                       const double edep,
                                                       const double ncluster)
{
    if (!fCuts[nNeutrons])
    {
        fCuts[nNeutrons] = new TCutG(TString(std::to_string(nNeutrons)), 4);
        fCuts.at(nNeutrons)->SetVarX("Total Energy [MeV]");
        fCuts.at(nNeutrons)->SetVarY("Number of Clusters");
    }
    auto cut = fCuts[nNeutrons];

    // nmin: p0 = p1 = (-1,-1)
    if (nNeutrons == fHists.begin()->first)
    {
        cut->SetPoint(0, -1, -1);
        cut->SetPoint(1, -1, -1);
    }
    else
    {
        cut->SetPoint(0, -1, ncluster * (nNeutrons - 1));
        cut->SetPoint(1, edep * (nNeutrons - 1), -1);
    }

    // nmax: p2 (inf, -1) p3 (-1, inf)
    if (nNeutrons == fHists.rbegin()->first)
    {
        cut->SetPoint(2, 100000, -1);
        cut->SetPoint(3, -1, 100000);
    }
    else
    {
        cut->SetPoint(2, edep * nNeutrons, -1);
        cut->SetPoint(3, -1, ncluster * nNeutrons);
    }

    return cut;
}

double R3BNeulandMultiplicityCalorimetricTrain::WastedEfficiency(const double* d)
{
    double edep = d[0];
    double ncluster = d[1];

    double wasted_efficiency = 0;
    for (auto& nh : fHists)
    {
        const unsigned int nNeutrons = nh.first;
        const double efficiency =
            ((double)GetCut(nNeutrons, edep, ncluster)->IntegralHist(nh.second) / (double)nh.second->GetEntries());
        wasted_efficiency += (1. - efficiency) * (1. + fWeight * nNeutrons);
    }
    return wasted_efficiency;
}

void R3BNeulandMultiplicityCalorimetricTrain::Print(Option_t*) const
{
    std::cout << "\t";
    for (const auto& nh : fHists)
    {
        std::cout << nh.first << "n\t";
    }
    std::cout << std::endl;

    for (const auto& nc : fCuts)
    {
        const unsigned int nOut = nc.first;
        const TCutG* cut = nc.second;

        std::cout << nOut << "n:\t";
        for (const auto& nh : fHists)
        {
            std::cout << ((double)cut->IntegralHist(nh.second) / (double)nh.second->GetEntries()) << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

TH2D* R3BNeulandMultiplicityCalorimetricTrain::GetOrBuildHist(const unsigned int i)
{
    if (fHists.find(i) == fHists.end())
    {
        const TString name = "hnPN" + std::to_string(i);
        fHists[i] = new TH2D(name, name, 250, 0, 5000, 50, 0, 100);
        fHists.at(i)->GetXaxis()->SetTitle("Total Energy [MeV]");
        fHists.at(i)->GetYaxis()->SetTitle("Number of Clusters");
    }
    return fHists.at(i);
}
