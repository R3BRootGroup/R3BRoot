#ifndef R3BFi4HITITEM_H
#define R3BFi4HITITEM_H

#include "TObject.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016
// adapted by Joachim March 2016

class R3BFi4HitItem : public TObject
{
 public:
  // Default Constructor
  R3BFi4HitItem();
  
  // Standard Constructor
  R3BFi4HitItem(UInt_t detector, UInt_t fiber, Float_t xpos, Float_t ypos, Float_t zpos, //pos
		Float_t energy_top, Float_t energy_bottom,                               //energy
		Float_t time_top, Float_t time_bottom);                                  //time
    
  // Destructor
  virtual ~R3BFi4HitItem() { }
  
  // Getters
  inline const UChar_t& GetDetector()       const { return fDetector; }
  inline const UInt_t&  GetFiber()          const { return fFiber_n;  }
  inline const Float_t& GetFiber_Xpos()     const { return fFiber_Xpos;  }
  inline const Float_t& GetFiber_Ypos()     const { return fFiber_Ypos;  }
  inline const Float_t& GetFiber_Zpos()     const { return fFiber_Zpos;  }
  inline const Float_t& GetEnergy_top()     const { return fEnergy_top;  }
  inline const Float_t& GetEnergy_bottom()  const { return fEnergy_bottom;   }
  inline const Float_t& GetTime_top()       const { return fTime_top;    }
  inline const Float_t& GetTime_bottom()    const { return fTime_bottom;  }
  
 private:
  UChar_t fDetector;
  UInt_t  fFiber_n;
  Float_t fFiber_Xpos; 
  Float_t fFiber_Ypos; 
  Float_t fFiber_Zpos; 
  Float_t fEnergy_top;   // 0xfff is energy data, Anything in 0xfffff000 indicates an error or overflow
  Float_t fEnergy_bottom;
  Float_t fTime_top;   
  Float_t fTime_bottom;   
    
 public:
    ClassDef(R3BFi4HitItem, 2)
      };

#endif
