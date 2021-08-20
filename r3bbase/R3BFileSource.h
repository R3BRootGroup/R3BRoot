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

#ifndef R3BFileSource_H
#define R3BFileSource_H

#include "FairFileSource.h"
#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TFolder.h"
#include "TMCtls.h"
#include <Rtypes.h>
#include <fstream>
#include <list>

class FairEventHeader;
class FairFileHeader;
class FairMCEventHeader;
class TString;
class FairLogger;
class FairRuntimeDb;
class R3BEventHeader;

class R3BFileSource : public FairFileSource
{
  public:
    R3BFileSource(TFile* f, const char* Title = "InputRootFile", UInt_t identifier = 0);
    R3BFileSource(const TString* RootFileName, const char* Title = "InputRootFile", UInt_t identifier = 0);
    R3BFileSource(const TString RootFileName, const char* Title = "InputRootFile", UInt_t identifier = 0);

    virtual ~R3BFileSource();

    /**
     * static instance
     */
    static R3BFileSource* Instance();

    Bool_t Init() override;
    Int_t ReadEvent(UInt_t i = 0) override;

    /**Check the maximum event number we can run to*/
    virtual Int_t CheckMaxEventNo(Int_t EvtEnd = 0) override;

    const TFile* GetRootFile() { return fRootFile; }
    /** Add a friend file (input) by name)*/
    void AddFriend(TString FileName);
    /**Add ROOT file to input, the file will be chained to already added files*/
    void AddFile(TString FileName);
    void AddFriendsToChain();
    void PrintFriendList();
    Bool_t CompareBranchList(TFile* fileHandle, TString inputLevel);
    void CheckFriendChains();
    void CreateNewFriendChain(TString inputFile, TString inputLevel);
    TTree* GetInTree() { return fInChain->GetTree(); }
    TChain* GetInChain() { return fInChain; }
    TFile* GetInFile() { return fRootFile; }
    void CloseInFile()
    {
        if (fRootFile)
        {
            fRootFile->Close();
        }
    }
    /**Set the input tree when running on PROOF worker*/
    void SetInTree(TTree* tempTree);
    TObjArray* GetListOfFolders() { return fListFolder; }
    TFolder* GetBranchDescriptionFolder() { return fCbmroot; }
    UInt_t GetEntries() { return fNoOfEntries; }

    void SetInputFile(TString name);

    void SetFileHeader(FairFileHeader* f) { fFileHeader = f; }

    /**Set the status of the EvtHeader
     *@param Status:  True: The header was creatged in this session and has to be filled
              FALSE: We use an existing header from previous data level
     */
    void SetEvtHeaderNew(Bool_t Status) { fEvtHeaderIsNew = Status; }
    Bool_t IsEvtHeaderNew() { return fEvtHeaderIsNew; }

    /** Allow to disable the testing the file layout when adding files to a chain.
     */
    void SetCheckFileLayout(Bool_t enable) { fCheckFileLayout = enable; }

    void SetInputFileName(TString tstr) { fInputFileName = tstr; }

  private:
    // static pointer to this class
    static TMCThreadLocal R3BFileSource* fSourceInstance;
    /** Title of input source, could be input, background or signal*/
    TString fInputTitle;
    /**ROOT file*/
    TFile* fRootFile;
    /** Current Entry number */
    Int_t fCurrentEntryNr; //!
    /** List of all files added with AddFriend */
    std::list<TString> fFriendFileList;                               //!
    std::list<TString> fInputChainList;                               //!
    std::map<TString, TChain*> fFriendTypeList;                       //!
    std::map<TString, std::list<TString>*> fCheckInputBranches;       //!
    std::list<TString> fInputLevel;                                   //!
    std::map<TString, std::multimap<TString, TArrayI>> fRunIdInfoAll; //!
    /**Input Chain */
    TChain* fInChain;
    /**Input Tree */
    TTree* fInTree;
    /** list of folders from all input (and friends) files*/
    TObjArray* fListFolder; //!
    /** RuntimeDb*/
    FairRuntimeDb* fRtdb;
    /**folder structure of output*/
    TFolder* fCbmout;
    /**folder structure of input*/
    TFolder* fCbmroot;
    /***/
    UInt_t fSourceIdentifier;
    /**No of Entries in this source*/
    UInt_t fNoOfEntries;
    /**Initialization flag, true if initialized*/
    Bool_t IsInitialized;

    /** MC Event header */
    FairMCEventHeader* fMCHeader; //!

    /**Event Header*/
    R3BEventHeader* fEvtHeader; //!

    /**File Header*/
    FairFileHeader* fFileHeader; //!

    /** This is true if the event time used, came from simulation*/
    Bool_t fEventTimeInMCHeader; //!
    /**This flag is true if the event header was created in this session
     * otherwise it is false which means the header was created in a previous data
     * level and used here (e.g. in the digi)
     */
    Bool_t fEvtHeaderIsNew; //!

    /** for internal use, to return the same event time for the same entry*/
    UInt_t fCurrentEntryNo; //!
    /** for internal use, to return the same event time for the same entry*/
    UInt_t fTimeforEntryNo; //!

    /** Time of event since th start (ns) */
    Double_t fEventTime; //
    /** True if the file layout should be checked when adding files to a chain.
     *  Default value is true.
     */
    Bool_t fCheckFileLayout; //!

    /** GetRunid method to obtain the runid as function of timestamps */
    Int_t GetRunid(uint64_t ts);

    /** input file with runids */
    std::ifstream fInputFile;
    TString fInputFileName;

    UInt_t fExpid;
    std::vector<UInt_t> fRunid;
    std::vector<uint64_t> fTimestamp;
    uint64_t prevts, nextts;

    ClassDefOverride(R3BFileSource, 0)
};

#endif // R3BFileSource_H
