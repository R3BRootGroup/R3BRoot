#include "R3BDBRunInfo.h"

#include "FairDbLogFormat.h"
#include "FairDbLogService.h"
#include "FairDbOutTableBuffer.h"         // for FairDbOutRowStream
#include "FairDbStatement.h"            // for FairDbStatement
#include "FairDbStreamer.h"            // for FairDbStatement

#include "FairParamList.h"              // for FairParamList

#include "Riosfwd.h"                    // for ostream
#include "TString.h"                    // for TString

#include <stdlib.h>                     // for exit
#include <memory>                       // for auto_ptr, etc
#include <vector>                       // for vector, vector<>::iterator

#include <unistd.h>
#include <sys/time.h>


using namespace std;

ClassImp(R3BDBRunInfo);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BDBRunInfo>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BDBRunInfo>;



R3BDBRunInfo::R3BDBRunInfo(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
	fCompId(0),
	fRunId(0),
    fRunTime(0)
{

  // Set the default Db Entry to the first slot
  fDbEntry=0;
  //  Writer Meta-Class for SQL IO
  fParam_Writer=NULL;
  //  Reader Meta-Class for SQL IO
  fParam_Reader=NULL;

  // Reset all parameters
  clear();

  // ConnectionPool
  fMultConn=FairDbTableInterfaceStore::Instance().fConnectionPool;

}


R3BDBRunInfo::~R3BDBRunInfo()
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


void R3BDBRunInfo::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BDBRunInfo::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("comp_id",  fCompId);
  list->add("run_id", fRunId);
  time_t t =   fRunTime.GetSec();
  list->add("run_time", (double) t);
}

Bool_t R3BDBRunInfo::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("comp_id", &fCompId)) { return kFALSE; }
  if (!list->fill("run_id", &fRunId)) { return kFALSE; }
  Double_t epoch_sec;
  if (!list->fill("run_time", &epoch_sec)) { return kFALSE; }
  timespec_t value ={(time_t)epoch_sec,0};
  fRunTime = ValTimeStamp(value); 

  return kTRUE;
}

void R3BDBRunInfo::clear()
{
  fCompId=fRunId=0;
}


string R3BDBRunInfo::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BDBRUNINFO"; }
  sql += "( SEQNO                 INT NOT NULL,";
  sql += "  ROW_ID                INT NOT NULL,";
  sql += "  COMP_ID               INT,";
  sql += "  RUN_ID                INT,";
  sql += "  RUN_TIME              DATETIME,";
  sql += "  primary key(SEQNO,ROW_ID),"; 
  sql += "index(RUN_ID))";
  //  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}


void R3BDBRunInfo::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  res_in >> fCompId  >> fRunId  >> fRunTime;
}

void R3BDBRunInfo::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{
  res_out << fCompId  << fRunId   << fRunTime ;
}


void R3BDBRunInfo::fill(UInt_t rid)
{
  // Get Reader Meta Class
  fParam_Reader=GetParamReader();

  // Define a Context
  ValTimeStamp ts(rid);
    ValCondition context(FairDbDetector::kLand,DataType::kData,ts);

  // Activate reading for this Context
  fParam_Reader->Activate(context, GetVersion());

  // By default use the latest row entry
  // (Other rows would correspond to outdated versions)
  Int_t numRows = fParam_Reader->GetNumRows();
  if ( numRows > 1 ) { numRows = 1; }

  for (int i = 0; i < numRows; ++i) {
    R3BDBRunInfo* cgd = (R3BDBRunInfo*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fCompId = cgd->GetCompId();

  }

}


void R3BDBRunInfo::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BDBRunInfo::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BDBRUNINFO") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BDBRUNINFO").Data());
    sql_cmds.push_back(R3BDBRunInfo::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BDBRunInfo::Store() ******* Error Executing SQL commands ***********  " << endl;
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

  fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa Crystal Shapes");

  // Object to Table mapping
  *fParam_Writer<< (*this);

  // Check for eventual IO problems
  if ( !fParam_Writer->Close() ) {
    fail = true;
    cout << "-E- R3BDBRunInfo::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BDBRunInfo::Print()
{
  std::cout<<"   R3BDBRunInfo Parameters: "<<std::endl;
  std::cout<<"   fCompId: "<<  fCompId <<  std::endl;

}


Bool_t R3BDBRunInfo::Compare(const R3BDBRunInfo& that ) const {
  //  
  Bool_t test_h =     (fCompId   == that.fCompId) 
	               && (fRunId   == that.fRunId)
  	               && (fRunTime   == that.fRunTime);
  return test_h;
 }



FairDbWriter<R3BDBRunInfo>* R3BDBRunInfo::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BDBRunInfo::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BDBRUNINFO") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BDBRUNINFO").Data());
	  sql_cmds.push_back(R3BDBRunInfo::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BDBRunInfo::Store() ******* Error Executing SQL commands ***********  " << endl;
	  }
	  
	}
	
	// Refresh list of tables in connected database
	// for the choosen DB entry
	fMultConn->GetConnection(GetDbEntry())->SetTableExists();
	
	// Writer Meta-Class Instance
	fParam_Writer = GetParamWriter();
		
	fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Run Info Parameter");

	return fParam_Writer;
	
  }

  return NULL;
}

Int_t R3BDBRunInfo::CalcUniqueSeqNo(Int_t run)
{
  // supposing run uniques?
   return run;
}


