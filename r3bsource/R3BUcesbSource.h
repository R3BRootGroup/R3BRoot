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

/* R3BUcesbSource.h
 * R3BROOT
 *
 * Author: Bastian Löher <b.loeher@gsi.de>
 * Date: 29/05/15
 * */

#ifndef __R3BROOT__R3BUCESBSOURCE__
#define __R3BROOT__R3BUCESBSOURCE__

#include "FairSource.h"
#include "R3BReader.h"
#include "TObjArray.h"
#include "TString.h"

/* External data client interface (ucesb) */
#include "ext_data_clnt.hh"
#include "ext_data_struct_info.hh"

/* External data structure
 * This structure is produced using ucesb in the following way:
 * ./ucesb --ntuple=UNPACK:EVENTNO,UNPACK:TRIGGER,RAW,STRUCT_HH,ext_h101.h
 * */
struct EXT_STR_h101_t;
typedef struct EXT_STR_h101_t EXT_STR_h101;
/*#include "ext_h101.h"*/

class FairLogger;

class R3BUcesbSource : public FairSource
{
  public:
    R3BUcesbSource(const TString&, const TString&, const TString&, EXT_STR_h101*, size_t);
    ~R3BUcesbSource();

    Source_Type GetSourceType() { return kONLINE; }

    /* Init() will fork a ucesb instance to deliver the unpacked data for
     * R3Broot. It makes use of the ext_data_ interface of ucesb.
     * */
    Bool_t Init();
    Bool_t InitUnpackers();
    void SetParUnpackers();
    Bool_t ReInitUnpackers();
    /* A wrapper for ext_data_fetch_event() */
    Int_t ReadEvent(UInt_t);
    /* Close the ext_data_ interface */
    void Close();
    /* Reset the readers */
    void Reset();
    /* The reader interface */
    void AddReader(R3BReader* a_reader) { fReaders->Add(a_reader); }
    /* Limit the number of events */
    void SetMaxEvents(int a_max) { fLastEventNo = a_max; }
    /* Get readers */
    const TObjArray* GetReaders() const { return fReaders; }

  private:
    /* File descriptor returned from popen() */
    FILE* fFd;
    /* The ucesb interface class */
    ext_data_clnt fClient;
    /* The ucesb structure info class */
    ext_data_struct_info fStructInfo;
    /* The filename to read */
    const TString fFileName;
    /* Ucesb output options */
    const TString fNtupleOptions;
    /* The location of the unpacker */
    const TString fUcesbPath;
    /* The event counter */
    unsigned int fNEvent;
    /* The full event structure */
    EXT_STR_h101* fEvent;
    size_t fEventSize;
    /* Last event requested */
    int fLastEventNo;
    /* FairLogger */
    FairLogger* fLogger;
    /* The array of readers */
    TObjArray* fReaders;

  public:
    /* Create dictionary */
    ClassDef(R3BUcesbSource, 1)
};

#endif
