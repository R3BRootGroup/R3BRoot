#include "R3BCaloDetectionUnitPar.h"

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

ClassImp(R3BCaloDetectionUnitPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BCaloDetectionUnitPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BCaloDetectionUnitPar>;




R3BCaloDetectionUnitPar::R3BCaloDetectionUnitPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
    fId(0),
    fTypeId(0),
    fCrystal(0),   
    fAPD(0),    
    fOrigin(""),
    fLocation(""),
    fPosition(0),
    fObservation(""),
    fDbEntry(0),  // Set the default Db Entry to the first slot
    fParam_Writer(NULL), //  Writer Meta-Class for SQL IO
    fParam_Reader(NULL), //  Reader Meta-Class for SQL IO
    fMultConn(FairDbTableInterfaceStore::Instance().fConnectionPool)
{
  
}


R3BCaloDetectionUnitPar::~R3BCaloDetectionUnitPar()
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


void R3BCaloDetectionUnitPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloDetectionUnitPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("DetectionUnitPar_Id", fId);
  list->add("DetectionUnitPar_TypeId", fTypeId);
  list->add("DetectionUnitPar_Crystal", fCrystal);
  list->add("DetectionUnitPar_APD", fAPD);
  list->add("DetectionUnitPar_Origin", (Text_t*)fOrigin.c_str());
  list->add("DetectionUnitPar_Location", (Text_t*)fLocation.c_str());
  list->add("DetectionUnitPar_Position", fPosition);
  list->add("DetectionUnitPar_Observation", (Text_t*)fObservation.c_str());
}

Bool_t R3BCaloDetectionUnitPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("DetectionUnitPar_Id", &fId)) { return kFALSE; }
  if (!list->fill("DetectionUnitPar_TypeId", &fTypeId)) { return kFALSE; }
  if (!list->fill("DetectionUnitPar_Crystal", &fCrystal)) { return kFALSE; }
  if (!list->fill("DetectionUnitPar_APD", &fAPD)) { return kFALSE; }
  Text_t originaux[80];
  if (!list->fill("DetectionUnitPar_Origin", originaux, 80)) { return kFALSE; }
  fOrigin = originaux;
  Text_t locationaux[80];
  if (!list->fill("DetectionUnitPar_Location", locationaux, 80)) { return kFALSE; }
  fLocation = locationaux;
  if (!list->fill("DetectionUnitPar_Position", &fPosition)) { return kFALSE; }
  Text_t observationaux[80];
  if (!list->fill("DetectionUnitPar_Observation", observationaux, 80)) { return kFALSE; }
  fObservation = observationaux;

  return kTRUE;
}

void R3BCaloDetectionUnitPar::clear()
{
  fId=fTypeId=fCrystal=fAPD=fPosition=0;
  fOrigin=fLocation=fObservation="";

  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }
}


string R3BCaloDetectionUnitPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BCALODETECTIONUNITPAR"; }
  sql += "( SEQNO            		INT NOT NULL,";
  sql += "  ROW_ID           		INT NOT NULL,";
  sql += "  ID               		INT NOT NULL,";
  sql += "  TYPEID     			INT NOT NULL,";
  sql += "  CRYSTAL      		INT NOT NULL,";
  sql += "  APD     			INT NOT NULL,";
  sql += "  LOCATION			TEXT,";
  sql += "  POSITION 		    	INT,";
  sql += "  ORIGIN			TEXT,";
  sql += "  OBSERVATION			TEXT,";
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BCaloDetectionUnitPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  clear();

  res_in >> fId  >> fTypeId >> fCrystal >> fAPD >> fOrigin >> fLocation >> fPosition >> fObservation;

}

void R3BCaloDetectionUnitPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{

  res_out << fId  << fTypeId  << fCrystal << fAPD << fOrigin << fLocation << fPosition << fObservation;

}


void R3BCaloDetectionUnitPar::fill(UInt_t rid)
{
  // Get Reader Meta Class
  fParam_Reader=GetParamReader();

  // Define a Context
  ValTimeStamp ts(rid);
  ValCondition context(FairDbDetector::EDetector::kCal,DataType::kData,ts);

  // Activate reading for this Context
  fParam_Reader->Activate(context, GetVersion());

  // By default use the latest row entry
  // (Other rows would correspond to outdated versions)
  Int_t numRows = fParam_Reader->GetNumRows();
  if ( numRows > 1 ) { numRows = 1; }

  for (int i = 0; i < numRows; ++i) {
    R3BCaloDetectionUnitPar* cgd = (R3BCaloDetectionUnitPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fId =  cgd->GetId();
    fTypeId =  cgd->GetTypeId();
    fCrystal =  cgd->GetCrystal();
    fAPD =  cgd->GetAPD();
    fOrigin =  cgd->GetOrigin();
    fLocation =  cgd->GetLocation();
    fObservation =  cgd->GetObservation();
    fPosition =  cgd->GetPosition();
  }

}


void R3BCaloDetectionUnitPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloDetectionUnitPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALODETECTIONUNITPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BCALODETECTIONUNITPAR").Data());
    sql_cmds.push_back(R3BCaloDetectionUnitPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BCaloDetectionUnitPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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

  fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa DetectionUnit Node Pars");

  // Object to Table mapping
  *fParam_Writer<< (*this);

  // Check for eventual IO problems
  if ( !fParam_Writer->Close() ) {
    fail = true;
    cout << "-E- R3BCaloDetectionUnitPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BCaloDetectionUnitPar::Print()
{

  cout<<  " DetectionUnit Id: " << fId << " DetectionUnit Id Hamamatsu: "<< fTypeId <<  endl;
  cout << " DetectionUnit Crystal: " << fCrystal << endl;
  cout << " DetectionUnit APD : " << fAPD << endl;
  cout << " DetectionUnit Origin : " << fOrigin << endl;
  cout << " DetectionUnit Location : " << fLocation << endl;
  cout << " DetectionUnit Position : " << fPosition << endl;
  cout << " DetectionUnit Observation : " << fObservation << endl;
}


Bool_t R3BCaloDetectionUnitPar::Compare(const R3BCaloDetectionUnitPar& that ) const {
  //  
  Bool_t test_h =      (fId          == that.fId)
 	               &&  (fTypeId   == that.fTypeId)
 	               &&  (fCrystal   == that.fCrystal)
 	               &&  (fAPD   == that.fAPD)
		       &&  (fPosition   == that.fPosition)
 	               &&  (fOrigin   == that.fOrigin)
                       &&  (fLocation   == that.fLocation)
		       &&  (fObservation   == that.fObservation);
  
 return (test_h); 
}


FairDbWriter<R3BCaloDetectionUnitPar>* R3BCaloDetectionUnitPar::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BCaloDetectionUnitPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALODETECTIONUNITPAR") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BCALODETECTIONUNITPAR").Data());
	  sql_cmds.push_back(R3BCaloDetectionUnitPar::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BCaloDetectionUnitPar::Store() ******* Error Executing SQL commands ***********  " << endl;
	  }
	  
	}
	
	// Refresh list of tables in connected database
	// for the choosen DB entry
	fMultConn->GetConnection(GetDbEntry())->SetTableExists();
	
	// Writer Meta-Class Instance
	fParam_Writer = GetParamWriter();
		
	fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa DetectionUnit Node Pars");
	
  }
  
  return fParam_Writer;
}

void R3BCaloDetectionUnitPar::update(Int_t seqno)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloDetectionUnitPar::Update()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  FairDbString command;
  command << "UPDATE R3BCALODETECTIONUNITPARVAL SET TIMEEND = NOW() WHERE SEQNO = " << seqno <<  ";\0";

  stmtDbn->Commit(command.c_str());
  if ( stmtDbn->PrintExceptions() ) {
   fail = true;
   cout << "-E- R3BCaloDetectionUnitPar::Update() ******* Error Executing SQL commands ***********  " << endl;
  }

}
