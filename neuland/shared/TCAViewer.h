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

#ifndef TCAVIEWER
#define TCAVIEWER

#include "FairRootManager.h"
#include "TClonesArray.h"
#include <gsl/gsl>
#include <string_view>
#include <utility>

namespace TCAViewer
{
    enum Mode
    {
        in,
        out
    };

    template <typename Datatype>
    class TCAOutput_SafeGuard;

    template <typename DataType, Mode mode = in>
    class Pointers
    {
      public:
        friend TCAOutput_SafeGuard<DataType>;
        explicit Pointers(std::string branchName = "")
            : mBranchName{ std::move(branchName) }
        {
        }

        void SetBranch(std::string branchName) { mBranchName = std::move(branchName); }

        auto GetBranchName() -> std::string& { return mBranchName; }

        void Init(FairRootManager* rootMan = FairRootManager::Instance())
        {
            if (rootMan == nullptr)
            {
                LOG(fatal) << "TCAViewer: No FairRootManager for" << mBranchName;
            }

            if (mBranchName.empty())
            {
                LOG(fatal) << "TCAViewer: cannot init without branch name specified.";
            }

            mTCA = (mode == in) ? InputInit(rootMan) : OutputInit(rootMan);
        }

        // [[nodiscard]] auto GetData() -> gsl::span<DataType*>
        [[nodiscard]] auto Get() -> decltype(auto)
        {
            if constexpr (mode == in)
            {
                return InputGet();
            }
            else if constexpr (mode == out)
            {
                return OutputGet();
            }
        }

        auto GetTCA() -> TClonesArray* { return mTCA; }

      private:
        void DisableGuard() { mSafeGuard = false; }
        void EnableGuard() { mSafeGuard = true; }

        void Clear()
        {
            if (mTCA == nullptr)
            {
                LOG(error) << "TCAViewer: Cannot reset non-existed TClonesArray!";
            }
            mTCA->Clear("C");
        }

        auto InputInit(FairRootManager* rootMan) -> TClonesArray*
        {
            auto* tTCA = dynamic_cast<TClonesArray*>(rootMan->GetObject(mBranchName.data()));
            if (tTCA == nullptr)
            {
                LOG(fatal) << "TCAViewer: No TClonesArray called " << mBranchName
                           << "could be obtained from the FairRootManager!";
            }
            return tTCA;
        }

        auto OutputInit(FairRootManager* rootMan) -> TClonesArray*
        {
            mTCA_out = std::make_unique<TClonesArray>(DataType::Class_Name());
            rootMan->Register(mBranchName.data(), "", mTCA_out.get(), kTRUE);
            return mTCA_out.get();
        }

        [[nodiscard]] auto InputGet() -> gsl::span<DataType*>
        {
            static_assert(mode == in, "TCAViewer::Data cannot be called in output mode!");
            if (mTCA == nullptr)
            {
                LOG(warn) << "TCAViewer: get data from a non-existed TClonesArray!";
            }
            auto is_sucess = (mFirstAddr == nullptr) ? SetFirstAddr() : true;
            return is_sucess ? gsl::span<DataType*>(mFirstAddr, mTCA->GetEntriesFast()) : gsl::span<DataType*>();
        }

        [[nodiscard]] auto OutputGet() -> std::unique_ptr<TCAOutput_SafeGuard<DataType>>
        {
            if (mSafeGuard)
            {
                LOG(fatal) << "TCAViewer: Get() function can only be called once in the scope. ";
            }
            else
            {
                Clear();
                EnableGuard();
            }
            return std::make_unique<TCAOutput_SafeGuard<DataType>>(this);
        }

        auto SetFirstAddr() -> bool
        {
            auto is_success = false;
            if (mTCA != nullptr && mTCA->Capacity() != 0)
            {
                if (dynamic_cast<DataType*>((*mTCA)[0]) == nullptr)
                {
                    LOG(fatal) << "cannot convert the element in " << mBranchName << " to the type "
                               << DataType::Class_Name();
                }
                else
                {
                    mFirstAddr = reinterpret_cast<DataType**>(&(*mTCA)[0]);
                    is_success = true;
                }
            }
            return is_success;
        }

        template <typename... Args>
        void Emplace_back(Args&&... args)
        {
            static_assert(mode == out, "TCAViewer::Emplace_back cannot be called in input mode!");
            if (mTCA == nullptr)
            {
                LOG(error) << "TCAViewer: Cannot emplace back a non-existed TClonesArray!";
            }
            new ((*mTCA_out)[mTCA_out->GetEntries()]) DataType(std::forward<Args>(args)...);
        }

        std::string mBranchName;
        DataType** mFirstAddr = nullptr;
        TClonesArray* mTCA = nullptr;
        bool mSafeGuard = false;
        std::unique_ptr<TClonesArray> mTCA_out; // owning
    };

    template <typename Datatype>
    class TCAOutput_SafeGuard
    {
      public:
        using Pointers_out = Pointers<Datatype, out>;
        explicit TCAOutput_SafeGuard(Pointers_out* pointers)
            : mPointers{ pointers }
        {
        }

        ~TCAOutput_SafeGuard() { mPointers->DisableGuard(); }
        TCAOutput_SafeGuard(const TCAOutput_SafeGuard&) = delete;
        TCAOutput_SafeGuard(TCAOutput_SafeGuard&&) = delete;
        auto operator=(const TCAOutput_SafeGuard&) -> TCAOutput_SafeGuard& = delete;
        auto operator=(TCAOutput_SafeGuard &&) -> TCAOutput_SafeGuard& = delete;

        template <typename... Args>
        void Emplace_back(Args&&... args)
        {
            mPointers->Emplace_back(std::forward<Args>(args)...);
        }

      private:
        Pointers_out* mPointers = nullptr; // non-owning
    };
} // namespace TCAViewer
#endif
