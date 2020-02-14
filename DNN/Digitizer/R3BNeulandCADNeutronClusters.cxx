#include "R3BNeulandCADNeutronClusters.h"
#include "TH1D.h"
#include <algorithm>

R3BNeulandCADNeutronClusters::R3BNeulandCADNeutronClusters(TString input,
                                                     TString outputPrimary,
                                                     TString outputSecondary,
                                                     Double_t maxDist)
    : FairTask("R3B Neuland Neutron Cluster Finder")
    , fMaxDist(maxDist)
    , fPrimaryNeutronInteractionPoints("NeulandPrimaryNeutronInteractionPoints")
    , fClusters(input)
    , fPrimaryClusters(outputPrimary)
    , fPrimaryClustersMultiple(outputPrimary + "Multiple")
    , fPrimaryClustersSO(outputPrimary + "SO")
    , fPrimaryClustersMO(outputPrimary + "MO")
    , fSecondaryClusters(outputSecondary)
    , OutputNameTag("")
{
}

InitStatus R3BNeulandCADNeutronClusters::Init()
{
    fhNeutronClusterDistance = new TH1D("fhNeutronClusterDistance"+OutputNameTag, "fhNeutronClusterDistance", 1000, 0, 100);
    fhNeutronClusterUsage = new TH1D("fhNeutronClusterUsage"+OutputNameTag, "fhNeutronClusterUsage", 10, 0, 10);
    fhUnmatchedClusters = new TH1D("fhUnmatchedClusters"+OutputNameTag, "fhUnmatchedClusters", 10, 0, 10);
    
    fPrimaryNeutronInteractionPoints.SetTag(""); // Because this one is MC, so there is only one!
    fPrimaryNeutronInteractionPoints.Init();
    fClusters.SetTag(OutputNameTag);
    fClusters.Init();
    fPrimaryClusters.SetTag(OutputNameTag);
    fPrimaryClusters.Init();
    fPrimaryClustersMultiple.SetTag(OutputNameTag);
    fPrimaryClustersMultiple.Init();
    fPrimaryClustersSO.SetTag(OutputNameTag);
    fPrimaryClustersSO.Init();
    fPrimaryClustersMO.SetTag(OutputNameTag);
    fPrimaryClustersMO.Init();
    fSecondaryClusters.SetTag(OutputNameTag);
    fSecondaryClusters.Init();

    return kSUCCESS;
}

void R3BNeulandCADNeutronClusters::Exec(Option_t*)
{
    const auto clusters = fClusters.Retrieve();
    const auto npnips = fPrimaryNeutronInteractionPoints.Retrieve();

    std::map<const R3BNeulandCADCluster*, int> marks;
    for (const auto& cluster : clusters)
    {
        marks[cluster] = 0;
    }

    TVector3 npnippos;
    for (const FairMCPoint* npnip : npnips)
    {
        npnip->Position(npnippos);

        // Search for the cluster with the smallest distance to the PNIP
        auto min = std::min_element(
            clusters.begin(), clusters.end(), [&](const R3BNeulandCADCluster* a, const R3BNeulandCADCluster* b) {
                return (a->GetPosition() - npnippos).Mag() < (b->GetPosition() - npnippos).Mag();
            });
        if (min != clusters.end())
        {
            const Double_t minDist = ((*min)->GetPosition() - npnippos).Mag();
            fhNeutronClusterDistance->Fill(minDist);
            if (minDist < fMaxDist)
            {
                // Cluster for this PNIP found
                marks.at(*min)++;
                fhUnmatchedClusters->Fill(0);
            }
            else
            {
                // Cluster is too far away
                fhUnmatchedClusters->Fill(1);
            }
        }
        else
        {
            // No clusters in event ;-(
            fhUnmatchedClusters->Fill(2);
        }
    }

    std::vector<R3BNeulandCADCluster> primaryClusters;
    std::vector<R3BNeulandCADCluster> primaryClustersMultiple;
    std::vector<R3BNeulandCADCluster> primaryClustersSO;
    std::vector<R3BNeulandCADCluster> primaryClustersMO;
    std::vector<R3BNeulandCADCluster> secondaryClusters;

    for (const auto& cb : marks)
    {
        fhNeutronClusterUsage->Fill(cb.second);

        if (cb.second == 0)
        {
            // It is not a primary cluster
            secondaryClusters.emplace_back(*cb.first);
        }
        else
        {
            // It corresponds to one or more primary clusters
            primaryClusters.emplace_back(*cb.first);

            for (int i = 0; i < cb.second; i++)
            {
                primaryClustersMultiple.emplace_back(*cb.first);
            }

            if (cb.second > 1)
            {
                primaryClustersMO.emplace_back(*cb.first);
            }
            else
            {
                primaryClustersSO.emplace_back(*cb.first);
            }
        }
    }

    fPrimaryClusters.Store(primaryClusters);
    fPrimaryClustersMultiple.Store(primaryClustersMultiple);
    fPrimaryClustersSO.Store(primaryClustersSO);
    fPrimaryClustersMO.Store(primaryClustersMO);
    fSecondaryClusters.Store(secondaryClusters);
}

void R3BNeulandCADNeutronClusters::Finish()
{
    fhNeutronClusterDistance->Write();
    fhNeutronClusterUsage->Write();
    fhUnmatchedClusters->Write();
}

ClassImp(R3BNeulandCADNeutronClusters)
