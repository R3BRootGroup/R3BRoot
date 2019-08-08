// Copy of RooDouble to store a double value in a TNamed to store it in a tree.

#ifndef R3B_Double
#define R3B_Double

#include "Rtypes.h"

class R3BDouble {
public:

  R3BDouble() : fValue(0) {}
  R3BDouble(Double_t value);
  R3BDouble(const R3BDouble& other) : fValue(other.fValue) {}
  virtual ~R3BDouble() {}

  inline operator Double_t() const { 
    return fValue; 
  }
  R3BDouble& operator=(Double_t value) { 
    fValue = value; 
    return *this; 
  }

protected:
  Double_t fValue; // Value payload
};

#endif
