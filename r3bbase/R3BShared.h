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
#include <type_traits>
#include <utility>

class TF1;
class TH1;
class TTree;

namespace r3b
{
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
} // namespace r3b
