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

#ifndef R3BNEULANDNEUTRON_H
#define R3BNEULANDNEUTRON_H

#include "R3BNeulandCluster.h"
#include "R3BNeulandHit.h"
#include "TObject.h"
#include <Math/Vector3D.h>
#include <Math/Vector3Dfwd.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <iostream>

class R3BNeulandNeutron : public TObject
{
  public:
    R3BNeulandNeutron() = default;
    R3BNeulandNeutron(int paddle, double time, const ROOT::Math::XYZVector& pos, const ROOT::Math::XYZVector& pix);
    explicit R3BNeulandNeutron(const R3BNeulandHit&);
    explicit R3BNeulandNeutron(const R3BNeulandCluster&);

    [[nodiscard]] auto GetPaddle() const -> int { return fPaddle; }
    [[nodiscard]] auto GetT() const -> double { return fT; }
    [[nodiscard]] auto GetPosition() const -> ROOT::Math::XYZVector { return fPosition; }
    [[nodiscard]] auto GetPixel() const -> ROOT::Math::XYZVector { return fPixel; }

    [[nodiscard]] auto GetEtot() const -> double;
    [[nodiscard]] auto GetEkin() const -> double;
    [[nodiscard]] auto GetGamma() const -> double;
    [[nodiscard]] auto GetP() const -> ROOT::Math::XYZVector;

    void Print(const Option_t* /*option*/) const override;

  private:
    int fPaddle{};
    double fT{};
    ROOT::Math::XYZVector fPosition;
    ROOT::Math::XYZVector fPixel;

    ClassDefOverride(R3BNeulandNeutron, 1)
};

auto operator<<(std::ostream&, const R3BNeulandNeutron&) -> std::ostream&; // Support easy printing

#endif // R3BNEULANDNEUTRON_H
