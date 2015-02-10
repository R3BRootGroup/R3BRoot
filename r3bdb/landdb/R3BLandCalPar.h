#ifndef R3BLANDCALPAR_H
#define R3BLANDCALPAR_H

#include "FairParGenericSet.h"          // for FairParGenericSet

#include "TObjArray.h"
#include "TGeoMaterial.h" 
#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

#include "R3BLandTCalPar.h"

using namespace std;

class FairParamList;


class R3BLandCalPar : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BLandCalPar (const char* name="R3BLandCalPar",
                  const char* title="Land Time Calibration Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BLandCalPar(void);

    void   clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    void   Print();

	// Lists handling  
    void   AddTCalPar(R3BLandTCalPar* tch){fTCalParams->Add(tch);}  
    TObjArray* GetListOfTCalPar(Int_t side) {return fTCalParams;}
    Int_t GetNumTCalPar() {return fTCalParams->GetEntries();} 
    R3BLandTCalPar* GetTCalParAt(Int_t idx){return (R3BLandTCalPar*) fTCalParams->At(idx);} 

  private:
    TObjArray* fTCalParams;

    ClassDef(R3BLandCalPar,1); // R3BLandCalPar Parameter Container example
};

#endif /* !R3BLANDCALPARPAR_H*/

