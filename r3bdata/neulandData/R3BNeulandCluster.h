#ifndef R3BNEULANDCLUSTER_H
#define R3BNEULANDCLUSTER_H

#include <vector>
#include <iostream>

#include "TObject.h"

#include "R3BNeulandDigi.h"

class R3BNeulandCluster : public TObject
{
  public:
    R3BNeulandCluster() {}

    R3BNeulandCluster(const R3BNeulandDigi& digi)
        : fDigis({ digi })
    {
    }

    R3BNeulandCluster(std::vector<R3BNeulandDigi>::iterator begin, std::vector<R3BNeulandDigi>::iterator end)
        : fDigis(begin, end)
    {
    }

    R3BNeulandCluster(const std::vector<R3BNeulandDigi> digis)
        : fDigis(digis)
    {
    }

    // Rule of zero: Non-ownership-managing classes should not have custom
    // destructors, copy/move constructors or copy/move assignment operators.

    // TODO: #ROOT6: Declare functions overriding virtual functions override
    void Print(const Option_t*) const; // override

    void AddDigi(const R3BNeulandDigi& digi) { fDigis.push_back(digi); }
    std::vector<R3BNeulandDigi> GetDigis() const { return fDigis; }

    R3BNeulandDigi GetFirstDigi() const;
    R3BNeulandDigi GetLastDigi() const;
    R3BNeulandDigi GetMaxEnergyDigi() const;
    R3BNeulandDigi GetForemostDigi() const;
    Double_t GetT() const;
    Double_t GetE() const;
    UInt_t GetSize() const { return fDigis.size(); }
    TVector3 GetEnergyCentroid() const;
    Double_t GetEnergyMoment() const;

  protected:
    std::vector<R3BNeulandDigi> fDigis;

  public:
    ClassDef(R3BNeulandCluster, 1)
};

// Support easy printing with energy.g. R3BNeulandCluster mycluster; cout << mycluster;
std::ostream& operator<<(std::ostream&, const R3BNeulandCluster&);

#endif // R3BNEULANDCLUSTER_H