Bool_t R3BDBRunInfo::Commit()
{
  // Test Run number validity 
  if (fRunId == 0 ) return kFALSE;   
   
  Bool_t fail = kFALSE; 
  
  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
	cout << "-E-  R3BDBRunInfo::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		 << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	exit(1);
  }
  
  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::string tbl  = "R3BDBRUNINFO";
  std::string tblv = "R3BDBRUNINFOVAL";;


  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();
  
  if (! fMultConn->GetConnection(GetDbEntry())->TableExists(tbl.c_str()) ) {
	sql_cmds.push_back(FairDb::GetValDefinition(tbl.c_str()).Data());
	sql_cmds.push_back(R3BDBRunInfo::GetTableDefinition());
    fMultConn->GetConnection(GetDbEntry())->SetTableExists(tbl.c_str()); 
  }
  
  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
	std::string& sql_cmd(*itr++);
	stmtDbn->Commit(sql_cmd.c_str());
	if ( stmtDbn->PrintExceptions() ) {
	  fail = kTRUE;
	  cout << "-E- R3BDBRunInfo::Insert() ******* Error Executing SQL commands ***********  " << endl;
	}
	
  }
  
  // Stop if failure i.e table could not be created
  if (fail) return fail;

  // Get Seq No
  Int_t seqno = CalcUniqueSeqNo(fRunId);


  FairDbString sqlrm, sqlrmv;
  
  sqlrm  << "delete from " << tbl  << " where SEQNO=" << seqno << ";\0";
  Int_t nrm  = stmtDbn->ExecuteUpdate(sqlrm.c_str());
  stmtDbn->PrintExceptions();

  sqlrmv << "delete from " << tblv << " where SEQNO=" << seqno << ";\0";
  Int_t nrmv = stmtDbn->ExecuteUpdate(sqlrmv.c_str());
  stmtDbn->PrintExceptions();

  FairDbString sqlcol, sqlval;
  sqlcol << " SEQNO ";
  sqlval << seqno;
  sqlcol << ", RUN_ID ";
  sqlval << "," << fRunId;
  sqlcol << ", ROW_ID ";
  sqlval << "," << 1;
  sqlcol << ", COMP_ID ";
  sqlval << "," << fCompId;
  sqlcol << ", RUN_TIME ";
  sqlval << "," << "'" << fRunTime.AsString("s") << "'";
    
  FairDbString sqlinsert;
  sqlinsert << "insert into " << tbl
            << " ( " << sqlcol.c_str() << " ) "
            << " values ( " << sqlval.c_str() << " ) ;\0";
  
  DBLOG("FairDb",FairDbLog::kInfo)   << " Generated Insert string: " << sqlinsert.c_str() << endl;
 
  // Define  a process version 
  const Int_t version = 0;
  
  // Define maximal time range
  ValTimeStamp now;
  ValTimeStamp tsStart(1970, 1, 1, 0, 0, 0);
  ValTimeStamp   tsEnd(2038, 1,18,19,14, 7);


  FairDbString sqlcolv, sqlvalv;
  sqlcolv << " SEQNO";
  sqlvalv << seqno;
  sqlcolv << ", TIMESTART ";
  sqlvalv << ",'" << fRunTime.AsString("s") << "'";
  sqlcolv << ", TIMEEND ";
  sqlvalv << ",'" << tsEnd.AsString("s") << "'";
  sqlcolv << ", DETID ";
    sqlvalv << "," << FairDbDetector::kLand;
  sqlcolv << ", DATAID ";
  sqlvalv << "," << DataType::kData;
  sqlcolv << ", VERSION ";
  sqlvalv << "," << version;
  sqlcolv << ", COMPOSITEID ";
  sqlvalv << "," << GetComboNo();
  sqlcolv << ", TIMEINCR ";
  sqlvalv << ",'" << now.AsString("s") << "'" ;
  sqlcolv << ", TIMETRANS ";
  sqlvalv << ",'" << now.AsString("s") << "'";
  
  FairDbString sqlinsertv;
  sqlinsertv << "insert into " << tblv
             << " ( " << sqlcolv.c_str() << " ) "
             << " VALUES ( " << sqlvalv.c_str() << " ) ;\0";
  
  DBLOG("FairDb",FairDbLog::kInfo)  << "insert into " << tblv
									<< " ( " << sqlcolv.c_str() << " ) "
									<< " VALUES ( " << sqlvalv.c_str() << " ) ;"
									<< endl;
  
  Int_t ninsert = stmtDbn->ExecuteUpdate(sqlinsert.c_str());
  stmtDbn->PrintExceptions();
  
  Int_t ninsertv = stmtDbn->ExecuteUpdate(sqlinsertv.c_str());
  stmtDbn->PrintExceptions();
  
  // Info
  DBLOG("FairDb",FairDbLog::kInfo) << sqlinsert.c_str() << endl;
  DBLOG("FairDb",FairDbLog::kInfo) << sqlinsertv.c_str() << endl;
  
  char tmpbuf[1024];
  sprintf(tmpbuf,"Insert R3B RUN for Detector: %c%8.8d ",
          FairDbDetector::AsString(FairDbDetector::kUnknown)[0],fRunId);

  DBLOG("FairDb",FairDbLog::kInfo)
    << tmpbuf
    << " rm/in "
    << nrm << "," << nrmv << "/"
    << ninsert << "," << ninsertv 
    << endl;
  
  printf("%s rm=%d,%d in=%d,%d @ %s \n",
           tmpbuf,nrm,nrmv,ninsert,ninsertv,fRunTime.AsString("s"));
  
  // Insert returns value
  Bool_t success = (ninsert==1) && (ninsertv==1);  

  return success;
}
