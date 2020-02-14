#ifndef R3BNEULANDCADCLUSTER_H
#define R3BNEULANDCADCLUSTER_H

#include "R3BNeulandCADHit.h"
#include "TObject.h"
#include <iostream>
#include <utility>
#include <vector>

class R3BNeulandCADCluster : public TObject
{
  public:
    R3BNeulandCADCluster() = default;
    explicit R3BNeulandCADCluster(const R3BNeulandCADHit& hit)
        : fHits({ hit })
    {
    }
    R3BNeulandCADCluster(std::vector<R3BNeulandCADHit>::iterator begin, std::vector<R3BNeulandCADHit>::iterator end)
        : fHits(begin, end)
    {
    }
    explicit R3BNeulandCADCluster(std::vector<R3BNeulandCADHit> hits)
        : fHits(std::move(hits))
    {
    }

    const std::vector<R3BNeulandCADHit>& GetHits() const { return fHits; }
    R3BNeulandCADHit GetFirstHit() const;
    R3BNeulandCADHit GetLastHit() const;
    R3BNeulandCADHit GetMaxEnergyHit() const;
    R3BNeulandCADHit GetForemostHit() const;
    TVector3 GetPosition() const;
    Double_t GetT() const;
    Double_t GetE() const;
    Double_t GetBeta() const { return GetFirstHit().GetBeta(); }
    Double_t GetEToF() const { return GetFirstHit().GetEToF(); }
    Size_t GetSize() const { return fHits.size(); }
    TVector3 GetEnergyCentroid() const;
    Double_t GetEnergyMoment() const;
    Double_t GetRCluster(Double_t beta) const;

    void Print(const Option_t*) const override;

    bool operator==(const R3BNeulandCADCluster& b) const { return this->GetHits() == b.GetHits(); }

  private:
    std::vector<R3BNeulandCADHit> fHits;

    ClassDefOverride(R3BNeulandCADCluster, 1)
};

std::ostream& operator<<(std::ostream&, const R3BNeulandCADCluster&); // Support easy printing

#endif // R3BNEULANDCLUSTER_H
