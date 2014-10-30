#include "R3BDBLmdFileInfo.h"

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

ClassImp(R3BDBLmdFileInfo);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BDBLmdFileInfo>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BDBLmdFileInfo>;

const std::string R3BDBLmdFileInfo::kNoFileName  = "<<NoFileName>>";
const std::string R3BDBLmdFileInfo::kNoFileLabel = "<<NoFileLabel>>";
const std::string R3BDBLmdFileInfo::kNoExpLabel  = "<<NoExpLabel>>";
const std::string R3BDBLmdFileInfo::kNoExpPhase  = "<<NoExpPhase>>";
const std::string R3BDBLmdFileInfo::kNoFileCom  = "<<NoFileComment>>";
const std::string R3BDBLmdFileInfo::kNoRunType  = "<<NoRunType>>";


R3BDBLmdFileInfo::R3BDBLmdFileInfo(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own)
     ,fCompId(0)
     ,fFileId(0)
     ,fRunNr(0)
     ,fFileNr(0)
     ,fExpLabel(kNoExpLabel)
     ,fExpPhase(kNoExpPhase)
     ,fRunType(kNoRunType) 
     ,fStartTime((time_t)0,0)
     ,fEndTime((time_t)0,0)
     ,fOffsetTime(0)
     ,fType(0)
     ,fSubType(0)
     ,fLength(0)
     ,fFragmented(0)
     ,fFileLabel(kNoFileLabel)
     ,fFileName(kNoFileName)
     ,fFileComment(kNoFileCom)      
{

  // Set the default Db Entry to the first slot
  fDbEntry=0;
  // Table Name
  fTableName=name;
  //  Writer Meta-Class for SQL IO
  fParam_Writer=NULL;
  //  Reader Meta-Class for SQL IO
  fParam_Reader=NULL;

  // Reset all parameters
  clear();

  // ConnectionPool
  fMultConn=FairDbTableInterfaceStore::Instance().fConnectionPool;

}


R3BDBLmdFileInfo::~R3BDBLmdFileInfo()
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


void R3BDBLmdFileInfo::putParams(FairParamList* list)
{

  std::cout<<"-I- R3BDBLmdFileInfo::putParams() called"<<std::endl;

  if(!list) { return; }
  list->add("comp_id",                fCompId);
  list->add("file_id",                fFileId);
  list->add("run_nr",                 fRunNr);
  list->add("file_nr",                fFileNr);
  list->add("exp_label",    (Text_t*) &fExpLabel);
  list->add("exp_phase",    (Text_t*) &fExpPhase);
  list->add("run_type",     (Text_t*) &fRunType );
  time_t tstart =   fStartTime.GetSec();
  list->add("start_time",     (Double_t) tstart);
  time_t tstop =   fEndTime.GetSec();
  list->add("end_time",       (Double_t) tstop);
  list->add("time_offset",       fOffsetTime);
  list->add("type",                    fType);
  list->add("subtype",              fSubType);
  list->add("length",                fLength);
  list->add("fragmentation",     fFragmented);
  list->add("file_label",       (Text_t*)  &fFileLabel);
  list->add("file_name",        (Text_t*)  &fFileName);
  list->add("file_comment",     (Text_t*)  &fFileComment);
}

