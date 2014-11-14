#ifndef R3BCALODETECTIONUNITSIMGEOPAR_H
#define R3BCALODETECTIONUNITSIMGEOPAR_H

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


class R3BCaloDetectionUnitSimGeoPar : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BCaloDetectionUnitSimGeoPar (const char* name="R3BCaloDetectionUnitSimGeoPar",
                  const char* title="Califa Geometry Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloDetectionUnitSimGeoPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    /** Accessor functions **/
    Int_t GetComboNo()       const {return fCompId;}
    Int_t GetCompId()   const {return fCompId;}
    Int_t GetCrystalType() const {return fCrystalTypeId;}
    TVector3* GetVertexAt(Int_t i) const {return (TVector3*) fVertices->At(i);}
    TObjArray* GetVertices() const {return fVertices;} 
    Double_t GetDz() const {return fDZ;}

    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}
    void SetCrystalType(Int_t i) {fCrystalTypeId=i;}
    void AddVertexAt(Double_t x, Double_t y, Double_t z, Int_t i) {fVertices->AddAt(new TVector3(x,y,z),i);}
    void SetDz(Double_t z){fDZ=z;}
    Bool_t SetShape( TGeoArb8* trap);

    UInt_t GetIndex(UInt_t /*def*/) const { return fCrystalTypeId; }

    Bool_t Compare(const R3BCaloDetectionUnitSimGeoPar& that ) const; 
    void InitArray(); 

    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloDetectionUnitSimGeoPar();
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
    FairDbReader<R3BCaloDetectionUnitSimGeoPar>* GetParamReader();
    FairDbWriter<R3BCaloDetectionUnitSimGeoPar>* GetParamWriter();

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fCrystalTypeId; 
    Double_t fDZ;
    TObjArray* fVertices;
    
    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloDetectionUnitSimGeoPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloDetectionUnitSimGeoPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloDetectionUnitSimGeoPar,1); // R3BCaloDetectionUnitSimGeoPar Parameter Container example
};



inline  FairDbReader<R3BCaloDetectionUnitSimGeoPar>* R3BCaloDetectionUnitSimGeoPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloDetectionUnitSimGeoPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloDetectionUnitSimGeoPar>* R3BCaloDetectionUnitSimGeoPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloDetectionUnitSimGeoPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALODETECTIONUNITSIMGEOPAR_H*/
