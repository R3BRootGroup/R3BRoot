#include "R3BNeulandNeutronsKeras.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "TPython.h"
#include <TClonesArray.h>
#include <iostream>
#include <utility>

R3BNeulandNeutronsKeras::R3BNeulandNeutronsKeras(TString model,
                                                 TString scaler,
                                                 TString inputMult,
                                                 TString inputCluster,
                                                 TString output)
    : FairTask("R3BNeulandNeutronsKeras")
    , fInputMult(std::move(inputMult))
    , fMultiplicity(nullptr)
    , fInputCluster(std::move(inputCluster))
    , fClusters(nullptr)
    , fNeutrons(std::move(output))
    , fMinProb(0.1)
{
    // Warning: The python instance is shared. Here use prefix keras_ for all variables
    // Report python version
    TPython::Exec("import sys; v = sys.version.replace('\\n', '')");
    TPython::Exec("print(f'R3BNeulandNeutronsKeras running TPython with Python version {v}')");

    // Load Keras model, report keras and tensorflow version
    TPython::Exec(
        "import numpy as np; import tensorflow; from tensorflow import keras; keras_model = keras.models.load_model('" +
        model + "')");
    TPython::Exec("print(f'... with tensorflow {tensorflow.__version__} and keras {keras.__version__}')");

    // Load scaler
    TPython::Exec("import joblib; keras_scaler = joblib.load('" + scaler + "')");

    // Prepare cluster container exchange
    TPython::Exec("import ROOT; keras_tca = ROOT.TClonesArray(\"R3BNeulandCluster\")");
}

InitStatus R3BNeulandNeutronsKeras::Init()
{
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        LOG(fatal) << "R3BNeulandNeutronsKeras: No FairRootManager";
        return kFATAL;
    }

    fMultiplicity = ioman->InitObjectAs<const R3BNeulandMultiplicity*>(fInputMult);
    if (fMultiplicity == nullptr)
    {
        throw std::runtime_error(("R3BNeulandNeutronsKeras: R3BNeulandMultiplicity " + fInputMult +
                                  " could not be provided by the FairRootManager")
                                     .Data());
    }

    fClusters = (TClonesArray*)ioman->GetObject(fInputCluster);
    if (fClusters != nullptr && !TString(fClusters->GetClass()->GetName()).EqualTo("R3BNeulandCluster"))
    {
        throw std::runtime_error(("R3BNeulandNeutronsKeras: TClonesArray " + fInputCluster +
                                  " does not contain elements of type R3BNeulandCluster")
                                     .Data());
    }

    fNeutrons.Init();
    return kSUCCESS;
}

void R3BNeulandNeutronsKeras::Exec(Option_t*)
{
    fNeutrons.Reset();

    if (fClusters->GetEntries() == 0)
    {
        return;
    }

    // Let Python know about the new clusters
    TPython::Bind(fClusters, "keras_tca");
    TPython::Exec("keras_data = np.array([[cluster.GetT(), cluster.GetE(), cluster.GetSize(), cluster.GetEToF(), "
                  "cluster.GetEnergyMoment(), (cluster.GetLastHit().GetT() - cluster.GetFirstHit().GetT()),  "
                  "cluster.GetMaxEnergyHit().GetE(), cluster.GetPosition().X(), cluster.GetPosition().Y(), "
                  "cluster.GetPosition().Z()] for cluster in keras_tca])");
    // TPython::Exec("print(keras_data[0])");
    TPython::Exec("keras_data_scaled = keras_scaler.transform(keras_data)");
    // TPython::Exec("print(keras_data_scaled[0])");

    // Use model to predict probabilities - **SLOW**
    TPython::Exec("keras_results = keras_model.predict(keras_data_scaled)[:, 1]");
    // TPython::Exec("print(keras_results)");

    // Make a new container with scored clusters
    std::vector<ClusterWithProba> cwps;
    const int nClusters = fClusters->GetEntries();
    cwps.reserve(nClusters);
    for (int i = 0; i < nClusters; i++)
    {
        cwps.emplace_back(ClusterWithProba{ (R3BNeulandCluster*)fClusters->At(i),
                                            (Double_t)TPython::Eval(TString::Format("keras_results[%d]", i)) });
    }

    // Sort scored clusters, high probability first
    std::sort(cwps.begin(), cwps.end(), std::greater<ClusterWithProba>());

    // Log sorted results
    if (FairLogger::GetLogger()->IsLogNeeded(fair::Severity::debug))
    {
        LOG(debug) << "R3BNeulandNeutronsKeras::Exec";
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

ClassImp(R3BNeulandNeutronsKeras)
