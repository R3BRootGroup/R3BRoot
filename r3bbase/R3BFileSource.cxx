/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

/* R3BFileSource.h
 * R3BROOT
 *
 * Author: J.L. Rodriguez-Sanchez
 * Date: 12/05/21
 * */

#include "R3BFileSource.h"
#include "FairEventHeader.h"
#include "FairFileHeader.h"
#include "FairLogger.h"
#include "FairMCEventHeader.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "R3BEventHeader.h"
#include "TChainElement.h"
#include "TFolder.h"
#include "TObjArray.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TString.h"
#include <TObjString.h>
#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <typeinfo>

using std::map;
using std::set;

#include <cstdlib>
#include <iostream>
using namespace std;

TMCThreadLocal R3BFileSource* R3BFileSource::fSourceInstance = 0;

R3BFileSource::R3BFileSource(TFile* f, const char* Title, UInt_t)
    : FairFileSource(f, Title)
    , fInputTitle(Title)
    , fRootFile(f)
    , fCurrentEntryNr(0)
    , fFriendFileList()
    , fInputChainList()
    , fFriendTypeList()
    , fCheckInputBranches()
    , fInputLevel()
    , fRunIdInfoAll()
    , fInChain(0)
    , fInTree(0)
    , fListFolder(new TObjArray(16))
    , fRtdb(FairRuntimeDb::instance())
    , fCbmout(0)
    , fCbmroot(0)
    , fSourceIdentifier(0)
    , fNoOfEntries(-1)
    , IsInitialized(kFALSE)
    , fMCHeader(0)
    , fEvtHeader(0)
    , fFileHeader(0)
    , fEvtHeaderIsNew(kFALSE)
    , fCurrentEntryNo(0)
    , fTimeforEntryNo(-1)
    , fEventTime(0.)
    , fCheckFileLayout(kTRUE)
    , fInputFile()
    , fExpid(0)
    , prevts(0)
    , nextts(0)
{
    if (fSourceInstance)
    {
        Fatal("R3BFileSource", "Singleton instance already exists.");
        return;
    }
    fSourceInstance = this;

    if (fRootFile->IsZombie())
    {
        LOG(fatal) << "Error opening the Input file";
    }
    LOG(debug) << "R3BFileSource created------------";
}

R3BFileSource::R3BFileSource(const TString* RootFileName, const char* Title, UInt_t)
    : FairFileSource(RootFileName, Title)
    , fInputTitle(Title)
    , fRootFile(0)
    , fCurrentEntryNr(0)
    , fFriendFileList()
    , fInputChainList()
    , fFriendTypeList()
    , fCheckInputBranches()
    , fInputLevel()
    , fRunIdInfoAll()
    , fInChain(0)
    , fInTree(0)
    , fListFolder(new TObjArray(16))
    , fRtdb(FairRuntimeDb::instance())
    , fCbmout(0)
    , fCbmroot(0)
    , fSourceIdentifier(0)
    , fNoOfEntries(-1)
    , IsInitialized(kFALSE)
    , fMCHeader(0)
    , fEvtHeader(0)
    , fFileHeader(0)
    , fEvtHeaderIsNew(kFALSE)
    , fCurrentEntryNo(0)
    , fTimeforEntryNo(-1)
    , fEventTime(0.)
    , fCheckFileLayout(kTRUE)
    , fInputFile()
    , fExpid(0)
    , prevts(0)
    , nextts(0)
{
    if (fSourceInstance)
    {
        Fatal("R3BFileSource", "Singleton instance already exists.");
        return;
    }
    fSourceInstance = this;

    fRootFile = TFile::Open(RootFileName->Data());
    if (fRootFile->IsZombie())
    {
        LOG(fatal) << "Error opening the Input file";
    }
    LOG(debug) << "R3BFileSource created------------";
}

