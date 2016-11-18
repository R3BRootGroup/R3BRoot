#ifndef R3BNEULANDNEUTRON_H
#define R3BNEULANDNEUTRON_H

#include <iostream>

#include "TObject.h"
#include "TVector3.h"

class R3BNeulandNeutron : public TObject
{
  public:
    R3BNeulandNeutron()
        : fT(0)
        , fPosition(TVector3(0, 0, 0))
    {
    }

    R3BNeulandNeutron(const Double_t time, const TVector3& pos)
        : fT(time)
        , fPosition(pos)
    {
    }

    // Rule of zero: Non-ownership-managing classes should not have custom
    // destructors, copy/move constructors or copy/move assignment operators.

    // TODO: #ROOT6: Declare functions overrriding virtual functions overrride
    void Print(const Option_t*) const; // override

    void SetT(const Double_t time) { fT = time; }
    Double_t GetT() const { return fT; }

    void SetPosition(const TVector3& position) { fPosition = position; }
    TVector3 GetPosition() const { return fPosition; }

    Double_t GetEtot() const;
    Double_t GetEkin() const;
    Double_t GetGamma() const;
    TVector3 GetP() const;

  protected:
    Double_t fT;
    TVector3 fPosition;

  public:
    ClassDef(R3BNeulandNeutron, 1)
};

// Support easy printing with R3BNeulandNeutron myneutron; cout << myneutron;
std::ostream& operator<<(std::ostream&, const R3BNeulandNeutron&);

#endif // R3BNEULANDNEUTRON_H
