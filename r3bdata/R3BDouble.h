// Copy of RooDouble to store a double value in a TNamed to store it in a tree.

#ifndef R3B_Double
#define R3B_Double

#include "Rtypes.h"

class R3BDouble {
public:

  R3BDouble() : _value(0) {}
  R3BDouble(Double_t value);
  R3BDouble(const R3BDouble& other) : _value(other._value) {}
  virtual ~R3BDouble() {}

  inline operator Double_t() const { 
    return _value ; 
  }
  R3BDouble& operator=(Double_t value) { 
    _value = value ; return *this ; 
  }

protected:
  Double_t _value ; // Value payload
};

#endif
