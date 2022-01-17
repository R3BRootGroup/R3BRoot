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

#ifndef R3BRPCCALDATA_H
#define R3BRPCCALDATA_H

#include "FairMultiLinkedData.h"
#include "TObject.h"

class R3BRpcCalData : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BRpcCalData();

    /** Constructor with arguments
     *@param fChannelId   Channel unique identifier
     //MODIFY ME!!!!!!!!!!!!!!!!!
     *@param fEnergy      Total energy deposited ([GeV] in sim)
     *@param fTime        Time since event start [ns]
     *@param fToT_Energy  Total energy deposited from ToT ([GeV] in sim)
     **/
    R3BRpcCalData(Int_t ident,
                  // MODIFY ME!!!!!!!!!!!!!!!!!
                  Double_t energy,
                  uint64_t time,
                  Double_t tot_energy = 0);

    /** Copy constructor **/
    R3BRpcCalData(const R3BRpcCalData&);

    R3BRpcCalData& operator=(const R3BRpcCalData&) { return *this; }

    /** Destructor **/
    virtual ~R3BRpcCalData() {}

    /** Accessors **/
    inline const Int_t& GetChannelId() const { return fChannelId; }

    // MODIFY ME!!!!!!!!!!!!!!!!!
    inline const Double_t& GetEnergy() const { return fEnergy; }
    inline const Double_t& GetToT_Energy() const { return fToT_Energy; }
    inline const uint64_t& GetTime() const { return fTime; }

    /** Modifiers **/
    void SetChannelId(Int_t ident) { fChannelId = ident; }

    // MODIFY ME!!!!!!!!!!!!!!!!!
    void SetEnergy(Double32_t energy) { fEnergy = energy; }
    void SetToT_Energy(Double32_t energy) { fToT_Energy = energy; }
    void SetTime(uint64_t time) { fTime = time; }
    void AddMoreEnergy(Double32_t moreEnergy) { fEnergy += moreEnergy; }

  protected:
    Int_t fChannelId; // channel unique identifier

    // MODIFY ME!!!!!!!!!!!!!!!!!
    Double32_t fEnergy;     // total energy
    Double32_t fToT_Energy; // total energy from ToT
    uint64_t fTime;         // time of the interaction

  public:
    ClassDef(R3BRpcCalData, 1)
};

#endif
