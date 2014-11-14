#ifndef R3BCALODETECTIONUNITPAR_H
#define R3BCALODETECTIONUNITPAR_H

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


class R3BCaloDetectionUnitPar : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BCaloDetectionUnitPar (const char* name="R3BCaloDetectionUnitPar",
                  const char* title="Califa Geometry Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloDetectionUnitPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();
    /** Accessor functions **/
    Int_t GetId()          		const {return fId;} 
    Int_t GetTypeId()          		const {return fTypeId;}
    Int_t GetCrystal()          	const {return fCrystal;}
    Int_t GetAPD()      		const {return fAPD;}
    string GetOrigin()     		const {return fOrigin;}
    string GetLocation()     		const {return fLocation;}
    Int_t GetPosition()          	const {return fPosition;}
    string GetObservation()     	const {return fObservation;}

    void SetId(Int_t x) 			{fId  = x;}
    void SetTypeId(Int_t x) 			{fTypeId  = x;}
    void SetCrystal(Int_t x) 			{fCrystal  = x;}
    void SetAPD(Int_t x) 			{fAPD  = x;}
    void SetOrigin(string x) 			{fOrigin  = x;}
    void SetLocation(string x) 	        	{fLocation  = x;}
    void SetPosition(Int_t x) 			{fPosition  = x;}
    void SetObservation(string x) 	        {fObservation  = x;}
    
    //UInt_t GetIndex(UInt_t /*def*///) const { return fDetectionUnitNodeId;}

    Bool_t Compare(const R3BCaloDetectionUnitPar& that ) const; 


    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloDetectionUnitPar();
    }

    // Atomic IO (intrinsic)
    virtual void Fill(FairDbResultPool& res_in,
                      const FairDbValRecord* valrec);
    virtual void Store(FairDbOutTableBuffer& res_out,
                       const FairDbValRecord* valrec) const;
    // Global IO using run_id
    virtual void fill(UInt_t rid);
    virtual void store(UInt_t rid);
    virtual void update(Int_t seqno);

    // Validity frame definition
    virtual ValCondition GetContext(UInt_t rid) {
      return ValCondition(FairDbDetector::kCal,
                          DataType::kData,
                          ValTimeStamp(rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BCaloDetectionUnitPar>* GetParamReader();
    FairDbWriter<R3BCaloDetectionUnitPar>* GetParamWriter();
	FairDbWriter<R3BCaloDetectionUnitPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    Int_t fId;
    Int_t fTypeId;
    Int_t fCrystal;
    Int_t fAPD;
    string fOrigin;
    string fLocation;
    Int_t fPosition;
    string fObservation;

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloDetectionUnitPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloDetectionUnitPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloDetectionUnitPar,1); // R3BCaloDetectionUnitPar Parameter Container example
};



inline  FairDbReader<R3BCaloDetectionUnitPar>* R3BCaloDetectionUnitPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloDetectionUnitPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloDetectionUnitPar>* R3BCaloDetectionUnitPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloDetectionUnitPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALODETECTIONUNITPAR_H*/
