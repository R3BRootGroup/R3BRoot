#include "R3BCaloCrystalLabPar.h"

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

ClassImp(R3BCaloCrystalLabPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BCaloCrystalLabPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BCaloCrystalLabPar>;




R3BCaloCrystalLabPar::R3BCaloCrystalLabPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
    fIdCrystal(0),
    fResolution(0.),
    fChannelCo1(0.),
    fChannelCo1Gain(0.),
    fChannelCo1Temp(0.),
    fResolutionCo1(0.),
    fResolutionCo1Gain(0.),
    fResolutionCo1Temp(0.),
    fChannelCo2(0.),
    fChannelCo2Gain(0.),
    fChannelCo2Temp(0.),
    fResolutionCo2(0.),
    fResolutionCo2Gain(0.),
    fResolutionCo2Temp(0.),
    fChannelCs(0.),
    fChannelCsGain(0.),
    fChannelCsTemp(0.),
    fResolutionCs(0.),
    fResolutionCsGain(0.),
    fResolutionCsTemp(0.),
    fChannelBs1(0.),
    fChannelBs1Gain(0.),
    fChannelBs1Temp(0.),
    fResolutionBs1(0.),
    fResolutionBs1Gain(0.),
    fResolutionBs1Temp(0.),
    fChannelBs2(0.),
    fChannelBs2Gain(0.),
    fChannelBs2Temp(0.),
    fResolutionBs2(0.),
    fResolutionBs2Gain(0.),
    fResolutionBs2Temp(0.),
    fPreamplifier(0),
    fAmplifier(0),
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


R3BCaloCrystalLabPar::~R3BCaloCrystalLabPar()
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


void R3BCaloCrystalLabPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloCrystalLabPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("CrystalLabPar_IdCrystal", fIdCrystal);
  list->add("CrystalLabPar_Resolution", fResolution);
  list->add("CrystalLabPar_ChannelCo1", fChannelCo1);
  list->add("CrystalLabPar_ChannelCo1Gain", fChannelCo1Gain);
  list->add("CrystalLabPar_ChannelCo1Temp",  fChannelCo1Temp);
  list->add("CrystalLabPar_ResolutionCo1",  fResolutionCo1);
  list->add("CrystalLabPar_ResolutionCo1Gain",  fResolutionCo1Gain);
  list->add("CrystalLabPar_ResolutionCo1Temp",  fResolutionCo1Temp);
  list->add("CrystalLabPar_ChannelCo2",  fChannelCo2);
  list->add("CrystalLabPar_ChannelCo2Gain",  fChannelCo2Gain);
  list->add("CrystalLabPar_ChannelCo2Temp",  fChannelCo2Temp);
  list->add("CrystalLabPar_ResolutionCo2",  fResolutionCo2);
  list->add("CrystalLabPar_ResolutionCo2Gain",  fResolutionCo2Gain);
  list->add("CrystalLabPar_ResolutionCo2Temp",  fResolutionCo2Temp);
  list->add("CrystalLabPar_ChannelCs",  fChannelCs);
  list->add("CrystalLabPar_ChannelCsGain",  fChannelCsGain);
  list->add("CrystalLabPar_ChannelCsTemp",  fChannelCsTemp);
  list->add("CrystalLabPar_ResolutionCs",  fResolutionCs);
  list->add("CrystalLabPar_ResolutionCsGain",  fResolutionCsGain);
  list->add("CrystalLabPar_ResolutionCsTemp",  fResolutionCsTemp);
  list->add("CrystalLabPar_ChannelBs1",  fChannelBs1);
  list->add("CrystalLabPar_ChannelBs1Gain",  fChannelBs1Gain);
  list->add("CrystalLabPar_ChannelBs1Temp",  fChannelBs1Temp);
  list->add("CrystalLabPar_ResolutionBs1",  fResolutionBs1);
  list->add("CrystalLabPar_ResolutionBs1Gain",  fResolutionBs1Gain);
  list->add("CrystalLabPar_ResolutionBs1Temp",  fResolutionBs1Temp);
  list->add("CrystalLabPar_ChannelBs2",  fChannelBs2);
  list->add("CrystalLabPar_ChannelBs2Gain",  fChannelBs2Gain);
  list->add("CrystalLabPar_ChannelBs2Temp",  fChannelBs2Temp);
  list->add("CrystalLabPar_ResolutionBs2",  fResolutionBs2);
  list->add("CrystalLabPar_ResolutionBs2Gain",  fResolutionBs2Gain);
  list->add("CrystalLabPar_ResolutionBs2Temp",  fResolutionBs2Temp);
  list->add("CrystalLabPar_Preamplifier",  fPreamplifier);
  list->add("CrystalLabPar_Amplifier",  fAmplifier);
  list->add("CrystalLabPar_MCAScale",  fMCAScale);
  list->add("CrystalLabPar_TimeMeasurement",  fTimeMeasurement);
  list->add("CrystalLabPar_ShapingTimeMeasurement",  fShapingTimeMeasurement);
  list->add("CrystalLabPar_DateMeasurement",  (Text_t*)fDateMeasurement.c_str());
  list->add("CrystalLabPar_Observation",  (Text_t*)fObservation.c_str());
}

Bool_t R3BCaloCrystalLabPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("CrystalLabPar_IdCrystal", &fIdCrystal)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_Resolution", &fResolution)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelCo1", &fChannelCo1)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelCo1Gain", &fChannelCo1Gain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelCo1Temp", &fChannelCo1Temp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionCo1", &fResolutionCo1)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionCo1Gain", &fResolutionCo1Gain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionCo1Temp", &fResolutionCo1Temp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelCo2", &fChannelCo2)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelCo2Gain", &fChannelCo2Gain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelCo2Temp", &fChannelCo2Temp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionCo2", &fResolutionCo2)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionCo2Gain", &fResolutionCo2Gain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionCo2Temp", &fResolutionCo2Temp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelCs", &fChannelCs)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelCsGain", &fChannelCsGain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelCsTemp", &fChannelCsTemp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionCs", &fResolutionCs)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionCsGain", &fResolutionCsGain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionCsTemp", &fResolutionCsTemp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelBs1", &fChannelBs1)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelBs1Gain", &fChannelBs1Gain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelBs1Temp", &fChannelBs1Temp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionBs1", &fResolutionBs1)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionBs1Gain", &fResolutionBs1Gain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionBs1Temp", &fResolutionBs1Temp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelBs2", &fChannelBs2)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelBs2Gain", &fChannelBs2Gain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ChannelBs2Temp", &fChannelBs2Temp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionBs2", &fResolutionBs2)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionBs2Gain", &fResolutionBs2Gain)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ResolutionBs2Temp", &fResolutionBs2Temp)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_Preamplifier", &fPreamplifier)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_Amplifier", &fAmplifier)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_MCAScale", &fMCAScale)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_TimeMeasurement", &fTimeMeasurement)) { return kFALSE; }
  if (!list->fill("CrystalLabPar_ShapingTimeMeasurement", &fShapingTimeMeasurement)) { return kFALSE; }
  Text_t datemeasurementaux[80];
  if (!list->fill("CrystalLab_DateMeasurement", datemeasurementaux, 80)) { return kFALSE; }
  fDateMeasurement = datemeasurementaux;
  Text_t observationaux[80];
  if (!list->fill("CrystalLabPar_Observation", observationaux, 80)) { return kFALSE; }
  fObservation = observationaux;

  return kTRUE;
}

