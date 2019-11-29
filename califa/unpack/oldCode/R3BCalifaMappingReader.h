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

#ifndef _R3BCALIFAMAPPINGREADER_H_
#define _R3BCALIFAMAPPINGREADER_H_

#include <map>
#include <string>

#include <FairTask.h>

class TClonesArray;

class R3BCalifaMappingReader : public FairTask
{
  public:
    R3BCalifaMappingReader(const char* filename);

    virtual void Exec(Option_t* option);

  protected:
    virtual InitStatus Init();

    std::string fname;
    TClonesArray* fCrystalCalDataCA;
    std::map<int, int> channelMap;

  public:
    ClassDef(R3BCalifaMappingReader, 0)
};

#endif
