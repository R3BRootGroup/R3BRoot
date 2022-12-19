#include "R3BNeulandMultiplicityScikit.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "TPython.h"
#include <numeric>
#include <utility>

R3BNeulandMultiplicityScikit::R3BNeulandMultiplicityScikit(TString model, TString input, TString output)
    : FairTask("R3BNeulandMultiplicityScikit")
    , fClusters(std::move(input))
    , fMultiplicity(new R3BNeulandMultiplicity())
    , fOutputName(std::move(output))
{
    TPython::Exec("import sys; v = sys.version.replace('\\n', '')");
    TPython::Exec("print(f'R3BNeulandMultiplicityScikit running TPython with Python version {v}')");

    TPython::Exec("import numpy as np; import joblib; multmodel = joblib.load('" + model + "')");
    // Note: ModuleNotFoundError: No module named 'sklearn. ...' -> That means wrong python version!

    TPython::Exec("import ROOT; mult = ROOT.R3BNeulandMultiplicity()");
    TPython::Bind(fMultiplicity, "mult"); // Bind here to prevent 1st event missing
}

R3BNeulandMultiplicityScikit::~R3BNeulandMultiplicityScikit() { delete fMultiplicity; }

InitStatus R3BNeulandMultiplicityScikit::Init()
{
    // Input
    fClusters.Init();

    // Output
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "R3BNeulandMultiplicityScikit: No FairRootManager";
        return kFATAL;
    }
    ioman->RegisterAny(fOutputName, fMultiplicity, true);

    return kSUCCESS;
}

void R3BNeulandMultiplicityScikit::Exec(Option_t*)
{
    fMultiplicity->m.fill(0.);
    const auto clusters = fClusters.Retrieve();
    const int nClusters = clusters.size();

    if (nClusters == 0)
    {
        LOG(debug) << "R3BNeulandMultiplicityScikit::Exec 0 Clusters -> Mult 0";
        fMultiplicity->m[0] = 1.;
        return;
    }

    const int nHits = std::accumulate(
        clusters.cbegin(), clusters.cend(), 0, [](size_t s, const R3BNeulandCluster* c) { return s + c->GetSize(); });
    const int Edep = (int)std::accumulate(
        clusters.cbegin(), clusters.cend(), 0., [](Double_t s, const R3BNeulandCluster* c) { return s + c->GetE(); });

    // Use model to predict probabilities. Note: In the model tested here, no case "0" -> i + 1
    TPython::Exec(
        TString::Format("for i, p in enumerate(multmodel.predict_proba([[%d, %d, %d]])[0]):\n    mult.m[i + 1] = p",
                        nHits,
                        nClusters,
                        Edep));

    // Python -> C++ -> Autosave in branch
    TPython::Bind(fMultiplicity, "mult");

    // Log
    if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug))
    {
        TPython::Exec("print([p for p in mult.m])");
        LOG(debug) << "R3BNeulandMultiplicityScikit::Exec "
                   << std::accumulate(fMultiplicity->m.cbegin(),
                                      fMultiplicity->m.cend(),
                                      std::string(),
                                      [](std::string a, double b) { return std::move(a) + ", " + std::to_string(b); });
    }
}

ClassImp(R3BNeulandMultiplicityScikit)
