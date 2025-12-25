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
// -----                      R3BFieldPar header file                  -----
// -------------------------------------------------------------------------

/** R3BFieldPar.h
 **
 ** Parameter set for the R3B magnetic field. For the runtime database.
 **/

#pragma once

#include <FairField.h>
#include <FairParGenericSet.h>

class FairParamList;

const int kMaxFieldMapType = 5;

class R3BFieldPar : public FairParGenericSet
{
  public:
    /** Standard constructor  **/
    R3BFieldPar(const char* name, const char* title, const char* context);

    /** default constructor  **/
    R3BFieldPar();

    /** Put parameters **/
    void putParams(FairParamList* list) override;

    /** Get parameters **/
    auto getParams(FairParamList* list) -> bool override;

    /** Set parameters from R3BField  **/
    void SetParameters(FairField* field);

    /** Accessors **/
    [[nodiscard]] auto GetType() const -> int { return fType; }
    [[nodiscard]] auto GetXmin() const -> double { return fXmin; }
    [[nodiscard]] auto GetXmax() const -> double { return fXmax; }
    [[nodiscard]] auto GetYmin() const -> double { return fYmin; }
    [[nodiscard]] auto GetYmax() const -> double { return fYmax; }
    [[nodiscard]] auto GetZmin() const -> double { return fZmin; }
    [[nodiscard]] auto GetZmax() const -> double { return fZmax; }
    [[nodiscard]] auto GetBx() const -> double { return fBx; }
    [[nodiscard]] auto GetBy() const -> double { return fBy; }
    [[nodiscard]] auto GetBz() const -> double { return fBz; }
    auto GetFileName() -> const std::string& { return fMapFileName; }
    [[nodiscard]] auto GetPositionX() const -> double { return fPosX; }
    [[nodiscard]] auto GetPositionY() const -> double { return fPosY; }
    [[nodiscard]] auto GetPositionZ() const -> double { return fPosZ; }
    [[nodiscard]] auto GetXAngle() const -> double { return fXAngle; }
    [[nodiscard]] auto GetYAngle() const -> double { return fYAngle; }
    [[nodiscard]] auto GetZAngle() const -> double { return fZAngle; }
    [[nodiscard]] auto GetScale() const -> double { return fScale; }
    [[nodiscard]] auto GetCurrent() const -> double { return fCurrent; }
    void MapName(std::string& name) { name = fMapName; }
    void MapName(TString& name) { name = fMapName; }

  private:
    /** Field type
     ** 0 = constant field
     ** 1 = field map
     ** 2 = Glad field map
     **/
    int fType = -1;

    /** Field limits in case of constant field **/
    double fXmin{};
    double fXmax{};
    double fYmin{};
    double fYmax{};
    double fZmin{};
    double fZmax{};

    /** Field values in case of constant field [kG] **/
    double fBx{};
    double fBy{};
    double fBz{};

    /** Field map name in case of field map **/
    std::string fMapName;
    std::string fMapFileName;

    /** Field centre position for field map **/
    double fPosX{};
    double fPosY{};
    double fPosZ{};

    /** Euler rotations of the field map **/
    double fXAngle{};
    double fYAngle{};
    double fZAngle{};

    /** Scaling factor for field map **/
    double fScale{};

    double fCurrent{};

  public:
    ClassDefOverride(R3BFieldPar, 3); // NOLINT
};