void R3BCaloCrystalLabPar::clear()
{
  fIdCrystal=fPreamplifier=fAmplifier=fMCAScale=fTimeMeasurement=fShapingTimeMeasurement=0;

  fResolution=fChannelCo1=fChannelCo1Gain=fChannelCo1Temp=fResolutionCo1=fResolutionCo1Gain=fResolutionCo1Temp=
  fChannelCo2=fChannelCo2Gain=fChannelCo2Temp=fResolutionCo2=fResolutionCo2Gain=fResolutionCo2Temp=
  fChannelCs=fChannelCsGain=fChannelCsTemp=fResolutionCs=fResolutionCsGain=fResolutionCsTemp=
  fChannelBs1=fChannelBs1Gain=fChannelBs1Temp=fResolutionBs1=fResolutionBs1Gain=fResolutionBs1Temp=
  fChannelBs2=fChannelBs2Gain=fChannelBs2Temp=fResolutionBs2=fResolutionBs2Gain=fResolutionBs2Temp=0.;

  fDateMeasurement=fObservation="";

  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }
}


string R3BCaloCrystalLabPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BCALOCRYSTALLABPAR"; }
  sql += "( SEQNO            		INT NOT NULL,";
  sql += "  ROW_ID           		INT NOT NULL,";
  sql += "  IDCRYSTAL               	INT NOT NULL,";
  sql += "  RESOLUTION     	     	DOUBLE,";
  sql += "  CHANNELCO1     	     	DOUBLE,";
  sql += "  CHANNELCO1GAIN     	     	DOUBLE,";
  sql += "  CHANNELCO1TEMP        	DOUBLE,";
  sql += "  RESOLUTIONCO1    		DOUBLE,";
  sql += "  RESOLUTIONCO1GAIN		DOUBLE,";
  sql += "  RESOLUTIONCO1TEMP      	DOUBLE,";
  sql += "  CHANNELCO2  		DOUBLE,";
  sql += "  CHANNELCO2GAIN	     	DOUBLE,";
  sql += "  CHANNELCO2TEMP       	DOUBLE,";
  sql += "  RESOLUTIONCO2    		DOUBLE,";
  sql += "  RESOLUTIONCO2GAIN		DOUBLE,";
  sql += "  RESOLUTIONCO2TEMP      	DOUBLE,";
  sql += "  CHANNELCS       		DOUBLE,";
  sql += "  CHANNELCSGAIN       	DOUBLE,";
  sql += "  CHANNELCSTEMP       	DOUBLE,";
  sql += "  RESOLUTIONCS    		DOUBLE,";
  sql += "  RESOLUTIONCSGAIN		DOUBLE,";
  sql += "  RESOLUTIONCSTEMP      	DOUBLE,";
  sql += "  CHANNELBS1     	     	DOUBLE,";
  sql += "  CHANNELBS1GAIN     	     	DOUBLE,";
  sql += "  CHANNELBS1TEMP        	DOUBLE,";
  sql += "  RESOLUTIONBS1    		DOUBLE,";
  sql += "  RESOLUTIONBS1GAIN		DOUBLE,";
  sql += "  RESOLUTIONBS1TEMP      	DOUBLE,";
  sql += "  CHANNELBS2     	     	DOUBLE,";
  sql += "  CHANNELBS2GAIN     	     	DOUBLE,";
  sql += "  CHANNELBS2TEMP        	DOUBLE,";
  sql += "  RESOLUTIONBS2    		DOUBLE,";
  sql += "  RESOLUTIONBS2GAIN		DOUBLE,";
  sql += "  RESOLUTIONBS2TEMP      	DOUBLE,";
  sql += "  PREAMPLIFIER     	     	INT,";
  sql += "  AMPLIFIER               	INT,";
  sql += "  MCASCALE     	     	INT,";
  sql += "  TIMEMEASUREMENT             INT,";
  sql += "  SHAPINGTIMEMEASUREMENT     	INT,";
  sql += "  DATEMEASUREMENT 		TEXT,";
  sql += "  OBSERVATION 		TEXT,";
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BCaloCrystalLabPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  clear();

  res_in >> fIdCrystal >> fResolution >> 
  fChannelCo1 >> fChannelCo1Gain >> fChannelCo1Temp >> fResolutionCo1 >> fResolutionCo1Gain >> fResolutionCo1Temp >> 
  fChannelCo2 >> fChannelCo2Gain >> fChannelCo2Temp >> fResolutionCo2 >> fResolutionCo2Gain >> fResolutionCo2Temp >>
  fChannelCs >> fChannelCsGain >> fChannelCsTemp >> fResolutionCs >> fResolutionCsGain >> fResolutionCsTemp >>
  fChannelBs1 >> fChannelBs1Gain >> fChannelBs1Temp >> fResolutionBs1 >> fResolutionBs1Gain >> fResolutionBs1Temp >> 
  fChannelBs2 >> fChannelBs2Gain >> fChannelBs2Temp >> fResolutionBs2 >> fResolutionBs2Gain >> fResolutionBs2Temp >>
  fPreamplifier >> fAmplifier >> fMCAScale >> fTimeMeasurement >> fShapingTimeMeasurement >> fDateMeasurement >> fObservation;

}

