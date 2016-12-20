#ifndef R3BSTRAWTUBESCAL2HIT
#define R3BSTRAWTUBESCAL2HIT

#include "FairTask.h"

class TClonesArray;

class R3BStrawtubesCal2Hit: public FairTask
{
  public:
    R3BStrawtubesCal2Hit(const char * = "StrawtubesCal2Hit", Int_t = 1);
    virtual ~R3BStrawtubesCal2Hit();
    virtual void Exec(Option_t *);
    virtual void FinishEvent();
    virtual void FinishTask();
    virtual InitStatus Init();
    virtual InitStatus ReInit();

  private:
    TClonesArray *fCalItems;
    TClonesArray *fHitItems;
    UInt_t fNofHitItems;

  public:
    ClassDef(R3BStrawtubesCal2Hit, 0)
};

#endif

