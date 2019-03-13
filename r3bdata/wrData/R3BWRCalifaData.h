// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BWRCalifaData                            -----
// -----                  Created 28/02/2019 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#ifndef R3BWRCalifaData_H
#define R3BWRCalifaData_H
#include "TObject.h"

class R3BWRCalifaData : public TObject {

public:
  // Default Constructor
  R3BWRCalifaData();
  
  /** Standard Constructor
   *@param timestamp
   **/
  R3BWRCalifaData(uint64_t timestamp);
  
  //Destructor
  ~R3BWRCalifaData() { }
  
  //Getters
  inline const uint64_t GetTimeStamp() const { return fTimeStamp;   }
  
protected:
  uint64_t fTimeStamp;  // timestamp for califa
   
public:
  ClassDef(R3BWRCalifaData,1)
};

#endif
