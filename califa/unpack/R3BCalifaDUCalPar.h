// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                         R3BCalifaDUCalPar                         -----
// -----                 Created by P.Cabanelas 21/03/2017                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALIFADUCALPAR_H
#define R3BCALIFADUCALPAR_H

#include "FairParGenericSet.h"          // for FairParGenericSet
#include "DataType.h"                   // for EDataType::kData
#include "ValCondition.h"               // for ValCondition
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

class R3BCalifaDUCalPar : public FairDbObjTableMap
{

using TObject::Compare;

  public :
  R3BCalifaDUCalPar (const char* name="CalifaDUCalPar",
				 const char* title="Califa Calibrated Detection Unit Parameter",
				 const char* context="TestDefaultContext",
				 Bool_t own=kTRUE);
    virtual ~R3BCalifaDUCalPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    /** Accessor functions **/
    Int_t GetComboNo()       		const {return fCompId;}
    Int_t GetCompId()   		const {return fCompId;}
    Int_t GetDetectionUnit() 		const {return fDetectionUnit;}
    Double_t GetGammaCal_offset() 	const {return fGammaCal_offset;}
    Double_t GetGammaCal_gain() 	const {return fGammaCal_gain;}
    Double_t GetToTCal_par0() 		const {return fToTCal_par0;}
    Double_t GetToTCal_par1() 		const {return fToTCal_par1;}
    Double_t GetToTCal_par2() 		const {return fToTCal_par2;}
    Double_t GetRangeCal_offset() 	const {return fRangeCal_offset;}
    Double_t GetRangeCal_gain() 	const {return fRangeCal_gain;}
    Double_t GetQuenchingFactor() 	const {return fQuenchingFactor;}
    Double_t GetPidGain()               const {return fPidGain;}
    
    void SetCompId(Int_t x)  			{SetComboNo(x); fCompId=x;}
    void SetDetectionUnit(Int_t i) 		{fDetectionUnit=i;}
    void SetGammaCal_offset(Double_t i) 	{fGammaCal_offset=i;}
    void SetGammaCal_gain(Double_t i) 		{fGammaCal_gain=i;}
    void SetToTCal_par0(Double_t i) 		{fToTCal_par0=i;}
    void SetToTCal_par1(Double_t i) 		{fToTCal_par1=i;}
    void SetToTCal_par2(Double_t i) 		{fToTCal_par2=i;}
    void SetRangeCal_offset(Double_t i) 	{fRangeCal_offset=i;}
    void SetRangeCal_gain(Double_t i) 	        {fRangeCal_gain=i;}
    void SetQuenchingFactor(Double_t i) 	{fQuenchingFactor=i;}
    void SetPidGain(Double_t g)                 {fPidGain=g;}
    
    UInt_t GetIndex(UInt_t /*def*/) const { return fDetectionUnit; }

    Bool_t Compare(const R3BCalifaDUCalPar& that ) const; 

    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCalifaDUCalPar();
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
                          ValTimeStamp((Double_t) rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BCalifaDUCalPar>* GetParamReader();
    FairDbWriter<R3BCalifaDUCalPar>* GetParamWriter();
	FairDbWriter<R3BCalifaDUCalPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    Int_t fCompId;
    Int_t fDetectionUnit; 
    Double_t fGammaCal_offset; //offset in linear gamma calibration
    Double_t fGammaCal_gain;   //gain in linear gamma calibration
    Double_t fToTCal_par0;  //E=par0*exp(TOT/par1)+par2
    Double_t fToTCal_par1;  
    Double_t fToTCal_par2;  
    Double_t fRangeCal_offset; //offset in the linear 30 to 300 MeV range calibration
    Double_t fRangeCal_gain;   //gain in the linear 30 to 300 MeV range calibration
    Double_t fQuenchingFactor; //quenching factor for protons (tentative) calculated from proton and pulser measurements
    Double_t fPidGain;

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCalifaDUCalPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCalifaDUCalPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCalifaDUCalPar,3); // R3BCalifaDUCalPar Parameter Container example
};



inline  FairDbReader<R3BCalifaDUCalPar>* R3BCalifaDUCalPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCalifaDUCalPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCalifaDUCalPar>* R3BCalifaDUCalPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCalifaDUCalPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALIFADUCALPAR_H*/
