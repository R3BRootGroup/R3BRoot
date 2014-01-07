// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          R3BCaloRawHit                            -----
// -----                           Version 0.1                             -----
// -----                    Created 15.10.2013 by Y.Gonzalez               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALORAWHIT_H
#define R3BCALORAWHIT_H

#include "TObject.h"

class R3BCaloRawHit : public TObject {
public:
  // Default Constructor
  R3BCaloRawHit();
  
  /** Standard Constructor
   *@param crystalId   Crystal unique identifier
   *@param energy      Total energy deposited on the crystal [GeV]
   *@param time        Time since event start [ns]
   **/
  R3BCaloRawHit(UShort_t crystalId, UShort_t energy, ULong_t time);
  
  //Destructor
  ~R3BCaloRawHit() { }
  
  //Getters
  inline const UShort_t& GetCrystalId() const { return fCrystalId; }
  inline const UShort_t& GetEnergy()    const { return fEnergy;    }
  inline const ULong_t&  GetTime()      const { return fTime;      }
	
protected:
  UShort_t fCrystalId; //crystal unique identifier
  UShort_t fEnergy;    //total energy in the crystal
  ULong_t  fTime;      //time of the interaction
  
public:
  ClassDef(R3BCaloRawHit,1)
};

#endif
