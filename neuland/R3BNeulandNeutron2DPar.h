#ifndef R3BNEULANDNEUTRON2DPAR_H
#define R3BNEULANDNEUTRON2DPAR_H

#include <map>
#include "FairParGenericSet.h"
#include "TCutG.h"
class FairParamList;
class TMap;

/**
 * NeuLAND number of clusters / energy - neutron multiplicty parameter storage
 * @author Jan Mayer
 *
 * Stores the cuts for the 2D Calibr method, can be asked about the neutron muliplicity
 */

class R3BNeulandNeutron2DPar : public FairParGenericSet
{
  public:
    // needs to be public?
    TMap* fNeutronCuts;

    R3BNeulandNeutron2DPar(const char* name = "R3BNeulandNeutron2DPar",
                           const char* title = "Neuland Neutron 2D Parameters",
                           const char* context = "TestDefaultContext");
    ~R3BNeulandNeutron2DPar();

    // TODO: #ROOT6: Declare functions overrriding virtual functions overrride
    void clear();                     // override
    void putParams(FairParamList*);   // override
    Bool_t getParams(FairParamList*); // override
    void printParams();               // override

    std::map<UInt_t, TCutG*> GetNeutronCuts() const;
    void SetNeutronCuts(const std::map<UInt_t, TCutG*>& cuts);
    UInt_t GetNeutronMultiplicity(const Double_t energy, const Double_t nClusters) const;

  private:
    R3BNeulandNeutron2DPar(const R3BNeulandNeutron2DPar&);
    R3BNeulandNeutron2DPar& operator=(const R3BNeulandNeutron2DPar&);

    ClassDef(R3BNeulandNeutron2DPar, 1)
};

#endif // R3BNEULANDNEUTRON2DPAR_H
