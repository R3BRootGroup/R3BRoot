/******************************************************************************
 *   Copyright (C) 2023 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2023-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BFileSource2.h"
#include "R3BEventHeader.h"
#include "R3BException.h"
#include "R3BShared.h"
#include <FairEventHeader.h>
#include <FairFileHeader.h>
#include <FairMCEventHeader.h>
#include <FairRootManager.h>
#include <FairRun.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TBranchElement.h>
#include <TClonesArray.h>
#include <TCollection.h>
#include <TFolder.h>
#include <TKey.h>
#include <TObjString.h>
#include <TObject.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fairlogger/Logger.h>
#include <fmt/base.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

namespace
{
    constexpr auto DEFAULT_TITLE = "InputRootFile";
    constexpr auto DEFAULT_RUN_ID = 999;

    template <typename ContainerType, typename DataType>
    auto Vector2TContainer(std::vector<DataType>& vec) -> std::unique_ptr<ContainerType>
    {
        using RawType = std::remove_reference_t<std::remove_pointer_t<DataType>>;
        static_assert(std::is_base_of_v<TObject, RawType>);
        auto list = std::make_unique<ContainerType>();
        for (auto& iter : vec)
        {
            if constexpr (std::is_pointer_v<DataType>)
            {
                list->Add(iter);
            }
            else
            {
                list->Add(&iter);
            }
        }
        return list;
    }

    template <typename StringType = std::string>
    auto GetBranchList(TFile* rootFile, std::string_view listName) -> std::vector<StringType>
    {
        auto branchList = std::vector<StringType>{};
        if (auto* list = dynamic_cast<TList*>(rootFile->Get(listName.data())); list != nullptr)
        {
            for (const auto& str : TRangeDynCast<TObjString>(list))
            {
                branchList.emplace_back(str->GetString().Data());
            }
        }
        else
        {
            throw R3B::logic_error(
                fmt::format("No branch list named {0} in input file {1}", listName, rootFile->GetName()));
        }
        return branchList;
    }

    template <typename UnaryFunc>
    void loop_through_branch_elements(TFile* root_file, std::string_view tree_name, UnaryFunc action)
    {
        auto* tree = root_file->Get<TTree>(tree_name.data());
        auto* branches = tree->GetListOfBranches();
        LOGP(debug, "Get {} branches from the tree file {}", branches->GetEntries(), root_file->GetName());
        for (auto* branch_obj : TRangeDynCast<TObject>(branches))
        {
            auto* branch = dynamic_cast<TBranchElement*>(branch_obj);
            if (branch == nullptr)
            {
                continue;
            }
            action(branch);
        }
    }

    template <typename UnaryFunc>
    void loop_through_branches(TFile* root_file, std::string_view tree_name, UnaryFunc action)
    {
        auto* tree = root_file->Get<TTree>(tree_name.data());
        auto* branches = tree->GetListOfBranches();
        LOGP(debug, "Get {} branches from the tree file {}", branches->GetEntries(), root_file->GetName());
        for (auto* branch_obj : TRangeDynCast<TObject>(branches))
        {
            auto* branch = dynamic_cast<TBranch*>(branch_obj);
            if (branch == nullptr)
            {
                continue;
            }
            action(branch);
        }
    }

    template <typename StringType = std::string>
    auto GetBranchListFromTree(TFile* root_file, std::string_view tree_name) -> std::vector<StringType>
    {
        auto branch_name_list = std::vector<StringType>{};
        loop_through_branch_elements(root_file,
                                     tree_name,
                                     [&branch_name_list](auto* branch)
                                     { branch_name_list.emplace_back(branch->GetName()); });
        return branch_name_list;
    }

    auto get_tca_data_class(TBranchElement* branch) -> std::string
    {
        TClonesArray* buffer = nullptr;
        branch->SetAddress(&buffer); // NOLINT: Nasty ROOT API
        branch->GetEntry(0);
        branch->SetAddress(nullptr);
        if (buffer != nullptr)
        {
            auto class_name = std::string{ buffer->GetClass()->GetName() };
            LOGP(debug, "Determine the class name {:?} of the branch {:?}", class_name, branch->GetName());
            return class_name;
        }
        LOGP(warn, "Cannot determine the class name of the branch {:?}", branch->GetName());
        return std::string{ "TObject" };
    }

    void add_branches_to_folder(TFolder* folder, TFile* root_file, std::string_view tree_name)
    {
        loop_through_branch_elements(root_file,
                                     tree_name,
                                     [folder](auto* branch)
                                     {
                                         auto class_name = std::string_view{ branch->GetClassName() };
                                         if (class_name == "TClonesArray")
                                         {
                                             const auto data_class = get_tca_data_class(branch);
                                             auto tca_obj = std::make_unique<TClonesArray>(data_class.data());
                                             tca_obj->SetName(branch->GetName());
                                             folder->Add(tca_obj.release());
                                         }
                                     });

        // TODO: what if it's branch with single literal value?
    }

    auto HasBranchList(TFile* rootFile, const std::vector<std::string>& branchList) -> bool
    {
        auto const newBranchList = GetBranchList(rootFile, "BranchList");
        auto view1 = std::vector<std::string_view>(branchList.begin(), branchList.end());
        auto view2 = std::vector<std::string_view>(newBranchList.begin(), newBranchList.end());

#ifdef HAS_CPP_STANDARD_17
        std::sort(view1.begin(), view1.end());
        std::sort(view2.begin(), view2.end());
#else
        std::ranges::sort(view1);
        std::ranges::sort(view2);
#endif
        return view1 == view2;
    }

    template <typename ContainerType>
    auto GetDataFromAnyFolder(TFile* rootFile, const ContainerType& folderNames) -> std::optional<TKey*>
    {
        for (auto const& name : folderNames)
        {
            LOGP(debug, "Looking for {}", name);
            auto* dataFolder = dynamic_cast<TKey*>(rootFile->FindKey(name.c_str()));
            if (dataFolder != nullptr)
            {
                LOGP(debug, "{} has been found!", name);
                return dataFolder;
            }
        }
        return {};
    }

    auto Get_TChain_FromFairRM(FairRootManager* rootMan) -> TChain*
    {
        auto const chainTitle = "/" + std::string{ FairRootManager::GetFolderName() };
        auto inChain = std::make_unique<TChain>(FairRootManager::GetTreeName(), chainTitle.c_str());
        LOGP(debug, "Chain created");
        LOG(info) << "chain name: " << FairRootManager::GetTreeName();
        rootMan->SetInChain(inChain.release());
        return FairRootManager::Instance()->GetInChain();
    }
} // namespace

void R3BEventProgressPrinter::SetRefreshRate_Hz(float rate)
{
    if (rate <= 0.)
    {
        throw R3B::logic_error(fmt::format("Refresh rate {} must be a positive floating point value", rate));
    }

    refresh_rate_ = rate;
    refresh_period_ = std::chrono::milliseconds(static_cast<int>(1000. / rate));
}

void R3BEventProgressPrinter::ShowProgress(uint64_t event_num)
{
    if (event_num < 1)
    {
        return;
    }
    if (max_event_num_ == 0)
    {
        throw R3B::logic_error("Maximal event number has not been set up!");
    }
    const auto now_t = std::chrono::steady_clock::now();
    const auto time_spent = std::chrono::ceil<std::chrono::milliseconds>(now_t - previous_t_);
    if (time_spent > refresh_period_)
    {
        const auto total_time_spent = std::chrono::ceil<std::chrono::milliseconds>(now_t - begin_t_);
        const auto processed_events = event_num - previous_event_num_;
        const auto events_per_millisecond = [&, this]()
        {
            if (mode_ == Mode::piecewise)
            {
                return static_cast<double>(processed_events) / static_cast<double>(time_spent.count());
            }
            return static_cast<double>(event_num) / static_cast<double>(total_time_spent.count());
        }();

        Print(event_num, events_per_millisecond);

        previous_t_ = now_t;
        previous_event_num_ = event_num;
    }
}

void R3BEventProgressPrinter::Print(uint64_t event_num, double speed_per_ms)
{
    if (speed_per_ms <= 0.)
    {
        return;
    }
    const auto event_num_str =
        fmt::format(fg(fmt::terminal_color::bright_green) | fmt::emphasis::bold, "{:^5d}k", event_num / 1000);
    const auto speed_str = fmt::format("{:^6.1F}k/s", speed_per_ms);
    const auto progress_str = fmt::format(fg(fmt::terminal_color::bright_yellow) | fmt::emphasis::bold,
                                          "{:^6.2F}",
                                          100. * static_cast<double>(event_num) / static_cast<double>(max_event_num_));
    const auto time_left_ms =
        std::chrono::milliseconds{ (max_event_num_ - event_num) / static_cast<int>(std::ceil(speed_per_ms)) };
    fmt::print("Events processed: {0} ({1})  Progress: {2}% (time left: {3:%H h %M m %S s})   Run ID: {4}\r",
               event_num_str,
               speed_str,
               progress_str,
               std::chrono::ceil<std::chrono::seconds>(time_left_ms),
               run_id_);
    std::fflush(stdout);
}

auto R3BInputRootFiles::AddFileName(std::string fileName, bool is_tree_file) -> std::optional<std::string>
{
    LOGP(info, "Adding {} to file source\n", fileName);
    if (fileNames_.empty())
    {
        Initialize(fileName, is_tree_file);
        register_branch_name();
    }
    if (!ValidateFile(fileName, is_tree_file))
    {
        return fileName;
    }
    fileNames_.emplace_back(std::move(fileName));
    return {};
}

void R3BInputRootFiles::register_branch_name()
{

    for (auto const& branchName : branchList_)
    {
        FairRootManager::Instance()->AddBranchToList(branchName.c_str());
    }
}

void R3BInputRootFiles::SetInputFileChain(TChain* chain)
{
    if (rootChain_ != nullptr)
    {
        throw R3B::logic_error("TChain has already been created!");
    }
    rootChain_ = chain;
    for (auto const& filename : fileNames_)
    {
        rootChain_->AddFile(filename.c_str(), TTree::kMaxEntries, treeName_.c_str());
    }
}
void R3BInputRootFiles::RegisterTo(FairRootManager* rootMan)
{
    if (is_friend_)
    {
        return;
    }

    if (validMainFolders_.empty())
    {
        throw R3B::runtime_error("There is no main folder to be registered!");
    }

    if (!is_friend_)
    {
        auto listOfFolders = Vector2TContainer<TObjArray>(validMainFolders_);
        LOGP(debug, "Set {} main folder(s) to FairRootManager.", listOfFolders->GetEntries());
        rootMan->SetListOfFolders(listOfFolders.release());
        rootMan->SetTimeBasedBranchNameList(Vector2TContainer<TList>(timeBasedBranchList_).release());
        SetInputFileChain(Get_TChain_FromFairRM(rootMan));
    }
}

auto R3BInputRootFiles::ExtractMainFolder(TFile* rootFile) -> std::optional<TKey*>
{
    auto const folderNames =
        std::array<std::string, 4>{ FairRootManager::GetFolderName(), "r3broot", "cbmout", "cbmroot" };

    return GetDataFromAnyFolder(rootFile, folderNames);
}

auto R3BInputRootFiles::ValidateFile(const std::string& filename, bool is_tree_file) -> bool
{
    auto rootFile = R3B::make_rootfile(filename.c_str());

    if (is_tree_file)
    {
        if (!is_friend_)
        {
            auto folder = std::make_unique<TFolder>("r3broot", "r3broot");
            add_branches_to_folder(folder.get(), rootFile.get(), treeName_);
            validRootFiles_.push_back(std::move(rootFile));
            validMainFolders_.push_back(folder.release());
        }
        return true;
    }

    auto folderKey = ExtractMainFolder(rootFile.get());
    auto res1 = folderKey.has_value();
    auto res2 = HasBranchList(rootFile.get(), branchList_);
    if (res1 and res2)
    {
        if (!folderName_.empty() && (folderKey.value()->GetName() != folderName_))
        {
            LOGP(warn, "Different folder name!");
        }
        if (!is_friend_)
        {
            validRootFiles_.push_back(std::move(rootFile));
            validMainFolders_.push_back((folderKey.value())->ReadObject<TFolder>());
        }
    }
    else
    {
        if (not res1)
        {
            LOGP(warn, "folder has no key");
        }
        if (not res2)
        {
            LOGP(warn, "HasBranchList is false!");
        }
    }
    return res1 and res2;
}

auto R3BInputRootFiles::ExtractRunId(TFile* rootFile) -> std::optional<int>
{
    //
    auto* header = rootFile->Get<FairFileHeader>(fileHeader_.c_str());
    if (header == nullptr)
    {
        return {};
    }
    auto runID = header->GetRunId();
    return runID;
}

void R3BInputRootFiles::Initialize(std::string_view filename, bool is_tree_file)
{
    auto file = R3B::make_rootfile(filename.data());

    if (is_tree_file)
    {
        branchList_ = GetBranchListFromTree(file.get(), treeName_);
        return;
    }

    if (const auto runID = ExtractRunId(file.get()); runID.has_value() && runID.value() != 0)
    {
        LOGP(debug, "Successfully extract RunID {} from root file {:?}", runID.value(), filename);
        initial_RunID_ = runID.value();
    }
    else
    {
        LOGP(error, "Failed to extract RunID from root file {:?}", filename);
    }

    if (auto folderKey = ExtractMainFolder(file.get()); folderKey.has_value())
    {
        folderName_ = folderKey.value()->GetName();
    }
    else
    {
        throw R3B::logic_error(fmt::format("Cannot find main folder from the root file {}!", filename));
    }

    branchList_ = GetBranchList(file.get(), "BranchList");

    if (timeBasedBranchList_ = GetBranchList<TObjString>(file.get(), "TimeBasedBranchList");
        timeBasedBranchList_.empty())
    {
        LOG(info) << "No time based branch list in input file";
    }
}

void R3BInputRootFiles::SetFriend(R3BInputRootFiles& friendFiles)
{
    if (is_friend_)
    {
        throw R3B::logic_error("Can not set friendFiles with another friendFile!");
    }
    auto chain = std::make_unique<TChain>(friendFiles.GetTitle().c_str(), friendFiles.GetFolderName().c_str());
    friendFiles.SetInputFileChain(chain.get());
    rootChain_->AddFriend(chain.release());
}

[[nodiscard]] auto R3BInputRootFiles::GetEntries() const -> int64_t
{
    if (rootChain_ == nullptr)
    {
        throw R3B::logic_error("Can't get entries before being initialized!");
    }
    return rootChain_->GetEntries();
}

R3BFileSource2::R3BFileSource2(std::vector<std::string> fileNames, std::string_view title)
{
    LOG(debug) << "Creating a new R3BFileSource!";
    inputDataFiles_.SetTitle(title);
    inputDataFiles_.SetFileHeaderName("FileHeader");
    for (auto& name : fileNames)
    {
        if (name.empty())
        {
            continue;
        }
        AddFile(std::move(name));
    }
}

R3BFileSource2::R3BFileSource2(std::string file, std::string_view title)
    : R3BFileSource2(std::vector<std::string>{ std::move(file) }, title)
{
}

R3BFileSource2::R3BFileSource2(std::vector<std::string> fileNames)
    : R3BFileSource2(std::move(fileNames), DEFAULT_TITLE)
{
}

R3BFileSource2::R3BFileSource2()
    : R3BFileSource2(std::string{})
{
}

void R3BFileSource2::AddFile(std::string file_name, bool is_tree_file)
{
    if (auto const res = inputDataFiles_.AddFileName(std::move(file_name), is_tree_file); res.has_value())
    {
        if (not dataFileNames_.empty())
        {

            LOGP(error,
                 "Root file {:?} is incompatible with the first root file {:?}",
                 res.value(),
                 dataFileNames_.front());
        }
        else
        {
            LOGP(error, "Failed to add the first root file {:?}", file_name);
        }
    }
    dataFileNames_.emplace_back(file_name);
}

void R3BFileSource2::AddFile(std::vector<std::string> file_names, bool is_tree_file)
{
    for (auto& file_name : file_names)
    {
        AddFile(std::move(file_name), is_tree_file);
    }
}

void R3BFileSource2::AddFriend(std::vector<std::string> file_names, bool is_tree_file)
{
    for (auto& file_name : file_names)
    {
        AddFriend(std::move(file_name), is_tree_file);
    }
}

void R3BFileSource2::AddFriend(std::string file_name, bool is_tree_file)
{
    //
    auto rootfile = R3B::make_rootfile(file_name.c_str());
#ifdef HAS_CPP_STANDARD_17
    auto friendGroup = std::find_if(inputFriendFiles_.begin(),
                                    inputFriendFiles_.end(),
                                    [&rootfile](const auto& friends)
                                    { return HasBranchList(rootfile.get(), friends.GetBranchListRef()); });
#else
    auto friendGroup = std::ranges::find_if(inputFriendFiles_,
                                            [&rootfile](const auto& friends)
                                            { return HasBranchList(rootfile.get(), friends.GetBranchListRef()); });
#endif
    if (friendGroup == inputFriendFiles_.end())
    {
        auto newFriendGroup = R3BInputRootFiles{};
        newFriendGroup.Make_as_friend();
        inputFriendFiles_.push_back(std::move(newFriendGroup));
        friendGroup = --inputFriendFiles_.end();
        friendGroup->SetTitle(fmt::format("FriendTree_{}", inputFriendFiles_.size()));
    }
    auto res = friendGroup->AddFileName(file_name, is_tree_file);
    if (res.has_value())
    {
        LOGP(error,
             "Friend file {} is incompatible with the first friend file {:?}",
             res.value(),
             friendGroup->GetBaseFileName());
    }
    else
    {
        // TODO: really need it?
        friendFileNames_.emplace_back(std::move(file_name));
    }
}

Bool_t R3BFileSource2::Init()
{
    if (inputDataFiles_.is_empty())
    {
        throw R3B::logic_error{ "No input file available!" };
    }

    inputDataFiles_.RegisterTo(FairRootManager::Instance());

    for (auto& friendGroup : inputFriendFiles_)
    {
        inputDataFiles_.SetFriend(friendGroup);
    }

    event_progress_.SetMaxEventNum(inputDataFiles_.GetEntries());
    event_progress_.SetRunID(inputDataFiles_.GetInitialRunID());

    r3b_event_header_ = dynamic_cast<R3BEventHeader*>(FairRootManager::Instance()->GetObject("EventHeader."));
    mc_event_header_ = dynamic_cast<FairMCEventHeader*>(FairRootManager::Instance()->GetObject("MCEventHeader."));

    if (r3b_event_header_ == nullptr and mc_event_header_ == nullptr)
    {
        throw R3B::runtime_error(
            "Event header from the input root file can neither be casted as R3BEventHeader nor FairMCEventHeader!");
    }

    return true;
}

void R3BFileSource2::FillEventHeader(FairEventHeader* evtHeader)
{
    if (evtHeader == nullptr)
    {
        throw R3B::logic_error("Filled event header is empty!");
    }

    auto* event_header = dynamic_cast<R3BEventHeader*>(evtHeader);
    if (event_header == nullptr)
    {
        if (r3b_event_header_ != nullptr)
        {
            r3b_event_header_->Copy(*evtHeader);
        }
    }
    else
    {
        if (r3b_event_header_ != nullptr)
        {
            (*event_header) = *r3b_event_header_;
        }
        else if (mc_event_header_ != nullptr)
        {
            (*event_header) = *mc_event_header_;
        }
    }

    // Set runID for event header:
    auto const init_runID = inputDataFiles_.GetInitialRunID();

    if (init_runID == 0)
    {
        throw R3B::logic_error("RunId is not being set!");
    }

    if (GetRunId() != DEFAULT_RUN_ID and init_runID != GetRunId())
    {
        LOGP(warn,
             "runID {} being set is different from the runID {} in the data file! Use the runID in the data file.",
             GetRunId(),
             init_runID);
    }
    SetRunId(init_runID); // NOLINT
    evtHeader->SetRunId(init_runID);
}

Int_t R3BFileSource2::CheckMaxEventNo(Int_t EvtEnd)
{
    event_end_ = (EvtEnd <= 0) ? inputDataFiles_.GetEntries() : EvtEnd; // NOLINT
    LOGP(info, "Setting printing event max to {}", event_end_);
    event_progress_.SetMaxEventNum(event_end_);
    return event_end_;
}

void R3BFileSource2::ReadBranchEvent(const char* BrName)
{
    auto const currentEventID = r3b_event_header_->GetMCEntryNumber();
    ReadBranchEvent(BrName, currentEventID);
}

void R3BFileSource2::ReadBranchEvent(const char* BrName, int entryID)
{
    auto const read_bytes = inputDataFiles_.GetChain()->FindBranch(BrName)->GetEntry(entryID);
    if (read_bytes == 0)
    {
        LOG(warn) << fmt::format("Failed to read the data of the event {0} from the branch {1}", entryID, BrName);
    }
}

Int_t R3BFileSource2::ReadEvent(unsigned int eventID)
{
    auto* chain = inputDataFiles_.GetChain();
    if (fair::Logger::GetConsoleSeverity() == fair::Severity::info)
    {
        event_progress_.ShowProgress(eventID);
    }

    auto read_bytes = chain->GetEntry(eventID);
    if (read_bytes == 0)
    {
        LOG(warn) << fmt::format("Failed to read the data of the event {0} from the source", eventID);
        return 1;
    }
    return 0;
}

Bool_t R3BFileSource2::ActivateObject(TObject** obj, const char* BrName)
{
    auto* chain = inputDataFiles_.GetChain();
    chain->SetBranchStatus(BrName, true);
    chain->SetBranchAddress(BrName, obj);
    return kTRUE;
}

Bool_t R3BFileSource2::ActivateObjectAny(void** obj, const std::type_info& info, const char* BrName)
{
    auto* chain = inputDataFiles_.GetChain();
    if (chain != nullptr)
    {
        return ActivateObjectAnyImpl(chain, obj, info, BrName);
    }
    return kFALSE;
}

ClassImp(R3BFileSource2);