Bool_t R3BDBLmdFileInfo::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BDBLmdFileInfo::getParams() called"<<std::endl;

  if (!list) { return kFALSE; }
  if (!list->fill("comp_id", &fCompId)) { return kFALSE; }
  if (!list->fill("file_id", &fFileId)) { return kFALSE; }
  if (!list->fill("run_nr",  &fRunNr))     { return kFALSE; }
  if (!list->fill("file_nr", &fFileNr)) { return kFALSE; }

  Text_t explabel[80];
  if (!list->fill("exp_label",explabel,80)) { return kFALSE; }
  fExpLabel = explabel;

  Text_t expphase[80];
  if (!list->fill("exp_phase",expphase,80)) { return kFALSE; }
  fExpPhase = expphase;

  Text_t runtype[80];
  if (!list->fill("run_type",runtype,80)) { return kFALSE; }
  fRunType = runtype;

  Double_t starttime;
  if (!list->fill("start_time", &starttime)) { return kFALSE; }
  timespec_t start_t ={(time_t)starttime,0};
  fStartTime = ValTimeStamp(start_t);

  Double_t endtime;
  if (!list->fill("end_time", &endtime)) { return kFALSE; }
  timespec_t end_t ={(time_t)endtime,0};
  fEndTime = ValTimeStamp(end_t); 

  if (!list->fill("time_offset", &fOffsetTime)) { return kFALSE; }
  if (!list->fill("type", &fType)) { return kFALSE; }
  if (!list->fill("subtype", &fSubType)) { return kFALSE; }
  if (!list->fill("length", &fLength)) { return kFALSE; }
  if (!list->fill("fragmentation", &fFragmented)) { return kFALSE; }

  Text_t filelabel[80];
  if (!list->fill("file_label",filelabel,80)) { return kFALSE; }
  fFileLabel = filelabel;

  Text_t filename[80];
  if (!list->fill("file_name",filename,80)) { return kFALSE; }
  fFileName = filename;

  Text_t filecom[80];
  if (!list->fill("file_comment",filecom,80)) { return kFALSE; }
  fFileComment = filecom;

  return kTRUE;
}

void R3BDBLmdFileInfo::clear()
{
  fCompId=fFileId=fRunNr=fFileNr=fOffsetTime=fType=fSubType=fLength=fFragmented=0;
  fExpLabel=kNoExpLabel;
  fExpPhase=kNoExpPhase;
  fRunType=kNoRunType;
  fFileLabel=kNoFileLabel;
  fFileName=kNoFileName;
  fFileComment=kNoFileCom;
  ValTimeStamp t_reset(0,0);  
  fStartTime = t_reset; 
  fEndTime   = t_reset; 
}


string R3BDBLmdFileInfo::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { 
	if (fExpLabel == kNoExpLabel) {
      cout << "-E-  R3BDBLmdFileInfo::GetTableDefinition: NoExpLabel --> cannot create FileInfo table " << endl; 
      return kNoExpLabel;
	}
  
  sql +=  fTableName; 
  }

  sql += "( SEQNO                 INT NOT NULL,";
  sql += "  ROW_ID                INT NOT NULL,";
  sql += "  COMP_ID               INT,";
  sql += "  FILE_ID               INT,";
  sql += "  RUN_NR                INT,";
  sql += "  FILE_NR               INT,";
  sql += "  EXP_LABEL             TEXT,";
  sql += "  EXP_PHASE             TEXT,";
  sql += "  RUN_TYPE              TEXT,";
  sql += "  START_TIME            DATETIME,";
  sql += "  END_TIME              DATETIME,";
  sql += "  TIME_OFFSET           INT,";
  sql += "  FILE_TYPE             INT,";
  sql += "  FILE_SUBTYPE          INT,";
  sql += "  FILE_LENGTH           INT,";
  sql += "  FILE_FRAG             INT,";
  sql += "  FILE_LABEL            TEXT,";
  sql += "  FILE_NAME             TEXT,";
  sql += "  FILE_COMMENT          TEXT,";
  sql += "  primary key(SEQNO,ROW_ID),"; 
  sql += "  index(FILE_ID))";
  //  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}


void R3BDBLmdFileInfo::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  res_in >> fCompId  >> fFileId  >> fRunNr >> fFileNr >> fExpLabel >> fExpPhase >> fRunType 
         >> fStartTime >> fEndTime >> fOffsetTime >> fType >> fSubType >> fLength 
         >> fFragmented >> fFileLabel >> fFileName >> fFileComment;
}

void R3BDBLmdFileInfo::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{
  res_out << fCompId  << fFileId  << fRunNr << fFileNr << fExpLabel << fExpPhase << fRunType 
		  << fStartTime << fEndTime << fOffsetTime << fType << fSubType << fLength 
          << fFragmented << fFileLabel << fFileName << fFileComment;
}