R3BFileSource::R3BFileSource(const TString RootFileName, const char* Title, UInt_t)
    : FairFileSource(RootFileName, Title)
    , fInputTitle(Title)
    , fRootFile(0)
    , fCurrentEntryNr(0)
    , fFriendFileList()
    , fInputChainList()
    , fFriendTypeList()
    , fCheckInputBranches()
    , fInputLevel()
    , fRunIdInfoAll()
    , fInChain(0)
    , fInTree(0)
    , fListFolder(new TObjArray(16))
    , fRtdb(FairRuntimeDb::instance())
    , fCbmout(0)
    , fCbmroot(0)
    , fSourceIdentifier(0)
    , fNoOfEntries(-1)
    , IsInitialized(kFALSE)
    , fMCHeader(0)
    , fEvtHeader(0)
    , fFileHeader(0)
    , fEvtHeaderIsNew(kFALSE)
    , fCurrentEntryNo(0)
    , fTimeforEntryNo(-1)
    , fEventTime(0.)
    , fCheckFileLayout(kTRUE)
    , fInputFile()
    , fExpid(0)
    , prevts(0)
    , nextts(0)
{
    if (fSourceInstance)
    {
        Fatal("R3BFileSource", "Singleton instance already exists.");
        return;
    }
    fSourceInstance = this;

    fRootFile = TFile::Open(RootFileName.Data());
    if (fRootFile->IsZombie())
    {
        LOG(fatal) << "Error opening the Input file";
    }
    LOG(debug) << "R3BFileSource created------------";
}

R3BFileSource* R3BFileSource::Instance() { return fSourceInstance; }

R3BFileSource::~R3BFileSource()
{
    LOG(debug) << "Enter Destructor of R3BFileSource";
    delete fEvtHeader;
    LOG(debug) << "Leave Destructor of R3BFileSource";
}

