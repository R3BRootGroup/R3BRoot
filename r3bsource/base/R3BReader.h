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

#ifndef _R3BREADER_H
#define _R3BREADER_H 1

#include "TObject.h"
#include "TString.h"

extern "C"
{
#include "ext_data_struct_info.hh"
}

class TClonesArray;
class R3BEventHeader;

class R3BReader : public TObject
{
  public:
    R3BReader(TString const&);
    virtual ~R3BReader();

    /* Setup structure information */
    virtual Bool_t Init(ext_data_struct_info*) = 0;
    virtual void SetParContainers() {}
    virtual Bool_t ReInit() { return kTRUE; }
    /* Read data from full event structure */
    virtual Bool_t Read() = 0;
    /* Reset */
    virtual void Reset() = 0;
    /* Return actual name of the reader */
    const char* GetName() { return fName.Data(); }

  protected:
    TString fName;

  public:
    ClassDef(R3BReader, 0);
};

#endif /* _R3BREADER_H */
