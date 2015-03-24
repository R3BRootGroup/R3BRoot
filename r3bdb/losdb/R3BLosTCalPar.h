#ifndef R3BLOSTCALPAR_H
#define R3BLOSTCALPAR_H

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

#define NCHMAX 2000

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;



class R3BLosTCalPar : public FairDbObjTableMap
{

using TObject::Compare;

  public :
  R3BLosTCalPar (const char* name="R3BLosTCalPar",
				 const char* title="Los Calibrated Time Parameter",
				 const char* context="TestDefaultContext",
				 Bool_t own=kTRUE);
    virtual ~R3BLosTCalPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    /** Accessor functions **/
    Int_t GetAggregateNo()       const {return fCompId;}
    Int_t GetCompId()   const {return fCompId;}
    Int_t GetChannel() const {return fChannel;}
    Double_t GetTimeAt(Int_t i) const {return fTime[i];}
    Int_t    GetBinLowAt(Int_t i) const {return fBinLow[i];}
    Int_t    GetBinUpAt(Int_t i) const {return fBinUp[i];}

    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}
    void SetChannel(Int_t i) {fChannel=i;}
    void SetBinLowAt(Int_t ch,Int_t i) {fBinLow[i]=ch;}
	void SetBinUpAt(Int_t ch,Int_t i) {fBinUp[i]=ch;}
    void SetTimeAt(Double_t t,Int_t i) {fTime[i]= t;}

    UInt_t GetIndex(UInt_t /*def*/) const { return fChannel; }

    Bool_t Compare(const R3BLosTCalPar& that ) const; 

    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    /*virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BLosTCalPar();
    }*/

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
        return ValCondition(FairDbDetector::kLos,
                          DataType::kData,
                          ValTimeStamp((Double_t) rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BLosTCalPar>* GetParamReader();
    FairDbWriter<R3BLosTCalPar>* GetParamWriter();
	FairDbWriter<R3BLosTCalPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fChannel;
    Int_t fBinLow[NCHMAX];// lower channel
    Int_t fBinUp[NCHMAX];// upper channel
    Double_t fTime[NCHMAX];// Cal. Time

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BLosTCalPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BLosTCalPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BLosTCalPar,1); // R3BLosTCalPar Parameter Container example
};



inline  FairDbReader<R3BLosTCalPar>* R3BLosTCalPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BLosTCalPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BLosTCalPar>* R3BLosTCalPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BLosTCalPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BLOSTCALPAR_H*/
