#ifndef R3BCALOCRYSTALLABPAR_H
#define R3BCALOCRYSTALLABPAR_H

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


class R3BCaloCrystalLabPar : public FairDbObjTableMap
{

using TObject::Compare;

  public :
    R3BCaloCrystalLabPar (const char* name="R3BCaloCrystalLabPar",
                  const char* title="Califa Geometry Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloCrystalLabPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();
    /** Accessor functions **/
    Int_t GetIdCrystal()          		const {return fIdCrystal;} 
    Double_t GetResolution()          		const {return fResolution;}
    Double_t GetChannelCo1()          		const {return fChannelCo1;}
    Double_t GetChannelCo1Gain()        	const {return fChannelCo1Gain;}
    Double_t GetChannelCo1Temp()       		const {return fChannelCo1Temp;}
    Double_t GetResolutionCo1()         	const {return fResolutionCo1;}
    Double_t GetResolutionCo1Gain()     	const {return fResolutionCo1Gain;}
    Double_t GetResolutionCo1Temp()     	const {return fResolutionCo1Temp;}
    Double_t GetChannelCo2()            	const {return fChannelCo2;}
    Double_t GetChannelCo2Gain()		const {return fChannelCo2Gain;}
    Double_t GetChannelCo2Temp()        	const {return fChannelCo2Temp;}
    Double_t GetResolutionCo2()         	const {return fResolutionCo2;}
    Double_t GetResolutionCo2Gain()     	const {return fResolutionCo2Gain;}
    Double_t GetResolutionCo2Temp()     	const {return fResolutionCo2Temp;}
    Double_t GetChannelCs()          		const {return fChannelCs;}
    Double_t GetChannelCsGain()         	const {return fChannelCsGain;}
    Double_t GetChannelCsTemp()       		const {return fChannelCsTemp;}
    Double_t GetResolutionCs()          	const {return fResolutionCs;}
    Double_t GetResolutionCsGain()      	const {return fResolutionCsGain;}
    Double_t GetResolutionCsTemp()     		const {return fResolutionCsTemp;}
    Double_t GetChannelBs1()          		const {return fChannelBs1;}
    Double_t GetChannelBs1Gain()          	const {return fChannelBs1Gain;}
    Double_t GetChannelBs1Temp()       		const {return fChannelBs1Temp;}
    Double_t GetResolutionBs1()         	const {return fResolutionBs1;}
    Double_t GetResolutionBs1Gain()     	const {return fResolutionBs1Gain;}
    Double_t GetResolutionBs1Temp()     	const {return fResolutionBs1Temp;}
    Double_t GetChannelBs2()          		const {return fChannelBs2;}
    Double_t GetChannelBs2Gain()          	const {return fChannelBs2Gain;}
    Double_t GetChannelBs2Temp()       		const {return fChannelBs2Temp;}
    Double_t GetResolutionBs2()         	const {return fResolutionBs2;}
    Double_t GetResolutionBs2Gain()     	const {return fResolutionBs2Gain;}
    Double_t GetResolutionBs2Temp()     	const {return fResolutionBs2Temp;}
    Int_t GetPreamplifier()          		const {return fPreamplifier;}
    Int_t GetAmplifier()          		const {return fAmplifier;}
    Int_t GetMCAScale()          		const {return fMCAScale;}
    Int_t GetTimeMeasurement()          	const {return fTimeMeasurement;}
    Int_t GetShapingTimeMeasurement()   	const {return fShapingTimeMeasurement;}
    string GetDateMeasurement()			const {return fDateMeasurement;}
    string GetObservation()			const {return fObservation;}   

    void SetIdCrystal(Int_t x) 				{fIdCrystal  = x;}
    void SetResolution(Double_t x) 			{fResolution  = x;}
    void SetChannelCo1(Double_t x) 			{fChannelCo1  = x;}
    void SetChannelCo1Gain(Double_t x) 			{fChannelCo1Gain  = x;}
    void SetChannelCo1Temp(Double_t x) 			{fChannelCo1Temp=x;}
    void SetResolutionCo1(Double_t x) 			{fResolutionCo1=x;}
    void SetResolutionCo1Gain(Double_t x) 		{fResolutionCo1Gain=x;}
    void SetResolutionCo1Temp(Double_t x) 		{fResolutionCo1Temp=x;}
    void SetChannelCo2(Double_t x) 			{fChannelCo2=x;}
    void SetChannelCo2Gain(Double_t x) 			{fChannelCo2Gain  = x;}
    void SetChannelCo2Temp(Double_t x) 			{fChannelCo2Temp=x;}   
    void SetResolutionCo2(Double_t x) 			{fResolutionCo2=x;}
    void SetResolutionCo2Gain(Double_t x) 		{fResolutionCo2Gain=x;}
    void SetResolutionCo2Temp(Double_t x) 		{fResolutionCo2Temp=x;}
    void SetChannelCs(Double_t x) 			{fChannelCs  = x;}
    void SetChannelCsGain(Double_t x) 			{fChannelCsGain  = x;}
    void SetChannelCsTemp(Double_t x) 			{fChannelCsTemp=x;}
    void SetResolutionCs(Double_t x) 			{fResolutionCs=x;}
    void SetResolutionCsGain(Double_t x) 		{fResolutionCsGain=x;}
    void SetResolutionCsTemp(Double_t x) 		{fResolutionCsTemp=x;}
    void SetChannelBs1(Double_t x) 			{fChannelBs1  = x;}
    void SetChannelBs1Gain(Double_t x) 			{fChannelBs1Gain  = x;}
    void SetChannelBs1Temp(Double_t x) 			{fChannelBs1Temp=x;}
    void SetResolutionBs1(Double_t x) 			{fResolutionBs1=x;}
    void SetResolutionBs1Gain(Double_t x) 		{fResolutionBs1Gain=x;}
    void SetResolutionBs1Temp(Double_t x) 		{fResolutionBs1Temp=x;}
    void SetChannelBs2(Double_t x) 			{fChannelBs2  = x;}
    void SetChannelBs2Gain(Double_t x) 			{fChannelBs2Gain  = x;}
    void SetChannelBs2Temp(Double_t x) 			{fChannelBs2Temp=x;}
    void SetResolutionBs2(Double_t x) 			{fResolutionBs2=x;}
    void SetResolutionBs2Gain(Double_t x) 		{fResolutionBs2Gain=x;}
    void SetResolutionBs2Temp(Double_t x) 		{fResolutionBs2Temp=x;}
    void SetPreamplifier(Int_t x) 			{fPreamplifier  = x;}
    void SetAmplifier(Int_t x) 				{fAmplifier  = x;}
    void SetMCAScale(Int_t x) 				{fMCAScale  = x;}
    void SetTimeMeasurement(Int_t x) 			{fTimeMeasurement  = x;}
    void SetShapingTimeMeasurement(Int_t x) 		{fShapingTimeMeasurement  = x;}
    void SetDateMeasurement(string x) 			{fDateMeasurement  = x;}
    void SetObservation(string x) 			{fObservation  = x;}

    //UInt_t GetIndex(UInt_t /*def*///) const { return fAPDNodeId;}

    Bool_t Compare(const R3BCaloCrystalLabPar& that ) const; 


    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloCrystalLabPar();
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
    FairDbReader<R3BCaloCrystalLabPar>* GetParamReader();
    FairDbWriter<R3BCaloCrystalLabPar>* GetParamWriter();
	FairDbWriter<R3BCaloCrystalLabPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    Int_t fIdCrystal;
    Double_t fResolution;
    Double_t fChannelCo1;
    Double_t fChannelCo1Gain;
    Double_t fChannelCo1Temp;
    Double_t fResolutionCo1;
    Double_t fResolutionCo1Gain; 
    Double_t fResolutionCo1Temp;
    Double_t fChannelCo2;
    Double_t fChannelCo2Gain;
    Double_t fChannelCo2Temp;
    Double_t fResolutionCo2;
    Double_t fResolutionCo2Gain;
    Double_t fResolutionCo2Temp;
    Double_t fChannelCs;
    Double_t fChannelCsGain;
    Double_t fChannelCsTemp;
    Double_t fResolutionCs;
    Double_t fResolutionCsGain;
    Double_t fResolutionCsTemp;
    Double_t fChannelBs1;
    Double_t fChannelBs1Gain;
    Double_t fChannelBs1Temp;
    Double_t fResolutionBs1;
    Double_t fResolutionBs1Gain;
    Double_t fResolutionBs1Temp;
    Double_t fChannelBs2;
    Double_t fChannelBs2Gain;
    Double_t fChannelBs2Temp;
    Double_t fResolutionBs2;
    Double_t fResolutionBs2Gain;
    Double_t fResolutionBs2Temp;
    Int_t fPreamplifier;
    Int_t fAmplifier;
    Int_t fMCAScale;
    Int_t fTimeMeasurement;
    Int_t fShapingTimeMeasurement;
    string fDateMeasurement;
    string fObservation;

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloCrystalLabPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloCrystalLabPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloCrystalLabPar,1); // R3BCaloCrystalLabPar Parameter Container example
};



inline  FairDbReader<R3BCaloCrystalLabPar>* R3BCaloCrystalLabPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloCrystalLabPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloCrystalLabPar>* R3BCaloCrystalLabPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloCrystalLabPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALOCRYSTALLABPAR_H*/
