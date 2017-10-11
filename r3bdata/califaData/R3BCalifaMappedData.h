// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                       R3BCalifaMappedData                         -----
// -----                   Created 15/10/2013 by Y.Gonzalez                -----
// -----                  Modified 19/12/2014 by P.Cabanelas               -----
// -----------------------------------------------------------------------------

#ifndef R3BCALIFAMAPPEDDATA_H
#define R3BCALIFAMAPPEDDATA_H
#include "TObject.h"

class R3BCalifaMappedData : public TObject {

  
public:
  // Default Constructor
  R3BCalifaMappedData();
  
  /** Standard Constructor
   *@param crystalId   Crystal unique identifier
   *@param energy      Total energy deposited in the crystal [GeV]
   *@param nf          Total fast amplitude deposited in the crystal [a.u.]
   *@param ns          Total slow amplitude deposited on the crystal [a.u.]
   *@param time        Time since event start [ns]
   *@param tot         Time over threshold
   **/
  R3BCalifaMappedData(UShort_t crystalId, Int_t energy, Int_t nf, Int_t ns, ULong_t time, UChar_t error, UShort_t tot);
  
  //Destructor
  ~R3BCalifaMappedData() { }
  
  //Getters
  inline const UShort_t& GetCrystalId() const { return fCrystalId; }
  inline const Int_t&    GetEnergy()    const { return fEnergy;    }
  inline const Int_t&    GetNf()        const { return fNf;        }
  inline const Int_t&    GetNs()        const { return fNs;        }
  inline const ULong_t&  GetTime()      const { return fTime;      }
  inline const UChar_t&  GetError()     const { return fError;     }	
  inline const UShort_t& GetTot()       const { return fTot;       }  
  
protected:
  UShort_t fCrystalId; // crystal unique identifier
  Int_t    fEnergy;    // total energy in the crystal
  Int_t    fNf;        // total fast amplitude in the crystal
  Int_t    fNs;        // total slow amplitude in the crystal
  ULong_t  fTime;      // time-stamp (common to all the hits in the event)
  UChar_t  fError;     // bit coded error flag
  UShort_t fTot;       // time-over-treshold
   
public:
  ClassDef(R3BCalifaMappedData,1)
};

#endif
