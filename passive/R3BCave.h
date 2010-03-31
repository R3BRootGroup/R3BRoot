#ifndef R3BCAVE_H
#define R3BCAVE_H

#include "FairDetector.h"
#include "FairModule.h"


class R3BCave : public FairModule {
  public:
    R3BCave(const char * name, const char *Title="Exp Cave");
    R3BCave();
    virtual ~R3BCave();
    virtual void ConstructGeometry();
    

private:
     Double_t world[3]; //!
     ClassDef(R3BCave,1) //R3BCaveSD
};

#endif //R3BCave_H

