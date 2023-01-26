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

#include "FairRun.h"
#include "TClonesArray.h"
#include <FairEventHeader.h>
#include <boost/core/span.hpp>
#include <string>
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

    template <typename DataType,
              Mode mode = in,
              typename = typename std::enable_if<std::is_base_of<TObject, DataType>::value>::type,
              typename = typename std::enable_if<sizeof(TObject*) == sizeof(DataType*)>::type>
    class Data
    {
      public:
        friend TCAOutput_SafeGuard<DataType>;
        explicit Data(std::string branchName = "")
            : mBranchName{ std::move(branchName) }
        {
        }

        void SetBranch(std::string branchName) { mBranchName = std::move(branchName); }

        auto GetBranchName() -> std::string& { return mBranchName; }

        void Init(FairRun* run = FairRun::Instance(), FairRootManager* rootMan = FairRootManager::Instance())
        {
            if (rootMan == nullptr)
            {
                LOG(fatal) << "TCAViewer: No FairRootManager for" << mBranchName;
            }

            if (run == nullptr)
            {
                LOG(fatal) << "TCAViewer: No FairRun for" << mBranchName;
            }

            if (mBranchName.empty())
            {
                LOG(fatal) << "TCAViewer: cannot init without branch name specified.";
            }

            eventHeader = run->GetEventHeader();

            mTCA = (mode == in) ? InputInit(rootMan) : OutputInit(rootMan);
        }

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

        auto CheckCapcitySize() -> bool
        {
            auto is_changed = true;
            auto tempSize = mTCA->Capacity();
            if (mContCapcitySize)
            {
                is_changed = mContCapcitySize != tempSize;
            }
            if (is_changed)
            {
                mContCapcitySize = tempSize;
            }
            return is_changed;
        }

      private:
        void DisableGuard() { mSafeGuard = false; }
        void EnableGuard() { mSafeGuard = true; }

        void Clear()
        {
            if (mTCA == nullptr)
            {
                LOG(error) << "TCAViewer<out>: Cannot reset non-existed TClonesArray!";
            }
            mTCA->Clear("C");
        }

        auto InputInit(FairRootManager* rootMan) -> TClonesArray*
        {
            auto* tTCA = dynamic_cast<TClonesArray*>(rootMan->GetObject(mBranchName.data()));
            if (tTCA == nullptr)
            {
                LOG(fatal) << "TCAViewer<in>: No TClonesArray called " << mBranchName
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

        [[nodiscard]] auto InputGet() -> decltype(auto)
        {
            if (mTCA == nullptr)
            {
                LOG(warn) << "TCAViewer<in>: get data from a non-existed TClonesArray!";
            }
            auto is_sucess = CheckCapcitySize() ? SetFirstAddr() : true;
            return is_sucess ? std::span<DataType* const>(mCont, mTCA->GetEntriesFast()) : std::span<DataType* const>();
        }

        [[nodiscard]] auto OutputGet() -> TCAOutput_SafeGuard<DataType>
        {
            if (mSafeGuard)
            {
                LOG(fatal) << "TCAViewer<out>: Get() function can only be called once in the scope. ";
            }
            else
            {
                // make sure this function get called every event
                LogMCEntryNumber();
                Clear();
            }
            return TCAOutput_SafeGuard<DataType>(this);
        }

        template <typename... Args>
        void Emplace_back(Args&&... args)
        {
            static_assert(mode == out, "TCAViewer<out>::Emplace_back cannot be called in input mode!");
            if (mTCA == nullptr)
            {
                LOG(error) << "TCAViewer<out>: Cannot emplace back a non-existed TClonesArray!";
            }
            new ((*mTCA_out)[mTCA_out->GetEntries()]) DataType(std::forward<Args>(args)...);
        }

        auto SetFirstAddr() -> bool
        {
            auto is_success = false;
            if (mTCA != nullptr && mTCA->Capacity() != 0)
            {
                if (dynamic_cast<DataType*>((*mTCA)[0]) == nullptr)
                {
                    LOG(fatal) << "TCAViewer<in>: Cannot convert the element in " << mBranchName << " to the type "
                               << DataType::Class_Name();
                }
                else
                {
                    mCont = reinterpret_cast<DataType**>(mTCA->GetObjectRef());
                    is_success = true;
                }
            }
            return is_success;
        }

        void LogMCEntryNumber()
        {
            auto tempEntryNumber = GetMCEntryNumber();
            if (mMCEntryNumber.has_value())
            {
                MCEntryNumDiagnose(tempEntryNumber);
            }
            mMCEntryNumber = tempEntryNumber;
        }

        void MCEntryNumDiagnose(int currentEntryNumber)
        {
            if (mMCEntryNumber.value() == currentEntryNumber)
            {
                LOG(fatal) << "TCAViewer<out>: Event number diagnositcs fails. Event number is not incremented.";
            }
            else if (currentEntryNumber - mMCEntryNumber.value() > 1)
            {
                LOG(fatal) << "TCAViewer<out>: Event number diagnositcs fails. Please make sure Get() is called in "
                              "every event!";
            }
        }

        inline auto GetMCEntryNumber() -> int { return eventHeader->GetMCEntryNumber(); }

        std::optional<int> mMCEntryNumber;
        std::optional<int> mContCapcitySize;
        std::string mBranchName;
        DataType** mCont = nullptr;
        TClonesArray* mTCA = nullptr;
        bool mSafeGuard = false;
        std::unique_ptr<TClonesArray> mTCA_out; // owning
        FairEventHeader* eventHeader = nullptr; // non-owning
    };

    template <typename Datatype>
    class TCAOutput_SafeGuard
    {
      public:
        using Pointers_out = Data<Datatype, out>;
        explicit TCAOutput_SafeGuard(Pointers_out* pointers)
            : mPointers{ pointers }
        {
            mPointers->EnableGuard();
        }

        ~TCAOutput_SafeGuard() noexcept { mPointers->DisableGuard(); }
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
