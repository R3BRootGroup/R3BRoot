#ifndef NEULANDRECONSTRUCTIONOPERATIONH
#define NEULANDRECONSTRUCTIONOPERATIONH

#include "ClusteringEngine.h"
#include "FairMCPoint.h"
#include "Filterable.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"
#include "ReconstructionEngine.h"
#include "TCAConnector.h"
#include <algorithm>
#include <memory>
#include <vector>

class R3BNeulandNeutron2DPar;
class TClonesArray;

namespace Neuland
{
    // abstract class, cannot be instantiated, but can be used as a base class.
    class ReconstructionOperation
    {
      public:
        using NeutronCandidates = std::vector<R3BNeulandCluster*>;
        using Multiplicity = UInt_t;
        using Neutrons = std::vector<R3BNeulandNeutron>;

        virtual void Init() {}
        inline virtual void Exec(NeutronCandidates&, Multiplicity&, Neutrons&) const {}
        virtual ~ReconstructionOperation() {}
    };

    class Multiplicity2DCalibr : public ReconstructionOperation
    {
      public:
        Multiplicity2DCalibr() = default;
        void Init() override;
        inline void Exec(NeutronCandidates&, Multiplicity&, Neutrons&) const override;

      private:
        R3BNeulandNeutron2DPar* fPar; // non-owning
    };

    class MultiplicityFixed : public ReconstructionOperation
    {
      public:
        MultiplicityFixed(const Int_t n)
            : fMult(n)
        {
        }
        inline void Exec(NeutronCandidates&, Multiplicity& n, Neutrons&) const override { n = fMult; }

      private:
        Int_t fMult;
    };

    class MultiplicityCheating : public ReconstructionOperation
    {
      public:
        MultiplicityCheating()
            : fNPNIPs("NeulandPrimaryNeutronInteractionPoints")
        {
        }
        void Init() override { fNPNIPs.Init(); }
        inline void Exec(NeutronCandidates&, Multiplicity& n, Neutrons&) const override
        {
            n = fNPNIPs.Retrieve().size();
        }

      private:
        TCAInputConnector<FairMCPoint> fNPNIPs;
    };

    class FilteringOperation : public ReconstructionOperation
    {
      public:
        using Filter = Filterable<R3BNeulandCluster*>::Filter;

        FilteringOperation() = default;
        FilteringOperation(const std::vector<Filter>& filters)
            : fClusterFilters(filters)
        {
        }
        inline void Exec(NeutronCandidates& clusters, Multiplicity&, Neutrons&) const override
        {
            std::remove_if(
                clusters.begin(), clusters.end(), [&](R3BNeulandCluster* c) { return !fClusterFilters.IsValid(c); });
        }
        inline void AddFilter(const Filter f) { fClusterFilters.Add(f); }

      private:
        Filterable<R3BNeulandCluster*> fClusterFilters;
    };

    class GenericOperation : public ReconstructionOperation
    {
      public:
        using Operation = std::function<void(NeutronCandidates&, Multiplicity&, Neutrons&)>;
        GenericOperation(Operation& fun)
            : fFunction(fun)
        {
        }
        inline void Exec(NeutronCandidates& clusters, Multiplicity& multiplicity, Neutrons& neutrons) const override
        {
            fFunction(clusters, multiplicity, neutrons);
        }

      private:
        Operation fFunction;
    };

    class GetNeutrons : public ReconstructionOperation
    {
      public:
        void Exec(NeutronCandidates&, Multiplicity&, Neutrons&) const override;
    };

    // Try to track elastic scattering and remove clusters created by secondary interactions
    class ElasticProtonScatteringRemoveSecondaries : public ReconstructionOperation
    {
      public:
        ElasticProtonScatteringRemoveSecondaries();
        void Exec(NeutronCandidates&, Multiplicity&, Neutrons&) const override;

      private:
        Neuland::ClusteringEngine<R3BNeulandCluster*> fTracker;
    };

    class UseClustersWithProtonTrackMatchingEToF : public ReconstructionOperation
    {
      public:
        UseClustersWithProtonTrackMatchingEToF(const Double_t maxNeutronEnergyDifference)
            : fMaxNeutronEnergyDifference(maxNeutronEnergyDifference)
        {
        }

        void Exec(NeutronCandidates&, Multiplicity&, Neutrons&) const override;

      private:
        Double_t fMaxNeutronEnergyDifference;
    };

} // namespace Neuland

namespace Neuland
{
    class ReconstructionOperations : public ReconstructionEngine
    {
      public:
        using Op = std::shared_ptr<Neuland::ReconstructionOperation>;

        void Init() override;
        // Copy, will be modified inside
        std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const override;

        inline void AddOperation(const Op& op) { fOps.push_back(op); }

        inline void AddOperation(Neuland::ReconstructionOperation* op) { fOps.push_back(Op(op)); }

      private:
        std::vector<Op> fOps;
    };
} // namespace Neuland

#endif // NEULANDRECONSTRUCTIONOPERATIONH
