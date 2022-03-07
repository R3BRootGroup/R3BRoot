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

#ifndef R3BRpcRefMappedData_H
#define R3BRpcRefMappedData_H

#include "TObject.h"
#include <stdint.h>

class R3BRpcRefMappedData : public TObject
{

  public:
    // Default Constructor
    R3BRpcRefMappedData();

    /** Standard Constructor
     *@param channelId   Channel unique identifier
     *@param fineTime    Fine Time
     *@param coarseTime  Coarse Time
     *@param edge        Leading or Trailing
     **/
    R3BRpcRefMappedData(UShort_t channelId, uint64_t fineTime, uint64_t coarseTime);

    // Destructor
    virtual ~R3BRpcRefMappedData() {}

    // Getters
    inline UShort_t GetChannelId() const { return fChannelId; }
    inline uint64_t GetCoarseTime() const { return fCoarseTime; }
    inline uint64_t GetFineTime() const { return fFineTime; }

  protected:
    UShort_t fChannelId;     // Channel unique identifier
    uint64_t fFineTime;      // Fine time
    uint64_t fCoarseTime;    // Coarse time

  public:
    ClassDef(R3BRpcRefMappedData, 1)
};

#endif