void R3BDBLmdFileInfo::fill(UInt_t rid)
{
  // Get Reader Meta Class
  fParam_Reader=GetParamReader();

  // Define a Context
  ValTimeStamp ts(rid);
  ValCondition context(Detector::kLand,DataType::kData,ts);

  // Activate reading for this Context
  fParam_Reader->Activate(context, GetVersion());

  // By default use the latest row entry
  // (Other rows would correspond to outdated versions)
  Int_t numRows = fParam_Reader->GetNumRows();
  if ( numRows > 1 ) { numRows = 1; }

  for (int i = 0; i < numRows; ++i) {
    R3BDBLmdFileInfo* cgd = (R3BDBLmdFileInfo*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fCompId = cgd->GetCompId();
    // <DB> Check me ! 
	// .... 
  }

}


void R3BDBLmdFileInfo::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BDBLmdFileInfo::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists(fTableName.c_str()) ) {
    sql_cmds.push_back(FairDb::GetValDefinition(fTableName.c_str()).Data());
    sql_cmds.push_back(R3BDBLmdFileInfo::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BDBLmdFileInfo::Store() ******* Error Executing SQL commands ***********  " << endl;
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

  fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Lmd File Info");

  // Object to Table mapping
  *fParam_Writer<< (*this);

  // Check for eventual IO problems
  if ( !fParam_Writer->Close() ) {
    fail = true;
    cout << "-E- R3BDBLmdFileInfo::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BDBLmdFileInfo::Print()
{
  std::cout<<"   R3BDBLmdFileInfo Parameters: "<<std::endl;
  std::cout<<"   fCompId: "<<  fCompId <<  std::endl;

}


Bool_t R3BDBLmdFileInfo::Compare(const R3BDBLmdFileInfo& that ) const {
  //  
  Bool_t test_h =     (fCompId           == that.fCompId) 
	               && (fFileId           == that.fFileId)
	               && (fRunNr            == that.fRunNr)
	               && (fFileNr           == that.fFileNr)
	               && (fExpLabel         == that.fExpLabel)
	               && (fExpPhase         == that.fExpPhase)
	               && (fRunType          == that.fRunType)
	               && (fStartTime        == that.fStartTime)
	               && (fEndTime          == that.fEndTime)
                   && (fOffsetTime       == that.fOffsetTime)
                   && (fType             == that.fType)
  	               && (fSubType          == that.fSubType)
                   && (fLength           == that.fLength)
                   && (fFragmented       == that.fFragmented)
	               && (fFileLabel        == that.fFileLabel)
	               && (fFileName         == that.fFileName)
	               && (fFileComment      == that.fFileComment);
  return test_h;
 }



FairDbWriter<R3BDBLmdFileInfo>* R3BDBLmdFileInfo::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BDBLmdFileInfo::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists(fTableName.c_str()) ) {
	  sql_cmds.push_back(FairDb::GetValDefinition(fTableName.c_str()).Data());
	  sql_cmds.push_back(R3BDBLmdFileInfo::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BDBLmdFileInfo::Store() ******* Error Executing SQL commands ***********  " << endl;
	  }
	  
	}
	
	// Refresh list of tables in connected database
	// for the choosen DB entry
	fMultConn->GetConnection(GetDbEntry())->SetTableExists();
	
	// Writer Meta-Class Instance
	fParam_Writer = GetParamWriter();
		
	fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"File Info Parameter");

	return fParam_Writer;
	
  }

  return NULL;
}

Int_t R3BDBLmdFileInfo::CalcUniqueSeqNo(Int_t fileId)
{
   return fileId;
}


