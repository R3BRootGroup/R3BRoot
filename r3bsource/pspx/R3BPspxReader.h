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

#ifndef R3BPSPXREADER_H
#define R3BPSPXREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_PSP_t;
typedef struct EXT_STR_h101_PSP_t EXT_STR_h101_PSP;
class FairLogger;
class TClonesArray;

/**
 * Class to unpack (with ucesb) to Mapped data for PSPX detector data.
 * This includes: Checking for error messages.
 * @author Ralf Plag (?), Bastian Loeher(?), Ina Syndikus
 * Modified by M. Holl, Dec 2019
 */

class R3BPspxReader : public R3BReader
{
  public:
    /** Standard Constructor **/
    R3BPspxReader(EXT_STR_h101_PSP*, UInt_t);
    /** Destructor **/
    ~R3BPspxReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }
    void Reset(); /**< Reset the output array **/

  private:
    EXT_STR_h101_PSP* fData; /**< Reader specific data structure from ucesb */
    UInt_t fOffset;          /**< Data Offset */
    FairLogger* fLogger;     /**< FairLogger */
    // Don't store data for online
    Bool_t fOnline;
    std::vector<TClonesArray*> fMappedItems; /**< Array holding output (Mapped) data */

  public:
    ClassDef(R3BPspxReader, 4);
};

#endif
