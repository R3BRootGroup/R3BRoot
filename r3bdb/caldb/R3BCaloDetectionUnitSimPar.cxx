#include "R3BCaloDetectionUnitSimPar.h"

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

ClassImp(R3BCaloDetectionUnitSimPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BCaloDetectionUnitSimPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BCaloDetectionUnitSimPar>;




R3BCaloDetectionUnitSimPar::R3BCaloDetectionUnitSimPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairDbObjTableMap(name,title,context, own),
    fCompId(0),
    fCrystalNodeId(0),
    fCrystalTypeId(0),
    fWrapThickness(0.),
    fCenter(),
    fRotEuler(),        
    fDbEntry(0),  // Set the default Db Entry to the first slot
    fParam_Writer(NULL), //  Writer Meta-Class for SQL IO
    fParam_Reader(NULL), //  Reader Meta-Class for SQL IO
    fMultConn(FairDbTableInterfaceStore::Instance().fConnectionPool)
{
  for(Int_t i=0;i<3;i++) {fCenter[i]=fRotEuler[i]=0.;}
}


R3BCaloDetectionUnitSimPar::~R3BCaloDetectionUnitSimPar()
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


void R3BCaloDetectionUnitSimPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloDetectionUnitSimPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("comp_id",  fCompId);
  list->add("crystal_nodeid", fCrystalNodeId);
  list->add("crystal_typeid", fCrystalTypeId);
  list->add("wrap_thickness",  fWrapThickness);
  list->add("crystal_center", fCenter, 3);
  list->add("crystal_euler", fRotEuler,3);
}

Bool_t R3BCaloDetectionUnitSimPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("comp_id", &fCompId)) { return kFALSE; }
  if (!list->fill("crystal_nodeid", &fCrystalNodeId)) { return kFALSE; }
  if (!list->fill("crystal_typeid", &fCrystalTypeId)) { return kFALSE; }
  if (!list->fill("wrap_thickness", &fWrapThickness)) { return kFALSE; }
  Double_t center[3]={0.,0.,0.};
  Double_t angles[3]={0.,0.,0.};
  if (!list->fill("crystal_center", center, 3)) { return kFALSE; }
  if (!list->fill("crystal_euler",  angles, 3)) { return kFALSE; }
  for(Int_t i=0;i<3;i++) {
	fCenter[i]   = center[i];
    fRotEuler[i] = angles[i]; 
  }

  return kTRUE;
}

void R3BCaloDetectionUnitSimPar::clear()
{
  fCompId=fCrystalNodeId=fCrystalTypeId=0;

  fWrapThickness = 0.;

  for(Int_t i=0;i<3;i++) {
	fCenter[i]   = 0.;
    fRotEuler[i] = 0.;
  } 

  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }
}


string R3BCaloDetectionUnitSimPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BCALODETECTIONUNITSIMPAR"; }
  sql += "( SEQNO            INT NOT NULL,";
  sql += "  ROW_ID           INT NOT NULL,";
  sql += "  COMPID           INT,";
  sql += "  NODE_ID          INT,";
  sql += "  TYPE_ID          INT,";
  sql += "  WRAP             DOUBLE,";
  sql += "  XC               DOUBLE,";
  sql += "  YC               DOUBLE,";
  sql += "  ZC               DOUBLE,";
  sql += "  PSI              DOUBLE,";
  sql += "  THETA            DOUBLE,";
  sql += "  PHI              DOUBLE,";
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BCaloDetectionUnitSimPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  clear();

  res_in >> fCompId >> fCrystalNodeId  >> fCrystalTypeId >> fWrapThickness  
         >> fCenter[0] >> fCenter[1] >> fCenter[2] 
		 >> fRotEuler[0] >> fRotEuler[1] >> fRotEuler[2];

}

void R3BCaloDetectionUnitSimPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{

  res_out << fCompId  << fCrystalNodeId  << fCrystalTypeId << fWrapThickness 
		  << fCenter[0] << fCenter[1] << fCenter[2] 
		  << fRotEuler[0] << fRotEuler[1] << fRotEuler[2];

}


