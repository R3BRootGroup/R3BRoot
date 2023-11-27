/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/
#pragma once

#include "R3BPaddleTamexMappedData2.h"
#include "R3BReader.h"
#include "ext_h101_raw_nnp_tamex.h"
#include <R3BDataMonitor.h>
#include <Rtypes.h>
#include <map>

class TClonesArray;

struct EXT_STR_h101_raw_nnp_tamex_t;
using EXT_STR_h101_raw_nnp_tamex = struct EXT_STR_h101_raw_nnp_tamex_t;
using EXT_STR_h101_raw_nnp_tamex_onion = struct EXT_STR_h101_raw_nnp_tamex_onion_t;
class ext_data_struct_info;

class R3BNeulandTamexReader2 : public R3BReader
{
  public:
    using InputDataType = std::remove_all_extents_t<decltype(EXT_STR_h101_raw_nnp_tamex_onion::NN_P)>;
    using MappedDataVector = R3BPaddleTamexMappedDataContainer;
    using TrigMappedDataVector = R3BPaddleTamexTrigDataContainer;

    enum class Errors
    {
        module_size,
        data_size,
        indices,
        divider
    };

    // Standard constructor
    R3BNeulandTamexReader2(EXT_STR_h101_raw_nnp_tamex_onion*, size_t);

    // rule of 5:
    ~R3BNeulandTamexReader2() override { print_error(); };
    R3BNeulandTamexReader2(const R3BNeulandTamexReader2&) = default;
    R3BNeulandTamexReader2(R3BNeulandTamexReader2&&) = delete;
    auto operator=(const R3BNeulandTamexReader2&) -> R3BNeulandTamexReader2& = default;
    auto operator=(R3BNeulandTamexReader2&&) -> R3BNeulandTamexReader2& = delete;

    // Read data from full event structure
    auto ReadSignals(EXT_STR_h101_raw_nnp_tamex_onion* inputData) -> bool;
    auto ReadTriggerSignals(EXT_STR_h101_raw_nnp_tamex_onion* inputData) -> bool;

    // Accessor to select online mode
    void SetOnline(bool is_online) { is_online_ = is_online; }

    // Set the maximum number of planes
    void SetMaxNbPlanes(uint max) { numPlanes_ = max; }

    // Accessor to skip trigger times
    void SetSkipTriggerTimes() { is_triggered_ = false; }

    void SetPrintErrorPerNums(size_t max_limit) { max_limit_ = max_limit; }

    template <typename Optn>
    void AddHistAction(std::unique_ptr<TH1> hist, Optn&& optn);

  private:
    bool is_online_ = false;   // Don't store data for online
    bool is_triggered_ = true; // whether Skip trigger times
    size_t counter_ = 0;
    size_t max_limit_ = 0;
    size_t fOffset = 0;                           // Data offset
    uint numPlanes_ = 0;                          // Number of planes
    EXT_STR_h101_raw_nnp_tamex_onion* inputData_; // Reader specific data structure from ucesb
    MappedDataVector mappedData_;                 // Output array
    TrigMappedDataVector mappedTrigData_;         // Output array, cards' trigger
    MappedDataVector* mappedDataPtr_ = nullptr;
    TrigMappedDataVector* mappedTrigDataPtr_ = nullptr;
    R3B::DataMonitor histograms_;
    std::map<std::string, std::function<void(const R3B::PaddleTamexMappedData&, TH1*)>> hist_actions_;
    std::map<Errors, uint> error_log_;

    auto R3BRead() -> bool override;
    // Reset
    void Reset() override;
    // Setup structure information
    auto Init(ext_data_struct_info* /*unused*/) -> bool override;
    void Close() override {}
    [[nodiscard]] auto MismappedItemRequired(std::string_view item_name) const -> bool override;

    void print_error();
    template <typename ViewType>
    auto extract_plane_signals(const ViewType& signalsPlane, int planeNum);
    void histogram_init();
    void histogram_action(const R3B::PaddleTamexMappedData& mappedData);
    [[nodiscard]] auto check_trigger_needed(std::string_view item_name) const -> bool;
    [[nodiscard]] auto check_bar_needed(std::string_view item_name) const -> bool;

  public:
    ClassDefOverride(R3BNeulandTamexReader2, 1);
};

template <typename Optn>
void R3BNeulandTamexReader2::AddHistAction(std::unique_ptr<TH1> hist, Optn&& optn)
{
    const auto* histName = hist->GetName();
    histograms_.add_hist(std::move(hist));
    hist_actions_.insert(std::make_pair(histName, std::forward<Optn>(optn)));
}
