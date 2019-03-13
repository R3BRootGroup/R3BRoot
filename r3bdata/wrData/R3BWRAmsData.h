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
  R3BWRAmsData(uint64_t timestamp);
  
  //Destructor
  ~R3BWRAmsData() { }
  
  //Getters
  inline const uint64_t GetTimeStamp() const { return fTimeStamp;   }
  
protected:
  uint64_t fTimeStamp;  // timestamp for ams detectors
   
public:
  ClassDef(R3BWRAmsData,1)
};

#endif
