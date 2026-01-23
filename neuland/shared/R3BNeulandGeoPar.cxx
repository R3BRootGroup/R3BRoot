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

#include "R3BNeulandGeoPar.h"
#include "FairParamList.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include <FairParGenericSet.h>
#include <Math/Vector3Dfwd.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TGeoNode.h>
#include <algorithm>
#include <array>
#include <iostream>

// NOLINTNEXTLINE (misc-include-cleaner)
#include <Math/Vector3D.h>

R3BNeulandGeoPar::R3BNeulandGeoPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNeulandGeoNode(nullptr)
{
}

R3BNeulandGeoPar::~R3BNeulandGeoPar() = default;

void R3BNeulandGeoPar::clear() {}

void R3BNeulandGeoPar::putParams(FairParamList* lst)
{
    if (lst == nullptr)
    {
        return;
    }
    lst->addObject("NeulandGeoNode", fNeulandGeoNode);
}

auto R3BNeulandGeoPar::getParams(FairParamList* lst) -> bool
{
    if (lst == nullptr)
    {
        return kFALSE;
    }
    if (!lst->fillObject("NeulandGeoNode", fNeulandGeoNode))
    {
        return kFALSE;
    }
    BuildPaddleLookup();
    return kTRUE;
}

void R3BNeulandGeoPar::printParams()
{
    std::cout << "R3BNeulandGeoPar: Neuland Paddle Positions ...\n";

    fNeulandGeoNode->GetMatrix()->Print();

    for (int i = 0; i < fNeulandGeoNode->GetNdaughters(); i++)
    {
        TGeoNode* node = fNeulandGeoNode->GetDaughter(i);
        std::cout << node->GetNumber() << "\n";
        node->GetMatrix()->Print();
    }
}

void R3BNeulandGeoPar::SetNeulandGeoNode(const TGeoNode* const node)
{
    fNeulandGeoNode = dynamic_cast<TGeoNode*>(node->Clone());
    BuildPaddleLookup();
}

auto R3BNeulandGeoPar::GetPaddleHalfLength() const -> double
{
    // All paddles have to have the same length
    return (dynamic_cast<TGeoBBox*>(fNeulandGeoNode->GetDaughter(0)->GetVolume()->GetShape()))->GetDX();
}

// Convert positions of e.g. points to the local coordinate of the respective paddle [(-135,135),(-2.5,2.5),(-2.5,2.5)]
auto R3BNeulandGeoPar::ConvertToLocalCoordinates(const ROOT::Math::XYZVector& position, const int paddleID) const
    -> ROOT::Math::XYZVector
{
    auto pos_in = std::array{ position.X(), position.Y(), position.Z() };
    auto pos_tmp = std::array<double, 3>{};
    auto pos_out = std::array<double, 3>{};

    // First, convert to Neuland-local coordinates (consisting of all paddles)
    fNeulandGeoNode->GetMatrix()->MasterToLocal(pos_in.data(), pos_tmp.data());
    // Second, convert to the repective paddle
    fPaddleGeoNodes.at(paddleID)->MasterToLocal(pos_tmp.data(), pos_out.data());

    return { pos_out[0], pos_out[1], pos_out[2] };
}

auto R3BNeulandGeoPar::ConvertToGlobalCoordinates(const ROOT::Math::XYZVector& position, const int paddleID) const
    -> ROOT::Math::XYZVector
{
    auto pos_in = std::array{ position.X(), position.Y(), position.Z() };
    auto pos_tmp = std::array<double, 3>{};
    auto pos_out = std::array<double, 3>{};

    // Note reverse order of Global->Local
    fPaddleGeoNodes.at(paddleID)->LocalToMaster(pos_in.data(), pos_tmp.data());

    fNeulandGeoNode->GetMatrix()->LocalToMaster(pos_tmp.data(), pos_out.data());

    return { pos_out[0], pos_out[1], pos_out[2] };
}

auto R3BNeulandGeoPar::ConvertGlobalToPixel(const ROOT::Math::XYZVector& position) const -> ROOT::Math::XYZVector
{
    const auto nPixels = 50;
    const auto sizePixel = 5.;
    const auto nPaddles = fNeulandGeoNode->GetNdaughters();
    const auto nPlanes = nPaddles / nPixels;

    auto pos_in = std::array{ position.X(), position.Y(), position.Z() };
    auto pos_tmp = std::array<double, 3>{};

    // First, convert to Neuland-local coordinates (consisting of all paddles)
    fNeulandGeoNode->GetMatrix()->MasterToLocal(pos_in.data(), pos_tmp.data());

    // Note: PaddleHalfLength is 135 (light guides)
    // Map x and y values with [-125.:125.] float to [0:nPixels-1] int
    const auto x_pos = std::min(std::max(0, (nPixels / 2) + static_cast<int>((pos_tmp[0] / sizePixel))), nPixels - 1);
    const auto y_pos =
        std::min(std::max<int>(0, (nPixels / 2) + static_cast<int>(pos_tmp[1] / sizePixel)), nPixels - 1);

    // Map z to [0:nPlanes-1]
    const auto z_pos =
        std::min(std::max<int>(0, (nPlanes / 2) + static_cast<int>(pos_tmp[2] / sizePixel)), nPlanes - 1);

    return { static_cast<double>(x_pos), static_cast<double>(y_pos), static_cast<double>(z_pos) };
}

void R3BNeulandGeoPar::BuildPaddleLookup()
{
    for (int i = 0; i < fNeulandGeoNode->GetNdaughters(); i++)
    {
        TGeoNode* node = fNeulandGeoNode->GetDaughter(i);
        fPaddleGeoNodes[node->GetNumber()] = node;
    }
}
ClassImp(R3BNeulandGeoPar);
