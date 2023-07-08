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

#include "R3BFileSource2.h"
#include "R3BException.h"
#include "R3BLogger.h"
#include <FairEventHeader.h>
#include <FairFileHeader.h>
#include <FairRootManager.h>
#include <TFolder.h>
#include <TKey.h>
#include <vector>

namespace
{
    constexpr auto DEFAULT_TITLE = "InputRootFile";

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

    auto HasBranchList(TFile* rootFile, const std::vector<std::string>& branchList) -> bool
    {
        auto const newBranchList = GetBranchList(rootFile, "BranchList");
        auto view1 = std::vector<std::string_view>(branchList.begin(), branchList.end());
        auto view2 = std::vector<std::string_view>(newBranchList.begin(), newBranchList.end());

        std::sort(view1.begin(), view1.end());
        std::sort(view2.begin(), view2.end());
        return view1 == view2;
    }

    template <typename ContainerType>
    auto GetDataFromAnyFolder(TFile* rootFile, const ContainerType& folderNames) -> std::optional<TKey*>
    {
        for (auto const& name : folderNames)
        {
            R3BLOG(debug, "looking for " + name);
            auto* dataFolder = dynamic_cast<TKey*>(rootFile->FindKey(name.c_str()));
            if (dataFolder != nullptr)
            {
                R3BLOG(debug, name + " has been found!");
                return dataFolder;
            }
        }
        return {};
    }

    auto Get_TChain_FromFairRM(FairRootManager* rootMan) -> TChain*
    {
        auto const chainTitle = "/" + std::string{ FairRootManager::GetFolderName() };
        auto inChain = std::make_unique<TChain>(FairRootManager::GetTreeName(), chainTitle.c_str());
        R3BLOG(debug, "Chain created");
        LOG(info) << "chain name: " << FairRootManager::GetTreeName();
        rootMan->SetInChain(inChain.release());
        return FairRootManager::Instance()->GetInChain();
    }
} // namespace

