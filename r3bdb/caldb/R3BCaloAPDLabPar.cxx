#include "R3BCaloAPDLabPar.h"

#include "FairDbLogFormat.h"
#include "FairDbLogService.h"
#include "FairDbOutTableBuffer.h"         // for FairDbOutRowStream
#include "FairDbStatement.h"            // for FairDbStatement

#include "FairParamList.h"              // for FairParamList

#include "Riosfwd.h"                    // for ostream
#include "TString.h"                    // for TString

#include <stdlib.h>                     // for exit
#include <memory>                       // for auto_ptr, etc
#include <vector>                       // for vector, vector<>::iterator


using namespace std;

ClassImp(R3BCaloAPDLabPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BCaloAPDLabPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BCaloAPDLabPar>;




R3BCaloAPDLabPar::R3BCaloAPDLabPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
    fIdAPD(0),
    fVrG40(0.),
    fVrG50(0.),
    fVrG60(0.),
    fThreshold(0.),
    fThresholdGain(0.),
    fSlopeTemperature(""),
    fDarkCurrent(0.),
    fDarkCurrentGain(0.),
    fDarkCurrentTemp(0.),
    fResolutionLED(0.),
    fResolutionLEDTemp(0.),
    fResolutionFe(0.),
    fResolutionFeGain(0.),
    fChannelFe(0.),
    fChannelFeGain(0.),
    fChannelFeTemp(0.),
    fLEDVoltage(0),
    fPreamplifier(""),
    fAmplifier(""),
    fMCAScale(0),
    fTimeMeasurement(0),
    fShapingTimeMeasurement(0),
    fDateMeasurement(""),       
    fObservation(""),    
    fDbEntry(0),  // Set the default Db Entry to the first slot
    fParam_Writer(NULL), //  Writer Meta-Class for SQL IO
    fParam_Reader(NULL), //  Reader Meta-Class for SQL IO
    fMultConn(FairDbTableInterfaceStore::Instance().fConnectionPool)
{
  
}


R3BCaloAPDLabPar::~R3BCaloAPDLabPar()
{
  if (fParam_Writer) {
    delete fParam_Writer;
    fParam_Writer=NULL;
  }

  if (fParam_Reader) {
    delete fParam_Reader;
    fParam_Reader=NULL;
  }

}


void R3BCaloAPDLabPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloAPDLabPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("APDLabPar_IdAPD", fIdAPD);
  list->add("APDLabPar_VrG40", fVrG40);
  list->add("APDLabPar_VrG50", fVrG50);
  list->add("APDLabPar_VrG60", fVrG60);
  list->add("APDLabPar_Threshold",  fThreshold);
  list->add("APDLabPar_ThresholdGain",  fThresholdGain);
  list->add("APDLabPar_SlopeTemperature",  (Text_t*)fSlopeTemperature.c_str());
  list->add("APDLabPar_DarkCurrent",  fDarkCurrent);
  list->add("APDLabPar_DarkCurrentGain",  fDarkCurrentGain);
  list->add("APDLabPar_DarkCurrentTemp",  fDarkCurrentTemp);
  list->add("APDLabPar_ResolutionLED",  fResolutionLED);
  list->add("APDLabPar_ResolutionLEDTemp",  fResolutionLEDTemp);
  list->add("APDLabPar_ResolutionFe",  fResolutionFe);
  list->add("APDLabPar_ResolutionFeGain",  fResolutionFeGain);
  list->add("APDLabPar_ChannelFe",  fChannelFe);
  list->add("APDLabPar_ChannelFeGain",  fChannelFeGain);
  list->add("APDLabPar_ChannelFeTemp",  fChannelFeTemp);
  list->add("APDLabPar_LEDVoltage",  fLEDVoltage);
  list->add("APDLabPar_Preamplifier",  (Text_t*)fPreamplifier.c_str());
  list->add("APDLabPar_Amplifier",  (Text_t*)fAmplifier.c_str());
  list->add("APDLabPar_MCAScale",  fMCAScale);
  list->add("APDLabPar_TimeMeasurement",  fTimeMeasurement);
  list->add("APDLabPar_ShapingTimeMeasurement",  fShapingTimeMeasurement);
  list->add("APDLabPar_DateMeasurement",  (Text_t*)fDateMeasurement.c_str());
  list->add("APDLabPar_Observation",  (Text_t*)fObservation.c_str());
}

