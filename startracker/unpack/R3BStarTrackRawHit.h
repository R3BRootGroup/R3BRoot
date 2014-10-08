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
  R3BStarTrackRawHit( ULong_t WR, ULong_t WRlb, UInt_t wordtype, UInt_t hitbit, UInt_t moduleId, UInt_t side, UInt_t asicId, UInt_t stripId, UInt_t adc_data, ULong_t time, ULong_t timelb,  ULong_t timeExt, ULong_t timeExtlb, ULong_t infofield, UInt_t infocode);
  
  //Destructor
  ~R3BStarTrackRawHit() { }
  
  //Getters
  inline const ULong_t&  GetWR()      const { return fWR;      }
  inline const ULong_t&  GetWRlb()      const { return fWRlb;      }
   inline const UInt_t& GetWordType() const { return fWordType; }
  inline const UInt_t& GetHitBit() const { return fHitBit; }
  inline const UInt_t& GetModuleId() const { return fModuleId; }
  inline const UInt_t& GetSide() const { return fSide; }
  inline const UInt_t& GetAsicId() const { return fAsicId; }
  inline const UInt_t& GetStripId() const { return fStripId; }
  inline const UInt_t& GetADCdata()    const { return fADCdata;    }
  inline const ULong_t&  GetTime()      const { return fTime;      }
  inline const ULong_t&  GetTimelb()      const { return fTimelb;      }
  inline const ULong_t&  GetTimeExt()      const { return fTimeExt;      }
  inline const ULong_t&  GetTimeExtlb()      const { return fTimeExtlb;      }
  inline const UInt_t&  GetInfoCode()      const { return fInfoCode;      }
  inline const ULong_t&  GetInfoField()      const { return fInfoField;      }
	
protected:
  ULong_t  fWR;        //header White Rabbit time (all 64 bits) of the interaction
  ULong_t  fWRlb;        //header White Rabbit time (low bit) of the interaction
  UInt_t fWordType;    // 
  UInt_t fHitBit;      // 
  UInt_t fModuleId;    //module unique identifier
  UInt_t fSide;        //module side unique identifier (128 strip per chip)
  UInt_t fAsicId;      //chip unique identifier
  UInt_t fStripId;     //strip unique identifier
  UInt_t fADCdata;     //total energy in the crystal
  ULong_t  fTime;        //time (all 64 bits) of the interaction
  ULong_t  fTimelb;        //time (low bit) of the interaction
  ULong_t  fTimeExt;        //time (all 64 bits) of the external input
  ULong_t  fTimeExtlb;        //time (low bit) of the external input
  UInt_t  fInfoCode;      //time of the interaction
  ULong_t  fInfoField;     //time of the interaction
  
public:
  ClassDef(R3BStarTrackRawHit,1)
};

#endif
