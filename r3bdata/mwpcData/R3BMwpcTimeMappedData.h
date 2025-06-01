/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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
// -----                R3BMwpcTimeMappedData header file              -----
// -----            Created 09/10/19 by JL Rodriguez-Sanchez           -----
// -------------------------------------------------------------------------

#ifndef R3BMwpcTimeMappedData_H
#define R3BMwpcTimeMappedData_H 1

#include "TObject.h"

class R3BMwpcTimeMappedData : public TObject
{
  public:
    /** Default constructor **/
    R3BMwpcTimeMappedData();

    /** Constructor with arguments
     *@param plane   1 for Xup, 2 for Xdown, and 3 for Y
     *@param time   Time diff with respect to the trigger [16bits]
     *              attention one time per bus
     *              will not work for MWPC4: FIXME
     **/
    R3BMwpcTimeMappedData(UInt_t plane, UInt_t time);

    /** Destructor **/
    virtual ~R3BMwpcTimeMappedData() {}

    /** Accessors **/
    inline const UInt_t& GetPlane() const { return fPlane; }
    inline const UInt_t& GetTime() const { return fTime; }

    /** Modifiers **/
    void SetPlane(UInt_t plane) { fPlane = plane; };
    void SetTime(UInt_t time) { fTime = time; };

  protected:
    UInt_t fPlane;
    UInt_t fTime;

  public:
    ClassDef(R3BMwpcTimeMappedData, 1)
};

#endif
