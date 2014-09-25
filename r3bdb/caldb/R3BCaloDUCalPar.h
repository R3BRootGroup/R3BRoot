#ifndef R3BCALODUCALPAR_H
#define R3BCALODUCALPAR_H

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

using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;



class R3BCaloDUCalPar : public FairParGenericSet
{

using TObject::Compare;

  public :
  R3BCaloDUCalPar (const char* name="CaloDUCalPar",
				 const char* title="Califa Calibrated Detection Unit Parameter",
				 const char* context="TestDefaultContext",
				 Bool_t own=kTRUE);
    virtual ~R3BCaloDUCalPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    /** Accessor functions **/
    Int_t GetComboNo()       		const {return fCompId;}
    Int_t GetCompId()   		const {return fCompId;}
    Int_t GetDetectionUnit() 		const {return fDetectionUnit;}
    Double_t GetParZero() 		const {return fParZero;}
    Double_t GetParOne() 		const {return fParOne;}
    /*Double_t GetGammaParZero() 		const {return fGammaParZero;}
    Double_t GetGammaParOne() 		const {return fGammaParOne;}
    Double_t GetProtonParZero() 	const {return fProtonParZero;}
    Double_t GetProtonParOne() 		const {return fProtonParOne;}*/
    Double_t GetConversionFactor() 	const {return fConversionFactor;}
    /*Double_t GetExtraOne() 		const {return fExtraOne;}
    Double_t GetExtraTwo() 		const {return fExtraTwo;}
    Double_t GetExtraThree() 		const {return fExtraThree;}
    Double_t GetExtraFour() 		const {return fExtraFour;}*/

    void SetCompId(Int_t x)  			{SetComboNo(x); fCompId=x;}
    void SetDetectionUnit(Int_t i) 		{fDetectionUnit=i;}
    void SetParZero(Double_t i) 		{fParZero=i;}
    void SetParOne(Double_t i) 			{fParOne=i;}
    /*void SetGammaParZero(Double_t i) 		{fGammaParZero=i;}
    void SetGammaParOne(Double_t i) 		{fGammaParOne=i;}
    void SetProtonParZero(Double_t i) 		{fProtonParZero=i;}
    void SetProtonParOne(Double_t i) 		{fProtonParOne=i;}*/
    void SetConversionFactor(Double_t i) 	{fConversionFactor=i;}
    /*void SetExtraOne(Double_t i) 		{fExtraOne=i;}
    void SetExtraTwo(Double_t i) 		{fExtraTwo=i;}
    void SetExtraThree(Double_t i) 		{fExtraThree=i;}
    void SetExtraFour(Double_t i) 		{fExtraFour=i;}*/
    
    UInt_t GetIndex(UInt_t /*def*/) const { return fDetectionUnit; }

    Bool_t Compare(const R3BCaloDUCalPar& that ) const; 

    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloDUCalPar();
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
                          ValTimeStamp((Double_t) rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BCaloDUCalPar>* GetParamReader();
    FairDbWriter<R3BCaloDUCalPar>* GetParamWriter();
	FairDbWriter<R3BCaloDUCalPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fDetectionUnit; 
    Double_t fParZero;
    Double_t fParOne;
    /*Double_t fGammaParZero;
    Double_t fGammaParOne;
    Double_t fProtonParZero;
    Double_t fProtonParOne;*/
    Double_t fConversionFactor;
    /*Double_t fExtraOne;
    Double_t fExtraTwo;
    Double_t fExtraThree;
    Double_t fExtraFour;*/

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloDUCalPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloDUCalPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloDUCalPar,1); // R3BCaloDUCalPar Parameter Container example
};



inline  FairDbReader<R3BCaloDUCalPar>* R3BCaloDUCalPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloDUCalPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloDUCalPar>* R3BCaloDUCalPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloDUCalPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALODUCALPAR_H*/
