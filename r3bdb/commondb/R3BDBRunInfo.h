#ifndef R3BDBRUNINFO_H
#define R3BDBRUNINFO_H

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

#define NCHMAX 500

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;



class R3BDBRunInfo : public FairParGenericSet
{

using TObject::Compare;

  public :
  R3BDBRunInfo (const char* name="R3BDBRunInfo",
				 const char* title="R3B DB Run Info",
				 const char* context="TestDefaultContext",
				 Bool_t own=kTRUE);
    virtual ~R3BDBRunInfo(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    /** Accessor functions **/
    Int_t GetComboNo()       const {return fCompId;}
    Int_t GetCompId()   const {return fCompId;}
    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}

    void   SetRunId(Int_t runId) {fRunId = runId;}  
    void   SetRunTime(ValTimeStamp run_time) {fRunTime = run_time;}  
    Int_t  GetRunId() {return fRunId;}
    ValTimeStamp  GetRunTime() {return fRunTime;}  

    UInt_t GetIndex(UInt_t /*def*/) const { return fRunId; }

    Bool_t Compare(const R3BDBRunInfo& that ) const; 

    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BDBRunInfo();
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
      return ValCondition(Detector::kUnknown,
                          DataType::kData,
                          ValTimeStamp(rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BDBRunInfo>* GetParamReader();
    FairDbWriter<R3BDBRunInfo>* GetParamWriter();
	FairDbWriter<R3BDBRunInfo>* ActivateWriter(Int_t rid);

    ValTimeStamp GetRunTime(Int_t runId);
	Bool_t Commit();

   Int_t        GetSeqNo() const
     { return CalcUniqueSeqNo(fRunId); }
   static Int_t  CalcUniqueSeqNo(Int_t run);

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fRunId;
    ValTimeStamp fRunTime;   

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BDBRunInfo>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BDBRunInfo>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BDBRunInfo,1); // R3BDBRunInfo Parameter Container example
};



inline  FairDbReader<R3BDBRunInfo>* R3BDBRunInfo::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BDBRunInfo>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BDBRunInfo>* R3BDBRunInfo::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BDBRunInfo>();
    return fParam_Writer;
  }
}

#endif /* !R3BDBRUNINFO_H*/
