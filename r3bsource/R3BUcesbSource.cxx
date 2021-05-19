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

#include <iostream>
#include <sstream>
#include <string>

#include "FairLogger.h"
#include "FairRootManager.h"
#include "R3BEventHeader.h"
#include "R3BUcesbSource.h"

#include "ext_data_client.h"

R3BUcesbSource::R3BUcesbSource(const TString& FileName,
                               const TString& NtupleOptions,
                               const TString& UcesbPath,
                               EXT_STR_h101* event,
                               size_t event_size)
    : FairSource()
    , fFd(nullptr)
    , fClient()
    , fStructInfo()
    , fFileName(FileName)
    , fNtupleOptions(NtupleOptions)
    , fUcesbPath(UcesbPath)
    , fNEvent(0)
    , fEvent(event)
    , fEventSize(event_size)
    , fLastEventNo(-1)
    , fLogger(FairLogger::GetLogger())
    , fEventHeader(nullptr)
    , fInputFile()
    , fEntryMax(0)
    , fReaders(new TObjArray())
{
}

R3BUcesbSource::~R3BUcesbSource()
{
    if (fReaders)
    {
        fReaders->Delete();
        delete fReaders;
    }
    if (fEventHeader)
        delete fEventHeader;
    Close();
}

Bool_t R3BUcesbSource::Init()
{
    Bool_t status;
    std::ostringstream command;

    /* Call ucesb with this command */
    command << fUcesbPath << " " << fFileName << " "
            << "--ntuple=" << fNtupleOptions << ",STRUCT,-";

    if (fLastEventNo != -1)
    {
        command << " --max-events=" << fLastEventNo;
    }

    LOG(INFO) << "Calling ucesb with command: " << command.str();

    /* Fork off ucesb (calls fork() and pipe()) */
    fFd = popen(command.str().c_str(), "r");
    if (nullptr == fFd)
    {
        perror("popen()");
        LOG(fatal) << "popen() failed";
        return kFALSE;
    }

    /* Connect to forked instance */
    status = fClient.connect(fileno(fFd));
    if (kFALSE == status)
    {
        perror("ext_data_clnt::connect()");
        LOG(error) << "ext_data_clnt::connect() failed";
        LOG(fatal) << "ucesb: " << fClient.last_error();
        return kFALSE;
    }

    // Register of R3BEventHeader in the output root file
    FairRootManager* frm = FairRootManager::Instance();
    if (!frm)
        LOG(FATAL) << "R3BUcesbSource::FairRootManager no found";
    LOG(INFO) << "R3BUcesbSource::Register of R3BEventHeader";
    fEventHeader = new R3BEventHeader();
    frm->Register("EventHeader.", "R3BEvtHeader", fEventHeader, kTRUE);

    // Open configuration file with runid values if needed in this step
    fInputFile.open(fInputFileName.Data(), std::fstream::in);
    if (!fInputFile.is_open())
    {
        LOG(WARNING) << "R3BUcesbSource::Init() Input file for RunIds could not be open, it is Ok!";
    }
    else
    {
        LOG(INFO) << "R3BUcesbSource::Init() Input file for RunIds " << fInputFileName.Data() << " is open!";
        fInputFile.clear();
        fInputFile.seekg(0, std::ios::beg);
    }

    return kTRUE;
}

Bool_t R3BUcesbSource::InitUnpackers()
{
    /* Initialize all readers */
    for (int i = 0; i < fReaders->GetEntriesFast(); ++i)
    {
        if (!((R3BReader*)fReaders->At(i))->Init(&fStructInfo))
        {
            LOG(fatal) << "ucesb: " << fClient.last_error();
            return kFALSE;
        }
    }

    /* Setup client */
#ifdef EXT_DATA_ITEM_MAP_MATCH
    /* this is the version for ucesb setup with extended mapping info */
    uint32_t struct_map_success = 0;
    Bool_t status = fClient.setup(NULL, 0, &fStructInfo, &struct_map_success, fEventSize);
#else
    Bool_t status = fClient.setup(NULL, 0, &fStructInfo, fEventSize);
#endif
    if (status != 0)
    {
        perror("ext_data_clnt::setup()");
        LOG(error) << "ext_data_clnt::setup() failed";
        LOG(fatal) << "ucesb: %s" << fClient.last_error();
        return kFALSE;
    }
#ifdef EXT_DATA_ITEM_MAP_MATCH
    /*
     * It is not needed, that *all* items are matched.
     * However, mapping should fail, if items are requested that don't exist
     * on the server, or if items are requested with wrong parameters.
     * See ucesb/hbook/ext_data_client.h for more information.
     */
    uint32_t map_ok = EXT_DATA_ITEM_MAP_OK | EXT_DATA_ITEM_MAP_NO_DEST;
    if (struct_map_success & ~(map_ok))
    {
        perror("ext_data_clnt::setup()");
        LOG(error) << "ext_data_clnt::setup() failed";
        ext_data_struct_info_print_map_success(fStructInfo, stderr, map_ok);
        return kFALSE;
    }
#endif

    return kTRUE;
}

