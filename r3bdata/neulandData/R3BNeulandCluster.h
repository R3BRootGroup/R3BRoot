#ifndef R3BNEULANDCLUSTER_H
#define R3BNEULANDCLUSTER_H

#include <iostream>
#include <utility>
#include <vector>

#include "TObject.h"

#include "R3BNeulandDigi.h"

class R3BNeulandCluster : public TObject
{
  public:
    R3BNeulandCluster() = default;

    explicit R3BNeulandCluster(const R3BNeulandDigi& digi)
        : fDigis({ digi })
    {
    }

    R3BNeulandCluster(std::vector<R3BNeulandDigi>::iterator begin, std::vector<R3BNeulandDigi>::iterator end)
        : fDigis(begin, end)
    {
    }

    explicit R3BNeulandCluster(std::vector<R3BNeulandDigi> digis)
        : fDigis(std::move(digis))
    {
    }

    void Print(const Option_t*) const override;

    void AddDigi(const R3BNeulandDigi& digi) { fDigis.push_back(digi); }
    std::vector<R3BNeulandDigi> GetDigis() const { return fDigis; }

    R3BNeulandDigi GetFirstDigi() const;
    R3BNeulandDigi GetLastDigi() const;
    R3BNeulandDigi GetMaxEnergyDigi() const;
    R3BNeulandDigi GetForemostDigi() const;
    TVector3 GetPosition() const;
    Double_t GetT() const;
    Double_t GetE() const;
    Double_t GetBeta() const { return GetFirstDigi().GetBeta(); }
    Double_t GetEToF() const { return GetFirstDigi().GetEToF(); }
    UInt_t GetSize() const { return fDigis.size(); }
    TVector3 GetEnergyCentroid() const;
    Double_t GetEnergyMoment() const;
    Double_t GetRCluster(Double_t beta) const;

    bool operator==(const R3BNeulandCluster& b) const { return this->GetDigis() == b.GetDigis(); }

  protected:
    std::vector<R3BNeulandDigi> fDigis;

  public:
    ClassDefOverride(R3BNeulandCluster, 1)
};

// Support easy printing with energy.g. R3BNeulandCluster mycluster; cout << mycluster;
std::ostream& operator<<(std::ostream&, const R3BNeulandCluster&);

#endif // R3BNEULANDCLUSTER_H