void R3BCaloCrystalLabPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{

  res_out << fIdCrystal << fResolution << 
  fChannelCo1 << fChannelCo1Gain << fChannelCo1Temp << fResolutionCo1 << fResolutionCo1Gain << fResolutionCo1Temp << 
  fChannelCo2 << fChannelCo2Gain << fChannelCo2Temp << fResolutionCo2 << fResolutionCo2Gain << fResolutionCo2Temp <<
  fChannelCs << fChannelCsGain << fChannelCsTemp << fResolutionCs << fResolutionCsGain << fResolutionCsTemp <<
  fChannelBs1 << fChannelBs1Gain << fChannelBs1Temp << fResolutionBs1 << fResolutionBs1Gain << fResolutionBs1Temp << 
  fChannelBs2 << fChannelBs2Gain << fChannelBs2Temp << fResolutionBs2 << fResolutionBs2Gain << fResolutionBs2Temp <<
  fPreamplifier << fAmplifier << fMCAScale << fTimeMeasurement << fShapingTimeMeasurement << fDateMeasurement << fObservation;

}


void R3BCaloCrystalLabPar::fill(UInt_t rid)
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
    R3BCaloCrystalLabPar* cgd = (R3BCaloCrystalLabPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fIdCrystal = cgd->GetIdCrystal();
    fResolution =  cgd->GetResolution();
    fChannelCo1 =  cgd->GetChannelCo1();
    fChannelCo1Gain =  cgd->GetChannelCo1Gain();
    fChannelCo1Temp =  cgd->GetChannelCo1Temp();
    fResolutionCo1 = cgd->GetResolutionCo1();
    fResolutionCo1Gain = cgd->GetResolutionCo1Gain();
    fResolutionCo1Temp = cgd->GetResolutionCo1Temp();
    fChannelCo2 =  cgd->GetChannelCo2();
    fChannelCo2Gain =  cgd->GetChannelCo2Gain();
    fChannelCo2Temp =  cgd->GetChannelCo2Temp();
    fResolutionCo2 = cgd->GetResolutionCo2();
    fResolutionCo2Gain = cgd->GetResolutionCo2Gain();
    fResolutionCo2Temp = cgd->GetResolutionCo2Temp();
    fChannelCs =  cgd->GetChannelCs();
    fChannelCsGain =  cgd->GetChannelCsGain();
    fChannelCsTemp =  cgd->GetChannelCsTemp();
    fResolutionCs = cgd->GetResolutionCs();
    fResolutionCsGain = cgd->GetResolutionCsGain();
    fResolutionCsTemp = cgd->GetResolutionCsTemp();
    fChannelBs1 =  cgd->GetChannelBs1();
    fChannelBs1Gain =  cgd->GetChannelBs1Gain();
    fChannelBs1Temp =  cgd->GetChannelBs1Temp();
    fResolutionBs1 = cgd->GetResolutionBs1();
    fResolutionBs1Gain = cgd->GetResolutionBs1Gain();
    fResolutionBs1Temp = cgd->GetResolutionBs1Temp();
    fChannelBs2 =  cgd->GetChannelBs2();
    fChannelBs2Gain =  cgd->GetChannelBs2Gain();
    fChannelBs2Temp =  cgd->GetChannelBs2Temp();
    fResolutionBs2 = cgd->GetResolutionBs2();
    fResolutionBs2Gain = cgd->GetResolutionBs2Gain();
    fResolutionBs2Temp = cgd->GetResolutionBs2Temp();
    fPreamplifier = cgd->GetPreamplifier();
    fAmplifier = cgd->GetAmplifier();
    fMCAScale = cgd->GetMCAScale();
    fTimeMeasurement = cgd->GetTimeMeasurement();
    fShapingTimeMeasurement = cgd->GetShapingTimeMeasurement();
    fDateMeasurement = cgd->GetDateMeasurement();
    fObservation = cgd->GetObservation();
  }

}


