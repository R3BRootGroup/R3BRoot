// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                        R3BStarTrackRawHit                         -----
// -----                           Version 0.1                             -----
// -----                    Created 28.05.2014 by M. Labiche               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKRAWHIT_H
#define R3BSTARTRACKRAWHIT_H

#include "TObject.h"

class R3BStarTrackRawHit : public TObject {
public:
  // Default Constructor
  R3BStarTrackRawHit();
  
  /** Standard Constructor
   *@param moduleId    module unique identifier
   *@param side        module Side unique identifier
   *@param asicId      asic unique identifier
   *@param stripId     Strip unique identifier
   *@param adc         Total energy deposited on the crystal [ch]
   *@param time        Time since event start [ns]
   **/
  R3BStarTrackRawHit( UShort_t wordtype, UShort_t hitbit, UShort_t moduleId, UShort_t side, UShort_t asicId, UShort_t stripId, UShort_t adc_data, ULong_t time, ULong_t infofield, ULong_t infocode);
  
  //Destructor
  ~R3BStarTrackRawHit() { }
  
  //Getters
  inline const UShort_t& GetWordType() const { return fWordType; }
  inline const UShort_t& GetHitBit() const { return fHitBit; }
  inline const UShort_t& GetModuleId() const { return fModuleId; }
  inline const UShort_t& GetSide() const { return fSide; }
  inline const UShort_t& GetAsicId() const { return fAsicId; }
  inline const UShort_t& GetStripId() const { return fStripId; }
  inline const UShort_t& GetADCdata()    const { return fADCdata;    }
  inline const ULong_t&  GetTime()      const { return fTime;      }
  inline const ULong_t&  GetInfoField()      const { return fInfoField;      }
  inline const ULong_t&  GetInfoCode()      const { return fInfoCode;      }
	
protected:
  UShort_t fWordType;    // 
  UShort_t fHitBit;      // 
  UShort_t fModuleId;    //module unique identifier
  UShort_t fSide;        //module side unique identifier (128 strip per chip)
  UShort_t fAsicId;      //chip unique identifier
  UShort_t fStripId;     //strip unique identifier
  UShort_t fADCdata;     //total energy in the crystal
  ULong_t  fTime;        //time of the interaction
  ULong_t  fInfoCode;      //time of the interaction
  ULong_t  fInfoField;     //time of the interaction
  
public:
  ClassDef(R3BStarTrackRawHit,1)
};

#endif
