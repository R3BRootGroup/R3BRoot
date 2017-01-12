#include "R3BNeulandNeutronReconstructionStatistics.h"
#include "FairLogger.h"

template <class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type almost_equal(T x, T y, int ulp)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
           // unless the result is subnormal
           || std::abs(x - y) < std::numeric_limits<T>::min();
}

R3BNeulandNeutronReconstructionStatistics::R3BNeulandNeutronReconstructionStatistics(const TString primary,
                                                                                     const TString secondary,
                                                                                     const TString predicted)
    : fPrimaryClusters(primary)
    , fSecondaryClusters(secondary)
    , fPredictedNeutrons(predicted)
    , fPredictedName(predicted)
    , fTP(nullptr)
    , fFP(nullptr)
    , fFN(nullptr)
    , fTN(nullptr)
{
}

InitStatus R3BNeulandNeutronReconstructionStatistics::Init()
{
    try
    {
        fPrimaryClusters.Init();
        fSecondaryClusters.Init();
        fPredictedNeutrons.Init();
    }
    catch (const std::exception& e)
    {
        LOG(FATAL) << "R3BNeulandNeutronReconstruction" << e.what() << FairLogger::endl;
    }

    TH1::AddDirectory(kFALSE);
    fTP = new TH1D("fTP", "True Positive", 10, 0., 10.);
    fFN = new TH1D("fFN", "False Negative", 10, 0., 10.);
    fFP = new TH1D("fFP", "False Positive", 10, 0., 10.);
    fTN = new TH1D("fTN", "True Negative", 30, 0., 30.);
    fF1 = new TH1D("fF1", "F1 Value", 11, 0., 1.1);

    return kSUCCESS;
}

void R3BNeulandNeutronReconstructionStatistics::Exec(Option_t*)
{
    const auto actualPositive = fPrimaryClusters.Retrieve();
    const auto actualNegative = fSecondaryClusters.Retrieve();
    const auto predictedPositive = fPredictedNeutrons.Retrieve();

    auto comp = [](const R3BNeulandNeutron* n, const R3BNeulandCluster* c) {
        return almost_equal(c->GetT(), n->GetT(), 2) && almost_equal(c->GetPosition().X(), n->GetPosition().X(), 2) &&
               almost_equal(c->GetPosition().Y(), n->GetPosition().Y(), 2) &&
               almost_equal(c->GetPosition().Z(), n->GetPosition().Z(), 2);
    };

    double truePositives = 0.;
    for (const auto& c : actualPositive)
    {
        for (const auto& n : predictedPositive)
        {
            if (comp(n, c))
            {
                truePositives++;
                break;
            }
        }
    }
    const double falseNegatives = (double)actualPositive.size() - truePositives;
    fTP->Fill(truePositives);
    fFN->Fill(falseNegatives);

    double falsePositives = 0.;
    for (const auto& c : actualNegative)
    {
        for (const auto& n : predictedPositive)
        {
            if (comp(n, c))
            {
                falsePositives++;
                break;
            }
        }
    }
    const double trueNegatives = (double)actualNegative.size() - falsePositives;
    fFP->Fill(falsePositives);
    fTN->Fill(trueNegatives);

    const double precision = truePositives / (truePositives + falsePositives);
    const double recall = truePositives / (truePositives + falseNegatives);

    auto Fbeta = [&](double beta) {
        if (precision == 0. && recall == 0.)
            return 0.;
        return (1. + beta * beta) * (precision * recall) / ((beta * beta * precision) + recall);
    };
    fF1->Fill(Fbeta(1.));
}

void R3BNeulandNeutronReconstructionStatistics::Finish()
{
    TDirectory* tmp = gDirectory;
    FairRootManager::Instance()->GetOutFile()->cd();

    gDirectory->mkdir(fPredictedName + "Statistics");
    gDirectory->cd(fPredictedName + "Statistics");

    fTP->Write();
    fFN->Write();
    fFP->Write();
    fTN->Write();
    fF1->Write();

    gDirectory = tmp;

    /*const double precision = (double)fTP / ((double)fTP + (double)fFP);
    const double recall = (double)fTP / ((double)fTP + (double)fFN);
    auto Fbeta = [&](double beta) {
        return (1. + beta * beta) * (precision * recall) / ((beta * beta * precision) + recall);
    };

    std::cout << fPredictedName << "\tTruePositive\t" << fTP << "\n"
              << fPredictedName << "\tFalsePositive\t" << fFP << "\n"
              << fPredictedName << "\tFalseNegative\t" << fFN << "\n"
              << fPredictedName << "\tTrueNegative\t" << fTN << "\n"
              << fPredictedName << "\tPrecision\t" << precision << "\n"
              << fPredictedName << "\tRecall\t" << recall << "\n"
              << fPredictedName << "\tF1\t" << Fbeta(1.) << "\n"
              << std::endl;*/
}