void R3BUcesbSource::SetParUnpackers()
{
    for (int i = 0; i < fReaders->GetEntriesFast(); ++i)
    {
        ((R3BReader*)fReaders->At(i))->SetParContainers();
    }
}

Bool_t R3BUcesbSource::ReInitUnpackers()
{
    /* Initialize all readers */
    for (int i = 0; i < fReaders->GetEntriesFast(); ++i)
    {
        if (!((R3BReader*)fReaders->At(i))->ReInit())
        {
            LOG(fatal) << "ReInit of a reader failed.";
            return kFALSE;
        }
    }

    return kTRUE;
}

Int_t R3BUcesbSource::ReadIntFromString(const std::string& wholestr, const std::string& pattern)
{
    std::string tempstr = wholestr;
    tempstr.replace(0, tempstr.find(pattern) + pattern.length(), "");
    tempstr.replace(0, tempstr.find('=') + 1, "");
    return atoi(tempstr.c_str());
}

Int_t R3BUcesbSource::ReadEvent(UInt_t i)
{
    const void* raw;
    ssize_t raw_words;

    int ret;
    (void)i; /* Why is i not used? Outer loop seems not to use it. */

    LOG(debug1) << "R3BUcesbSource::ReadEvent " << fNEvent;

    fNEvent++;

    if (fNEvent > fEntryMax && fEntryMax != -1 && fInputFile.is_open())
    {

        LOG(INFO) << "R3BUcesbSource::ReadEvent()";

        std::string buffer;
        do
        {
            getline(fInputFile, buffer);
            LOG(INFO) << "read from file: \"" << buffer << "\"";
            if (buffer.find("EVENT BEGIN") == 0)
            {
                fRunId = ReadIntFromString(buffer, "RUNID");
                fEventHeader->SetRunId(fRunId);
            }
            if (buffer.find("EVENT") == 0)
                continue;
            Int_t fInit = atoi(buffer.c_str());
            buffer.erase(0, buffer.find(' ') + 1);
            fEntryMax = atoi(buffer.c_str());

        } while (fInputFile && buffer.compare("EVENT END"));
    }

    /* Need to initialize first */
    if (nullptr == fFd)
    {
        Init();
    }

    /* Fetch data */
    ret = fClient.fetch_event(fEvent, fEventSize);
    if (0 == ret)
    {
        LOG(info) << "R3BUcesbSource::End of input";
        return 1;
    }
    if (-1 == ret)
    {
        perror("ext_data_clnt::fetch_event()");
        LOG(error) << "ext_data_clnt::fetch_event() failed";
        LOG(fatal) << "ucesb: " << fClient.last_error();
        return 0;
    }

    /* Get raw data, if any */
    ret = fClient.get_raw_data(&raw, &raw_words);
    if (0 != ret)
    {
        perror("ext_data_clnt::get_raw_data()");
        LOG(fatal) << "Failed to get raw data.";
        return 0;
    }

    /* Run detector specific readers */
    for (int r = 0; r < fReaders->GetEntriesFast(); ++r)
    {
        R3BReader* reader = (R3BReader*)fReaders->At(r);

        LOG(debug1) << "  Reading reader " << r << " (" << reader->GetName() << ")";
        reader->Read();
    }

    /* Display raw data */
    if (raw)
    {
        int w, j;
        const uint32_t* u = (const uint32_t*)raw;

        LOG(info) << "  Raw data:";
        for (w = 0; w < raw_words; w += 8)
        {
            printf("    RAW%4x:", w);
            for (j = 0; j < 8 && w + j < raw_words; j++)
                printf(" %08x", u[w + j]);
            printf("\n");
        }
    }

    return 0;
}

void R3BUcesbSource::Close()
{
    int ret;

    /* Close client connection */
    ret = fClient.close();
    if (0 != ret)
    {
        perror("ext_data_clnt::close()");
        LOG(fatal) << "ext_data_clnt::close() failed";
    }

    /* Close pipe */
    if (nullptr != fFd)
    {
        int status;
        status = pclose(fFd);
        if (-1 == status)
        {
            perror("pclose()");
            LOG(fatal) << "pclose() failed";
            abort();
        }
    }

    if (fInputFile.is_open())
        fInputFile.close();
}

void R3BUcesbSource::Reset()
{
    for (int i = 0; i < fReaders->GetEntriesFast(); ++i)
    {
        ((R3BReader*)fReaders->At(i))->Reset();
    }
}

//_____________________________________________________________________________
void R3BUcesbSource::FillEventHeader(FairEventHeader* feh) { ((R3BEventHeader*)feh)->SetRunId(fRunId); }

ClassImp(R3BUcesbSource)
