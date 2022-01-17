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

#ifndef R3BRPCSTRIPHITDATA_H
#define R3BRPCSTRIPHITDATA_H

#include "TObject.h"

#include "FairMultiLinkedData.h"
#include "R3BRpcStripCalData.h"

class R3BRpcStripHitData : public FairMultiLinkedData
{

  public:
    /** Default constructor **/
    R3BRpcStripHitData();

    /** Constructor with arguments
     *@param fChannelId   Channel unique identifier
     *@param fPos         Position
     *@param fTime        Time
     *@param fCharge      Charge
     **/
    R3BRpcStripHitData(UInt_t channel, double time, double pos, double charge);

    /** Copy constructor **/
    R3BRpcStripHitData(const R3BRpcStripHitData&);
    R3BRpcStripHitData& operator=(const R3BRpcStripHitData&) { return *this; }

    /** Destructor **/
    virtual ~R3BRpcStripHitData() {}

    /** Accessors **/
    UInt_t GetChannelId() const { return fChannelId; }
    double GetTime() const { return fTime; }
    double GetPos() const { return fPos; }
    double GetCharge() const { return fCharge; }

    /** Modifiers **/
    void SetChannelId(UInt_t ch) { fChannelId = ch; }
    void SetPos(double pos) { fPos = pos; }
    void SetTime(double tim) { fTime = tim; }
    void SetCharge(double Q) { fCharge = Q; }

  protected:
    // Basic Hit information
    UInt_t fChannelId;
    double fPos;
    double fTime;
    double fCharge;

    ClassDef(R3BRpcStripHitData, 1)
};

#endif
