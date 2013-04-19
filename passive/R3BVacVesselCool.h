//*-- AUTHOR : Marc Labiche
//*-- Created : 28/06/2012
#ifndef R3BVacVesselCool_H
#define R3BVacVesselCool_H

#include "FairDetector.h"
#include "R3BModule.h"
#include <TString.h>



class R3BVacVesselCool : public R3BModule {
  public:
    R3BVacVesselCool(const char * name, const char *Title="R3B VacVesselCool");
    R3BVacVesselCool();
    virtual ~R3BVacVesselCool();
    virtual void ConstructGeometry();

 protected:
     TString *fVacVesselCoolName; //!
     void ConstructGeometry1();


    ClassDef(R3BVacVesselCool,1)
  
};

#endif //R3BVacVesselCool_H

