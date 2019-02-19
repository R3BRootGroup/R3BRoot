#ifndef R3BNEULANDTACQUILAMAPPED2QCALPAR_H
#define R3BNEULANDTACQUILAMAPPED2QCALPAR_H

#include "FairTask.h"

class TClonesArray;
class TH1F;
class R3BEventHeader;
class R3BNeulandQCalPar;

class R3BNeulandTacquilaMapped2QCalPar : public FairTask
{

  public:
    R3BNeulandTacquilaMapped2QCalPar();
    R3BNeulandTacquilaMapped2QCalPar(const char* name, Int_t iVerbose = 1);
    virtual ~R3BNeulandTacquilaMapped2QCalPar();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

    void SetPlanes(Int_t planes) { fPlanes = planes; }

  private:
    Int_t fPlanes = 60;
    Int_t fPaddles = 50;

    TClonesArray* fHits;
    R3BNeulandQCalPar* fPar;

    Int_t fEventNumber = 0;

    R3BEventHeader* header;

    std::vector<std::vector<std::vector<TH1F*>>> fData;

  public:
    ClassDef(R3BNeulandTacquilaMapped2QCalPar, 1)
};

#endif
