#ifndef R3BCALOAPDLABPAR_H
#define R3BCALOAPDLABPAR_H

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


class R3BCaloAPDLabPar : public FairParGenericSet
{

using TObject::Compare;

  public :
    R3BCaloAPDLabPar (const char* name="R3BCaloAPDLabPar",
                  const char* title="Califa Geometry Parameter",
                  const char* context="TestDefaultContext",
                  Bool_t own=kTRUE);
    virtual ~R3BCaloAPDLabPar(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();
    /** Accessor functions **/
    Int_t GetIdAPD()          		const {return fIdAPD;} 
    Double_t GetVrG40()          	const {return fVrG40;}
    Double_t GetVrG50()          	const {return fVrG50;}
    Double_t GetVrG60()          	const {return fVrG60;}
    Double_t GetThreshold()       	const {return fThreshold;}
    Double_t GetThresholdGain()         const {return fThresholdGain;}
    string GetSlopeTemperature()	const {return fSlopeTemperature;}
    Double_t GetDarkCurrent()          	const {return fDarkCurrent;}
    Double_t GetDarkCurrentGain()       const {return fDarkCurrentGain;}
    Double_t GetDarkCurrentTemp()       const {return fDarkCurrentTemp;}
    Double_t GetResolutionLED()         const {return fResolutionLED;}
    Double_t GetResolutionLEDTemp()     const {return fResolutionLEDTemp;}
    Double_t GetResolutionFe()          const {return fResolutionFe;}
    Double_t GetResolutionFeGain()      const {return fResolutionFeGain;}
    Double_t GetChannelFe()          	const {return fChannelFe;}
    Double_t GetChannelFeGain()         const {return fChannelFeGain;}
    Double_t GetChannelFeTemp()         const {return fChannelFeTemp;}
    Int_t GetLEDVoltage()          	const {return fLEDVoltage;}
    string GetPreamplifier()          	const {return fPreamplifier;}
    string GetAmplifier()          	const {return fAmplifier;}
    Int_t GetMCAScale()          	const {return fMCAScale;}
    Int_t GetTimeMeasurement()          const {return fTimeMeasurement;}
    Int_t GetShapingTimeMeasurement()   const {return fShapingTimeMeasurement;}
    string GetDateMeasurement()		const {return fDateMeasurement;}
    string GetObservation()		const {return fObservation;}
    
    void SetIdAPD(Int_t x) 			{fIdAPD  = x;}
    void SetVrG40(Double_t x) 			{fVrG40  = x;}
    void SetVrG50(Double_t x) 			{fVrG50  = x;}
    void SetVrG60(Double_t x) 			{fVrG60  = x;}
    void SetThreshold(Double_t x) 		{fThreshold=x;}
    void SetThresholdGain(Double_t x) 		{fThresholdGain=x;}
    void SetSlopeTemperature(string x) 		{fSlopeTemperature  = x;}
    void SetDarkCurrent(Double_t x) 		{fDarkCurrent=x;}
    void SetDarkCurrentGain(Double_t x) 	{fDarkCurrentGain=x;}
    void SetDarkCurrentTemp(Double_t x) 	{fDarkCurrentTemp=x;}
    void SetResolutionLED(Double_t x) 		{fResolutionLED=x;}
    void SetResolutionLEDTemp(Double_t x) 	{fResolutionLEDTemp=x;}
    void SetResolutionFe(Double_t x) 		{fResolutionFe=x;}
    void SetResolutionFeGain(Double_t x) 	{fResolutionFeGain=x;}
    void SetChannelFe(Double_t x) 		{fChannelFe=x;}
    void SetChannelFeGain(Double_t x) 		{fChannelFeGain=x;}
    void SetChannelFeTemp(Double_t x) 		{fChannelFeTemp=x;}
    void SetLEDVoltage(Int_t x) 		{fLEDVoltage  = x;}
    void SetPreamplifier(string x) 		{fPreamplifier  = x;}
    void SetAmplifier(string x) 		{fAmplifier  = x;}
    void SetMCAScale(Int_t x) 			{fMCAScale  = x;}
    void SetTimeMeasurement(Int_t x) 		{fTimeMeasurement  = x;}
    void SetShapingTimeMeasurement(Int_t x) 	{fShapingTimeMeasurement  = x;}
    void SetDateMeasurement(string x) 		{fDateMeasurement  = x;}
    void SetObservation(string x) 		{fObservation  = x;}

    //UInt_t GetIndex(UInt_t /*def*///) const { return fAPDNodeId;}

    Bool_t Compare(const R3BCaloAPDLabPar& that ) const; 


    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);

    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BCaloAPDLabPar();
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
      return ValCondition(FairDbDetector::EDetector::kCal,
                          DataType::kData,
                          ValTimeStamp(rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BCaloAPDLabPar>* GetParamReader();
    FairDbWriter<R3BCaloAPDLabPar>* GetParamWriter();
	FairDbWriter<R3BCaloAPDLabPar>* ActivateWriter(Int_t rid);

  private:
    // Data Parameters
    Int_t fIdAPD;
    Double_t fVrG40;
    Double_t fVrG50;
    Double_t fVrG60;
    Double_t fThreshold;
    Double_t fThresholdGain;
    string fSlopeTemperature;
    Double_t fDarkCurrent;
    Double_t fDarkCurrentGain;
    Double_t fDarkCurrentTemp; 
    Double_t fResolutionLED;
    Double_t fResolutionLEDTemp;
    Double_t fResolutionFe;
    Double_t fResolutionFeGain;
    Double_t fChannelFe;
    Double_t fChannelFeGain;
    Double_t fChannelFeTemp;
    Int_t fLEDVoltage;
    string fPreamplifier;
    string fAmplifier;
    Int_t fMCAScale;
    Int_t fTimeMeasurement;
    Int_t fShapingTimeMeasurement;
    string fDateMeasurement;
    string fObservation;

    // Database Pool Index
    Int_t fDbEntry; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BCaloAPDLabPar>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BCaloAPDLabPar>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BCaloAPDLabPar,1); // R3BCaloAPDLabPar Parameter Container example
};



inline  FairDbReader<R3BCaloAPDLabPar>* R3BCaloAPDLabPar::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BCaloAPDLabPar>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BCaloAPDLabPar>* R3BCaloAPDLabPar::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BCaloAPDLabPar>();
    return fParam_Writer;
  }
}

#endif /* !R3BCALOAPDLABPAR_H*/
