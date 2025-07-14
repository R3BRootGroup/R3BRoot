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

#ifndef R3BNEULANDGEOPAR_H
#define R3BNEULANDGEOPAR_H

#include "FairParGenericSet.h"
#include "TGeoNode.h"
#include <Math/Vector3Dfwd.h>
#include <Rtypes.h>
#include <cstdint>
#include <map>
class FairParamList;
class TVector3;

/**
 * NeuLAND geometry parameter storage
 * @author Jan Mayer
 *
 * Stores the full Neuland geo node used in the simulation for later reference, especially for coordinate
 * transformation from and to local and global coordinates.
 */

class R3BNeulandGeoPar : public FairParGenericSet
{
  public:
    explicit R3BNeulandGeoPar(const char* name = "R3BNeulandGeoPar",
                              const char* title = "Neuland Geometry Parameters",
                              const char* context = "TestDefaultContext");
    R3BNeulandGeoPar(R3BNeulandGeoPar&&) = delete;
    auto operator=(R3BNeulandGeoPar&&) -> R3BNeulandGeoPar& = delete;
    R3BNeulandGeoPar(const R3BNeulandGeoPar&);
    auto operator=(const R3BNeulandGeoPar&) -> R3BNeulandGeoPar&;
    ~R3BNeulandGeoPar() override;

    void clear() override;
    void putParams(FairParamList* /*unused*/) override;
    auto getParams(FairParamList* /*unused*/) -> bool override;
    void printParams() override;

    [[nodiscard]] auto GetNumberOfModules() const -> int { return fNeulandGeoNode->GetNdaughters(); }
    [[nodiscard]] auto GetNeulandGeoNode() const -> TGeoNode* { return fNeulandGeoNode; }
    void SetNeulandGeoNode(const TGeoNode* node);

    [[nodiscard]] auto GetPaddleHalfLength() const -> double;
    [[nodiscard]] auto ConvertToLocalCoordinates(const ROOT::Math::XYZVector& position,
                                                 int paddleID) const -> ROOT::Math::XYZVector;
    [[nodiscard]] auto ConvertToGlobalCoordinates(const ROOT::Math::XYZVector& position,
                                                  int paddleID) const -> ROOT::Math::XYZVector;
    [[nodiscard]] auto ConvertGlobalToPixel(const ROOT::Math::XYZVector& position) const -> ROOT::Math::XYZVector;

  private:
    std::map<int32_t, TGeoNode*> fPaddleGeoNodes;
    TGeoNode* fNeulandGeoNode;
    void BuildPaddleLookup();

    ClassDefOverride(R3BNeulandGeoPar, 1)
};

#endif // R3BNEULANDGEOPAR_H
