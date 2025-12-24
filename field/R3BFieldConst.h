/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
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
// -----                    R3BFieldConst header file                  -----
// -------------------------------------------------------------------------

#pragma once

#include <FairField.h>

class R3BFieldPar;

/**
 * \brief R3BFieldConst class.
 *
 * A constant (homogeneous) magnetic field.
 */

class R3BFieldConst : public FairField
{

  public:
    /** Default constructor **/
    R3BFieldConst();

    /** Standard constructor
     ** @param name   Object name
     ** @param xMin,xMax   x region of field (global coordinates)
     ** @param yMin,yMax   y region of field (global coordinates)
     ** @param zMin,zMax   z region of field (global coordinates)
     ** @param bX,bY,bZ    Field values [kG]
     **/
    R3BFieldConst(const char* name,
                  double xMin,
                  double xMax,
                  double yMin,
                  double yMax,
                  double zMin,
                  double zMax,
                  double bX,
                  double bY,
                  double bZ);

    /** Constructor from R3BFieldPar **/
    R3BFieldConst(R3BFieldPar* fieldPar);

    /** Destructor **/
    ~R3BFieldConst() override;

    /** Set the field region
     ** @param xMin,xMax   x region of field (global coordinates)
     ** @param yMin,yMax   y region of field (global coordinates)
     ** @param zMin,zMax   z region of field (global coordinates)
     **/
    void SetFieldRegion(double xMin, double xMax, double yMin, double yMax, double zMin, double zMax);

    /** Set the field values
     ** @param bX,bY,bZ    Field values [kG]
     **/
    void SetField(double bX, double bY, double bZ);

    /** Get components of field at a given point
     ** @param x,y,z   Point coordinates [cm]
     **/
    auto GetBx(double x, double y, double z) -> double override;
    auto GetBy(double x, double y, double z) -> double override;
    auto GetBz(double x, double y, double z) -> double override;

    /** Accessors to field region **/
    [[nodiscard]] auto GetXmin() const -> double { return fXmin; }
    [[nodiscard]] auto GetXmax() const -> double { return fXmax; }
    [[nodiscard]] auto GetYmin() const -> double { return fYmin; }
    [[nodiscard]] auto GetYmax() const -> double { return fYmax; }
    [[nodiscard]] auto GetZmin() const -> double { return fZmin; }
    [[nodiscard]] auto GetZmax() const -> double { return fZmax; }

    /** Accessors to field values **/
    [[nodiscard]] auto GetBx() const -> double { return fBx; }
    [[nodiscard]] auto GetBy() const -> double { return fBy; }
    [[nodiscard]] auto GetBz() const -> double { return fBz; }

    /** Screen output **/
    void Print(Option_t* option = "") const override;

  private:
    /** Limits of the field region **/
    double fXmin{};
    double fXmax{};
    double fYmin{};
    double fYmax{};
    double fZmin{};
    double fZmax{};

    /** Field components inside the field region **/
    double fBx{};
    double fBy{};
    double fBz{};

    void FillParContainer() override;

  public:
    ClassDefOverride(R3BFieldConst, 1);
};
