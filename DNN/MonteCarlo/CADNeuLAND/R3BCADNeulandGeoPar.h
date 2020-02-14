#ifndef R3BCADNEULANDGEOPAR_H
#define R3BCADNEULANDGEOPAR_H

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
 * NeuLAND geometry parameter storage
 * @author Jan Mayer
 *
 * Stores the full Neuland geo node used in the simulation for later reference, especially for coordinate
 * transformation from and to local and global coordinates.
 */

class R3BCADNeulandGeoPar : public FairParGenericSet
{
  public:
    // needs to be public?
    TGeoNode* fNeulandGeoNode;

    R3BCADNeulandGeoPar(const char* name = "R3BCADNeulandGeoPar",
                     const char* title = "Neuland Geometry Parameters",
                     const char* context = "TestDefaultContext");
    ~R3BCADNeulandGeoPar() override;

    void clear() override;
    void putParams(FairParamList*) override;
    Bool_t getParams(FairParamList*) override;
    void printParams() override;

    TGeoNode* GetNeulandGeoNode() { return fNeulandGeoNode; } // FIXME: const?
    void SetNeulandGeoNode(const TGeoNode* const p);

    Double_t GetPaddleHalfLength() const;
    TVector3 ConvertToLocalCoordinates(const TVector3& position, const Int_t paddleID) const;
    TVector3 ConvertToGlobalCoordinates(const TVector3& position, const Int_t paddleID) const;
    TVector3 ConvertGlobalToPixel(const TVector3& position) const;

  private:
    std::map<Int_t, TGeoNode*> fPaddleGeoNodes;
    void BuildPaddleLookup();

    R3BCADNeulandGeoPar(const R3BCADNeulandGeoPar&);
    R3BCADNeulandGeoPar& operator=(const R3BCADNeulandGeoPar&);

    ClassDefOverride(R3BCADNeulandGeoPar, 1)
};

#endif // R3BNEULANDGEOPAR_H