Bool_t R3BFileSource::Init()
{
    LOG(INFO) << "R3BFileSource::Init()";

    if (IsInitialized)
    {
        LOG(INFO) << "R3BFileSource already initialized";
        return kTRUE;
    }
    if (!fInChain)
    {
        fInChain = new TChain(FairRootManager::GetTreeName(), Form("/%s", FairRootManager::GetFolderName()));
        LOG(debug) << "R3BFileSource::Init() chain created";
        FairRootManager::Instance()->SetInChain(fInChain);
    }
    fInChain->Add(fRootFile->GetName());

    // Get the folder structure from file which describes the input tree.
    // There are two different names possible, so check both.
    fCbmroot = dynamic_cast<TFolder*>(fRootFile->Get(FairRootManager::GetFolderName()));
    if (!fCbmroot)
    {
        fCbmroot = dynamic_cast<TFolder*>(fRootFile->Get("cbmroot"));
        if (!fCbmroot)
        {
            fCbmroot = dynamic_cast<TFolder*>(fRootFile->Get("cbmout"));
            if (!fCbmroot)
            {
                fCbmroot = gROOT->GetRootFolder()->AddFolder(FairRootManager::GetFolderName(), "Main Folder");
            }
            else
            {
                fCbmroot->SetName(FairRootManager::GetFolderName());
            }
        }
    }
    // Get The list of branches from the input file and add it to the
    // actual list of existing branches.
    // Add this list of branches also to the map of input trees, which
    // stores the information which branches belong to which input tree.
    // There is at least one primary input tree, but there can be many
    // additional friend trees.
    // This information is needed to add new files to the correct friend
    // tree. With this information it is also possible to check if the
    // input files which are added to the input chain all have the same
    // branch structure. Without this check it is possible to add trees
    // with a different branch structure but the same tree name. ROOT
    // probably only checks if the name of the tree is the same.
    TList* list = dynamic_cast<TList*>(fRootFile->Get("BranchList"));
    if (list == 0)
    {
        LOG(fatal) << "No Branch list in input file";
    }
    TString chainName = fInputTitle;
    TString ObjName;
    fInputLevel.push_back(chainName);
    fCheckInputBranches[chainName] = new std::list<TString>;
    if (list)
    {
        TObjString* Obj = 0;
        LOG(debug) << "Enteries in the list " << list->GetEntries();
        for (Int_t i = 0; i < list->GetEntries(); i++)
        {
            Obj = dynamic_cast<TObjString*>(list->At(i));
            if (Obj != 0)
            {
                ObjName = Obj->GetString();
                LOG(debug) << "Branch name " << ObjName.Data();
                fCheckInputBranches[chainName]->push_back(ObjName.Data());

                FairRootManager::Instance()->AddBranchToList(ObjName.Data());
            }
        }
    }

    gROOT->GetListOfBrowsables()->Add(fCbmroot);
    fListFolder->Add(fCbmroot);

    // Store the information about the unique runids in the input file
    // together with the filename and the number of events for each runid
    // this information is needed later to check if inconsitencies exist
    // between the main input chain and any of the friend chains.

    //  GetRunIdInfo(fInFile->GetName(), chainName);

    // Add all additional input files to the input chain and do a
    // consitency check
    for (auto fileName : fInputChainList)
    {
        // Store global gFile pointer for safety reasons.
        // Set gFile to old value at the end of the routine.R
        TFile* temp = gFile;

        // Temporarily open the input file to extract information which
        // is needed to bring the friend trees in the correct order
        TFile* inputFile = TFile::Open(fileName);
        if (inputFile->IsZombie())
        {
            LOG(fatal) << "Error opening the file " << fileName.Data()
                       << " which should be added to the input chain or as friend chain";
        }

        if (fCheckFileLayout)
        {
            // Check if the branchlist is the same as for the first input file.
            Bool_t isOk = CompareBranchList(inputFile, chainName);
            if (!isOk)
            {
                LOG(fatal) << "Branch structure of the input file " << fRootFile->GetName()
                           << " and the file to be added " << fileName.Data() << " are different.";
                return kFALSE;
            }
        }

        // Add the runid information for all files in the chain.
        // GetRunIdInfo(inputFile->GetName(), chainName);
        // Add the file to the input chain
        fInChain->Add(fileName);

        // Close the temporarly file and restore the gFile pointer.
        inputFile->Close();
        gFile = temp;
    }
    fNoOfEntries = fInChain->GetEntries();

    LOG(debug) << "Entries in this Source " << fNoOfEntries;

    for (Int_t i = 0; i < fListFolder->GetEntriesFast(); i++)
    {
        TFolder* fold = static_cast<TFolder*>(fListFolder->At(i));
        fEvtHeader = static_cast<R3BEventHeader*>(fold->FindObjectAny("EventHeader."));
        fMCHeader = static_cast<FairMCEventHeader*>(fold->FindObjectAny("MCEventHeader."));
        if (fEvtHeader)
        {
            ActivateObject(reinterpret_cast<TObject**>(&fEvtHeader), "EventHeader.");
        }
        if (fMCHeader)
        {
            ActivateObject(reinterpret_cast<TObject**>(&fMCHeader), "MCEventHeader.");
        }
    }

    FairRootManager::Instance()->SetListOfFolders(fListFolder);

    AddFriendsToChain();

    TList* timebasedlist = dynamic_cast<TList*>(fRootFile->Get("TimeBasedBranchList"));
    if (timebasedlist == 0)
    {
        LOG(warn) << "No time based branch list in input file";
    }
    else
    {
        FairRootManager::Instance()->SetTimeBasedBranchNameList(timebasedlist);
    }

    // Open configuration file with runid values if needed in this step
    fInputFile.open(fInputFileName.Data(), std::fstream::in);
    if (!fInputFile.is_open())
    {
        LOG(WARNING) << "R3BFileSource::Init() Input file for RunIds was not found, it is Ok!";
    }
    else
    {
        LOG(INFO) << "R3BFileSource::Init() Input file for RunIds " << fInputFileName.Data() << " is open!";
        fInputFile.clear();
        fInputFile.seekg(0, std::ios::beg);
    }

    if (fInputFile.is_open())
    {
        LOG(INFO) << "R3BFileSource::Init() Reading RunId file";
        std::string buffer;
        char *p1, *p2, *p3;
        do
        {
            getline(fInputFile, buffer);
            LOG(INFO) << "read from file: \"" << buffer << "\"";
            if (buffer.find("fRunID") == 0)
            {
                buffer.erase(0, buffer.find('=') + 1);
                UInt_t rid = strtol(buffer.c_str(), &p1, 16);
                if (buffer.c_str() == p1)
                {
                    LOG(ERROR) << "R3BFileSource::Init() Bad structure for RunId file, \n"
                               << "it must be (hex format): \n"
                               << "fRunID = a \n"
                               << "expid timestamp \n";
                    return kFALSE;
                }
                fRunid.push_back(rid);
                LOG(DEBUG) << "RuniD= " << rid;
            }
            if (buffer.find("FILE") == 0)
                continue;

            getline(fInputFile, buffer);
            LOG(INFO) << "read from file: \"" << buffer << "\"";

            std::string buf1 = buffer.c_str();
            buf1.erase(buf1.find(' '), buf1.length());
            buffer.erase(0, buffer.find(' ') + 1);
            fExpid = strtol(buf1.c_str(), &p2, 16);
            uint64_t ts = strtol(buffer.c_str(), &p3, 16);

            if (buf1.c_str() == p2 || buffer.c_str() == p3)
            {
                LOG(ERROR) << "R3BFileSource::Init() Bad structure for RunId file, \n"
                           << "it must be (hex format): \n"
                           << "fRunID = a \n"
                           << "expid timestamp \n";
                return kFALSE;
            }
            fTimestamp.push_back(ts);
            LOG(DEBUG) << fExpid << " / " << ts;

        } while (fInputFile && buffer.compare("FILE END"));
        LOG(INFO) << "R3BFileSource::Init() End of reading RunId file";
        fInputFile.close();
    }
    else
    {
        nextts = -1;
    }

    return kTRUE;
}

