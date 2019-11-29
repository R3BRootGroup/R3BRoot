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

//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

/////////////////////////////////////////////////////////////
// R3BGeoMagnet
//
// Class for the geometry of detector part Magnet
//
/////////////////////////////////////////////////////////////

#include "R3BGeoMagnet.h"

ClassImp(R3BGeoMagnet)

    R3BGeoMagnet::R3BGeoMagnet()
{
    // Constructor
    fName = "magnet";
    maxSectors = 0;
    maxModules = 1;
    strcpy(modName, "m");
    strcpy(eleName, "m");
}
