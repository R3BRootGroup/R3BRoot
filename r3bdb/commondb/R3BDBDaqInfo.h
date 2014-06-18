#ifndef R3BDBDAQINFO_H
#define R3BDBDAQINFO_H

#include "FairParGenericSet.h"          // for FairParGenericSet

#include "TObjArray.h"
#include "TGeoMaterial.h" 
#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string
#include <map>

#include "R3BDBLmdFileInfo.h";

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;


class R3BDBDaqInfo : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BDBDaqInfo (const char* name="R3BDBDaqInfo",
                  const char* title="Global Daq Info",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BDBDaqInfo(void);

    void   clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    void   Print();

	// File Info handling  
    Int_t GetNumIndexedFile() {return  fFileInfo.size();} 


    // Global IO using run_id
    virtual void fill(UInt_t rid);
    virtual void store(UInt_t rid);

  private:
	std::map<UInt_t, R3BDBLmdFileInfo*>  fFileInfo;

    ClassDef(R3BDBDaqInfo,1); // R3BDBDaqInfo Parameter Container example
};

#endif /* !R3BDBDAQINFO_H*/