void R3BFileSource::SetInTree(TTree* tempTree)
{
    fInTree = NULL;
    fInTree = tempTree;
    fRootFile = static_cast<TFile*>(tempTree->GetCurrentFile());
    fInChain->Reset();
    IsInitialized = kFALSE;
    Init();
}

// ----  Method GetRunid -----------------------------------------------------
Int_t R3BFileSource::GetRunid(uint64_t st)
{
    UInt_t fArraysize = fTimestamp.size();
    if (fArraysize != fRunid.size())
    {
        LOG(ERROR) << "\033[5m\033[31m R3BFileSource::GetRunid() Different number of RunIds and timestamps \033[0m";
        prevts = -1;
        nextts = -1;
        return 1;
    }

    for (Int_t j = 0; j < fArraysize - 1; j++)
        if (st >= fTimestamp[j] && st < fTimestamp[j + 1])
        {
            prevts = fTimestamp[j];
            nextts = fTimestamp[j + 1];
            LOG(DEBUG) << "New timestamp " << nextts << " for RunId " << fRunid[j];
            return fRunid[j];
        }

    if (st >= fTimestamp[fArraysize - 1])
    {
        prevts = fTimestamp[fArraysize - 1];
        nextts = fTimestamp[fArraysize - 1];
        LOG(DEBUG) << "Prev/next timestamp " << prevts << "/" << nextts << " for runid " << fRunid[fArraysize - 1];
        return fRunid[fArraysize - 1];
    }

    if (nextts > 0)
        LOG(WARNING) << "\033[5m\033[33m R3BFileSource::GetRunid() RunId was not found, it will be 1 \033[0m";
    prevts = -1;
    nextts = -1;

    return 1;
}

