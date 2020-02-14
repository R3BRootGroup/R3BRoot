#ifndef R3BCADNEBULAVETONEUTRON2DPAR_H
#define R3BCADNEBULAVETONEUTRON2DPAR_H

#include "FairParGenericSet.h"
#include "TCutG.h"
#include <map>
class FairParamList;
class TMap;

/**
 * NEBULAVETO number of clusters / energy - neutron multiplicity parameter storage
 * @author Jan Mayer
 *
 * Stores the cuts for the 2D Calibr method, can be asked about the neutron multiplicity
 */

class R3BCADNEBULAVETONeutron2DPar : public FairParGenericSet
{
  public:
    // needs to be public?
    TMap* fNeutronCuts;

    R3BCADNEBULAVETONeutron2DPar(const char* name = "R3BCADNEBULAVETONeutron2DPar",
                           const char* title = "NEBULAVETO Neutron 2D Parameters",
                           const char* context = "TestDefaultContext");
    ~R3BCADNEBULAVETONeutron2DPar() override;

    void clear() override;
    void putParams(FairParamList*) override;
    Bool_t getParams(FairParamList*) override;
    void printParams() override;

    std::map<UInt_t, TCutG*> GetNeutronCuts() const;
    TCutG* GetNeutronCut(const Int_t n) const;
    void SetNeutronCuts(const std::map<UInt_t, TCutG*>& cuts);
    UInt_t GetNeutronMultiplicity(const Double_t energy, const Double_t nClusters) const;

  private:
    R3BCADNEBULAVETONeutron2DPar(const R3BCADNEBULAVETONeutron2DPar&);
    R3BCADNEBULAVETONeutron2DPar& operator=(const R3BCADNEBULAVETONeutron2DPar&);

    ClassDefOverride(R3BCADNEBULAVETONeutron2DPar, 2)
};

#endif // R3BNEBULAVETONEUTRON2DPAR_H
