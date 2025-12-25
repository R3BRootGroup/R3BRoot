/******************************************************************************
 *   Copyright (C) 2009 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2009-2026 Members of R3B Collaboration                     *
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
// -----                    R3BFieldConst source file                  -----
// -------------------------------------------------------------------------

#include "R3BFieldConst.h"
#include "R3BException.h"
#include "R3BFieldPar.h"

#include <FairLogger.h>
#include <FairRun.h>
#include <FairRuntimeDb.h>

#include <fmt/format.h>
#include <iostream>

using fmt::format;

// -----   Default constructor   -------------------------------------------
R3BFieldConst::R3BFieldConst()
    : R3BFieldConst{ "R3BConstField", 0., 0., 0., 0., 0., 0., 0., 0., 0. }
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BFieldConst::R3BFieldConst(const char* name,
                             double xMin,
                             double xMax,
                             double yMin,
                             double yMax,
                             double zMin,
                             double zMax,
                             double bX,
                             double bY,
                             double bZ)
    : FairField(name)
    , fXmin(xMin)
    , fXmax(xMax)
    , fYmin(yMin)
    , fYmax(yMax)
    , fZmin(zMin)
    , fZmax(zMax)
    , fBx(bX)
    , fBy(bY)
    , fBz(bZ)
{
    fType = 0;
}
// -------------------------------------------------------------------------

// --------   Constructor from R3BFieldPar   -------------------------------
R3BFieldConst::R3BFieldConst(R3BFieldPar* fieldPar)
{
    if (!fieldPar)
    {
        LOG(warn) << "R3BFieldConst: Empty parameter container.";
        fType = 0;
    }
    else
    {
        fXmin = fieldPar->GetXmin();
        fXmax = fieldPar->GetXmax();
        fYmin = fieldPar->GetYmin();
        fYmax = fieldPar->GetYmax();
        fZmin = fieldPar->GetZmin();
        fZmax = fieldPar->GetZmax();
        fBx = fieldPar->GetBx();
        fBy = fieldPar->GetBy();
        fBz = fieldPar->GetBz();
        fType = fieldPar->GetType();
    }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BFieldConst::~R3BFieldConst() {}
// -------------------------------------------------------------------------

// -----   Set field region   ----------------------------------------------
void R3BFieldConst::SetFieldRegion(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax)
{
    fXmin = xMin;
    fXmax = xMax;
    fYmin = yMin;
    fYmax = yMax;
    fZmin = zMin;
    fZmax = zMax;
}
// -------------------------------------------------------------------------

// -----   Set field values   ----------------------------------------------
void R3BFieldConst::SetField(double bX, double bY, double bZ)
{
    fBx = bX;
    fBy = bY;
    fBz = bZ;
}
// -------------------------------------------------------------------------

// -----   Get x component of field   --------------------------------------
double R3BFieldConst::GetBx(double x, double y, double z)
{
    if (x < fXmin || x > fXmax || y < fYmin || y > fYmax || z < fZmin || z > fZmax)
        return 0.;
    return fBx;
}
// -------------------------------------------------------------------------

// -----   Get y component of field   --------------------------------------
double R3BFieldConst::GetBy(double x, double y, double z)
{
    if (x < fXmin || x > fXmax || y < fYmin || y > fYmax || z < fZmin || z > fZmax)
        return 0.;
    return fBy;
}
// -------------------------------------------------------------------------

// -----   Get z component of field   --------------------------------------
double R3BFieldConst::GetBz(double x, double y, double z)
{
    if (x < fXmin || x > fXmax || y < fYmin || y > fYmax || z < fZmin || z > fZmax)
        return 0.;
    return fBz;
}
// -------------------------------------------------------------------------

// -----   Screen output   -------------------------------------------------
void R3BFieldConst::Print(Option_t*) const
{
    std::cout << "======================================================" << std::endl;
    std::cout << format("----  {} : {}\n", fTitle.Data(), fName.Data());
    std::cout << "----  Field type    : constant\n";
    std::cout << "----  Field regions :\n";
    std::cout << format("----        x = {:6.2f} to {:6.2f} cm\n", fXmin, fXmax);
    std::cout << format("----        y = {:6.2f} to {:6.2f} cm\n", fYmin, fYmax);
    std::cout << format("----        z = {:6.2f} to {:6.2f} cm\n", fZmin, fZmax);
    std::cout << format("----  B = ( {:.4f}, {:.4f}, {:.4f} ) kG\n", fBx, fBy, fBz);
}
// -------------------------------------------------------------------------

void R3BFieldConst::FillParContainer()
{
    auto* run = FairRun::Instance();
    auto* rtdb = run->GetRuntimeDb();
    auto* par = std::make_unique<R3BFieldPar>().release();
    par->SetParameters(this);
    par->setChanged();
    if (rtdb->addContainer(par); par == nullptr)
    {
        throw R3B::runtime_error("Calibration parameter becomes nullptr!");
    }
}

ClassImp(R3BFieldConst)