// ----  Method ReadEvent ----------------------------------------------------
Int_t R3BFileSource::ReadEvent(UInt_t i)
{
    /** TODO
     ** We should use here the timestamp from the header to look for the right runId
     ** and set up the parameters for the analysis of the root files.
     ** std::cout << fEvtHeader->GetTimeStamp() << std::endl;
     **/

    printf("Processed: \033[32m %d \033[0m of \033[34m %d \033[0m (\033[33m %.2f \033[0m of 100), current RunId "
           "\033[31m %d \033[0m \r",
           i + 1,
           fNoOfEntries,
           100. * i / (double)(fNoOfEntries),
           fRunId);
    fflush(stdout);

    if (nextts >= 0 && prevts >= 0 && (fEvtHeader->GetTimeStamp() > nextts || fEvtHeader->GetTimeStamp() < prevts))
    {
        fRunId = GetRunid(fEvtHeader->GetTimeStamp());
    }

    fCurrentEntryNo = i;
    fEventTime = GetEventTime();
    if (fInChain->GetEntry(i))
        return 0;

    return 1;
}

void R3BFileSource::AddFriend(TString fName) { fFriendFileList.push_back(fName); }

void R3BFileSource::AddFile(TString FileName) { fInputChainList.push_back(FileName); }

void R3BFileSource::AddFriendsToChain()
{
    // Loop over all Friend files and extract the type. The type is defined by
    // the tree which is stored in the file. If there is already a chain of with
    // this type of tree then the file will be added to this chain.
    // If there is no such chain it will be created.
    //
    // Check if the order of runids and the event numbers per runid for all
    // friend chains is the same as the one defined by the input chain.
    // TODO: Should the order be corrected or should the execution be stopped.
    // The order in the input tree defined by the order in which the files have
    // been added. A file is defined by the runid.

    // In the old way it was needed sometimes to add a freind file more
    // than once. This is not needed any longer, so we remove deuplicates
    // from the list and display a warning.
    std::list<TString> friendList;
    for (auto fileName : fFriendFileList)
    {
        if (find(friendList.begin(), friendList.end(), fileName) == friendList.end())
        {
            friendList.push_back(fileName);
        }
    }
    // TODO: print a warning if it was neccessary to remove a filname from the
    // list. This can be chacked by comparing the size of both list

    TFile* temp = gFile;

    Int_t friendType = 1;
    // Loop over all files which have been added as friends
    for (auto fileName : friendList)
    {
        TString inputLevel;
        // Loop over all already defined input levels to check if this type
        // of friend tree is already added.
        // If this type of friend tree already exist add the file to the
        // then already existing friend chain. If this type of friend tree
        // does not exist already create a new friend chain and add the file.
        Bool_t inputLevelFound = kFALSE;
        TFile* inputFile;
        for (auto level : fInputLevel)
        {
            inputLevel = level;

            inputFile = TFile::Open(fileName);
            if (inputFile->IsZombie())
            {
                LOG(fatal) << "Error opening the file " << level.Data()
                           << " which should be added to the input chain or as friend chain";
            }

            // Check if the branchlist is already stored in the map. If it is
            // already stored add the file to the chain.
            Bool_t isOk = CompareBranchList(inputFile, inputLevel);
            if (isOk)
            {
                inputLevelFound = kTRUE;
                inputFile->Close();
                continue;
            }
            inputFile->Close();
        }
        if (!inputLevelFound)
        {
            inputLevel = Form("FriendTree_%i", friendType);
            CreateNewFriendChain(fileName, inputLevel);
            friendType++;
        }

        TChain* chain = static_cast<TChain*>(fFriendTypeList[inputLevel]);
        chain->AddFile(fileName, 1234567890, FairRootManager::GetTreeName());
    }
    gFile = temp;

    // Check if all friend chains have the same runids and the same
    // number of event numbers as the corresponding input chain
    // CheckFriendChains();

    // Add all the friend chains which have been created to the main input chain.
    for (const auto& mi : fFriendTypeList)
    {
        TChain* chain = static_cast<TChain*>(mi.second);
        fInChain->AddFriend(chain);
    }

    // Print some output about the input structure
    PrintFriendList();
}

