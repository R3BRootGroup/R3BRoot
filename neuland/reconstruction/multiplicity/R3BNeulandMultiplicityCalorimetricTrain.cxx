#include "R3BNeulandMultiplicityCalorimetricTrain.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRuntimeDb.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
//#include "Math/GeneticMinimizer.h"
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

R3BNeulandMultiplicityCalorimetricTrain::R3BNeulandMultiplicityCalorimetricTrain(TString clusters,
                                                                                 TString tracks,
                                                                                 TString phits)
    : FairTask("R3BNeulandMultiplicityCalorimetricTrain")
    , fClusters(std::move(clusters))
    , fTracks(std::move(tracks))
    , fPHits(std::move(phits))
    , fPar(nullptr)
    , fUseHits(false)
    , fEdepOpt({ 200, 25, 50, 1500 })
    , fEdepOffOpt({ 5, 1, 0, 250 })
    , fNclusterOpt({ 10, 5, 5, 50 })
    , fNclusterOffOpt({ 2, 1, 0, 10 })
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
    fPHits.Init();

    // Output Parameter Container
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityCalorimetricTrain:Init: No FairRootManager";
        return kFATAL;
    }

    auto rtdb = FairRuntimeDb::instance();
    if (rtdb == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityCalorimetricTrain::Init: No FairRuntimeDb!";
        return kFATAL;
    }
    fPar = (R3BNeulandMultiplicityCalorimetricPar*)rtdb->getContainer("R3BNeulandMultiplicityCalorimetricPar");
    if (fPar == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityCalorimetricTrain::Init: No R3BNeulandMultiplicityCalorimetricPar!";
        return kFATAL;
    }
    // FIXME: FairRuntimeDB needs to be forced to load the Data from the second file with Run Id 1
    // rtdb->initContainers(rtdb->getCurrentRun()->getRunId(), 1);

    return kSUCCESS;
}

void R3BNeulandMultiplicityCalorimetricTrain::Exec(Option_t*)
{
    const int nPN = fUseHits ? fPHits.Retrieve().size() : fTracks.Retrieve().size();

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
    ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Genetic");

    ROOT::Math::Functor f([&](const double* d) { return WastedEfficiency(d); }, 4);
    min->SetFunction(f);

    min->SetLimitedVariable(0, "edep", fEdepOpt.at(0), fEdepOpt.at(1), fEdepOpt.at(2), fEdepOpt.at(3));
    min->SetLimitedVariable(1, "edepoff", fEdepOffOpt.at(0), fEdepOffOpt.at(1), fEdepOffOpt.at(2), fEdepOffOpt.at(3));
    min->SetLimitedVariable(
        2, "ncluster", fNclusterOpt.at(0), fNclusterOpt.at(1), fNclusterOpt.at(2), fNclusterOpt.at(3));
    min->SetLimitedVariable(
        3, "nclusteroff", fNclusterOffOpt.at(0), fNclusterOffOpt.at(1), fNclusterOffOpt.at(2), fNclusterOffOpt.at(3));

    min->Minimize();

    LOG(info) << "R3BNeulandMultiplicityCalorimetricTrain::Optimize done!";
}

TCutG* R3BNeulandMultiplicityCalorimetricTrain::GetCut(const unsigned int nNeutrons,
                                                       const double edep,
                                                       const double edepoff,
                                                       const double ncluster,
                                                       const double nclusteroff)
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
        cut->SetPoint(0, -1, ncluster * (nNeutrons - 1) + nclusteroff);
        cut->SetPoint(1, edep * (nNeutrons - 1) + edepoff, -1);
    }

    // nmax: p2 (inf, -1) p3 (-1, inf)
    if (nNeutrons == fHists.rbegin()->first)
    {
        cut->SetPoint(2, 100000, -1);
        cut->SetPoint(3, -1, 100000);
    }
    else
    {
        cut->SetPoint(2, edep * nNeutrons + edepoff, -1);
        cut->SetPoint(3, -1, ncluster * nNeutrons + nclusteroff);
    }

    return cut;
}

double R3BNeulandMultiplicityCalorimetricTrain::WastedEfficiency(const double* d)
{
    double edep = d[0];
    double edepoff = d[1];
    double ncluster = d[2];
    double nclusteroff = d[3];

    double wasted_efficiency = 0;
    for (auto& nh : fHists)
    {
        const unsigned int nNeutrons = nh.first;
        const double efficiency =
            ((double)GetCut(nNeutrons, edep, edepoff, ncluster, nclusteroff)->IntegralHist(nh.second) /
             (double)nh.second->GetEntries());
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
        fHists[i] = new TH2D(name, name, 150, 0, 5000, 50, 0, 100);
        fHists.at(i)->GetXaxis()->SetTitle("Total Energy [MeV]");
        fHists.at(i)->GetYaxis()->SetTitle("Number of Clusters");
    }
    return fHists.at(i);
}
