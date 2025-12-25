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

#include "R3BGladMagnet.h"
#include "R3BLogger.h"

#include <TGeoMatrix.h>
#include <sstream>

R3BGladMagnet::R3BGladMagnet()
    : R3BGladMagnet("")
{
}

R3BGladMagnet::R3BGladMagnet(const TString& geoFile, ExpArea cave)
    : R3BModule("GLAD Magnet", "GLAD Magnet", kFALSE, geoFile)
{
    auto rot_y = fRotDeg;
    std::string exparea = "Cave-C";
    if (cave == HEC14)
    {
        fPosY = 0.;
        fPosX = -fPosX - 7.;
        rot_y = -fRotDeg;
        exparea = "HEC";
    }
    else if (cave == HEC9)
    {
        fPosY = 0.;
        fPosX = -fPosX;
        rot_y = -9.;
        exparea = "HEC";
    }

    TGeoRotation rot;
    rot.RotateY(90.0);
    rot.RotateZ(-90.0);
    rot.RotateY(rot_y);
    rot.RotateZ(180.0);

    TGeoTranslation trans(fPosX, fPosY, fPosZ);

    std::ostringstream msg;
    msg << "GLAD magnet configuration:\n"
        << "  Experimental area  : " << exparea << " \n"
        << "  Rotation-Y         : " << rot_y << " deg\n"
        << "  Position (X,Y,Z)   : (" << fPosX << ", " << fPosY << ", " << fPosZ << ") cm\n";
    R3BLOG(info, msg.str());

    // Apply rotation first, then translate to the correct position
    R3BModule::SetRotation(rot);
    R3BModule::SetPosition(trans);
}

ClassImp(R3BGladMagnet)
