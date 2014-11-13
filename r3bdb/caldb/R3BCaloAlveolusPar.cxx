#include "R3BCaloAlveolusPar.h"

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

ClassImp(R3BCaloAlveolusPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BCaloAlveolusPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BCaloAlveolusPar>;




R3BCaloAlveolusPar::R3BCaloAlveolusPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
    fCompId(0),
    fAlveolusNodeId(0),
    fAlveolusTypeId(0),
    fCFThickness(0.),
    fCenter(),
    fRotEuler(),        
    fDbEntry(0),  // Set the default Db Entry to the first slot
    fParam_Writer(NULL), //  Writer Meta-Class for SQL IO
    fParam_Reader(NULL), //  Reader Meta-Class for SQL IO
    fMultConn(FairDbTableInterfaceStore::Instance().fConnectionPool)
{
  for(Int_t i=0;i<3;i++) {fCenter[i]=fRotEuler[i]=0.;}
}


R3BCaloAlveolusPar::~R3BCaloAlveolusPar()
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


void R3BCaloAlveolusPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloAlveolusPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("comp_id",  fCompId);
  list->add("alveolus_nodeid", fAlveolusNodeId);
  list->add("alveolus_typeid", fAlveolusTypeId);
  list->add("cf_thickness",  fCFThickness);
  list->add("alveolus_center", fCenter, 3);
  list->add("alveolus_euler", fRotEuler,3);
}

Bool_t R3BCaloAlveolusPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("comp_id", &fCompId)) { return kFALSE; }
  if (!list->fill("alveolus_nodeid", &fAlveolusNodeId)) { return kFALSE; }
  if (!list->fill("alveolus_typeid", &fAlveolusTypeId)) { return kFALSE; }
  if (!list->fill("cf_thickness", &fCFThickness)) { return kFALSE; }
  Double_t center[3]={0.,0.,0.};
  Double_t angles[3]={0.,0.,0.};
  if (!list->fill("alveolus_center", center, 3)) { return kFALSE; }
  if (!list->fill("alveolus_euler",  angles, 3)) { return kFALSE; }
  for(Int_t i=0;i<3;i++) {
	fCenter[i]   = center[i];
    fRotEuler[i] = angles[i]; 
  }

  return kTRUE;
}

void R3BCaloAlveolusPar::clear()
{
  fCompId=fAlveolusNodeId=fAlveolusTypeId=0;

  fCFThickness = 0.;

  for(Int_t i=0;i<3;i++) {
	fCenter[i]   = 0.;
    fRotEuler[i] = 0.;
  } 

  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }
}


string R3BCaloAlveolusPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BCALOALVEOLUSPAR"; }
  sql += "( SEQNO            INT NOT NULL,";
  sql += "  ROW_ID           INT NOT NULL,";
  sql += "  COMPID           INT,";
  sql += "  NODE_ID          INT,";
  sql += "  TYPE_ID          INT,";
  sql += "  CFT              DOUBLE,";
  sql += "  XC               DOUBLE,";
  sql += "  YC               DOUBLE,";
  sql += "  ZC               DOUBLE,";
  sql += "  PSI              DOUBLE,";
  sql += "  THETA            DOUBLE,";
  sql += "  PHI              DOUBLE,";
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BCaloAlveolusPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  clear();

  res_in >> fCompId >> fAlveolusNodeId  >> fAlveolusTypeId >> fCFThickness  
         >> fCenter[0] >> fCenter[1] >> fCenter[2] 
		 >> fRotEuler[0] >> fRotEuler[1] >> fRotEuler[2];

}

void R3BCaloAlveolusPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{

  res_out << fCompId  << fAlveolusNodeId  << fAlveolusTypeId << fCFThickness 
		  << fCenter[0] << fCenter[1] << fCenter[2] 
		  << fRotEuler[0] << fRotEuler[1] << fRotEuler[2];

}


void R3BCaloAlveolusPar::fill(UInt_t rid)
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
    R3BCaloAlveolusPar* cgd = (R3BCaloAlveolusPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fCompId = cgd->GetCompId();
    fAlveolusNodeId =  cgd->GetAlveolusNodeId();
    fAlveolusTypeId =  cgd->GetAlveolusTypeId();
    fCFThickness =  cgd->GetCFThickness();
    const Double_t* center   =  GetCenter();
    const Double_t* rot      =  GetRotEuler();
	for(Int_t j=0;j<3;j++) {
	  fCenter[j]   = center[j];
	  fRotEuler[j] = rot[j];
	} 
  }

}


void R3BCaloAlveolusPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloAlveolusPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALOALVEOLUSPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BCALOALVEOLUSPAR").Data());
    sql_cmds.push_back(R3BCaloAlveolusPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BCaloAlveolusPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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

  fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa Alveolus Node Pars");

  // Object to Table mapping
  *fParam_Writer<< (*this);

  // Check for eventual IO problems
  if ( !fParam_Writer->Close() ) {
    fail = true;
    cout << "-E- R3BCaloAlveolusPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BCaloAlveolusPar::Print()
{

  cout<<  " Alveolus Node Id: " << fAlveolusNodeId << " CompId: "<< fCompId <<  endl;
  cout << " Alveolus Type: " << fAlveolusTypeId << endl;
  cout << " Alveolus CF Thickness : " << fCFThickness << endl;

  for(Int_t i=0; i<3;i++){
	cout << " i= " << i << " center: " << fCenter[i] << " rot: " << fRotEuler[i] << endl;
  }
  
}


Bool_t R3BCaloAlveolusPar::Compare(const R3BCaloAlveolusPar& that ) const {
  //  
  Bool_t test_h =      (fCompId          == that.fCompId)
 	               &&  (fAlveolusTypeId   == that.fAlveolusTypeId)
 	               &&  (fAlveolusNodeId   == that.fAlveolusNodeId)
 	               &&  (fCFThickness   == that.fCFThickness);
  
 
  Bool_t test_d=kTRUE;
  for(Int_t i=0; i<3;i++){
    if ( (fCenter[i] != that.fCenter[i])
         ||
         (fRotEuler[i] != that.fRotEuler[i]) 
		 ) test_d = kFALSE;   
  }  
  return (test_h && test_d); 
}


FairDbWriter<R3BCaloAlveolusPar>* R3BCaloAlveolusPar::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BCaloAlveolusPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALOALVEOLUSPAR") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BCALOALVEOLUSPAR").Data());
	  sql_cmds.push_back(R3BCaloAlveolusPar::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BCaloAlveolusPar::Store() ******* Error Executing SQL commands ***********  " << endl;
	  }
	  
	}
	
	// Refresh list of tables in connected database
	// for the choosen DB entry
	fMultConn->GetConnection(GetDbEntry())->SetTableExists();
	
	// Writer Meta-Class Instance
	fParam_Writer = GetParamWriter();
		
	fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa Alveolus Node Pars");

	return fParam_Writer;
	
  }

  return NULL;
}
