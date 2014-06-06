#ifndef R3BLOSCALPAR_H
#define R3BLOSCALPAR_H

#include "FairParGenericSet.h"          // for FairParGenericSet

#include "TObjArray.h"
#include "TGeoMaterial.h" 
#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

#include "R3BLosTCalPar.h"

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;


class R3BLosCalPar : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BLosCalPar (const char* name="R3BLosCalPar",
                  const char* title="LOS Time Calibration Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BLosCalPar(void);

    void   clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    void   Print();

	// Lists handling  
    void   AddTCalPar(R3BLosTCalPar* tch){fTCalParams->Add(tch);}
    TObjArray* GetListOfTCalPar(Int_t side) {return fTCalParams;}
    Int_t GetNumTCalPar() {return fTCalParams->GetEntries();} 
    R3BLosTCalPar* GetTCalParAt(Int_t idx){return (R3BLosTCalPar*) fTCalParams->At(idx);}


    // Global IO using run_id
    virtual void fill(UInt_t rid);
    virtual void store(UInt_t rid);

  private:
    TObjArray* fTCalParams;

    ClassDef(R3BLosCalPar,1); // R3BLosCalPar Parameter Container example
};

#endif /* !R3BLOSCALPARPAR_H*/

