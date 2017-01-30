#ifndef R3BFi4CALITEM_H
#define R3BFi4CALITEM_H

#include "TObject.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016

class R3BFi4CalItem : public TObject
{
 public:
  // Default Constructor
  R3BFi4CalItem();
  
  // Standard Constructor
  R3BFi4CalItem(UChar_t detector, UInt_t fiber_n, UInt_t mppc_top, UInt_t mppc_bottom, 
		Float_t energy_top, Float_t energy_bottom, 
		Float_t time_top, Float_t time_bottom);
    
  // Destructor
  virtual ~R3BFi4CalItem() { }
  
  // Getters
  inline const UChar_t& GetDetector()       const { return fDetector; }
  inline const UInt_t&  GetFiber()          const { return fFiber_n;  }
  inline const UInt_t&  GetMPPC_top()       const { return fMPPC_top;  }
  inline const UInt_t&  GetMPPC_bottom()    const { return fMPPC_bottom;  }
  inline const Float_t&  GetEnergy_top()     const { return fEnergy_top;  }
  inline const Float_t&  GetEnergy_bottom()  const { return fEnergy_bottom;   }
  inline const Float_t& GetTime_top()       const { return fTime_top;    }
  inline const Float_t& GetTime_bottom()    const { return fTime_bottom;  }
  
 private:
  UChar_t fDetector;
  UInt_t  fFiber_n; 
  UInt_t  fMPPC_top;  
  UInt_t  fMPPC_bottom;  
  Float_t fEnergy_top;   // 0xfff is energy data, Anything in 0xfffff000 indicates an error or overflow
  Float_t fEnergy_bottom;
  Float_t fTime_top;   
  Float_t fTime_bottom;   
    
 public:
    ClassDef(R3BFi4CalItem, 2)
      };

#endif
