#include "R3BNeulandNeutronReconstructionEvaluation.h"

#include <iostream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <utility>
#include <functional>

#include "TClonesArray.h"
#include "TH1D.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRtdbRun.h"
#include "FairLogger.h"
#include "FairMCPoint.h"

#include "R3BNeulandNeutron.h"

const Double_t massNeutron = 939.565; // MeV/c²

Double_t Distance(const R3BNeulandNeutron& nn, const FairMCPoint& mc)
{
    TVector3 v;
    mc.Position(v);
    v -= nn.GetPosition();
    return std::sqrt(v.Dot(v));
}

Double_t Score(const std::vector<std::pair<R3BNeulandNeutron, FairMCPoint>>& combination)
{
    return std::accumulate(combination.begin(),
                           combination.end(),
                           0.,
                           [](const Double_t sum, const std::pair<R3BNeulandNeutron, FairMCPoint>& pair)
                           {
                               return sum + Distance(pair.first, pair.second);
                           });
}

template <typename T, typename U>
std::vector<std::vector<std::pair<T, U>>> GetAllCombinations(std::vector<T> ts /* Copy! */,
                                                             std::vector<U> us /* Copy! */,
                                                             std::function<bool(const U&, const U&)> comparator)
{
    std::vector<std::vector<std::pair<T, U>>> out;

    // Bring both inputs up to the same length
    if (ts.size() < us.size())
    {
        ts.resize(us.size());
    }
    if (us.size() < ts.size())
    {
        us.resize(ts.size());
    }

    std::sort(us.begin(), us.end(), comparator);
    do
    {
        std::vector<std::pair<T, U>> tmp;
        for (size_t i = 0; i < ts.size(); i++)
        {
            tmp.push_back({ ts.at(i), us.at(i) });
        }
        out.push_back(std::move(tmp));
    } while (std::next_permutation(us.begin(), us.end(), comparator));

    return out;
}

R3BNeulandNeutronReconstructionEvaluation::R3BNeulandNeutronReconstructionEvaluation()
    : FairTask("R3B NeuLAND NeulandCluster Monitor")
{
    LOG(INFO) << "Using R3B Neuland Neutron Reconstruction" << FairLogger::endl;
}

R3BNeulandNeutronReconstructionEvaluation::~R3BNeulandNeutronReconstructionEvaluation() {}

InitStatus R3BNeulandNeutronReconstructionEvaluation::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstructionEvaluation::Init: No FairRootManager" << FairLogger::endl;
        return kFATAL;
    }

    // Set Input: TClonesArray of R3BNeulandNeutrons
    if ((TClonesArray*)ioman->GetObject("NeulandNeutrons") == nullptr)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstructionEvaluation::Init No NeulandNeutrons!" << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject("NeulandNeutrons"))->GetClass()->GetName())
             .EqualTo("R3BNeulandNeutron"))
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstructionEvaluation::Init Branch NeulandNeutrons does not contain "
                      "R3BNeulandNeutrons!" << FairLogger::endl;
        return kFATAL;
    }
    fReconstructedNeutrons = (TClonesArray*)ioman->GetObject("NeulandNeutrons");

    // Set Input: TClonesArray of FairMCPoints
    if ((TClonesArray*)ioman->GetObject("NeulandPrimaryNeutronInteractionPoints") == nullptr)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstructionEvaluation::Init No NeulandPrimaryNeutronInteractionPoints!"
                   << FairLogger::endl;
        return kFATAL;
    }
    if (!TString(((TClonesArray*)ioman->GetObject("NeulandPrimaryNeutronInteractionPoints"))->GetClass()->GetName())
             .EqualTo("FairMCPoint"))
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstructionEvaluation::Init Branch NeulandPrimaryNeutronInteractionPoints "
                      "does not contain FairMCPoints!" << FairLogger::endl;
        return kFATAL;
    }
    fPrimaryNeutronInteractionPoints = (TClonesArray*)ioman->GetObject("NeulandPrimaryNeutronInteractionPoints");

    fhScore = new TH1D("fhScore", "Neuland Neutron Reconstruction Score (lower is better)", 5000, 0, 5000);
    fhCount = new TH1D("fhCount", "Number of reacted primary Neutrons - Number of reconstructed Neutrons", 11, -5, 5);
    fhEdiff = new TH1D("fhEdiff", "Energy of primary Neutron - Energy of reconstructed Neutron", 2001, -1000, 1000);

    return kSUCCESS;
}

void R3BNeulandNeutronReconstructionEvaluation::Exec(Option_t*)
{
    const UInt_t nReconstructedNeutrons = fReconstructedNeutrons->GetEntries();
    std::vector<R3BNeulandNeutron> neutrons;
    neutrons.reserve(nReconstructedNeutrons);
    for (UInt_t i = 0; i < nReconstructedNeutrons; i++)
    {
        neutrons.push_back(*((R3BNeulandNeutron*)fReconstructedNeutrons->At(i)));
    }

    const UInt_t nPNIPS = fPrimaryNeutronInteractionPoints->GetEntries();
    std::vector<FairMCPoint> pnips;
    pnips.reserve(nReconstructedNeutrons);
    for (UInt_t i = 0; i < nPNIPS; i++)
    {
        pnips.push_back(*((FairMCPoint*)fPrimaryNeutronInteractionPoints->At(i)));
    }

    fhCount->Fill(nPNIPS - nReconstructedNeutrons);

    // Find the lowes score by brute force creating all combinations of MC-known and reconstructed neutron hits.
    // Score is the sum of distances. Note that detecting a wrong amount of neutrons will create a huge score
    // WiP. Slow because of all the copying
    const auto allCombinations =
        GetAllCombinations<R3BNeulandNeutron, FairMCPoint>(neutrons,
                                                           pnips,
                                                           [](const FairMCPoint& a, const FairMCPoint& b)
                                                           {
                                                               return a.GetTime() < b.GetTime();
                                                           });

    const auto bestCombination = *std::min_element(allCombinations.begin(),
                                                   allCombinations.end(),
                                                   [](const std::vector<std::pair<R3BNeulandNeutron, FairMCPoint>>& a,
                                                      const std::vector<std::pair<R3BNeulandNeutron, FairMCPoint>>& b)
                                                   {
                                                       return Score(a) < Score(b);
                                                   });

    const Double_t minScore = Score(bestCombination);
    fhScore->Fill(minScore);

    for (const auto& pair : bestCombination)
    {
        if (pair.second.GetEnergyLoss() > 0 || pair.first.GetE() > 0)
        {
            // TODO: Only for massNeutron
            if (pair.second.GetEnergyLoss() > 0)
            {
                fhEdiff->Fill(pair.second.GetEnergyLoss() * 1000 - massNeutron - pair.first.GetE());
            }
            else
            {
                fhEdiff->Fill(-pair.first.GetE());
            }
        }
    }
}

void R3BNeulandNeutronReconstructionEvaluation::Finish()
{
    fhCount->Write();
    fhScore->Write();
    fhEdiff->Write();
}

ClassImp(R3BNeulandNeutronReconstructionEvaluation)