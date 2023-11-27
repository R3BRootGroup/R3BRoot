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
#include <FairLogger.h>
#include <R3BValueError.h>
#include <TFile.h>
#include <type_traits>
#include <utility>

class TF1;
class TH1;
class TTree;

namespace R3B
{
    // -------------------------------------------------------------------------
    // root_owned:
    // ROOT types owned by ROOT: TH1, TF1;
    template <typename... Types>
    struct TypeCollection
    {
    };

    using RootTypes = TypeCollection<TF1, TH1, TTree>;

    template <typename Type, typename>
    inline constexpr bool is_based_on = false;

    template <typename Type, typename... BaseTypes>
    inline constexpr bool is_based_on<Type, TypeCollection<BaseTypes...>> = (std::is_base_of_v<BaseTypes, Type> || ...);

    template <typename Type>
    inline constexpr bool is_root_owned = is_based_on<Type, RootTypes>;

    template <typename RootType, typename... Args>
    inline auto root_owned(Args&&... args)
    {
        static_assert(is_root_owned<RootType>, "root_owned: such type cannot be owned by ROOT!");
        return new RootType(std::forward<Args>(args)...); // NOLINT
    }

    // -------------------------------------------------------------------------
    // make owned historgam

    template <typename Hist, typename... Args>
    inline auto make_hist(Args&&... args)
    {
        static_assert(std::is_base_of_v<TH1, Hist>, "make_hist: not a histogram type!");
        auto hist = std::make_unique<Hist>(std::forward<Args>(args)...);
        hist->SetDirectory(nullptr);
        return hist;
    }

    // -------------------------------------------------------------------------
    // make_rootfile:
    struct TFileDeleter
    {
        void operator()(TFile* rootfile)
        {
            LOG(debug2) << "Closing file " << rootfile->GetName();
            rootfile->Close();
            delete rootfile; // NOLINT
        }
    };

    using unique_rootfile = std::unique_ptr<TFile, TFileDeleter>;

    // make a root file that closes automatically
    template <typename... Args>
    inline auto make_rootfile(Args&&... args)
    {
        return unique_rootfile{ new TFile(std::forward<Args>(args)...) };
    }

    // -------------------------------------------------------------------------
    // sides enum class:
    // clang-format off
    // Get the length of a C array:
    template <typename DataType, std::size_t size>
    constexpr std::size_t GetSize(const DataType (&/*unused*/)[size]) // NOLINT
    {
        return size;
    }
    // clang-format on

    // -------------------------------------------------------------------------
    // side enums:
    enum class Side : bool
    {
        left,
        right
    };

    constexpr auto toIndex(Side side) -> size_t
    {
        if (side == Side::left)
        {
            return 0;
        }
        return 1;
    }

    constexpr auto IndexToSide(size_t index) -> Side
    {
        if (index == 0)
        {
            return Side::left;
        }
        return Side::right;
    }
    // -------------------------------------------------------------------------
    // left and right pair
    template <typename DataType>
    class LRPair
    {
      public:
        LRPair() = default;
        LRPair(const DataType& left, const DataType& right)
            : data_{ std::make_pair(left, right) }
            , is_valid(true)
        {
        }

        auto& left() { return data_.first; }
        auto& right() { return data_.second; }
        auto& left() const { return data_.first; }
        auto& right() const { return data_.second; }
        auto get(Side side) const -> const auto& { return (side == Side::left) ? data_.first : data_.second; }
        auto get(Side side) -> auto& { return (side == Side::left) ? data_.first : data_.second; }

      private:
        std::pair<DataType, DataType> data_;
        bool is_valid = false;

      public:
        ClassDefNV(LRPair, 1);
    };

    // -------------------------------------------------------------------------
    // fast exponential function:
    static const uint8_t DEFAULT_ITERATION = 8U;
    template <uint8_t iterations = DEFAULT_ITERATION>
    auto FastExp(const float val) -> float
    {
        auto exp = 1.F + val / (1U << iterations);
        for (auto i = 0; i < iterations; ++i)
        {
            exp *= exp;
        }
        return exp;
    }
} // namespace R3B
