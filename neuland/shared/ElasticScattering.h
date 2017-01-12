#ifndef NEULAND_ELASTICPROTONENERGY
#define NEULAND_ELASTICPROTONENERGY

#include "R3BNeulandCluster.h"

namespace Neuland
{
    Double_t RecoilProtonEnergy(const R3BNeulandCluster*);
    Double_t RecoilScatteringAngle(const R3BNeulandCluster*);
    Double_t ScatteredNeutronEnergy(const R3BNeulandCluster* first, const R3BNeulandCluster* second);
    Double_t ScatteredNeutronAngle(const R3BNeulandCluster* first, const R3BNeulandCluster* second);
    Double_t NeutronEnergyFromElasticProtonScattering(const R3BNeulandCluster*);
    Double_t NeutronEnergyFromElasticScattering(const R3BNeulandCluster* first,
                                                const R3BNeulandCluster* second,
                                                const Double_t targetMass);
    Double_t MaybeElasticScattering(const R3BNeulandCluster* first,
                                    const R3BNeulandCluster* second,
                                    const Double_t targetMass);
    Double_t ElasticScatteringTargetMass(const R3BNeulandCluster* first, const R3BNeulandCluster* second);
} // namespace

#endif // NEULAND_ELASTICPROTONENERGY
