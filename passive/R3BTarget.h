#ifndef R3BTarget_H
#define R3BTarget_H

#include "R3BModule.h"
#include <TString.h>


class R3BTarget : public R3BModule {
public:
  R3BTarget(const char* name, const char* Title = "R3B Target");
  R3BTarget(const char* name,
            TString geoFile,
            Float_t x = 0.,
            Float_t y = 0.,
            Float_t z = 0.,
            Float_t rot_x = 0.,
            Float_t rot_y = 0.,
            Float_t rot_z = 0.,
            const char* Title = "R3B Target");
  R3BTarget();
  virtual ~R3BTarget();

  virtual void ConstructGeometry();
  
  virtual Bool_t CheckIfSensitive(std::string name);
  
protected:
  TString *fTargetName; //!
  
public:
  ClassDef(R3BTarget,1)
};

#endif //R3BTarget_H
