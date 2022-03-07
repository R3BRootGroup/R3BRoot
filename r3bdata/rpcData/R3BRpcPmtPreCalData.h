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

#ifndef R3BRPCPMTPRECALDATA_H
#define R3BRPCPMTPRECALDATA_H

#include "FairMultiLinkedData.h"
#include "TObject.h"

class R3BRpcPmtPreCalData : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BRpcPmtPreCalData();


    /** Standard Constructor
     *@param channelId   Channel unique identifier
     *@param Time        Time
     *@param Tot         Coarse Time
     *@param Side        left or right
     **/
    R3BRpcPmtPreCalData(UShort_t channelId, double Time, double Tot, UShort_t Side);

    /** Copy constructor **/
    R3BRpcPmtPreCalData(const R3BRpcPmtPreCalData&);

    R3BRpcPmtPreCalData& operator=(const R3BRpcPmtPreCalData&) { return *this; }

    /** Destructor **/
    virtual ~R3BRpcPmtPreCalData() {}

    /** Accessors **/
    inline UShort_t GetChannelId() const { return fChannelId; }
    inline double GetTime() const { return fTime; }
    inline double GetTot() const { return fTot; }
    inline UShort_t GetSide() const { return fSide; }

  protected:
    UShort_t fChannelId;     // Channel unique identifier
    double fTime;      // Fine time
    double fTot;    // Coarse time
    Short_t fSide;           // Top Or Bottom

  public:
    ClassDef(R3BRpcPmtPreCalData, 1)
};

#endif
