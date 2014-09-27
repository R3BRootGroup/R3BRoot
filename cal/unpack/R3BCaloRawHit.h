// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          R3BCaloRawHit                            -----
// -----                           Version 0.1                             -----
// -----                    Created 15.10.2013 by Y.Gonzalez               -----
// -----                    Modified 24/09/2014 by A.Perea                                               -----
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
   *@param tot         Time over threshold
   **/
  R3BCaloRawHit(UShort_t crystalId, UShort_t energy,
		UShort_t nf, UShort_t ns, ULong_t time, UChar_t error, UShort_t tot);
  
  //Destructor
  ~R3BCaloRawHit() { }
  
  //Getters
  inline const UShort_t& GetCrystalId() const { return fCrystalId; }
  inline const UShort_t& GetEnergy()    const { return fEnergy;    }
  inline const UShort_t& GetNf()        const { return fNf;        }
  inline const UShort_t& GetNs()        const { return fNs;        }
  inline const ULong_t&  GetTime()      const { return fTime;      }
  inline const UChar_t&  GetError()     const { return fError;     }	
  inline const UShort_t& GetTot()       const { return fTot;       }  
  
protected:
  UShort_t fCrystalId; // crystal unique identifier
  UShort_t fEnergy;    // total energy in the crystal
  UShort_t fNf;        // total fast amplitude in the crystal
  UShort_t fNs;        // total slow amplitude in the crystal
  ULong_t  fTime;      // time-stamp (common to all the hits in the event)
  UChar_t  fError;     // bit coded error flag
  UShort_t fTot;       // time-over-treshold
   
public:
  ClassDef(R3BCaloRawHit,1)
};

#endif
