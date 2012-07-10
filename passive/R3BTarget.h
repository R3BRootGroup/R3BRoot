#ifndef R3BTarget_H
#define R3BTarget_H

#include "FairDetector.h"
#include "R3BModule.h"
#include <TString.h>


class R3BTarget : public R3BModule {
  public:
    R3BTarget(const char * name, const char *Title="R3B Target");
    R3BTarget();
    virtual ~R3BTarget();
    virtual void ConstructGeometry();

 protected:
     TString *fTargetName; //!
     void ConstructGeometry1();
     void ConstructGeometry2();
     void ConstructGeometry3();
     void ConstructGeometry4();
     void ConstructGeometry5();
     void ConstructGeometry6();
     void ConstructGeometry7();

    ClassDef(R3BTarget,1)
  
};

#endif //R3BTarget_H

