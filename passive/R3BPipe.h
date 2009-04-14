#ifndef R3BPIPE_H
#define R3BPIPE_H

#include "TNamed.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "FairDetector.h"
#include "FairModule.h"


class R3BPipe : public FairModule {
  public:
    R3BPipe(const char * name, const char *Title="R3B Pipe");
    R3BPipe();

    virtual ~R3BPipe();
    virtual void ConstructGeometry();
   
  ClassDef(R3BPipe,1) //R3BPIPE

};

#endif //R3BPIPE_H

