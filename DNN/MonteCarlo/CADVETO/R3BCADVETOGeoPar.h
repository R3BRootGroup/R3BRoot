#ifndef R3BCADVETOGEOPAR_H
#define R3BCADVETOGEOPAR_H

// C++ headers:
#include <algorithm>
#include <iostream>
#include <map>

// ROOT headers:
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TGeoNode.h"

// FAIR headers:
#include "FairParamList.h"
#include "FairParGenericSet.h"

class FairParamList;
class TVector3;

/**
 * VETO geometry parameter storage
 * @author Jan Mayer
 *
 * Stores the full VETO geo node used in the simulation for later reference, especially for coordinate
 * transformation from and to local and global coordinates.
 */

class R3BCADVETOGeoPar : public FairParGenericSet
{
  public:
    // needs to be public?
    TGeoNode* fVETOGeoNode;

    R3BCADVETOGeoPar(const char* name = "R3BCADVETOGeoPar",
                     const char* title = "VETO Geometry Parameters",
                     const char* context = "TestDefaultContext");
    ~R3BCADVETOGeoPar() override;

    void clear() override;
    void putParams(FairParamList*) override;
    Bool_t getParams(FairParamList*) override;
    void printParams() override;

    TGeoNode* GetVETOGeoNode() { return fVETOGeoNode; } // FIXME: const?
    void SetVETOGeoNode(const TGeoNode* const p);

    Double_t GetPaddleHalfLength() const;
    TVector3 ConvertToLocalCoordinates(const TVector3& position, const Int_t paddleID) const;
    TVector3 ConvertToGlobalCoordinates(const TVector3& position, const Int_t paddleID) const;
    TVector3 ConvertGlobalToPixel(const TVector3& position) const;

  private:
    std::map<Int_t, TGeoNode*> fPaddleGeoNodes;
    void BuildPaddleLookup();

    R3BCADVETOGeoPar(const R3BCADVETOGeoPar&);
    R3BCADVETOGeoPar& operator=(const R3BCADVETOGeoPar&);

    ClassDefOverride(R3BCADVETOGeoPar, 1)
};

#endif // R3BVETOGEOPAR_H
