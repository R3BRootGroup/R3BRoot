#ifndef R3BTarget_H
#define R3BTarget_H

#include "FairDetector.h"
#include "FairModule.h"

class R3BTarget : public FairModule {
  public:
    R3BTarget(const char * name, const char *Title="R3B Target");
    R3BTarget();
    virtual ~R3BTarget();
    virtual void ConstructGeometry();
    ClassDef(R3BTarget,1) 
  
};

#endif //R3BTarget_H

