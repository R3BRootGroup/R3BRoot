#ifndef R3BSci2HitPar_H
#define R3BSci2HitPar_H

#include "FairParGenericSet.h"
#include "TObjArray.h"
#include "TObject.h"
#include <TObjString.h>

class FairParamList;

class R3BSci2HitPar : public FairParGenericSet
{
  public:
    /** Standard constructor **/
    R3BSci2HitPar(const TString& name = "Sci2HitPar",
                  const TString& title = "Sci2 Hit Parameters",
                  const TString& context = "Sci2HitParContext");

    /** Destructor **/
    virtual ~R3BSci2HitPar();

    /** Reset all parameters **/
    virtual void clear();

    /** Store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Print values of parameters to the standard output **/
    virtual void print();
    void printParams();

    /** Accessor functions **/
    const Float_t GetPos_p0() { return fPos_p0; }
    const Float_t GetPos_p1() { return fPos_p1; }

    void SetPos_p0(Float_t value) { fPos_p0 = value; }
    void SetPos_p1(Float_t value) { fPos_p1 = value; }

  private:
    Float_t fPos_p0, fPos_p1;

    const R3BSci2HitPar& operator=(const R3BSci2HitPar&);
    R3BSci2HitPar(const R3BSci2HitPar&);

    ClassDef(R3BSci2HitPar, 1);
};

#endif
