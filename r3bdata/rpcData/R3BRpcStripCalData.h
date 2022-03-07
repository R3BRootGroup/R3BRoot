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

#ifndef R3BRPCSTRIPCALDATA_H
#define R3BRPCSTRIPCALDATA_H

#include "FairMultiLinkedData.h"
#include "TObject.h"

class R3BRpcStripCalData : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BRpcStripCalData();


    /** Standard Constructor
     *@param channelId   Channel unique identifier
     *@param Time        Time (R/L) 
     *@param Tot         Time over threshold (R/L)
     **/
    R3BRpcStripCalData(UShort_t channelId, double TimeRight, double TimeLeft, double TotRight, double TotLeft);

    /** Copy constructor **/
    R3BRpcStripCalData(const R3BRpcStripCalData&);

    R3BRpcStripCalData& operator=(const R3BRpcStripCalData&) { return *this; }

    /** Destructor **/
    virtual ~R3BRpcStripCalData() {}

    /** Accessors **/
    inline UShort_t GetChannelId() const { return fChannelId; }
    inline double GetTimeLeft() const { return fTimeLeft; }
    inline double GetTimeRight() const { return fTimeRight; }
    inline double GetTotLeft() const { return fTotLeft; }
    inline double GetTotRight() const { return fTotRight; }

  protected:
    UShort_t fChannelId;     // Channel unique identifier
    double fTimeRight;         // Time from Right channel
    double fTimeLeft;      // Time from Left channel
    double fTotRight;          // Tot from the Right channel
    double fTotLeft;       // Tot from the Left channel

  public:
    ClassDef(R3BRpcStripCalData, 1)
};

#endif
