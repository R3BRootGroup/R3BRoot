// -------------------------------------------------------------------------
// -----                      R3BMusicCalData header file              -----
// -----                  Created 15/10/19 by JL Rodriguez             -----
// -------------------------------------------------------------------------

#ifndef R3BMusicCalData_H
#define R3BMusicCalData_H
#include "TObject.h"

class R3BMusicCalData :  public TObject {

public:
  /** Default constructor **/
  R3BMusicCalData();

  /** Constructor with arguments
   *@param secID    Section ID
   *@param anodeID  Anode ID 
   *@param dtime    Drift time calibrated in [mm] for the position
   *@param energy   Energy deposit [channels], for simulation in [MeV]
   **/
  R3BMusicCalData(UChar_t secID, UChar_t anodeID, 
                                    Double_t dtime,
	                            Double_t energy);

  /** Destructor **/
  virtual ~R3BMusicCalData() { }


  /** Accessors **/
  inline const UChar_t&     GetSecID()     const { return fSecID;   }
  inline const UChar_t&     GetAnodeID()   const { return fAnodeID; }
  inline const Double_t&    GetDTime()     const { return fDT;      }
  inline const Double_t&    GetEnergy()    const { return fEnergy;  }


  /** Modifiers **/
  void SetSecID(UChar_t id)       { fSecID = id;     };
  void SetAnodeID(UChar_t id)     { fAnodeID = id;   };
  void SetDTime(Double_t dt)      { fDT = dt;        };
  void SetEnergy(Double_t energy) { fEnergy = energy;};


 protected:

  UChar_t fSecID, fAnodeID;
  Double_t fDT, fEnergy;

  ClassDef(R3BMusicCalData,1)
};

#endif
