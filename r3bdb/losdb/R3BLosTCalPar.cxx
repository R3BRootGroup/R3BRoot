#include "R3BLosTCalPar.h"

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


using namespace std;

ClassImp(R3BLosTCalPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BLosTCalPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BLosTCalPar>;



R3BLosTCalPar::R3BLosTCalPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
	fCompId(0),
	fChannel(0)
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


R3BLosTCalPar::~R3BLosTCalPar()
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


void R3BLosTCalPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BLosTCalPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("comp_id",  fCompId);
  list->add("channel", fChannel);
  list->add("bin_low", fBinLow, NCHMAX);
  list->add("bin_up", fBinUp, NCHMAX);
  list->add("time", fTime, NCHMAX);
  
}

Bool_t R3BLosTCalPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("comp_id", &fCompId)) { return kFALSE; }
  if (!list->fill("channel", &fChannel)) { return kFALSE; }
  if (!list->fill("bin_low", fBinLow, NCHMAX)) { return kFALSE; }
  if (!list->fill("bin_up", fBinUp, NCHMAX)) { return kFALSE; }
  if (!list->fill("time", fTime, NCHMAX)) { return kFALSE; }
 
  return kTRUE;
}

void R3BLosTCalPar::clear()
{
  fCompId=fChannel=0;
  // <DB> Not so much overhead here.
  for(Int_t i=0; i<NCHMAX; i++) { fBinLow[i]=fBinUp[i]=0;fTime[i]=0.;}
 
  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }

}


string R3BLosTCalPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BLOSTCALPAR"; }
  sql += "( SEQNO                 INT NOT NULL,";
  sql += "  ROW_ID                INT NOT NULL,";
  sql += "  COMP_ID               INT,";
  sql += "  CHANNEL               INT,";
  sql += "  BIN_LOW               TEXT,";
  sql += "  BIN_UP                TEXT,";
  sql += "  TIME                  TEXT,";
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BLosTCalPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  // Clear all structures
  clear();
  FairDbStreamer b_l(fBinLow, NCHMAX);
  FairDbStreamer b_u(fBinUp, NCHMAX);
  FairDbStreamer b_t(fTime, NCHMAX);

  res_in >> fCompId  >> fChannel   >> b_l >> b_u >> b_t;

  b_l.Fill(fBinLow);
  b_u.Fill(fBinUp);
  b_t.Fill(fTime);

}

void R3BLosTCalPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{
  FairDbStreamer b_l(fBinLow, NCHMAX);
  FairDbStreamer b_u(fBinUp, NCHMAX);
  FairDbStreamer b_t(fTime,NCHMAX);

  res_out << fCompId  << fChannel   << b_l << b_u << b_t;

}


void R3BLosTCalPar::fill(UInt_t rid)
{
  // Get Reader Meta Class
  fParam_Reader=GetParamReader();

  // Define a Context
  ValTimeStamp ts(rid);
  ValCondition context(Detector::kLos,DataType::kData,ts);

  // Activate reading for this Context
  fParam_Reader->Activate(context, GetVersion());

  // By default use the latest row entry
  // (Other rows would correspond to outdated versions)
  Int_t numRows = fParam_Reader->GetNumRows();
  if ( numRows > 1 ) { numRows = 1; }

  for (int i = 0; i < numRows; ++i) {
    R3BLosTCalPar* cgd = (R3BLosTCalPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fCompId = cgd->GetCompId();
    fChannel =  cgd->GetChannel();

    for(Int_t j=0;j<NCHMAX;j++){
	  SetBinLowAt(GetBinLowAt(j),j);
	  SetBinUpAt(GetBinUpAt(j),j);
	  SetTimeAt(GetTimeAt(j),j);
    }
  }

}


void R3BLosTCalPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BLosTCalPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BLOSTCALPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BLOSTCALPAR").Data());
    sql_cmds.push_back(R3BLosTCalPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BLosTCalPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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
    cout << "-E- R3BLosTCalPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BLosTCalPar::Print()
{
  std::cout<<"   R3BLosTCalPar: Time Calibration Parameters: "<<std::endl;
  std::cout<<"   fCompId: "<<  fCompId <<  std::endl;
  std::cout<<"   fChannel: "<<  fChannel <<  std::endl;
 for (Int_t i=0;i<NCHMAX;i++) {
   if ( (fBinLow[i] != 0) && (fBinUp[i] != 0) &&  (fTime[i] != 0) )
   std::cout<<"   BinLow: " <<  fBinLow[i]  <<  " BinUp "  << fBinUp[i] << " Time:" << fTime[i] <<  std::endl;
   }
}


Bool_t R3BLosTCalPar::Compare(const R3BLosTCalPar& that ) const {
  //  
  Bool_t test_h =  (fCompId   == that.fCompId)
    &&  (fChannel   == that.fChannel);
  
 
  Bool_t test_d=kTRUE;
  for(Int_t i=0; i<NCHMAX;i++){

    Int_t  a =  GetBinLowAt(i);
    Int_t  b =  that.GetBinLowAt(i);
    Int_t  c =  GetBinUpAt(i);
    Int_t  d =  that.GetBinUpAt(i);

    Double_t  t1 =  GetTimeAt(i);
    Double_t  t2 =  that.GetTimeAt(i);
	if ( ( a != b ) || ( c != d )  || (t1 != t2 ) ){
	  test_d = kFALSE;
      break;
    }      
  }
  
  return (test_h && test_d); 
}



FairDbWriter<R3BLosTCalPar>* R3BLosTCalPar::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BLosTCalPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BLOSTCALPAR") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BLOSTCALPAR").Data());
	  sql_cmds.push_back(R3BLosTCalPar::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BLosTCalPar::Store() ******* Error Executing SQL commands ***********  " << endl;
	  }
	  
	}
	
	// Refresh list of tables in connected database
	// for the choosen DB entry
	fMultConn->GetConnection(GetDbEntry())->SetTableExists();
	
	// Writer Meta-Class Instance
	fParam_Writer = GetParamWriter();
		
	fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"LOS Time Calib Pars");

    //ValTimeStamp tStart(rid);  
    //ValInterval valI(Detector::kLos, DataType::kData, tStart, ValTimeStamp::GetEOT(), "Validation interval");       
	//fParam_Writer->Activate(valI,GetComboNo(), GetVersion(),GetDbEntry(),"LOS Time Calib Pars");

	return fParam_Writer;
	
  }
}
