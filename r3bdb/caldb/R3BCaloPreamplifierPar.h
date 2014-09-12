#ifndef R3BCALOPREAMPLIFIERPAR_H
#define R3BCALOPREAMPLIFIERPAR_H

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


class R3BCaloPreamplifierPar : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BCaloPreamplifierPar (const char* name="R3BCaloPreamplifierPar",
                  const char* title="Califa Geometry Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloPreamplifierPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();
    /** Accessor functions **/
    string GetId()          	 const {return fId;} 
    string GetSerialNumber()     const {return fSerialNumber;}
    string GetRange()          	 const {return fRange;}
    string GetOrigin()     	 const {return fOrigin;}
    string GetLocation()     	 const {return fLocation;}
    string GetObservation()      const {return fObservation;}

    void SetId(string x) 		{fId  = x;}
    void SetSerialNumber(string x) 	{fSerialNumber  = x;}
    void SetRange(string x) 		{fRange  = x;}
    void SetOrigin(string x) 			{fOrigin  = x;}
    void SetLocation(string x) 		{fLocation  = x;}
    void SetObservation(string x) 	{fObservation  = x;}

    //UInt_t GetIndex(UInt_t /*def*///) const { return fPreamplifierNodeId;}

    Bool_t Compare(const R3BCaloPreamplifierPar& that ) const; 


    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloPreamplifierPar();
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
      return ValCondition(Detector::kCal,
                          DataType::kData,
                          ValTimeStamp(rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BCaloPreamplifierPar>* GetParamReader();
    FairDbWriter<R3BCaloPreamplifierPar>* GetParamWriter();
	FairDbWriter<R3BCaloPreamplifierPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    string fId;
    string fSerialNumber;
    string fRange;
    string fOrigin;
    string fLocation;
    string fObservation;

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloPreamplifierPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloPreamplifierPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloPreamplifierPar,1); // R3BCaloPreamplifierPar Parameter Container example
};



inline  FairDbReader<R3BCaloPreamplifierPar>* R3BCaloPreamplifierPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloPreamplifierPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloPreamplifierPar>* R3BCaloPreamplifierPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloPreamplifierPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALOPREAMPLIFIERPAR_H*/
