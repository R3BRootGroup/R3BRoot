//*-- AUTHOR : Marc Labiche
//*-- Created : 28/06/2012
#ifndef R3BVacVesselCool_H
#define R3BVacVesselCool_H

#include "FairDetector.h"
#include "R3BModule.h"
#include <TString.h>


class R3BVacVesselCool : public R3BModule {
public:
  R3BVacVesselCool(const char* name, const char* Title = "R3B VacVesselCool");
  R3BVacVesselCool(const char* name,
                   TString geoFile,
                   Float_t x = 0.,
                   Float_t y = 0.,
                   Float_t z = 0.,
                   Float_t rot_x = 0.,
                   Float_t rot_y = 0.,
                   Float_t rot_z = 0.,
                   const char* Title = "R3B VacVesselCool");
  R3BVacVesselCool();
  virtual ~R3BVacVesselCool();

  virtual void ConstructGeometry();
  
  virtual Bool_t CheckIfSensitive(std::string name);
  
protected:
  TString *fVacVesselCoolName; //!
  void ConstructGeometry1();
  
  ClassDef(R3BVacVesselCool,1)
};

#endif //R3BVacVesselCool_H

