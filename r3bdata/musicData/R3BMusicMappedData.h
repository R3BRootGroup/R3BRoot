// -------------------------------------------------------------------------
// -----              R3BMusicMappedData header file                   -----
// -----             Created 15/10/19  by J.L. Rodriguez-Sanchez       -----
// -------------------------------------------------------------------------

/**  R3BMusicMappedData
 **  A HitData is the representation of the 
 **  information obtained from the MUSIC detector.
 **/

#ifndef R3BMusicMappedData_H
#define R3BMusicMappedData_H
#include "TObject.h"

class R3BMusicMappedData : public TObject {

public:
  /** Default constructor **/
  R3BMusicMappedData();

  /** Constructor with arguments
   *@param secID    Section ID
   *@param anodeID  Anode ID 
   *@param time     Time [channels]
   *@param energy   Energy deposit [channels]
   **/
  R3BMusicMappedData(UChar_t secID, 
                       UChar_t anodeID, 
                       UShort_t time,
	               UShort_t energy);


  /** Destructor **/
  virtual ~R3BMusicMappedData() { }


  /** Accessors **/
  inline const UChar_t&    GetSecID()     const { return fSecID;   }
  inline const UChar_t&    GetAnodeID()   const { return fAnodeID; }
  inline const UShort_t&   GetTime()      const { return fTime;    }
  inline const UShort_t&   GetEnergy()    const { return fEnergy;  }


  /** Modifiers **/
  void SetSecID(UChar_t id)       { fSecID = id;     };
  void SetAnodeID(UChar_t id)     { fAnodeID = id;   };
  void SetDTime(UShort_t time)    { fTime = time;    };
  void SetEnergy(UShort_t energy) { fEnergy = energy;};


 protected:

  UChar_t fSecID;     
  UChar_t fAnodeID;   
  UShort_t fTime;
  UShort_t fEnergy;

  ClassDef(R3BMusicMappedData,1)
};

#endif