Bool_t R3BCaloAPDLabPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("APDLabPar_IdAPD", &fIdAPD)) { return kFALSE; }
  if (!list->fill("APDLabPar_VrG40", &fVrG40)) { return kFALSE; }
  if (!list->fill("APDLabPar_VrG50", &fVrG50)) { return kFALSE; }
  if (!list->fill("APDLabPar_VrG60", &fVrG60)) { return kFALSE; }
  if (!list->fill("APDLabPar_Threshold", &fThreshold)) { return kFALSE; }
  if (!list->fill("APDLabPar_ThresholdGain", &fThresholdGain)) { return kFALSE; }
  Text_t slopetemperatureaux[80];
  if (!list->fill("APDLabPar_SlopeTemperature", slopetemperatureaux, 80)) { return kFALSE; }
  fSlopeTemperature = slopetemperatureaux;
  if (!list->fill("APDLabPar_DarkCurrent", &fDarkCurrent)) { return kFALSE; }
  if (!list->fill("APDLabPar_DarkCurrentGain", &fDarkCurrentGain)) { return kFALSE; }
  if (!list->fill("APDLabPar_DarkCurrentTemp", &fDarkCurrentTemp)) { return kFALSE; }
  if (!list->fill("APDLabPar_ResolutionLED", &fResolutionLED)) { return kFALSE; }
  if (!list->fill("APDLabPar_ResolutionLEDTemp", &fResolutionLEDTemp)) { return kFALSE; }
  if (!list->fill("APDLabPar_ResolutionFe", &fResolutionFe)) { return kFALSE; }
  if (!list->fill("APDLabPar_ResolutionFeGain", &fResolutionFeGain)) { return kFALSE; }
  if (!list->fill("APDLabPar_ChannelFe", &fChannelFe)) { return kFALSE; }
  if (!list->fill("APDLabPar_ChannelFeGain", &fChannelFeGain)) { return kFALSE; }
  if (!list->fill("APDLabPar_ChannelFeTemp", &fChannelFeTemp)) { return kFALSE; }
  if (!list->fill("APDLabPar_LEDVoltage", &fLEDVoltage)) { return kFALSE; }
  Text_t preamplifieraux[80];
  if (!list->fill("APDLabPar_Preamplifier", preamplifieraux, 80)) { return kFALSE; }
  fPreamplifier = preamplifieraux;
  Text_t amplifieraux[80];
  if (!list->fill("APDLabPar_Amplifier", amplifieraux, 80)) { return kFALSE; }
  fAmplifier = amplifieraux;
  if (!list->fill("APDLabPar_MCAScale", &fMCAScale)) { return kFALSE; }
  if (!list->fill("APDLabPar_TimeMeasurement", &fTimeMeasurement)) { return kFALSE; }
  if (!list->fill("APDLabPar_ShapingTimeMeasurement", &fShapingTimeMeasurement)) { return kFALSE; }
  Text_t datemeasurementaux[80];
  if (!list->fill("APDLabPar_DateMeasurement", datemeasurementaux, 80)) { return kFALSE; }
  fDateMeasurement = datemeasurementaux;
  Text_t observationaux[80];
  if (!list->fill("APDLabPar_Observation", observationaux, 80)) { return kFALSE; }
  fObservation = observationaux;

  return kTRUE;
}

