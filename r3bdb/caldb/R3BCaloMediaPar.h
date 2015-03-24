#ifndef R3BCALOMEDIA_H
#define R3BCALOMEDIA_H

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

#define MAX_MEDIA 50

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;


class R3BCaloMediaPar : public FairDbObjTableMap
{

using TObject::Compare;

  public :
    R3BCaloMediaPar (const char* name="R3BCaloMediaPar",
                  const char* title="Califa Media Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloMediaPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    Int_t GetComboNo()     const {return fCompId;}    
    Int_t GetCompId()      const {return fCompId;}
    Int_t GetID()          const {return fIDmixt;}
    string GetMixtName()       const {return fName;}
    Double_t GetDensity()  const {return fDensity;}     
    Double_t GetRadLen()   const {return fRadLen;}
    Double_t GetIntLen()   const {return fIntLen;}
    Double_t GetPressure() const {return fP;}
    Double_t GetTemp()     const {return fT;}
    Int_t    GetN()        const {return fN;}  
    Double_t* GetAmixt()   const {return fAmixt;}   
    Double_t* GetZmixt()   const {return fZmixt;}   
    Double_t* GetWmixt()   const {return fWmixt;}   
    
    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}
    void SetID(Int_t x) {fIDmixt  = x;}
    void SetMixtName(string name)  {fName=name;}
    void SetDensity(Double_t x) {fDensity=x;}     
    void SetRadLen(Double_t x)  {fRadLen=x;}
    void SetIntLen(Double_t x)  {fIntLen=x;}
    void SetPressure(Double_t x){fP=x;}
    void SetTemp(Double_t x)    {fT=x;}
    void SetN(Int_t nele);          
    void SetAmixt(Double_t* inp)  {for(Int_t i=0;i<fN;i++){fAmixt[i]=inp[i];}}   
    void SetZmixt(Double_t* inp)  {for(Int_t i=0;i<fN;i++){fZmixt[i]=inp[i];}}   
    void SetWmixt(Double_t* inp)  {for(Int_t i=0;i<fN;i++){fWmixt[i]=inp[i];}}   

    UInt_t GetIndex(UInt_t /*def*/) const { return fIDmixt; }

    // Logical (=) 
    Bool_t Compare(const R3BCaloMediaPar& that ) const;

    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloMediaPar();
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
    FairDbReader<R3BCaloMediaPar>* GetParamReader();
    FairDbWriter<R3BCaloMediaPar>* GetParamWriter();

  private:
    // Data Parameters
    string fName; 
    Double_t fDensity;
    Double_t fRadLen;
    Double_t fIntLen;
    Double_t fP;
    Double_t fT;    
    Int_t fN;         
    Int_t fCompId;
    Int_t fIDmixt;
    Double_t* fAmixt; //[fN]
    Double_t* fZmixt; //[fN]
    Double_t* fWmixt; //[fN]

 
    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloMediaPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloMediaPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloMediaPar,1); // R3BCaloMediaPar Parameter Container example
};



inline  FairDbReader<R3BCaloMediaPar>* R3BCaloMediaPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloMediaPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloMediaPar>* R3BCaloMediaPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloMediaPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALOMEDIAPAR_H*/