void R3BCaloCrystalLabPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloCrystalLabPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALOCRYSTALLABPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BCALOCRYSTALLABPAR").Data());
    sql_cmds.push_back(R3BCaloCrystalLabPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BCaloCrystalLabPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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
    cout << "-E- R3BCaloCrystalLabPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BCaloCrystalLabPar::Print()
{

  cout<<  " CrystalLab Id: " << fIdCrystal << endl;
  cout << " CrystalLab Resolution: " << fResolution << endl;
  cout << " CrystalLab ChannelCo1 : " << fChannelCo1 << " CrystalLab ChannelCo1 Gain : " << fChannelCo1Gain << " CrystalLab ChannelCo1 Temp : " << fChannelCo1Temp << endl;
  cout << " CrystalLab ResolutionCo1 : " << fResolutionCo1 << " CrystalLab ResolutionCo1 Gain : " << fResolutionCo1Gain << " CrystalLab ResolutionCo1 Temp : " << fResolutionCo1Temp << endl;
  cout << " CrystalLab ChannelCo2 : " << fChannelCo2 << " CrystalLab ChannelCo2 Gain : " << fChannelCo2Gain << " CrystalLab ChannelCo2 Temp : " << fChannelCo2Temp << endl;
  cout << " CrystalLab ResolutionCo2 : " << fResolutionCo2 << " CrystalLab ResolutionCo2 Gain : " << fResolutionCo2Gain << " CrystalLab ResolutionCo2 Temp : " << fResolutionCo2Temp << endl;
  cout << " CrystalLab ChannelCs : " << fChannelCs << " CrystalLab ChannelCs Gain : " << fChannelCsGain << " CrystalLab ChannelCs Temp : " << fChannelCsTemp << endl;
  cout << " CrystalLab ResolutionCs : " << fResolutionCs << " CrystalLab ResolutionCs Gain : " << fResolutionCsGain << " CrystalLab ResolutionCs Temp : " << fResolutionCsTemp << endl;
  cout << " CrystalLab ChannelBs1 : " << fChannelBs1 << " CrystalLab ChannelBs1 Gain : " << fChannelBs1Gain << " CrystalLab ChannelBs1 Temp : " << fChannelBs1Temp << endl;
  cout << " CrystalLab ResolutionBs1 : " << fResolutionBs1 << " CrystalLab ResolutionBs1 Gain : " << fResolutionBs1Gain << " CrystalLab ResolutionBs1 Temp : " << fResolutionBs1Temp << endl;
  cout << " CrystalLab ChannelBs2 : " << fChannelBs2 << " CrystalLab ChannelBs2 Gain : " << fChannelBs2Gain << " CrystalLab ChannelBs2 Temp : " << fChannelBs2Temp << endl;
  cout << " CrystalLab ResolutionBs2 : " << fResolutionBs2 << " CrystalLab ResolutionBs2 Gain : " << fResolutionBs2Gain << " CrystalLab ResolutionBs2 Temp : " << fResolutionBs2Temp << endl;
  cout << " CrystalLab Preamplifier: " << fPreamplifier << endl;
  cout << " CrystalLab Amplifier: " << fAmplifier << endl;
  cout << " CrystalLab MCAScale: " << fMCAScale << endl;
  cout << " CrystalLab TimeMeasurement: " << fTimeMeasurement << endl;
  cout << " CrystalLab ShapingTimeMeasurement: " << fShapingTimeMeasurement << endl;
  cout << " CrystalLab DateMeasurement: " << fDateMeasurement << endl;
  cout << " CrystalLab Observation: " << fObservation << endl;
}


Bool_t R3BCaloCrystalLabPar::Compare(const R3BCaloCrystalLabPar& that ) const {
  //  
  Bool_t test_h =      (fIdCrystal          == that.fIdCrystal)
 	               &&  (fResolution   == that.fResolution)
 	               &&  (fChannelCo1   == that.fChannelCo1)
 	               &&  (fChannelCo1Gain   == that.fChannelCo1Gain)
 	               &&  (fChannelCo1Temp   == that.fChannelCo1Temp)
 	               &&  (fResolutionCo1   == that.fResolutionCo1)
                       &&  (fResolutionCo1Gain   == that.fResolutionCo1Gain)
 	               &&  (fResolutionCo1Temp   == that.fResolutionCo1Temp)
 	               &&  (fChannelCo2   == that.fChannelCo2)
 	               &&  (fChannelCo2Gain   == that.fChannelCo2Gain)
 	               &&  (fChannelCo2Temp   == that.fChannelCo2Temp)
 	               &&  (fResolutionCo2   == that.fResolutionCo2)
                       &&  (fResolutionCo2Gain   == that.fResolutionCo2Gain)
 	               &&  (fResolutionCo2Temp   == that.fResolutionCo2Temp)
                       &&  (fChannelCs   == that.fChannelCs)
 	               &&  (fChannelCsGain   == that.fChannelCsGain)
 	               &&  (fChannelCsTemp   == that.fChannelCsTemp)
 	               &&  (fResolutionCs   == that.fResolutionCs)
                       &&  (fResolutionCsGain   == that.fResolutionCsGain)
 	               &&  (fResolutionCsTemp   == that.fResolutionCsTemp)
                       &&  (fChannelBs1   == that.fChannelBs1)
 	               &&  (fChannelBs1Gain   == that.fChannelBs1Gain)
 	               &&  (fChannelBs1Temp   == that.fChannelBs1Temp)
 	               &&  (fResolutionBs1   == that.fResolutionBs1)
                       &&  (fResolutionBs1Gain   == that.fResolutionBs1Gain)
 	               &&  (fResolutionBs1Temp   == that.fResolutionBs1Temp)
 	               &&  (fChannelBs2   == that.fChannelBs2)
 	               &&  (fChannelBs2Gain   == that.fChannelBs2Gain)
 	               &&  (fChannelBs2Temp   == that.fChannelBs2Temp)
 	               &&  (fResolutionBs2   == that.fResolutionBs2)
                       &&  (fResolutionBs2Gain   == that.fResolutionBs2Gain)
 	               &&  (fResolutionBs2Temp   == that.fResolutionBs2Temp)
                       &&  (fPreamplifier   == that.fPreamplifier)
                       &&  (fAmplifier   == that.fAmplifier)
                       &&  (fMCAScale   == that.fMCAScale)
                       &&  (fTimeMeasurement   == that.fTimeMeasurement)
                       &&  (fShapingTimeMeasurement   == that.fShapingTimeMeasurement)
                       &&  (fDateMeasurement   == that.fDateMeasurement) 
                       &&  (fObservation   == that.fObservation) ;
 
  return (test_h); 
}


FairDbWriter<R3BCaloCrystalLabPar>* R3BCaloCrystalLabPar::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BCaloCrystalLabPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALOCRYSTALLABPAR") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BCALOCRYSTALLABPAR").Data());
	  sql_cmds.push_back(R3BCaloCrystalLabPar::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BCaloCrystalLabPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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
