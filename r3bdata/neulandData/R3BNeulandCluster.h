#ifndef R3BNEULANDCLUSTER_H
#define R3BNEULANDCLUSTER_H

#include "R3BNeulandHit.h"
#include "TObject.h"
#include <iostream>
#include <utility>
#include <vector>

class R3BNeulandCluster : public TObject
{
  public:
    R3BNeulandCluster() = default;
    explicit R3BNeulandCluster(const R3BNeulandHit& hit)
        : fHits({ hit })
    {
    }
    R3BNeulandCluster(std::vector<R3BNeulandHit>::iterator begin, std::vector<R3BNeulandHit>::iterator end)
        : fHits(begin, end)
    {
    }
    explicit R3BNeulandCluster(std::vector<R3BNeulandHit> hits)
        : fHits(std::move(hits))
    {
    }

    const std::vector<R3BNeulandHit>& GetHits() const { return fHits; }
    R3BNeulandHit GetFirstHit() const;
    R3BNeulandHit GetLastHit() const;
    R3BNeulandHit GetMaxEnergyHit() const;
    R3BNeulandHit GetForemostHit() const;
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

    bool operator==(const R3BNeulandCluster& b) const { return this->GetHits() == b.GetHits(); }

  private:
    std::vector<R3BNeulandHit> fHits;

    ClassDefOverride(R3BNeulandCluster, 1)
};

std::ostream& operator<<(std::ostream&, const R3BNeulandCluster&); // Support easy printing

#endif // R3BNEULANDCLUSTER_H
