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

#ifndef R3BRPCHITDATA_H
#define R3BRPCHITDATA_H

#include "TObject.h"

#include "FairMultiLinkedData.h"
#include "R3BRpcCalData.h"

class R3BRpcHitData : public FairMultiLinkedData
{

  public:
    /** Default constructor **/
    R3BRpcHitData();

    /** Constructor with arguments
     *@param fChannelId		Channel unique identifier
     //MODIFY ME!!!!!!!!!!!!!!!!!
     *@param fEnergy				Total energy deposited
     *@param fTime				  Time
     **/
    R3BRpcHitData(UInt_t channel, Double_t ene, Double_t time);

    /** Copy constructor **/
    R3BRpcHitData(const R3BRpcHitData&);

    /** += operator **/
    R3BRpcHitData& operator+=(R3BRpcCalData& cH)
    {
        // MODIFY ME!!!!!!!!!!!!!!!!!
        this->fEnergy += cH.GetEnergy();
        return *this;
    }

    R3BRpcHitData& operator=(const R3BRpcHitData&) { return *this; }

    /** Destructor **/
    virtual ~R3BRpcHitData() {}

    /** Accessors **/
    UInt_t GetChannelId() const { return fChannelId; }
    // MODIFY ME!!!!!!!!!!!!!!!!!
    Double_t GetEnergy() const { return fEnergy; }
    Double_t GetTime() const { return fTime; }

    /** Modifiers **/
    void SetChannelId(UInt_t ch) { fChannelId = ch; }
    // MODIFY ME!!!!!!!!!!!!!!!!!
    void SetEnergy(Double_t ene) { fEnergy = ene; }
    void SetTime(Double_t tim) { fTime = tim; }

  protected:
    // Basic Hit information
    UInt_t fChannelId; //
    // MODIFY ME!!!!!!!!!!!!!!!!!
    Double_t fEnergy; //
    Double_t fTime;   //

    ClassDef(R3BRpcHitData, 1)
};

#endif
