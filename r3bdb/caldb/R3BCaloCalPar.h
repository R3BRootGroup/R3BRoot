#ifndef R3BCALOCALPAR1_H
#define R3BCALOCALPAR1_H

#include "FairParGenericSet.h"          // for FairParGenericSet

#include "TObjArray.h"
#include "TGeoMaterial.h" 
#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc
#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

#include "R3BCaloDUCalPar.h"

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;


class R3BCaloCalPar : public FairParGenericSet
{

  public :
    R3BCaloCalPar (const char* name="R3BCaloCalPar",
                  const char* title="Califa Calibration Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloCalPar(void);

    void   clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);
    void   Print();
    void ReadFile(string file);

	// Lists handling  
    void   AddDUCalPar(R3BCaloDUCalPar* tch){fDUCalParams->Add(tch);}  
    TObjArray* GetListOfDUCalPar(Int_t side) {return fDUCalParams;}
    Int_t GetNumDUCalPar() {return fDUCalParams->GetEntries();} 
    R3BCaloDUCalPar* GetDUCalParAt(Int_t idx){return (R3BCaloDUCalPar*) fDUCalParams->At(idx);} 


   // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0){std::string rr=""; return rr;}

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloCalPar();
    }

    // Atomic IO (intrinsic)
    virtual void Fill(FairDbResultPool& res_in,
                      const FairDbValRecord* valrec){;}
    virtual void Store(FairDbOutTableBuffer& res_out,
                       const FairDbValRecord* valrec) const {;}

    // Global IO using run_id
    virtual void fill(UInt_t rid);
    virtual void store(UInt_t rid);

    virtual ValCondition GetContext(UInt_t rid) {
      return ValCondition(FairDbDetector::EDetector::kCal,
                          DataType::kData,
                          ValTimeStamp(rid));
    }


  private:
    TObjArray* fDUCalParams;

    ClassDef(R3BCaloCalPar,1); // R3BCaloCalPar Parameter Container example
};

#endif /* !R3BCALOCALPAR_H*/

