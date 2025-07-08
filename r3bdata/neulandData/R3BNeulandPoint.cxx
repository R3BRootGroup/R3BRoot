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

#include "R3BNeulandPoint.h"
#include <Math/Vector3D.h>
#include <Math/Vector3Dfwd.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <iostream>
#include <ostream>

auto operator<<(std::ostream& ostream, const R3BNeulandPoint& point) -> std::ostream&
{
    ostream << "R3BNeulandPoint: NeuLAND Point in Paddle " << point.GetPaddle() << " with light yield "
            << point.GetLightYield() << "\n";
    // TODO: #fairroot: Move this to FairMCPoint
    ostream << "... with FairMCPoint for track " << point.GetTrackID() << "\n";
    ostream << "    Position XYZ: " << point.GetPosition().X() << "    " << point.GetPosition().X() << "    "
            << point.GetPosition().Z() << "\n";
    ostream << "    Momentum XYZ: " << point.GetMomentum().X() << "    " << point.GetMomentum().X() << "    "
            << point.GetMomentum().Z() << "\n";
    ostream << "    Time: " << point.GetTime() << "    Length: " << point.GetLength()
            << "    Energy loss: " << point.GetEnergyLoss() << "\n";
    return ostream;
}

void R3BNeulandPoint::Print(const Option_t* /*opt*/) const { std::cout << *this; }
