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

#ifndef R3BWhiterabbitCalifaReader_H
#define R3BWhiterabbitCalifaReader_H
#include "R3BReader.h"

struct EXT_STR_h101_WRCALIFA_t;
typedef struct EXT_STR_h101_WRCALIFA_t EXT_STR_h101_WRCALIFA;

class FairLogger;
class TClonesArray;
class R3BEventHeader;

class R3BWhiterabbitCalifaReader : public R3BReader
{
  public:
    R3BWhiterabbitCalifaReader(EXT_STR_h101_WRCALIFA*, UInt_t, UInt_t, UInt_t);
    ~R3BWhiterabbitCalifaReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    /* An event counter */
    UInt_t fNEvent;
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_WRCALIFA* fData;
    /* Offset of detector specific data in full data structure */
    UInt_t fOffset;
    /* FairLogger */
    FairLogger* fLogger;
    /* The whiterabbit subsystem ID */
    UInt_t fWhiterabbitId1;
    UInt_t fWhiterabbitId2;
    /* A pointer to the R3BEventHeader structure */
    R3BEventHeader* fEventHeader;
    // Don't store data for online
    Bool_t fOnline;
    /**< Output array. */
    TClonesArray* fArray;

  public:
    ClassDef(R3BWhiterabbitCalifaReader, 0);
};
#endif
