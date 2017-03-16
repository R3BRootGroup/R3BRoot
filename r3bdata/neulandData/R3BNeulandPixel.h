#ifndef R3BNEULANDPIXEL_H
#define R3BNEULANDPIXEL_H

#include <iostream>

#include "TObject.h"
#include "TVector3.h"

class R3BNeulandPixel : public TObject
{
  public:
    R3BNeulandPixel()
        : fX(0)
        , fY(0)
        , fZ(0)
        , fE(0.f)
        , fT(0.f)
    {
    }

    R3BNeulandPixel(const Int_t X, const Int_t Y, const Int_t Z, const Float_t E, const Float_t T)
        : fX(X)
        , fY(Y)
        , fZ(Z)
        , fE(E)
        , fT(T)
    {
    }

    // Rule of zero: Non-ownership-managing classes should not have custom
    // destructors, copy/move constructors or copy/move assignment operators.

    // TODO: #ROOT6: Declare functions overriding virtual functions override
    void Print(const Option_t*) const; // override

    void SetX(const Int_t X) { fX = X; }
    Int_t GetX() const { return fX; }

    void SetY(const Int_t Y) { fY = Y; }
    Int_t GetY() const { return fY; }

    void SetZ(const Int_t Z) { fZ = Z; }
    Int_t GetZ() const { return fZ; }

    void SetE(const Float_t E) { fE = E; }
    Float_t GetE() const { return fE; }

    void SetT(const Float_t T) { fT = T; }
    Float_t GetT() const { return fT; }

  protected:
    Int_t fX;
    Int_t fY;
    Int_t fZ;
    Float_t fE;
    Float_t fT;

  public:
    ClassDef(R3BNeulandPixel, 1)
};

// Support easy printing with R3BNeulandPixel mypixel; cout << mypixel;
std::ostream& operator<<(std::ostream&, const R3BNeulandPixel&);

#endif // R3BNEULANDPIXEL_H
