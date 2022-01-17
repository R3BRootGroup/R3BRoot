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

#ifndef R3BRPCSTRIPPRECALDATA_H
#define R3BRPCSTRIPPRECALDATA_H

#include "FairMultiLinkedData.h"
#include "TObject.h"

class R3BRpcStripPreCalData : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BRpcStripPreCalData();


    /** Standard Constructor
     *@param channelId   Channel unique identifier
     *@param Time        Time
     *@param Tot         Coarse Time
     *@param Side        left or right
     **/
    R3BRpcStripPreCalData(UShort_t channelId, double Time, double Tot, UShort_t Side);

    /** Copy constructor **/
    R3BRpcStripPreCalData(const R3BRpcStripPreCalData&);

    R3BRpcStripPreCalData& operator=(const R3BRpcStripPreCalData&) { return *this; }

    /** Destructor **/
    virtual ~R3BRpcStripPreCalData() {}

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
    ClassDef(R3BRpcStripPreCalData, 1)
};

#endif
