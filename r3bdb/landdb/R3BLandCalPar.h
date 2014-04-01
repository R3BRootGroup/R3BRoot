#ifndef R3BLANDCALPAR_H
#define R3BLANDCALPAR_H

#include "FairParGenericSet.h"          // for FairParGenericSet

#include "TObjArray.h"
#include "TGeoMaterial.h" 
#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

#include "R3BLandTCalPar.h";

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
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
    void   AddTChannel(R3BLandTCalPar* tch,Int_t side){fTChannels[side]->Add(tch);}  
    TObjArray* GetListOfTChannel(Int_t side) {return fTChannels[side];}
    Int_t GetNumTChannels(Int_t side) {return fTChannels[side]->GetEntries();} 
    R3BLandTCalPar* GetTChannelAt(Int_t idx, Int_t side){return (R3BLandTCalPar*) fTChannels[side]->At(idx);} 


    // Global IO using run_id
    virtual void fill(UInt_t rid);
    virtual void store(UInt_t rid);

  private:
    TObjArray* fTChannels[2];

    ClassDef(R3BLandCalPar,1); // R3BLandCalPar Parameter Container example
};

#endif /* !R3BLANDCALPARPAR_H*/