void R3BCaloAPDLabPar::clear()
{
  fIdAPD=fLEDVoltage=fMCAScale=fTimeMeasurement=fShapingTimeMeasurement=0;

  fThreshold=fVrG40=fVrG50=fVrG60=fThresholdGain=fDarkCurrent=fDarkCurrentGain=fDarkCurrentTemp=
  fResolutionLED=fResolutionLEDTemp=fResolutionFe=fResolutionFeGain=fChannelFe=fChannelFeGain=fChannelFeTemp=0.;

  fSlopeTemperature=fPreamplifier=fAmplifier=fDateMeasurement=fObservation="";

  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }
}


string R3BCaloAPDLabPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BCALOAPDLABPAR"; }
  sql += "( SEQNO            		INT NOT NULL,";
  sql += "  ROW_ID           		INT NOT NULL,";
  sql += "  IDAPD               	INT NOT NULL,";
  sql += "  VRG40     	     		DOUBLE,";
  sql += "  VRG50     	     		DOUBLE,";
  sql += "  VRG60     	     		DOUBLE,";
  sql += "  THRESHOLD        		DOUBLE,";
  sql += "  THRESHOLDGAIN    		DOUBLE,";
  sql += "  SLOPETEMPERATURE 		TEXT,";
  sql += "  DARKCURRENT      		DOUBLE,";
  sql += "  DARKCURRENTGAIN  		DOUBLE,";
  sql += "  DARKCURRENTTEMP     	DOUBLE,";
  sql += "  RESOLUTIONLED       	DOUBLE,";
  sql += "  RESOLUTIONLEDTEMP   	DOUBLE,";
  sql += "  RESOLUTIONFE  		DOUBLE,";
  sql += "  RESOLUTIONFEGAIN     	DOUBLE,";
  sql += "  CHANNELFE       		DOUBLE,";
  sql += "  CHANNELFEGAIN       	DOUBLE,";
  sql += "  CHANNELFETEMP       	DOUBLE,";
  sql += "  LEDVOLTAGE               	INT,";
  sql += "  PREAMPLIFIER     	     	TEXT NOT NULL,";
  sql += "  AMPLIFIER               	TEXT NOT NULL,";
  sql += "  MCASCALE     	     	INT,";
  sql += "  TIMEMEASUREMENT             INT,";
  sql += "  SHAPINGTIMEMEASUREMENT     	INT,";
  sql += "  DATEMEASUREMENT 		TEXT NOT NULL,";
  sql += "  OBSERVATION 		TEXT,";
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BCaloAPDLabPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  clear();

  res_in >> fIdAPD >> fVrG40 >> fVrG50 >> fVrG60 >> fThreshold >> fThresholdGain >> fSlopeTemperature >> fDarkCurrent >>       fDarkCurrentGain >> fDarkCurrentTemp >> fResolutionLED >> fResolutionLEDTemp >> fResolutionFe >> fResolutionFeGain >> fChannelFe >> fChannelFeGain >> fChannelFeTemp >> fLEDVoltage >> fPreamplifier >> fAmplifier >> fMCAScale >> fTimeMeasurement >> fShapingTimeMeasurement >> fDateMeasurement >> fObservation;

}

void R3BCaloAPDLabPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{

  res_out << fIdAPD << fVrG40 << fVrG50 << fVrG60 << fThreshold << fThresholdGain << fSlopeTemperature << fDarkCurrent << fDarkCurrentGain << fDarkCurrentTemp  << fResolutionLED << fResolutionLEDTemp << fResolutionFe << fResolutionFeGain << fChannelFe << fChannelFeGain << fChannelFeTemp << fLEDVoltage << fPreamplifier << fAmplifier << fMCAScale << fTimeMeasurement << fShapingTimeMeasurement << fDateMeasurement << fObservation;

}


