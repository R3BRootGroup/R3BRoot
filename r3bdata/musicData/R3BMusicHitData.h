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

// -------------------------------------------------------------------------
// -----                      R3BMusicHitData header file              -----
// -----                  Created 15/10/19 by JL Rodriguez             -----
// -------------------------------------------------------------------------

#ifndef R3BMusicHitData_H
#define R3BMusicHitData_H
#include "TObject.h"

class R3BMusicHitData : public TObject
{

  public:
    /** Default constructor **/
    R3BMusicHitData();

    /** Constructor with arguments
     *@param theta    Angle theta in [mrad]
     *@param z        Atomic number Z in charge units
     **/
    R3BMusicHitData(Double_t theta, Double_t z);

    /** Destructor **/
    virtual ~R3BMusicHitData() {}

    /** Accessors **/
    inline const Double_t& GetTheta() const { return fTheta; }
    inline const Double_t& GetZcharge() const { return fZ; }

    /** Modifiers **/
    void SetTheta(Double_t theta) { fTheta = theta; };
    void SetZcharge(Double_t z) { fZ = z; };

  protected:
    Double_t fTheta, fZ;

    ClassDef(R3BMusicHitData, 1)
};

#endif
