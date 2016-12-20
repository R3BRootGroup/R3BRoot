#ifndef R3BSTRAWTUBESMAPPED2CAL
#define R3BSTRAWTUBESMAPPED2CAL

#include "FairTask.h"

class R3BTCalPar;

class R3BStrawtubesMapped2Cal: public FairTask
{
  public:
    R3BStrawtubesMapped2Cal(const char * = "StrawtubesTcal", Int_t = 1);
    virtual ~R3BStrawtubesMapped2Cal();
    virtual void Exec(Option_t *);
    virtual void FinishEvent();
    virtual void FinishTask();
    virtual InitStatus Init();
    virtual InitStatus ReInit();
    virtual void SetParContainers();

  private:
    TClonesArray* fMappedItems;
    TClonesArray* fCalItems;
    Int_t fNofCalItems;
    R3BTCalPar* fTcalPar;

  public:
    ClassDef(R3BStrawtubesMapped2Cal, 0)
};

#endif
