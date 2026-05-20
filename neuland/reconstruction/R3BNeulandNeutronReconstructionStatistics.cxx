/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BNeulandNeutronReconstructionStatistics.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"
#include <FairRootManager.h>
#include <FairTask.h>
#include <RtypesCore.h>
#include <TFile.h>
#include <TH1.h>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <limits>
#include <numeric>
#include <string_view>
#include <type_traits>
#include <vector>

constexpr auto DEFAULT_MULTI = 100;

namespace
{
    template <class T>
    auto almost_equal(T left, T right, int ulp) -> std::enable_if_t<!std::numeric_limits<T>::is_integer, bool>
    {
        // the machine epsilon has to be scaled to the magnitude of the values used
        // and multiplied by the desired precision in ULPs (units in the last place)
        return std::abs(left - right) <= std::numeric_limits<T>::epsilon() * std::abs(left + right) * ulp
               // unless the result is subnormal
               || std::abs(left - right) < std::numeric_limits<T>::min();
    }

} // namespace

R3BNeulandNeutronReconstructionStatistics::R3BNeulandNeutronReconstructionStatistics(std::string_view primary,
                                                                                     std::string_view secondary,
                                                                                     std::string_view predicted)
    : primary_clusters_{ primary }
    , secondary_clusters_(secondary)
    , predicted_neutrons_(predicted)
    , predicted_name_(predicted)
    , multiplicities_{ std::vector<int>(DEFAULT_MULTI) }
{
}

auto R3BNeulandNeutronReconstructionStatistics::Init() -> InitStatus
{
    primary_clusters_.init();
    secondary_clusters_.init();
    predicted_neutrons_.init();

    constexpr auto HIST_BIN_SIZE10 = 10;
    constexpr auto HIST_BIN_SIZE30 = 10;
    constexpr auto HIST_BIN_SIZE_F1 = 11;
    constexpr auto HIST_MAX_BIN_F1 = 1.1;

    TH1::AddDirectory(kFALSE);
    hist_true_positive_ = data_monitor_.add_hist<TH1D>("fhTP", "True Positive", HIST_BIN_SIZE10, 0., HIST_BIN_SIZE10);
    hist_false_negative_ = data_monitor_.add_hist<TH1D>("fhFN", "False Negative", HIST_BIN_SIZE10, 0., HIST_BIN_SIZE10);
    hist_false_positive_ = data_monitor_.add_hist<TH1D>("fhFP", "False Positive", HIST_BIN_SIZE10, 0., HIST_BIN_SIZE10);
    hist_true_negative_ = data_monitor_.add_hist<TH1D>("fhTN", "True Negative", HIST_BIN_SIZE30, 0., HIST_BIN_SIZE30);
    hist_f1_value_ = data_monitor_.add_hist<TH1D>("fhF1", "F1 Value", HIST_BIN_SIZE_F1, 0., HIST_MAX_BIN_F1);

    return kSUCCESS;
}

void R3BNeulandNeutronReconstructionStatistics::Exec(Option_t* /*option*/)
{
    const auto& actualPositive = primary_clusters_.get();
    const auto& actualNegative = secondary_clusters_.get();
    const auto& predictedPositive = predicted_neutrons_.get();

    auto comp = [](const R3BNeulandNeutron& neutron, const R3BNeulandCluster& cluster)
    {
        return almost_equal(cluster.GetT(), neutron.GetT(), 2) &&
               almost_equal(cluster.GetPosition().X(), neutron.GetPosition().X(), 2) &&
               almost_equal(cluster.GetPosition().Y(), neutron.GetPosition().Y(), 2) &&
               almost_equal(cluster.GetPosition().Z(), neutron.GetPosition().Z(), 2);
    };

    multiplicities_.at(predictedPositive.size())++;

    auto truePositives = 0;
    for (const auto& cluster : actualPositive)
    {
        for (const auto& neutron : predictedPositive)
        {
            if (comp(neutron, cluster))
            {
                truePositives++;
                break;
            }
        }
    }
    const auto falseNegatives = static_cast<int>(actualPositive.size()) - truePositives;
    true_positive_ += truePositives;
    hist_true_positive_->Fill(truePositives);
    false_negative_ += falseNegatives;
    hist_false_negative_->Fill(falseNegatives);

    auto falsePositives = 0;
    for (const auto& cluster : actualNegative)
    {
        for (const auto& neutron : predictedPositive)
        {
            if (comp(neutron, cluster))
            {
                falsePositives++;
                break;
            }
        }
    }
    const auto trueNegatives = static_cast<int>(actualNegative.size()) - falsePositives;
    false_positive_ += falsePositives;
    hist_false_positive_->Fill(falsePositives);
    true_negative_ += trueNegatives;
    hist_true_negative_->Fill(trueNegatives);

    const auto precision = truePositives / static_cast<double>(truePositives + falsePositives);
    const auto recall = truePositives / static_cast<double>(truePositives + falseNegatives);

    const auto Fbeta = [&](double beta)
    {
        if (precision == 0. && recall == 0.)
        {
            return 0.;
        }
        return (1. + beta * beta) * (precision * recall) / ((beta * beta * precision) + recall);
    };
    hist_f1_value_->Fill(Fbeta(1.));
}

void R3BNeulandNeutronReconstructionStatistics::Finish()
{
    const auto dir_name = fmt::format("{}Statistics", predicted_name_);
    data_monitor_.save_to_sink(dir_name);

    const auto precision = true_positive_ / static_cast<double>(true_positive_ + false_positive_);
    const auto recall = true_positive_ / static_cast<double>(true_positive_ + false_negative_);
    auto Fbeta = [&](double beta)
    { return (1. + beta * beta) * (precision * recall) / ((beta * beta * precision) + recall); };
    const double accuracy = (true_positive_ + true_negative_) /
                            static_cast<double>(true_positive_ + true_negative_ + false_positive_ + false_negative_);

    LOGP(info, "PREC DATA");
    LOGP(info,
         "{}\tTruePositive \t{}\t{}",
         predicted_name_,
         true_positive_,
         true_positive_ / static_cast<double>(true_positive_ + false_negative_));
    LOGP(info,
         "{}\tFalsePositive \t{}\t{}",
         predicted_name_,
         false_positive_,
         false_positive_ / static_cast<double>(true_negative_ + false_positive_));
    LOGP(info,
         "{}\tFalseNegative \t{}\t{}",
         predicted_name_,
         false_negative_,
         false_negative_ / static_cast<double>(true_positive_ + false_negative_));
    LOGP(info,
         "{}\tTrueNegative \t{}\t{}",
         predicted_name_,
         true_negative_,
         true_negative_ / static_cast<double>(true_negative_ + false_positive_));

    LOGP(info, "{}\tAccuracy \t{}", predicted_name_, accuracy);
    LOGP(info, "{}\tPrecision \t{}", predicted_name_, precision);
    LOGP(info, "{}\tSensitivity \t{}", predicted_name_, recall);
    LOGP(info, "{}\tF1 \t{}", predicted_name_, Fbeta(1.));
    LOGP(info, "MULT DATA");

    const int sum = std::accumulate(multiplicities_.begin(), multiplicities_.end(), 0);
    for (unsigned int i = 0; i < multiplicities_.size(); i++)
    {
        LOGP(info, "{}\t{}\t{}", i, multiplicities_[i], multiplicities_[i] / static_cast<double>(sum));
    }
    LOGP(info, "END");
}
