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
   *@param energy      Total energy deposited in the crystal [GeV]
   *@param nf          Total fast amplitude deposited in the crystal [a.u.]
   *@param ns          Total slow amplitude deposited on the crystal [a.u.]
   *@param time        Time since event start [ns]
   **/
  R3BCaloRawHit(UShort_t crystalId, UShort_t energy,
		UShort_t nf, UShort_t ns, ULong_t time, Char_t error);
  
  //Destructor
  ~R3BCaloRawHit() { }
  
  //Getters
  inline const UShort_t& GetCrystalId() const { return fCrystalId; }
  inline const UShort_t& GetEnergy()    const { return fEnergy;    }
  inline const UShort_t& GetNf()        const { return fNf;        }
  inline const UShort_t& GetNs()        const { return fNs;        }
  inline const ULong_t&  GetTime()      const { return fTime;      }
  inline const Char_t&   GetError()     const { return fError;      }	
protected:
  UShort_t fCrystalId; //crystal unique identifier
  UShort_t fEnergy;    //total energy in the crystal
  UShort_t fNf;        //total fast amplitude in the crystal
  UShort_t fNs;        //total slow amplitude in the crystal
  ULong_t  fTime;      //time of the interaction
  Char_t   fError;     //bit coded error flag
  
public:
  ClassDef(R3BCaloRawHit,1)
};

#endif