void R3BFileSource::PrintFriendList()
{
    // Print information about the input structure
    // List files from the input chain together with all files of
    // all friend chains

    LOG(INFO) << "The input consists out of the following trees and files: ";
    LOG(INFO) << " - " << fInChain->GetName();
    TObjArray* fileElements = fInChain->GetListOfFiles();
    TIter next(fileElements);
    TChainElement* chEl = 0;
    while ((chEl = static_cast<TChainElement*>(next())))
    {
        LOG(INFO) << "    - " << chEl->GetTitle();
    }

    for (const auto& mi : fFriendTypeList)
    {
        TChain* chain = static_cast<TChain*>(mi.second);
        LOG(INFO) << " - " << chain->GetName();
        fileElements = chain->GetListOfFiles();
        TIter next1(fileElements);
        chEl = 0;
        while ((chEl = static_cast<TChainElement*>(next1())))
        {
            LOG(INFO) << "    - " << chEl->GetTitle();
        }
    }
}

void R3BFileSource::CheckFriendChains()
{
    std::multimap<TString, std::multimap<TString, TArrayI>>::iterator it1;
    std::multimap<TString, TArrayI> map1;

    // Get the structure from the input chain
    it1 = fRunIdInfoAll.find("InputChain");
    map1 = it1->second;
    std::vector<Int_t> runid;
    std::vector<Int_t> events;
    for (auto& mmi : map1)
    {
        TArrayI bla = mmi.second;
        runid.push_back(bla[0]);
        events.push_back(bla[1]);
    }

    // Now loop over all chains except the input chain and comapare the
    // runids and event numbers.
    // If there is a mismatch stop the execution.
    Int_t errorFlag = 0;
    TString inputLevel;
    for (auto level : fInputLevel)
    {
        inputLevel = level;
        if (!inputLevel.Contains("InputChain"))
        {
            it1 = fRunIdInfoAll.find(inputLevel);
            map1 = it1->second;
            if (runid.size() != map1.size())
            {
                errorFlag = 1;
                //        goto error_label;
                break;
            }
            Int_t counter = 0;
            for (auto& mmi : map1)
            {
                TArrayI bla = mmi.second;
                if ((bla[0] != runid[counter]) || (bla[1] != events[counter]))
                {
                    errorFlag = 2;
                    //          goto error_label;
                    break;
                }
                counter++;
            }
            if (errorFlag > 0)
            {
                break;
            }
        }
    }

    // Use goto to leave double loop at once in case of error
    // error_label:
    if (errorFlag > 0)
    {
        LOG(error) << "The input chain and the friend chain " << inputLevel.Data() << " have a different structure:";
        if (errorFlag == 1)
        {
            LOG(error) << "The input chain has the following runids and event numbers:";
            for (UInt_t i = 0; i < runid.size(); i++)
            {
                LOG(error) << " - Runid " << runid[i] << " with " << events[i] << " events";
            }
            LOG(error) << "The " << inputLevel.Data() << " chain has the following runids and event numbers:";
            for (auto& mmi : map1)
            {
                TArrayI bla = mmi.second;
                LOG(error) << " - Runid " << bla[0] << " with " << bla[1] << " events";
            }
        }
        if (errorFlag == 2)
        {
            Int_t counter = 0;
            for (auto& mmi : map1)
            {
                TArrayI bla = mmi.second;
                LOG(error) << "Runid Input Chain, " << inputLevel.Data() << " chain: " << bla[0] << ", "
                           << runid[counter];
                LOG(error) << "Event number Input Chain, " << inputLevel.Data() << " chain: " << bla[1] << ", "
                           << events[counter];
                counter++;
            }
        }
        LOG(fatal) << "Event structure mismatch";
    }
}

