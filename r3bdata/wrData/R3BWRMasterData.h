// ---------------------------------------------------------------------------
// -----                                                                 -----
// -----                      R3BWRMasterData                            -----
// -----                  Created 28/02/2019 by J.L. Rodriguez           -----
// -----                                                                 -----
// ---------------------------------------------------------------------------

#ifndef R3BWRMasterData_H
#define R3BWRMasterData_H
#include "TObject.h"

class R3BWRMasterData : public TObject {

public:
  // Default Constructor
  R3BWRMasterData();
  
  /** Standard Constructor
   *@param timestamp
   **/
  R3BWRMasterData(uint64_t timestamp);
  
  //Destructor
  ~R3BWRMasterData() { }
  
  //Getters
  inline const uint64_t GetTimeStamp() const { return fTimeStamp;   }
  
protected:
  uint64_t fTimeStamp;  // timestamp for master
   
public:
  ClassDef(R3BWRMasterData,1)
};

#endif
