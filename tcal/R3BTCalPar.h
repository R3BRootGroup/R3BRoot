#ifndef R3BTCALPAR_H
#define R3BTCALPAR_H

#define NMODULEMAX 6000

#include "FairParGenericSet.h" // for FairParGenericSet

#include "TObjArray.h"

#include "R3BTCalModulePar.h"

using namespace std;

class FairParamList;

class R3BTCalPar : public FairParGenericSet
{

  public:
    R3BTCalPar(const char* name = "TCalPar", const char* title = "TCAL calibration", const char* context = "TestDefaultContext", Bool_t own = kTRUE);
    virtual ~R3BTCalPar(void);

    void clear(void);
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    void printParams();

    // Lists handling
    void AddModulePar(R3BTCalModulePar* tch)
    {
        fTCalParams->Add(tch);
    }
    TObjArray* GetListOfModulePar(Int_t)
    {
        return fTCalParams;
    }
    Int_t GetNumModulePar()
    {
        return fTCalParams->GetEntries();
    }
    R3BTCalModulePar* GetModuleParAt(Int_t idx)
    {
        return (R3BTCalModulePar*)fTCalParams->At(idx);
    }

  private:
    const R3BTCalPar& operator=(const R3BTCalPar&);
    R3BTCalPar(const R3BTCalPar&);

    TObjArray* fTCalParams;

    ClassDef(R3BTCalPar, 1); // R3BTCalPar Parameter Container example
};

#endif /* !R3BTCALPAR_H*/
