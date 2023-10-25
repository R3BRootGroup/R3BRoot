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

#ifndef R3BSyncCheckData_H
#define R3BSyncCheckData_H 1

#include "TObject.h"
#include <Rtypes.h>

class R3BSyncCheckData : public TObject
{
  public:
    R3BSyncCheckData();
    R3BSyncCheckData(uint32_t master,
                     uint32_t masterref,
                     uint32_t music,
                     uint32_t rpc,
                     uint32_t s2,
                     uint32_t foot1,
                     uint32_t foot2);

    virtual ~R3BSyncCheckData() {}

    inline const uint32_t& GetMaster() const { return fMaster; }
    inline const uint32_t& GetMasterRef() const { return fMasterRef; }
    inline const uint32_t& GetMusic() const { return fMusic; }
    inline const uint32_t& GetRpc() const { return fRpc; }
    inline const uint32_t& GetS2() const { return fS2; }
    inline const uint32_t& GetFoot1() const { return fFoot1; }
    inline const uint32_t& GetFoot2() const { return fFoot2; }

  protected:
    // following the order from ext file
    uint32_t fMaster;
    uint32_t fMasterRef;
    uint32_t fMusic;
    uint32_t fRpc;
    uint32_t fS2;
    uint32_t fFoot1;
    uint32_t fFoot2;

  public:
    ClassDef(R3BSyncCheckData, 1)
};

#endif /* R3BSyncCheckData_H */