void R3BCaloDetectionUnitSimPar::fill(UInt_t rid)
{
  // Get Reader Meta Class
  fParam_Reader=GetParamReader();

  // Define a Context
  ValTimeStamp ts(rid);
  ValCondition context(FairDbDetector::kCal,DataType::kData,ts);

  // Activate reading for this Context
  fParam_Reader->Activate(context, GetVersion());

  // By default use the latest row entry
  // (Other rows would correspond to outdated versions)
  Int_t numRows = fParam_Reader->GetNumRows();
  if ( numRows > 1 ) { numRows = 1; }

  for (int i = 0; i < numRows; ++i) {
    R3BCaloDetectionUnitSimPar* cgd = (R3BCaloDetectionUnitSimPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fCompId = cgd->GetCompId();
    fCrystalNodeId =  cgd->GetCrystalNodeId();
    fCrystalTypeId =  cgd->GetCrystalTypeId();
    fWrapThickness =  cgd->GetWrapThickness();
    const Double_t* center   =  GetCenter();
    const Double_t* rot      =  GetRotEuler();
	for(Int_t j=0;j<3;j++) {
	  fCenter[j]   = center[j];
	  fRotEuler[j] = rot[j];
	} 
  }

}


void R3BCaloDetectionUnitSimPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloDetectionUnitSimPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALODETECTIONUNITSIMPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BCALODETECTIONUNITSIMPAR").Data());
    sql_cmds.push_back(R3BCaloDetectionUnitSimPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BCaloDetectionUnitSimPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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

  fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa Crystal Node Pars");

  // Object to Table mapping
  *fParam_Writer<< (*this);

  // Check for eventual IO problems
  if ( !fParam_Writer->Close() ) {
    fail = true;
    cout << "-E- R3BCaloDetectionUnitSimPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BCaloDetectionUnitSimPar::Print()
{

  cout<<  " Crystal Node Id: " << fCrystalNodeId << " CompId: "<< fCompId <<  endl;
  cout << " Crystal Type: " << fCrystalTypeId << endl;
  cout << " Crystal Wrap Thickness : " << fWrapThickness << endl;

  for(Int_t i=0; i<3;i++){
	cout << " i= " << i << " center: " << fCenter[i] << " rot: " << fRotEuler[i] << endl;
  }
  
}


Bool_t R3BCaloDetectionUnitSimPar::Compare(const R3BCaloDetectionUnitSimPar& that ) const {
  //  
  Bool_t test_h =      (fCompId          == that.fCompId)
 	               &&  (fCrystalTypeId   == that.fCrystalTypeId)
 	               &&  (fCrystalNodeId   == that.fCrystalNodeId)
 	               &&  (fWrapThickness   == that.fWrapThickness);
  
 
  Bool_t test_d=kTRUE;
  for(Int_t i=0; i<3;i++){
    if ( (fCenter[i] != that.fCenter[i])
         ||
         (fRotEuler[i] != that.fRotEuler[i]) 
		 ) test_d = kFALSE;   
  }  
  return (test_h && test_d); 
}


FairDbWriter<R3BCaloDetectionUnitSimPar>* R3BCaloDetectionUnitSimPar::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BCaloDetectionUnitSimPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALODETECTIONUNITSIMPAR") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BCALODETECTIONUNITSIMPAR").Data());
	  sql_cmds.push_back(R3BCaloDetectionUnitSimPar::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BCaloDetectionUnitSimPar::Store() ******* Error Executing SQL commands ***********  " << endl;
	  }
	  
	}
	
	// Refresh list of tables in connected database
	// for the choosen DB entry
	fMultConn->GetConnection(GetDbEntry())->SetTableExists();
	
	// Writer Meta-Class Instance
	fParam_Writer = GetParamWriter();
		
	fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa Crystal Node Pars");
	
  }

  return fParam_Writer;
}
