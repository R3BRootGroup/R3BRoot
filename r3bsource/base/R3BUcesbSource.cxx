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
#include "R3BLogger.h"
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
    , fEventHeader(nullptr)
    , fInputFile()
    , fEntryMax(0)
    , fReaders(new TObjArray())
    , fSkip(0)
{
}

R3BUcesbSource::~R3BUcesbSource()
{
    R3BLOG(DEBUG1, "R3BUcesbSource destructor.");
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
        R3BLOG(fatal, "popen() failed");
        return kFALSE;
    }

    /* Connect to forked instance */
    status = fClient.connect(fileno(fFd));
    if (kFALSE == status)
    {
        perror("ext_data_clnt::connect()");
        R3BLOG(error, "ext_data_clnt::connect() failed");
        R3BLOG(fatal, "ucesb error: " << fClient.last_error());
        return kFALSE;
    }

    // Register of R3BEventHeader in the output root file
    FairRootManager* frm = FairRootManager::Instance();
    R3BLOG_IF(FATAL, !frm, "FairRootManager no found");

    R3BLOG(INFO, "Register of R3BEventHeader");
    fEventHeader = new R3BEventHeader();
    frm->Register("EventHeader.", "R3BEvtHeader", fEventHeader, kTRUE);

    // Open configuration file with runid values if needed in this step
    fInputFile.open(fInputFileName.Data(), std::fstream::in);
    if (!fInputFile.is_open())
    {
        R3BLOG(WARNING, "Input file for RunIds could not be open, it is Ok!");
    }
    else
    {
        R3BLOG(INFO, "Input file for RunIds " << fInputFileName.Data() << " is open!");
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
            R3BLOG(fatal, "ucesb error: " << fClient.last_error());
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
        R3BLOG(ERROR, "ext_data_clnt::setup() failed");
        R3BLOG(fatal, "ucesb error: " << fClient.last_error());
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
        R3BLOG(ERROR, "ext_data_clnt::setup() failed");
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
            R3BLOG(fatal, "ReInit of a reader failed.");
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

    R3BLOG(debug1, "ReadEvent " << fNEvent);

    fNEvent++;

    if (fNEvent > fEntryMax && fEntryMax != -1 && fInputFile.is_open())
    {

        R3BLOG(INFO, "ReadEvent()");

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
        LOG(info) << "End of input";
        return 1;
    }
    if (-1 == ret)
    {
        perror("ext_data_clnt::fetch_event()");
        R3BLOG(error, "ext_data_clnt::fetch_event() failed");
        R3BLOG(fatal, "ucesb error: " << fClient.last_error());
        return 0;
    }

    /* Get raw data, if any */
    ret = fClient.get_raw_data(&raw, &raw_words);
    if (0 != ret)
    {
        perror("ext_data_clnt::get_raw_data()");
        R3BLOG(fatal, "Failed to get raw data.");
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

    if (fSkip)
    {
        if (fEventHeader->GetTpat() > 0)
        {
            FairRunOnline::Instance()->MarkFill(kTRUE);
        }
        else
        {
            FairRunOnline::Instance()->MarkFill(kFALSE);
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
        R3BLOG(fatal, "ext_data_clnt::close() failed");
    }

    /* Close pipe */
    if (nullptr != fFd)
    {
        int status;
        status = pclose(fFd);
        if (-1 == status)
        {
            perror("pclose()");
            R3BLOG(fatal, "pclose() failed");
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

Bool_t R3BUcesbSource::SpecifyRunId()
{
    if (ReadEvent(0) == 0)
        return true;
    return false;
}

//_____________________________________________________________________________
void R3BUcesbSource::FillEventHeader(FairEventHeader* feh) { ((R3BEventHeader*)feh)->SetRunId(fRunId); }

ClassImp(R3BUcesbSource);
