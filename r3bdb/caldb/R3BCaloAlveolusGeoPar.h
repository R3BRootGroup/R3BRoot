#ifndef R3BCALOALVEOLUSGEOPAR_H
#define R3BCALOALVEOLUSGEOPAR_H

#include "FairParGenericSet.h"          // for FairParGenericSet
#include "DataType.h"                    // for EDataType::kData
#include "ValCondition.h"                 // for ValCondition
#include "ValTimeStamp.h"               // for ValTimeStamp
#include "db_detector_def.h"            // for Detector, etc

#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc

#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

#include "TObjArray.h"                  // Store for vertices 
#include "TVector3.h"                   // Vertex
#include "TGeoArb8.h"                   // Trap
#include "FairDbWriter.h"
#include "FairDbReader.h"

#define NVERTICES 8

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;


class R3BCaloAlveolusGeoPar : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BCaloAlveolusGeoPar (const char* name="R3BCaloAlveolusGeoPar",
                  const char* title="Califa Geometry Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloAlveolusGeoPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    /** Accessor functions **/
    Int_t GetComboNo()       const {return fCompId;}
    Int_t GetCompId()   const {return fCompId;}
    Int_t GetAlveolusType() const {return fAlveolusTypeId;}
    TVector3* GetVertexAt(Int_t i) const {return (TVector3*) fVertices->At(i);}
    TObjArray* GetVertices() const {return fVertices;} 
    Double_t GetDz() const {return fDZ;}

    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}
    void SetAlveolusType(Int_t i) {fAlveolusTypeId=i;}
    void AddVertexAt(Double_t x, Double_t y, Double_t z, Int_t i) {fVertices->AddAt(new TVector3(x,y,z),i);}
    void SetDz(Double_t z){fDZ=z;}
    Bool_t SetShape( TGeoArb8* trap);

    UInt_t GetIndex(UInt_t /*def*/) const { return fAlveolusTypeId; }

    Bool_t Compare(const R3BCaloAlveolusGeoPar& that ) const; 
    void InitArray(); 

    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloAlveolusGeoPar();
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
      return ValCondition(Detector::kCal,
                          DataType::kData,
                          ValTimeStamp(rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BCaloAlveolusGeoPar>* GetParamReader();
    FairDbWriter<R3BCaloAlveolusGeoPar>* GetParamWriter();

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fAlveolusTypeId; 
    Double_t fDZ;
    TObjArray* fVertices;
    
    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloAlveolusGeoPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloAlveolusGeoPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloAlveolusGeoPar,1); // R3BCaloAlveolusGeoPar Parameter Container example
};



inline  FairDbReader<R3BCaloAlveolusGeoPar>* R3BCaloAlveolusGeoPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloAlveolusGeoPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloAlveolusGeoPar>* R3BCaloAlveolusGeoPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloAlveolusGeoPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALOALVEOLUSGEOPAR_H*/
