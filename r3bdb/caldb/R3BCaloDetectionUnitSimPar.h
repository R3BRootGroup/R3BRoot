#ifndef R3BCALODETECTIONUNITSIMPAR_H
#define R3BCALODETECTIONUNITSIMPAR_H

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


class R3BCaloDetectionUnitSimPar : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BCaloDetectionUnitSimPar (const char* name="R3BCaloDetectionUnitSimPar",
                  const char* title="Califa Geometry Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloDetectionUnitSimPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();
    /** Accessor functions **/
    Int_t GetComboNo()                const {return fCompId;}
    Int_t GetCompId()                 const {return fCompId;}
    Int_t GetCrystalNodeId()          const {return fCrystalNodeId;} 
    Int_t GetCrystalTypeId()          const {return fCrystalTypeId;}
    Double_t GetWrapThickness()       const {return fWrapThickness;}
    const Double_t*   GetCenter()           const {return &fCenter[0];}
    const Double_t*   GetRotEuler()         const {return &fRotEuler[0];}

    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}
    void SetCrystalNodeId(Int_t x) {fCrystalNodeId  = x;}
    void SetCrystalTypeId(Int_t x) {fCrystalTypeId  = x;}
    void SetCenter(Double_t x, Double_t y, Double_t z) {fCenter[0]=x;fCenter[1]=y;fCenter[2]=z;}
    void SetRotEuler(Double_t x, Double_t y, Double_t z) {fRotEuler[0]=x;fRotEuler[1]=y;fRotEuler[2]=z;}
	void SetWrapThickness(Double_t x) {fWrapThickness  = x;}   


    UInt_t GetIndex(UInt_t /*def*/) const { return fCrystalNodeId;}

    Bool_t Compare(const R3BCaloDetectionUnitSimPar& that ) const; 


    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloDetectionUnitSimPar();
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
    FairDbReader<R3BCaloDetectionUnitSimPar>* GetParamReader();
    FairDbWriter<R3BCaloDetectionUnitSimPar>* GetParamWriter();
	FairDbWriter<R3BCaloDetectionUnitSimPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fCrystalNodeId;
    Int_t fCrystalTypeId;
    Double_t fWrapThickness;    
    Double_t fCenter[3];
    Double_t fRotEuler[3];

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloDetectionUnitSimPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloDetectionUnitSimPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloDetectionUnitSimPar,1); // R3BCaloDetectionUnitSimPar Parameter Container example
};



inline  FairDbReader<R3BCaloDetectionUnitSimPar>* R3BCaloDetectionUnitSimPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloDetectionUnitSimPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloDetectionUnitSimPar>* R3BCaloDetectionUnitSimPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloDetectionUnitSimPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALODETECTIONUNITSIMPAR_H*/
