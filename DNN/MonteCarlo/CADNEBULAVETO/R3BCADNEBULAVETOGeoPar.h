#ifndef R3BCADNEBULAVETOGEOPAR_H
#define R3BCADNEBULAVETOGEOPAR_H

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
 * NEBULAVETO geometry parameter storage
 * @author Jan Mayer
 *
 * Stores the full NEBULAVETO geo node used in the simulation for later reference, especially for coordinate
 * transformation from and to local and global coordinates.
 */

class R3BCADNEBULAVETOGeoPar : public FairParGenericSet
{
  public:
    // needs to be public?
    TGeoNode* fNEBULAVETOGeoNode;

    R3BCADNEBULAVETOGeoPar(const char* name = "R3BCADNEBULAVETOGeoPar",
                     const char* title = "NEBULAVETO Geometry Parameters",
                     const char* context = "TestDefaultContext");
    ~R3BCADNEBULAVETOGeoPar() override;

    void clear() override;
    void putParams(FairParamList*) override;
    Bool_t getParams(FairParamList*) override;
    void printParams() override;

    TGeoNode* GetNEBULAVETOGeoNode() { return fNEBULAVETOGeoNode; } // FIXME: const?
    void SetNEBULAVETOGeoNode(const TGeoNode* const p);

    Double_t GetPaddleHalfLength() const;
    TVector3 ConvertToLocalCoordinates(const TVector3& position, const Int_t paddleID) const;
    TVector3 ConvertToGlobalCoordinates(const TVector3& position, const Int_t paddleID) const;
    TVector3 ConvertGlobalToPixel(const TVector3& position) const;

  private:
    std::map<Int_t, TGeoNode*> fPaddleGeoNodes;
    void BuildPaddleLookup();

    R3BCADNEBULAVETOGeoPar(const R3BCADNEBULAVETOGeoPar&);
    R3BCADNEBULAVETOGeoPar& operator=(const R3BCADNEBULAVETOGeoPar&);

    ClassDefOverride(R3BCADNEBULAVETOGeoPar, 1)
};

#endif // R3BNEBULAVETOGEOPAR_H