void R3BFileSource::CreateNewFriendChain(TString inputFile, TString inputLevel)
{

    TFile* temp = gFile;
    TFile* f = TFile::Open(inputFile);

    TFolder* added = NULL;
    TString folderName1 = FairRootManager::GetFolderName();
    TString folderName = Form("/%s", folderName1.Data());
    added = dynamic_cast<TFolder*>(f->Get(folderName1));
    if (!added)
    {
        folderName = "/cbmout";
        folderName1 = "cbmout";
        added = dynamic_cast<TFolder*>(f->Get("cbmout"));
        if (!added)
        {
            folderName = "/cbmroot";
            folderName1 = "cbmroot";
            added = dynamic_cast<TFolder*>(f->Get("cbmroot"));
            if (!added)
            {
                LOG(fatal) << "Could not find folder cbmout nor cbmroot.";
                exit(-1);
            }
        }
    }
    folderName1 = folderName1 + "_" + inputLevel;
    added->SetName(folderName1);
    fListFolder->Add(added);

    /**Get The list of branches from the friend file and add it to the actual list*/
    TList* list = dynamic_cast<TList*>(f->Get("BranchList"));
    TString chainName = inputLevel;
    fInputLevel.push_back(chainName);
    fCheckInputBranches[chainName] = new std::list<TString>;
    if (list)
    {
        TObjString* Obj = 0;
        for (Int_t i = 0; i < list->GetEntries(); i++)
        {
            Obj = dynamic_cast<TObjString*>(list->At(i));
            fCheckInputBranches[chainName]->push_back(Obj->GetString().Data());
            std::cout << Obj->GetString().Data() << std::endl;
            FairRootManager::Instance()->AddBranchToList(Obj->GetString().Data());
        }
    }

    TChain* chain = new TChain(inputLevel, folderName);
    fFriendTypeList[inputLevel] = chain;

    f->Close();
    gFile = temp;
}

Bool_t R3BFileSource::CompareBranchList(TFile* fileHandle, TString inputLevel)
{
    // fill a set with the original branch structure
    // This allows to use functions find and erase
    std::set<TString> branches;
    for (auto li : *(fCheckInputBranches[inputLevel]))
    {
        branches.insert(li);
    }

    // To do so we have to loop over the branches in the file and to compare
    // the branches in the file with the information stored in
    // fCheckInputBranches["InputChain"]. If both lists are equal everything
    // is okay

    // Get The list of branches from the input file one by one and compare
    // it to the reference list of branches which is defined for this tree.
    // If a branch with the same name is found, this branch is removed from
    // the list. If in the end no branch is left in the list everything is
    // fine.
    set<TString>::iterator iter1;
    TList* list = dynamic_cast<TList*>(fileHandle->Get("BranchList"));
    if (list)
    {
        TObjString* Obj = 0;
        for (Int_t i = 0; i < list->GetEntries(); i++)
        {
            Obj = dynamic_cast<TObjString*>(list->At(i));
            iter1 = branches.find(Obj->GetString().Data());
            if (iter1 != branches.end())
            {
                branches.erase(iter1);
            }
            else
            {
                // Not found is an error because branch structure is
                // different. It is impossible to add to tree with a
                // different branch structure
                return kFALSE;
            }
        }
    }
    // If the size of branches is !=0 after removing all branches also in the
    // reference list, this is also a sign that both branch list are not the
    // same
    if (branches.size() != 0)
    {
        LOG(INFO) << "Compare Branch List will return kFALSE. The list has " << branches.size() << " branches:";
        for (auto branchName : branches)
            LOG(INFO) << "  -> " << branchName;
        return kFALSE;
    }

    return kTRUE;
}

void R3BFileSource::SetInputFile(TString name)
{
    fRootFile = TFile::Open(name.Data());
    if (fRootFile->IsZombie())
    {
        LOG(fatal) << "Error opening the Input file";
    }
    LOG(INFO) << "R3BFileSource set------------";
}

Int_t R3BFileSource::CheckMaxEventNo(Int_t EvtEnd)
{
    Int_t MaxEventNo = 0;
    if (EvtEnd != 0)
    {
        MaxEventNo = EvtEnd;
    }
    else
    {
        MaxEventNo = fInChain->GetEntries();
    }
    return MaxEventNo;
}

ClassImp(R3BFileSource);
