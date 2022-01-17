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

#ifndef R3BRPCMAPPEDDATA_H
#define R3BRPCMAPPEDDATA_H

#include "TObject.h"
#include <stdint.h>

class R3BRpcMappedData : public TObject
{

  public:
    // Default Constructor
    R3BRpcMappedData();

    /** Standard Constructor
     *@param channelId   Channel unique identifier
     *@param time        Internal time per crystal [ns]
     *@param wrts        Timestamp or time since event start in simulation [ns]
     *@param side        Left (0) or right (1)
     **/
    R3BRpcMappedData(UShort_t channelId, uint64_t time, uint64_t wrts, Int_t side);

    // Destructor
    virtual ~R3BRpcMappedData() {}

    // Getters
    inline const UShort_t& GetChannelId() const { return fChannelId; }
    inline const uint64_t& GetTime() const { return fTime; }
    inline const uint64_t& GetWrts() const { return fWrts; }
    inline const Int_t& GetSide() const { return fSide; }

  protected:
    UShort_t fChannelId; // Channel unique identifier
    uint64_t fTime;      // Internal time
    uint64_t fWrts;      // Timestamp per channel
    Int_t fSide;         // Left (0) or right (1) or ...

  public:
    ClassDef(R3BRpcMappedData, 1)
};

#endif
