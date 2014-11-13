#ifndef R3BLANDTCALPAR_H
#define R3BLANDTCALPAR_H

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



class R3BLandTCalPar : public FairParGenericSet
{

using TObject::Compare;

  public :
  R3BLandTCalPar (const char* name="R3BLandTCalPar",
				 const char* title="Land Calibrated Time Parameter",
				 const char* context="TestDefaultContext",
				 Bool_t own=kTRUE);
    virtual ~R3BLandTCalPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    /** Accessor functions **/
    Int_t GetComboNo()       const {return fCompId;}
    Int_t GetCompId()   const {return fCompId;}
    Int_t GetBarId() const {return fBarId;}
    Int_t GetSide() const {return fSide;}
    Double_t GetTimeAt(Int_t i) const {return fTime[i];}
    Int_t    GetBinLowAt(Int_t i) const {return fBinLow[i];}
    Int_t    GetBinUpAt(Int_t i) const {return fBinUp[i];}

    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}
    void SetBarId(Int_t i) {fBarId=i;}
    void SetSide(Int_t i) {fSide=i;}
    void SetBinLowAt(Int_t ch,Int_t i) {fBinLow[i]=ch;}
	void SetBinUpAt(Int_t ch,Int_t i) {fBinUp[i]=ch;}
    void SetTimeAt(Double_t t,Int_t i) {fTime[i]= t;}

    UInt_t GetIndex(UInt_t /*def*/) const { return fBarId; }

    Bool_t Compare(const R3BLandTCalPar& that ) const; 

    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BLandTCalPar();
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
        return ValCondition(FairDbDetector::EDetector::kLand,
                          DataType::kData,
                          ValTimeStamp((Double_t) rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BLandTCalPar>* GetParamReader();
    FairDbWriter<R3BLandTCalPar>* GetParamWriter();
	FairDbWriter<R3BLandTCalPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fBarId; 
    Int_t fSide; 
    Int_t fBinLow[NCHMAX];// lower channel
    Int_t fBinUp[NCHMAX];// upper channel
    Double_t fTime[NCHMAX];// Cal. Time

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BLandTCalPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BLandTCalPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BLandTCalPar,1); // R3BLandTCalPar Parameter Container example
};



inline  FairDbReader<R3BLandTCalPar>* R3BLandTCalPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BLandTCalPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BLandTCalPar>* R3BLandTCalPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BLandTCalPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BLANDTCALPAR_H*/
