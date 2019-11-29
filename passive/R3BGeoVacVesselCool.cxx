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

//*-- AUTHOR : Marc Labiche
//*-- Created : 28/06/2012

/////////////////////////////////////////////////////////////
// R3BGeoVacVesselCool
//
// Class for geometry of Cooling Si Tracker
//
/////////////////////////////////////////////////////////////

#include "R3BGeoVacVesselCool.h"

ClassImp(R3BGeoVacVesselCool)

    R3BGeoVacVesselCool::R3BGeoVacVesselCool()
{
    // Constructor
    fName = "VacVesselcool";
    strcpy(modName, "v");
    strcpy(eleName, "v");
    maxSectors = 0;
    maxModules = 1;
}
