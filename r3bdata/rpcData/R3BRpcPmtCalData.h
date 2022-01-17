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

#ifndef R3BRPCPMTCALDATA_H
#define R3BRPCPMTCALDATA_H

#include "FairMultiLinkedData.h"
#include "TObject.h"

class R3BRpcPmtCalData : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BRpcPmtCalData();


    /** Standard Constructor
     *@param channelId   Channel unique identifier
     *@param Time        Time (T/B) 
     *@param Tot         Time over threshold (T/B)
     **/
    R3BRpcPmtCalData(UShort_t channelId, double TimeTop, double TimeBottom, double TotTop, double TotBottom);

    /** Copy constructor **/
    R3BRpcPmtCalData(const R3BRpcPmtCalData&);

    R3BRpcPmtCalData& operator=(const R3BRpcPmtCalData&) { return *this; }

    /** Destructor **/
    virtual ~R3BRpcPmtCalData() {}

    /** Accessors **/
    inline UShort_t GetChannelId() const { return fChannelId; }
    inline double GetTimeBottom() const { return fTimeBottom; }
    inline double GetTimeTop() const { return fTimeTop; }
    inline double GetTotBottom() const { return fTotBottom; }
    inline double GetTotTop() const { return fTotTop; }

  protected:
    UShort_t fChannelId;     // Channel unique identifier
    double fTimeTop;         // Time from top channel
    double fTimeBottom;      // Time from Bottom channel
    double fTotTop;          // Tot from the bottom channel
    double fTotBottom;       // Tot from the bottom channel

  public:
    ClassDef(R3BRpcPmtCalData, 1)
};

#endif
