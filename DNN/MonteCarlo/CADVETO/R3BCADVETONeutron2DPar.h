#ifndef R3BCADVETONEUTRON2DPAR_H
#define R3BCADVETONEUTRON2DPAR_H

#include "FairParGenericSet.h"
#include "TCutG.h"
#include <map>
class FairParamList;
class TMap;

/**
 * VETO number of clusters / energy - neutron multiplicity parameter storage
 * @author Jan Mayer
 *
 * Stores the cuts for the 2D Calibr method, can be asked about the neutron multiplicity
 */

class R3BCADVETONeutron2DPar : public FairParGenericSet
{
  public:
    // needs to be public?
    TMap* fNeutronCuts;

    R3BCADVETONeutron2DPar(const char* name = "R3BCADVETONeutron2DPar",
                           const char* title = "VETO Neutron 2D Parameters",
                           const char* context = "TestDefaultContext");
    ~R3BCADVETONeutron2DPar() override;

    void clear() override;
    void putParams(FairParamList*) override;
    Bool_t getParams(FairParamList*) override;
    void printParams() override;

    std::map<UInt_t, TCutG*> GetNeutronCuts() const;
    TCutG* GetNeutronCut(const Int_t n) const;
    void SetNeutronCuts(const std::map<UInt_t, TCutG*>& cuts);
    UInt_t GetNeutronMultiplicity(const Double_t energy, const Double_t nClusters) const;

  private:
    R3BCADVETONeutron2DPar(const R3BCADVETONeutron2DPar&);
    R3BCADVETONeutron2DPar& operator=(const R3BCADVETONeutron2DPar&);

    ClassDefOverride(R3BCADVETONeutron2DPar, 2)
};

#endif // R3BVETONEUTRON2DPAR_H