auto R3BInputRootFiles::AddFileName(std::string fileName) -> std::optional<std::string>
{
    auto const msg = fmt::format("Adding {} to file source\n", fileName);
    R3BLOG(info, msg);
    if (fileNames_.empty())
    {
        Intitialize(fileName);
    }
    if (!ValidateFile(fileName))
    {
        return fileName;
    }
    fileNames_.emplace_back(std::move(fileName));
    return {};
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
        throw R3B::runtime_error("There is no maian folder to be registered!");
    }

    if (!is_friend_)
    {
        auto listOfFolders = Vector2TContainer<TObjArray>(validMainFolders_);
        R3BLOG(debug, fmt::format("Set {} main folder(s) to FairRootManager.", listOfFolders->GetEntries()));
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

auto R3BInputRootFiles::ValidateFile(const std::string& filename) -> bool
{
    auto rootFile = R3B::make_rootfile(filename.c_str());
    auto folderKey = ExtractMainFolder(rootFile.get());
    auto res = folderKey.has_value() && HasBranchList(rootFile.get(), branchList_);
    if (res)
    {
        if (!folderName_.empty() && (folderKey.value()->GetName() != folderName_))
        {
            R3BLOG(warn, "Different folder name!");
        }
        if (!is_friend_)
        {
            validRootFiles_.push_back(std::move(rootFile));
            validMainFolders_.push_back((folderKey.value())->ReadObject<TFolder>());
        }
    }
    return res;
}

auto R3BInputRootFiles::ExtractRunId(TFile* rootFile) -> std::optional<uint>
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

void R3BInputRootFiles::Intitialize(std::string_view filename)
{
    auto file = R3B::make_rootfile(filename.data());

    if (const auto runID = ExtractRunId(file.get()); runID.has_value() && runID.value() != 0)
    {
        auto const msg = fmt::format(R"(Successfully extract RunID "{}" from root file "{}")", runID.value(), filename);
        R3BLOG(debug, msg);
        initial_RunID_ = runID.value();
    }
    else
    {
        auto const msg = fmt::format("Failed to extract RunID from root file \"{}\"", filename);
        R3BLOG(error, msg);
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
    for (auto const& branchName : branchList_)
    {
        FairRootManager::Instance()->AddBranchToList(branchName.c_str());
    }

    if (timeBasedBranchList_ = GetBranchList<TObjString>(file.get(), "TimeBasedBranchList");
        timeBasedBranchList_.empty())
    {
        LOG(warn) << "No time based branch list in input file";
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

void R3BFileSource2::AddFile(std::string fileName)
{

    if (auto const res = inputDataFiles_.AddFileName(std::move(fileName)); res.has_value())
    {
        R3BLOG(error,
               fmt::format(
                   "Root file {0} is incompatible with the first root file {1}", res.value(), dataFileNames_.front()));
    }
    dataFileNames_.emplace_back(fileName);
}

void R3BFileSource2::AddFriend(std::string_view fileName)
{
    //
    auto rootfile = R3B::make_rootfile(fileName.data());
    auto friendGroup = std::find_if(inputFriendFiles_.begin(),
                                    inputFriendFiles_.end(),
                                    [&rootfile](const auto& friends)
                                    { return HasBranchList(rootfile.get(), friends.GetBranchListRef()); });
    if (friendGroup == inputFriendFiles_.end())
    {
        auto newFriendGroup = R3BInputRootFiles{};
        newFriendGroup.Make_as_friend();
        inputFriendFiles_.push_back(std::move(newFriendGroup));
        friendGroup = --inputFriendFiles_.end();
        friendGroup->SetTitle(fmt::format("FriendTree_{}", inputFriendFiles_.size()));
    }
    auto res = friendGroup->AddFileName(std::string{ fileName });
    if (res.has_value())
    {
        R3BLOG(error,
               fmt::format("Friend file {0} is incompatible with the first friend file {1}",
                           res.value(),
                           friendGroup->GetBaseFileName()));
    }
    else
    {
        // TODO: really need it?
        friendFileNames_.emplace_back(fileName);
    }
}

Bool_t R3BFileSource2::Init()
{
    inputDataFiles_.RegisterTo(FairRootManager::Instance());

    for (auto& friendGroup : inputFriendFiles_)
    {
        inputDataFiles_.SetFriend(friendGroup);
    }

    return true;
}

void R3BFileSource2::FillEventHeader(FairEventHeader* evtHeader)
{
    if (evtHeader == nullptr)
    {
        throw R3B::logic_error("Filled event header is empty!");
    }
    evtHeader_ = evtHeader;

    // Set runID for event header:
    auto const init_runID = inputDataFiles_.GetInitialRunID();

    if (init_runID == 0)
    {
        throw R3B::logic_error("RunId is not being set!");
    }

    if (init_runID != GetRunId())
    {
        R3BLOG(
            error,
            fmt::format("runID {} being set is different from the runID {} in the data file!", GetRunId(), init_runID));
    }
    SetRunId(init_runID); // NOLINT
    evtHeader->SetRunId(init_runID);
}

Int_t R3BFileSource2::CheckMaxEventNo(Int_t EvtEnd)
{
    return (EvtEnd == 0) ? inputDataFiles_.GetEntries() : EvtEnd; // NOLINT
}

//----------------------------------------------------------------
void R3BFileSource2::ReadBranchEvent(const char* BrName)
{
    auto const currentEventID = evtHeader_->GetMCEntryNumber();
    ReadBranchEvent(BrName, currentEventID);
}

void R3BFileSource2::ReadBranchEvent(const char* BrName, Int_t entryID)
{
    auto const read_bytes = inputDataFiles_.GetChain()->FindBranch(BrName)->GetEntry(entryID);
    if (read_bytes == 0)
    {
        LOG(warn) << fmt::format("Failed to read the data of the event {0} from the branch {1}", entryID, BrName);
    }
}

Int_t R3BFileSource2::ReadEvent(UInt_t eventID)
{
    // fCurrentEntryNo = eventID;
    // fEventTime = GetEventTime();

    // TODO: make colors as variables
    // TODO: add disable option
    auto* chain = inputDataFiles_.GetChain();
    auto const total_entries_num = chain->GetEntries();
    if (allow_print_ && eventID > 0)
    {
        fmt::print("Processed: \033[32m {0} \033[0m of \033[34m {1} \033[0m (\033[33m {2:8.2f} \033[0m of "
                   "100), current RunId \033[31m {3:3d} \033[0m \r",
                   (eventID + 1),
                   total_entries_num,
                   100. * (eventID / static_cast<double>(total_entries_num)),
                   fRunId);
        std::cout << std::flush;
    }

    // TODO: clean this mess

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

ClassImp(R3BFileSource2);
