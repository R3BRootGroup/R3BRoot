// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BWRAmsData                               -----
// -----                  Created 28/02/2019 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#ifndef R3BWRAmsData_H
#define R3BWRAmsData_H
#include "TObject.h"

class R3BWRAmsData : public TObject {

public:
  // Default Constructor
  R3BWRAmsData();
  
  /** Standard Constructor
   *@param timestamp
   **/
  R3BWRAmsData(Int_t timestamp);
  
  //Destructor
  ~R3BWRAmsData() { }
  
  //Getters
  inline const Int_t& GetTimeStamp() const { return fTimeStamp;   }
  
protected:
  Int_t fTimeStamp;  // timestamp for master
   
public:
  ClassDef(R3BWRAmsData,1)
};

#endif