void R3BCaloAPDLabPar::fill(UInt_t rid)
{
  // Get Reader Meta Class
  fParam_Reader=GetParamReader();

  // Define a Context
  ValTimeStamp ts(rid);
  ValCondition context(Detector::kCal,DataType::kData,ts);

  // Activate reading for this Context
  fParam_Reader->Activate(context, GetVersion());

  // By default use the latest row entry
  // (Other rows would correspond to outdated versions)
  Int_t numRows = fParam_Reader->GetNumRows();
  if ( numRows > 1 ) { numRows = 1; }

  for (int i = 0; i < numRows; ++i) {
    R3BCaloAPDLabPar* cgd = (R3BCaloAPDLabPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fIdAPD = cgd->GetIdAPD();
    fVrG40 =  cgd->GetVrG40();
    fVrG50 =  cgd->GetVrG50();
    fVrG60 =  cgd->GetVrG60();
    fThreshold = cgd->GetThreshold();
    fThresholdGain = cgd->GetThresholdGain();
    fSlopeTemperature = cgd->GetSlopeTemperature();
    fDarkCurrent = cgd->GetDarkCurrent();
    fDarkCurrentGain = cgd->GetDarkCurrentGain();
    fDarkCurrentTemp = cgd->GetDarkCurrentTemp();
    fResolutionLED = cgd->GetResolutionLED();
    fResolutionLEDTemp = cgd->GetResolutionLEDTemp();
    fResolutionFe = cgd->GetResolutionFe();
    fResolutionFeGain = cgd->GetResolutionFeGain();
    fChannelFe = cgd->GetChannelFe();
    fChannelFeGain = cgd->GetChannelFeGain();
    fChannelFeTemp = cgd->GetChannelFeTemp();
    fLEDVoltage = cgd->GetLEDVoltage();
    fPreamplifier = cgd->GetPreamplifier();
    fAmplifier = cgd->GetAmplifier();
    fMCAScale = cgd->GetMCAScale();
    fTimeMeasurement = cgd->GetTimeMeasurement();
    fShapingTimeMeasurement = cgd->GetShapingTimeMeasurement();
    fDateMeasurement = cgd->GetDateMeasurement();
    fObservation = cgd->GetObservation();
  }

}


void R3BCaloAPDLabPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloAPDLabPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALOAPDLABPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BCALOAPDLABPAR").Data());
    sql_cmds.push_back(R3BCaloAPDLabPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BCaloAPDLabPar::Store() ******* Error Executing SQL commands ***********  " << endl;
    }

  }

  // Refresh list of tables in connected database
  // for the choosen DB entry
  fMultConn->GetConnection(GetDbEntry())->SetTableExists();

  // Writer Meta-Class Instance
  fParam_Writer = GetParamWriter();


  // Activate Writer Meta-Class with the proper
  // Validity Time Interval (run_id encapsulated)

  // Writer Activate() arguments list
  //                      <Arguments>                   <Type>                  <Comments>
  //
  //                      Validity Interval,            ValInterval
  //                      Composition      ,            Int_t                   set via cont. factory
  //                      Version          ,            Int_t                   set via cont. factory
  //                      DbEntry          ,            Int_t                   set via cont. factory
  //                      LogTitle         ,            std::string             set via cont. factory

  fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa APD Node Meas");

  // Object to Table mapping
  *fParam_Writer<< (*this);

  // Check for eventual IO problems
  if ( !fParam_Writer->Close() ) {
    fail = true;
    cout << "-E- R3BCaloAPDLabPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BCaloAPDLabPar::Print()
{

  cout<<  " APDLab Id APD: " << fIdAPD << endl;
  cout << " APDLab VrG40: " << fVrG40 << " APDLab VrG50: " << fVrG50 << " APDLab VrG60: " << fVrG60 << endl;
  cout << " APDLab Threshold : " << fThreshold << " APDLab Threshold Gain : " << fThresholdGain << endl;
  cout << " APDLab Slope Temperature: " << fSlopeTemperature << endl;
  cout << " APDLab Dark Current : " << fDarkCurrent << " APDLab DarkCurrent Gain : " << fDarkCurrentGain << " APDLab DarkCurrent Temp : " << fDarkCurrentTemp << endl;
  cout << " APDLab Resolution LED : " << fResolutionLED << " APDLab Resolution LED Temp : " << fResolutionLEDTemp << endl;
  cout << " APDLab Resolution Fe : " << fResolutionFe << " APDLab Resolution Fe Gain : " << fResolutionFeGain << endl;
  cout << " APDLab Channel Fe : " << fChannelFe << " APDLab Channel Fe Gain : " << fChannelFeGain << " APDLab Channel Fe Temp : " << fChannelFeTemp << endl;
  cout << " APDLab LED Voltage: " << fLEDVoltage << endl;
  cout << " APDLab Preamplifier: " << fPreamplifier << endl;
  cout << " APDLab Amplifier: " << fAmplifier << endl;
  cout << " APDLab MCAScale: " << fMCAScale << endl;
  cout << " APDLab TimeMeasurement: " << fTimeMeasurement << endl;
  cout << " APDLab ShapingTimeMeasurement: " << fShapingTimeMeasurement << endl;
  cout << " APDLab DateMeasurement: " << fDateMeasurement << endl;
  cout << " CrystalLab Observation: " << fObservation << endl;
}


Bool_t R3BCaloAPDLabPar::Compare(const R3BCaloAPDLabPar& that ) const {
  //  
  Bool_t test_h =      (fIdAPD          == that.fIdAPD)
 	               &&  (fVrG40   == that.fVrG40)
 	               &&  (fVrG50   == that.fVrG50)
 	               &&  (fVrG60   == that.fVrG60)
 	               &&  (fThreshold   == that.fThreshold)
 	               &&  (fThresholdGain   == that.fThresholdGain)
                       &&  (fSlopeTemperature   == that.fSlopeTemperature)
 	               &&  (fDarkCurrent   == that.fDarkCurrent)
 	               &&  (fDarkCurrentGain   == that.fDarkCurrentGain)
                       &&  (fDarkCurrentTemp   == that.fDarkCurrentTemp)
                       &&  (fResolutionLED   == that.fResolutionLED)
                       &&  (fResolutionLEDTemp   == that.fResolutionLEDTemp)
                       &&  (fResolutionFe   == that.fResolutionFe)
                       &&  (fResolutionFeGain   == that.fResolutionFeGain)
                       &&  (fChannelFe   == that.fChannelFe)
                       &&  (fChannelFeGain   == that.fChannelFeGain)
                       &&  (fChannelFeTemp   == that.fChannelFeTemp)
                       &&  (fLEDVoltage   == that.fLEDVoltage)
                       &&  (fPreamplifier   == that.fPreamplifier)
                       &&  (fAmplifier   == that.fAmplifier)
                       &&  (fMCAScale   == that.fMCAScale)
                       &&  (fTimeMeasurement   == that.fTimeMeasurement)
                       &&  (fShapingTimeMeasurement   == that.fShapingTimeMeasurement)
                       &&  (fDateMeasurement   == that.fDateMeasurement) 
                       &&  (fObservation   == that.fObservation) ;
 
  return (test_h); 
}


FairDbWriter<R3BCaloAPDLabPar>* R3BCaloAPDLabPar::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BCaloAPDLabPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALOAPDLABPAR") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BCALOAPDLABPAR").Data());
	  sql_cmds.push_back(R3BCaloAPDLabPar::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BCaloAPDLabPar::Store() ******* Error Executing SQL commands ***********  " << endl;
	  }
	  
	}
	
	// Refresh list of tables in connected database
	// for the choosen DB entry
	fMultConn->GetConnection(GetDbEntry())->SetTableExists();
	
	// Writer Meta-Class Instance
	fParam_Writer = GetParamWriter();
		
	fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa APD Node Meas");

	return fParam_Writer;
	
  }
}
