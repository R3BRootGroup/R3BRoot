#include "R3BNeulandGeoPar.h"

#include <iostream>

#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TGeoBBox.h"

#include "FairParamList.h"

R3BNeulandGeoPar::R3BNeulandGeoPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNeulandGeoNode(nullptr)
{
}

R3BNeulandGeoPar::~R3BNeulandGeoPar()
{
    // Note: Deleting stuff here or in clear() causes segfaults?
}

void R3BNeulandGeoPar::clear() {}

void R3BNeulandGeoPar::putParams(FairParamList* l)
{
    if (!l)
    {
        return;
    }
    l->addObject("NeulandGeoNode", fNeulandGeoNode);
}

Bool_t R3BNeulandGeoPar::getParams(FairParamList* l)
{
    if (!l)
    {
        return kFALSE;
    }
    if (!l->fillObject("NeulandGeoNode", fNeulandGeoNode))
    {
        return kFALSE;
    }
    BuildPaddleLookup();
    return kTRUE;
}

void R3BNeulandGeoPar::printParams()
{
    std::cout << "R3BNeulandGeoPar: Neuland Paddle Positions ..." << std::endl;

    fNeulandGeoNode->GetMatrix()->Print();

    for (Int_t i = 0; i < fNeulandGeoNode->GetNdaughters(); i++)
    {
        TGeoNode* node = fNeulandGeoNode->GetDaughter(i);
        std::cout << node->GetNumber() << std::endl;
        node->GetMatrix()->Print();
    }
}

void R3BNeulandGeoPar::SetNeulandGeoNode(const TGeoNode* const p)
{
    fNeulandGeoNode = (TGeoNode*)p->Clone();
    BuildPaddleLookup();
}

Double_t R3BNeulandGeoPar::GetPaddleHalfLength() const
{
    // All paddles have to have the same length
    return ((TGeoBBox*)fNeulandGeoNode->GetDaughter(0)->GetVolume()->GetShape())->GetDX();
}

// Convert positions of e.g. points to the local coordinate of the respective paddle [(-135,135),(-2.5,2.5),(-2.5,2.5)]
TVector3 R3BNeulandGeoPar::ConvertToLocalCoordinates(const TVector3& position, const Int_t paddleID) const
{
    Double_t pos_in[3] = { position.X(), position.Y(), position.Z() };
    Double_t pos_tmp[3];
    Double_t pos_out[3];

    // First, convert to Neuland-local coordinates (consisting of all paddles)
    fNeulandGeoNode->GetMatrix()->MasterToLocal(pos_in, pos_tmp);
    // Second, convert to the repective paddle
    fPaddleGeoNodes.at(paddleID)->MasterToLocal(pos_tmp, pos_out);

    return TVector3(pos_out[0], pos_out[1], pos_out[2]);
}

TVector3 R3BNeulandGeoPar::ConvertToGlobalCoordinates(const TVector3& position, const Int_t paddleID) const
{
    Double_t pos_in[3] = { position.X(), position.Y(), position.Z() };
    Double_t pos_tmp[3];
    Double_t pos_out[3];

    // Note reverse order of Global->Local
    fPaddleGeoNodes.at(paddleID)->LocalToMaster(pos_in, pos_tmp);

    fNeulandGeoNode->GetMatrix()->LocalToMaster(pos_tmp, pos_out);

    return TVector3(pos_out[0], pos_out[1], pos_out[2]);
}

void R3BNeulandGeoPar::BuildPaddleLookup()
{
    for (Int_t i = 0; i < fNeulandGeoNode->GetNdaughters(); i++)
    {
        TGeoNode* node = fNeulandGeoNode->GetDaughter(i);
        fPaddleGeoNodes[node->GetNumber()] = node;
    }
}

ClassImp(R3BNeulandGeoPar);
