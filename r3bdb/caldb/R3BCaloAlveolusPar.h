#ifndef R3BCALOALVEOLUSPAR_H
#define R3BCALOALVEOLUSPAR_H

#include "FairParGenericSet.h"          // for FairParGenericSet
#include "DataType.h"                    // for EDataType::kData
#include "ValCondition.h"                 // for ValCondition
#include "ValTimeStamp.h"               // for ValTimeStamp
#include "db_detector_def.h"            // for Detector, etc

#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc

#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

#include "FairDbWriter.h"
#include "FairDbReader.h"


using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;


class R3BCaloAlveolusPar : public FairDbObjTableMap
{

using TObject::Compare;

  public :
    R3BCaloAlveolusPar (const char* name="R3BCaloAlveolusPar",
                  const char* title="Califa Geometry Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloAlveolusPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();
    /** Accessor functions **/
    Int_t GetComboNo()                const {return fCompId;}
    Int_t GetCompId()                 const {return fCompId;}
    Int_t GetAlveolusNodeId()          const {return fAlveolusNodeId;} 
    Int_t GetAlveolusTypeId()          const {return fAlveolusTypeId;}
    Double_t GetCFThickness()       const {return fCFThickness;}
    const Double_t*   GetCenter()           const {return &fCenter[0];}
    const Double_t*   GetRotEuler()         const {return &fRotEuler[0];}

    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}
    void SetAlveolusNodeId(Int_t x) {fAlveolusNodeId  = x;}
    void SetAlveolusTypeId(Int_t x) {fAlveolusTypeId  = x;}
    void SetCenter(Double_t x, Double_t y, Double_t z) {fCenter[0]=x;fCenter[1]=y;fCenter[2]=z;}
    void SetRotEuler(Double_t x, Double_t y, Double_t z) {fRotEuler[0]=x;fRotEuler[1]=y;fRotEuler[2]=z;}
	void SetCFThickness(Double_t x) {fCFThickness  = x;}   


    UInt_t GetIndex(UInt_t /*def*/) const { return fAlveolusNodeId;}

    Bool_t Compare(const R3BCaloAlveolusPar& that ) const; 


    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloAlveolusPar();
    }

    // Atomic IO (intrinsic)
    virtual void Fill(FairDbResultPool& res_in,
                      const FairDbValRecord* valrec);
    virtual void Store(FairDbOutTableBuffer& res_out,
                       const FairDbValRecord* valrec) const;
    // Global IO using run_id
    virtual void fill(UInt_t rid);
    virtual void store(UInt_t rid);

    // Validity frame definition
    virtual ValCondition GetContext(UInt_t rid) {
      return ValCondition(FairDbDetector::kCal,
                          DataType::kData,
                          ValTimeStamp(rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BCaloAlveolusPar>* GetParamReader();
    FairDbWriter<R3BCaloAlveolusPar>* GetParamWriter();
	FairDbWriter<R3BCaloAlveolusPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fAlveolusNodeId;
    Int_t fAlveolusTypeId;
    Double_t fCFThickness;    
    Double_t fCenter[3];
    Double_t fRotEuler[3];

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloAlveolusPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloAlveolusPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloAlveolusPar,1); // R3BCaloAlveolusPar Parameter Container example
};



inline  FairDbReader<R3BCaloAlveolusPar>* R3BCaloAlveolusPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloAlveolusPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloAlveolusPar>* R3BCaloAlveolusPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloAlveolusPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALOALVEOLUSPAR_H*/
