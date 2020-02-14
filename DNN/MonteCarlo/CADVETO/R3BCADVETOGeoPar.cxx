// Inclusion of its own header:
#include "R3BCADVETOGeoPar.h"

R3BCADVETOGeoPar::R3BCADVETOGeoPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fVETOGeoNode(nullptr)
{
}

R3BCADVETOGeoPar::~R3BCADVETOGeoPar()
{
    // Note: Deleting stuff here or in clear() causes segfaults?
}

void R3BCADVETOGeoPar::clear() {}

void R3BCADVETOGeoPar::putParams(FairParamList* l)
{
    if (!l)
    {
        return;
    }
    l->addObject("VETOGeoNode", fVETOGeoNode);
}

Bool_t R3BCADVETOGeoPar::getParams(FairParamList* l)
{
    if (!l)
    {
        return kFALSE;
    }
    if (!l->fillObject("VETOGeoNode", fVETOGeoNode))
    {
        return kFALSE;
    }
    BuildPaddleLookup();
    return kTRUE;
}

void R3BCADVETOGeoPar::printParams()
{
    std::cout << "R3BCADVETOGeoPar: VETO Paddle Positions ..." << std::endl;

    fVETOGeoNode->GetMatrix()->Print();

    for (Int_t i = 0; i < fVETOGeoNode->GetNdaughters(); i++)
    {
        TGeoNode* node = fVETOGeoNode->GetDaughter(i);
        std::cout << node->GetNumber() << std::endl;
        node->GetMatrix()->Print();
    }
}

void R3BCADVETOGeoPar::SetVETOGeoNode(const TGeoNode* const p)
{
    fVETOGeoNode = (TGeoNode*)p->Clone();
    BuildPaddleLookup();
}

Double_t R3BCADVETOGeoPar::GetPaddleHalfLength() const
{
    // All paddles have to have the same length
    return ((TGeoBBox*)fVETOGeoNode->GetDaughter(0)->GetVolume()->GetShape())->GetDX();
}

// Convert positions of e.g. points to the local coordinate of the respective paddle [(-135,135),(-2.5,2.5),(-2.5,2.5)]
TVector3 R3BCADVETOGeoPar::ConvertToLocalCoordinates(const TVector3& position, const Int_t paddleID) const
{
    Double_t pos_in[3] = { position.X(), position.Y(), position.Z() };
    Double_t pos_tmp[3];
    Double_t pos_out[3];

    // First, convert to VETO-local coordinates (consisting of all paddles)
    fVETOGeoNode->GetMatrix()->MasterToLocal(pos_in, pos_tmp);
    // Second, convert to the repective paddle
    fPaddleGeoNodes.at(paddleID)->MasterToLocal(pos_tmp, pos_out);

    return TVector3(pos_out[0], pos_out[1], pos_out[2]);
}

TVector3 R3BCADVETOGeoPar::ConvertToGlobalCoordinates(const TVector3& position, const Int_t paddleID) const
{
    Double_t pos_in[3] = { position.X(), position.Y(), position.Z() };
    Double_t pos_tmp[3];
    Double_t pos_out[3];

    // Note reverse order of Global->Local
    fPaddleGeoNodes.at(paddleID)->LocalToMaster(pos_in, pos_tmp);

    fVETOGeoNode->GetMatrix()->LocalToMaster(pos_tmp, pos_out);

    return TVector3(pos_out[0], pos_out[1], pos_out[2]);
}

TVector3 R3BCADVETOGeoPar::ConvertGlobalToPixel(const TVector3& position) const
{
    const Int_t nPixels = 50;
    const Double_t sizePixel = 5;
    const Int_t nPaddles = fVETOGeoNode->GetNdaughters();
    const Int_t nPlanes = nPaddles / nPixels;

    Double_t pos_in[3] = { position.X(), position.Y(), position.Z() };
    Double_t pos_tmp[3];

    // First, convert to VETO-local coordinates (consisting of all paddles)
    fVETOGeoNode->GetMatrix()->MasterToLocal(pos_in, pos_tmp);

    // Note: PaddleHalfLength is 135 (light guides)
    // Map x and y values with [-125.:125.] float to [0:nPixels-1] int
    const Int_t x = std::min(std::max<Int_t>(0, pos_tmp[0] / sizePixel + nPixels / 2), nPixels - 1);
    const Int_t y = std::min(std::max<Int_t>(0, pos_tmp[1] / sizePixel + nPixels / 2), nPixels - 1);

    // Map z to [0:nPlanes-1]
    const Int_t z = std::min(std::max<Int_t>(0, pos_tmp[2] / sizePixel + nPlanes / 2), nPlanes - 1);

    return TVector3(x, y, z);
}

void R3BCADVETOGeoPar::BuildPaddleLookup()
{
    for (Int_t i = 0; i < fVETOGeoNode->GetNdaughters(); i++)
    {
        TGeoNode* node = fVETOGeoNode->GetDaughter(i);
        fPaddleGeoNodes[node->GetNumber()] = node;
    }
}
ClassImp(R3BCADVETOGeoPar);