Bool_t R3BDBLmdFileInfo::Commit()
{
  // Test File number validity 
  if (fFileId == 0 ) return kFALSE;   
   
  Bool_t fail = kFALSE; 
  
  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
	cout << "-E-  R3BDBLmdFileInfo::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		 << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	exit(1);
  }
 
  // TRY
   fMultConn->GetConnection(GetDbEntry())->SetTableExists();

 
  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function

  fTableName +=fExpLabel; 
  TString rtable(fTableName.c_str());
  rtable.ToUpper();
  fTableName = rtable.Data();
  std::string tbl  = fTableName;
  std::string tblv = fTableName + "VAL";


  std::vector<std::string> sql_cmds;

   
  if (!fMultConn->GetConnection(GetDbEntry())->TableExists(tbl.c_str()) ) {
    cout << " ++++++++++++++++++++++++++++++++++++++++++++++++ " << endl;
 	sql_cmds.push_back(FairDb::GetValDefinition(tbl.c_str()).Data());
	sql_cmds.push_back(R3BDBLmdFileInfo::GetTableDefinition());
	
  }
  
  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
	std::string& sql_cmd(*itr++);
	stmtDbn->Commit(sql_cmd.c_str());
	if ( stmtDbn->PrintExceptions() ) {
	  fail = kTRUE;
	  cout << "-E- R3BDBLmdFileInfo::Insert() ******* Error Executing SQL commands ***********  " << endl;
	}
	
  }
  
  // Refresh list of tables in connected database
  // for the choosen DB entry

  fMultConn->GetConnection(GetDbEntry())->SetTableExists(tbl.c_str()); 
 
  

  // Get Seq No
  Int_t seqno = CalcUniqueSeqNo(fFileId);


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
  sqlcol << ", ROW_ID ";
  sqlval << "," << 1;
  sqlcol << ", COMP_ID ";
  sqlval << "," << fCompId;
  sqlcol << ", FILE_ID ";
  sqlval << "," << fFileId;
  sqlcol << ", RUN_NR ";
  sqlval << "," << fRunNr;
  sqlcol << ", FILE_NR ";
  sqlval << "," << fFileNr;
  sqlcol << ", EXP_LABEL ";
  sqlval << "," << "'" << fExpLabel.c_str() << "'";
  sqlcol << ", EXP_PHASE ";
  sqlval << "," << "'" << fExpPhase.c_str() << "'";
  sqlcol << ", RUN_TYPE ";
  sqlval << "," << "'" << fRunType.c_str() << "'";
  sqlcol << ", START_TIME ";
  sqlval << "," << "'" << fStartTime.AsString("s") << "'";
  sqlcol << ", END_TIME ";
  sqlval << "," << "'" << fEndTime.AsString("s") << "'";
  sqlcol << ", TIME_OFFSET ";
  sqlval << "," << fOffsetTime;
  sqlcol << ", FILE_TYPE ";
  sqlval << "," << fType;
  sqlcol << ", FILE_LENGTH ";
  sqlval << "," << fLength;
  sqlcol << ", FILE_FRAG ";
  sqlval << "," << fFragmented;
  sqlcol << ", FILE_LABEL ";
  sqlval << "," << "'" << fFileLabel.c_str() << "'";
  sqlcol << ", FILE_NAME ";
  sqlval << "," << "'" << fFileName.c_str() << "'";
  sqlcol << ", FILE_COMMENT ";
  sqlval << "," << "'" << fFileComment.c_str() << "'";

   
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
  sqlvalv << ",'" << fStartTime.AsString("s") << "'";
  sqlcolv << ", TIMEEND ";
  sqlvalv << ",'" << tsEnd.AsString("s") << "'";
  sqlcolv << ", DETID ";
  sqlvalv << "," << Detector::kLand;
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
  sprintf(tmpbuf,"Insert R3B FILE for Detector: %c%8.8d ",
          Detector::AsString(Detector::kLand)[0],fFileId);

  DBLOG("FairDb",FairDbLog::kInfo)
    << tmpbuf
    << " rm/in " 
    << nrm << "," << nrmv << "/"
    << ninsert << "," << ninsertv 
    << endl;
  
  printf("%s rm=%d,%d in=%d,%d @ %s \n",
           tmpbuf,nrm,nrmv,ninsert,ninsertv,fStartTime.AsString("s"));
  
  // Insert returns value
  Bool_t success = (ninsert==1) && (ninsertv==1);  

  return success;
}
