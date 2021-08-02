#ifndef R3BROOT_R3BNEULANDNEUTRONSKERAS_H
#define R3BROOT_R3BNEULANDNEUTRONSKERAS_H

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandMultiplicity.h"
#include "R3BNeulandNeutron.h"
#include "TCAConnector.h"
#include "TClonesArray.h"

class R3BNeulandNeutronsKeras : public FairTask
{
  public:
    R3BNeulandNeutronsKeras(TString model,
                            TString scaler,
                            TString inputMult = "NeulandMultiplicity",
                            TString inputCluster = "NeulandClusters",
                            TString output = "NeulandNeutrons");
    ~R3BNeulandNeutronsKeras() override = default;
    void Exec(Option_t*) override;

    void SetMinProb(double p) { fMinProb = p; }

  protected:
    InitStatus Init() override;

  private:
    const TString fInputMult;                    //!
    const R3BNeulandMultiplicity* fMultiplicity; //!

    const TString fInputCluster; //!
    TClonesArray* fClusters;     //! non-owning

    TCAOutputConnector<R3BNeulandNeutron> fNeutrons; //!
    double fMinProb;
    // const TString fPredictor;                        //!
    // const boost::format fFormatter; //!
    // const char* fFmt; //!

    struct ClusterWithProba
    {
        R3BNeulandCluster* c;
        double p;

        bool operator<(const ClusterWithProba& o) const { return this->p < o.p; }
        bool operator>(const ClusterWithProba& o) const { return this->p > o.p; }
    };

    ClassDefOverride(R3BNeulandNeutronsKeras, 0)
};

#endif // R3BROOT_R3BNEULANDNEUTRONSKERAS_H
