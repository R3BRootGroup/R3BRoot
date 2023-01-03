#include "R3BNeulandNeutronsScikit.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "TPython.h"
#include <TClonesArray.h>
#include <iostream>
#include <utility>

R3BNeulandNeutronsScikit::R3BNeulandNeutronsScikit(TString model,
                                                   TString inputMult,
                                                   TString inputCluster,
                                                   TString output)
    : FairTask("R3BNeulandNeutronsScikit")
    , fInputMult(std::move(inputMult))
    , fMultiplicity(nullptr)
    , fInputCluster(std::move(inputCluster))
    , fClusters(nullptr)
    , fNeutrons(std::move(output))
    , fMinProb(0.1)
{
    TPython::Exec("import sys; v = sys.version.replace('\\n', '')");
    TPython::Exec("print(f'R3BNeulandNeutronsScikit running TPython with Python version {v}')");

    TPython::Exec("import numpy as np; import joblib; model = joblib.load('" + model + "')");
    // Note: ModuleNotFoundError: No module named 'sklearn.ensemble._forest' -> That means wrong python version!

    TPython::Exec("import ROOT; tca = ROOT.TClonesArray(\"R3BNeulandCluster\")");

    // Example to test model:
    // double prob = (Double_t)TPython::Eval("model.predict_proba([[63.421486, 1.582491, 1.0, 621.388550, 8.881784e-16,
    // 0.000000, 1.582491, -18.501257, -7.500000, 1522.5]])[0][1]");

    // Share single cluster between python and C++ **SLOW**
    // TPython::Exec("import ROOT");
    // TPython::Exec("cluster = ROOT.R3BNeulandCluster()");
    // R3BNeulandCluster* cluster = ...
    // TPython::Bind(cluster, "cluster");
    // TString fPredictor = "model.predict_proba([["
    //  "cluster.GetT(),\n"
    //  "cluster.GetE(),\n"
    //  "cluster.GetSize(),\n"
    //  "cluster.GetEToF(),\n"
    //  "cluster.GetEnergyMoment(),\n"
    //  "cluster.GetLastHit().GetT() - cluster.GetFirstHit().GetT(),\n"
    //  "cluster.GetMaxEnergyHit().GetE(),\n"
    //  "cluster.GetPosition().X(),\n"
    //  "cluster.GetPosition().Y(),\n"
    //  "cluster.GetPosition().Z(),\n"
    //  "]])[0][1]"
    // double prob = (Double_t)TPython::Eval(fPredictor);
}

InitStatus R3BNeulandNeutronsScikit::Init()
{
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "R3BNeulandNeutronsScikit: No FairRootManager";
        return kFATAL;
    }

    fMultiplicity = ioman->InitObjectAs<const R3BNeulandMultiplicity*>(fInputMult);
    if (fMultiplicity == nullptr)
    {
        throw std::runtime_error(("R3BNeulandNeutronsScikit: R3BNeulandMultiplicity " + fInputMult +
                                  " could not be provided by the FairRootManager")
                                     .Data());
    }

    fClusters = (TClonesArray*)ioman->GetObject(fInputCluster);
    if (fClusters != nullptr && !TString(fClusters->GetClass()->GetName()).EqualTo("R3BNeulandCluster"))
    {
        throw std::runtime_error(("R3BNeulandNeutronsScikit: TClonesArray " + fInputCluster +
                                  " does not contain elements of type R3BNeulandCluster")
                                     .Data());
    }

    fNeutrons.Init();
    return kSUCCESS;
}

void R3BNeulandNeutronsScikit::Exec(Option_t*)
{
    fNeutrons.Reset();

    if (fClusters->GetEntries() == 0)
    {
        return;
    }

    // Let Python know about the new clusters
    TPython::Bind(fClusters, "tca");
    TPython::Exec("data = np.array([[cluster.GetT(), cluster.GetE(), cluster.GetSize(), cluster.GetEToF(), "
                  "cluster.GetEnergyMoment(), (cluster.GetLastHit().GetT() - cluster.GetFirstHit().GetT()),  "
                  "cluster.GetMaxEnergyHit().GetE(), cluster.GetPosition().X(), cluster.GetPosition().Y(), "
                  "cluster.GetPosition().Z()] for cluster in tca])");

    // Use model to predict probabilities - **SLOW**
    TPython::Exec("results = model.predict_proba(data)[:, 1]");

    //// Make a new container with scored clusters
    std::vector<ClusterWithProba> cwps;
    const int nClusters = fClusters->GetEntries();
    cwps.reserve(nClusters);
    for (int i = 0; i < nClusters; i++)
    {
        cwps.emplace_back(ClusterWithProba{ (R3BNeulandCluster*)fClusters->At(i),
                                            (Double_t)TPython::Eval(TString::Format("results[%d]", i)) });
    }

    // Sort scored clusters, high probability first
    std::sort(cwps.begin(), cwps.end(), std::greater<ClusterWithProba>());

    // Log sorted results
    if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug))
    {
        LOG(debug) << "R3BNeulandNeutronsScikit::Exec";
        for (const auto& cwp : cwps)
        {
            LOG(debug) << cwp.c->GetPosition().X() << "\t" << cwp.p << "\t";
        }
    }

    // With the multiplicity from somewhere else, take the n clusters with the highest prob as neutrons
    const auto mult = fMultiplicity->GetMultiplicity();
    for (size_t n = 0; n < cwps.size() && n < mult; n++)
    {
        if (cwps.at(n).p > fMinProb)
        {
            fNeutrons.Insert(R3BNeulandNeutron(*(cwps.at(n).c)));
        }
    }
}

ClassImp(R3BNeulandNeutronsScikit)
