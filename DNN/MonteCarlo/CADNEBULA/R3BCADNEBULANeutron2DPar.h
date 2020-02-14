#ifndef R3BCADNEBULANEUTRON2DPAR_H
#define R3BCADNEBULANEUTRON2DPAR_H

#include "FairParGenericSet.h"
#include "TCutG.h"
#include <map>
class FairParamList;
class TMap;

/**
 * NEBULA number of clusters / energy - neutron multiplicity parameter storage
 * @author Jan Mayer
 *
 * Stores the cuts for the 2D Calibr method, can be asked about the neutron multiplicity
 */

class R3BCADNEBULANeutron2DPar : public FairParGenericSet
{
  public:
    // needs to be public?
    TMap* fNeutronCuts;

    R3BCADNEBULANeutron2DPar(const char* name = "R3BCADNEBULANeutron2DPar",
                           const char* title = "NEBULA Neutron 2D Parameters",
                           const char* context = "TestDefaultContext");
    ~R3BCADNEBULANeutron2DPar() override;

    void clear() override;
    void putParams(FairParamList*) override;
    Bool_t getParams(FairParamList*) override;
    void printParams() override;

    std::map<UInt_t, TCutG*> GetNeutronCuts() const;
    TCutG* GetNeutronCut(const Int_t n) const;
    void SetNeutronCuts(const std::map<UInt_t, TCutG*>& cuts);
    UInt_t GetNeutronMultiplicity(const Double_t energy, const Double_t nClusters) const;

  private:
    R3BCADNEBULANeutron2DPar(const R3BCADNEBULANeutron2DPar&);
    R3BCADNEBULANeutron2DPar& operator=(const R3BCADNEBULANeutron2DPar&);

    ClassDefOverride(R3BCADNEBULANeutron2DPar, 2)
};

#endif // R3BNEBULANEUTRON2DPAR_H
