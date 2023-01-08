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

// -------------------------------------------------------------------------
// -----              R3BMusicMappedData header file                   -----
// -----         Created 15/10/19 by J.L. Rodriguez-Sanchez            -----
// -------------------------------------------------------------------------

/**  R3BMusicMappedData
 **  A HitData is the representation of the
 **  information obtained from the MUSIC detector.
 **/

#ifndef R3BMusicMappedData_H
#define R3BMusicMappedData_H
#include "TObject.h"

class R3BMusicMappedData : public TObject
{

  public:
    /** Default constructor **/
    R3BMusicMappedData();

    /** Constructor with arguments
     *@param anodeID  Anode ID
     *@param time     Time [channels]
     *@param energy   Energy deposit [channels]
     **/
    R3BMusicMappedData(UShort_t anodeID, UShort_t time, UShort_t energy);

    /** Destructor **/
    virtual ~R3BMusicMappedData() {}

    /** Accessors **/
    inline const UShort_t& GetAnodeID() const { return fAnodeID; }
    inline const UShort_t& GetTime() const { return fTime; }
    inline const UShort_t& GetEnergy() const { return fEnergy; }

    /** Modifiers **/
    void SetAnodeID(UShort_t id) { fAnodeID = id; };
    void SetTime(UShort_t time) { fTime = time; };
    void SetEnergy(UShort_t energy) { fEnergy = energy; };

  protected:
    UShort_t fAnodeID;
    UShort_t fTime;
    UShort_t fEnergy;

    ClassDef(R3BMusicMappedData, 1)
};

#endif
