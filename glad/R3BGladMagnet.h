/******************************************************************************
 *   Copyright (C) 2010 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2010-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include "R3BModule.h"

#include <TString.h>

class R3BGladMagnet : public R3BModule
{
  public:
    // This defines the experimental area: CaveC or HEC
    enum ExpArea
    {
        CaveC,
        HEC14,
        HEC9
    };

    /** Default constructor **/
    R3BGladMagnet();

    /** Standard constructor
     ** @param geoFile       Name of field map file without extension
     ** @param cave   Experimental area
     **/
    R3BGladMagnet(const TString& geoFile, ExpArea cave = CaveC);

  private:
    double fPosX = -42.0;  // offset on the X axis
    double fPosY = 1.75;   // offset on the Y axis
    double fPosZ = 308.8;  // offset on the Z axis (distance from target)
    double fRotDeg = 14.0; // rotation around -Y axis

  public:
    ClassDefOverride(R3BGladMagnet, 4); // NOLINT
};
