#include "R3BCaloCrystalPar.h"

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

ClassImp(R3BCaloCrystalPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BCaloCrystalPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BCaloCrystalPar>;




R3BCaloCrystalPar::R3BCaloCrystalPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
    fId(0),
    fIdAmcrys(0),
    fEnergySource(""),
    fAbsoluteLO(0.),
    fLO(0.),
    fEnergyResolution(0.),
    fOrigin(""),
    fLocation(""),
    fObservation(""),
    fDbEntry(0),  // Set the default Db Entry to the first slot
    fParam_Writer(NULL), //  Writer Meta-Class for SQL IO
    fParam_Reader(NULL), //  Reader Meta-Class for SQL IO
    fMultConn(FairDbTableInterfaceStore::Instance().fConnectionPool)
{
  
}


R3BCaloCrystalPar::~R3BCaloCrystalPar()
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


void R3BCaloCrystalPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloCrystalPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("CrystalPar_Id", fId);
  list->add("CrystalPar_IdAmcrys", fIdAmcrys);
  list->add("CrystalPar_EnergySource", (Text_t*)fEnergySource.c_str());
  list->add("CrystalPar_AbsoluteLO", fAbsoluteLO);
  list->add("CrystalPar_LO", fLO);
  list->add("CrystalPar_EnergyResolution", fEnergyResolution);
  list->add("CrystalPar_Origin", (Text_t*)fOrigin.c_str());
  list->add("CrystalPar_Location", (Text_t*)fLocation.c_str());
  list->add("CrystalPar_Observation", (Text_t*)fObservation.c_str());
}

Bool_t R3BCaloCrystalPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("CrystalPar_Id", &fId)) { return kFALSE; }
  if (!list->fill("CrystalPar_IdAmcrys", &fIdAmcrys)) { return kFALSE; }
  Text_t energysourceaux[80];
  if (!list->fill("CrystalPar_EnergySource", energysourceaux, 80)) { return kFALSE; }
  fEnergySource = energysourceaux;
  if (!list->fill("CrystalPar_AbsoluteLO", &fAbsoluteLO)) { return kFALSE; }
  if (!list->fill("CrystalPar_LO", &fLO)) { return kFALSE; }
  if (!list->fill("CrystalPar_EnergyResolution", &fEnergyResolution)) { return kFALSE; }
  Text_t originaux[80];
  if (!list->fill("CrystalPar_Origin", originaux, 80)) { return kFALSE; }
  fOrigin = originaux;
  Text_t locationaux[80];
  if (!list->fill("CrystalPar_Location", locationaux, 80)) { return kFALSE; }
  fLocation = locationaux;
  Text_t observationaux[80];
  if (!list->fill("CrystalPar_Observation", observationaux, 80)) { return kFALSE; }
  fObservation = observationaux;

  return kTRUE;
}

void R3BCaloCrystalPar::clear()
{
  fId=fIdAmcrys=0;
  fAbsoluteLO=fAbsoluteLO=fEnergyResolution=0.;
  fEnergySource=fOrigin=fLocation=fObservation="";

  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }
}


string R3BCaloCrystalPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BCALOCRYSTALPAR"; }
  sql += "( SEQNO            		INT NOT NULL,";
  sql += "  ROW_ID           		INT NOT NULL,";
  sql += "  ID               		INT NOT NULL,";
  sql += "  ID_AMCRYS     		INT NOT NULL,";
  sql += "  ENERGYSOURCE		TEXT,";
  sql += "  ABSOLUTELO			DOUBLE,";
  sql += "  LO				DOUBLE,";
  sql += "  ENERGYRESOLUTION		DOUBLE,";
  sql += "  ORIGIN			TEXT,";
  sql += "  LOCATION			TEXT,";
  sql += "  OBSERVATION			TEXT,";
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BCaloCrystalPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  clear();

  res_in >> fId  >> fIdAmcrys >> fEnergySource >> fAbsoluteLO >> fLO >> fEnergyResolution >> fOrigin >> fLocation >> fObservation;

}

void R3BCaloCrystalPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{

  res_out << fId  << fIdAmcrys << fEnergySource << fAbsoluteLO << fLO << fEnergyResolution << fOrigin <<  fLocation << fObservation;

}


void R3BCaloCrystalPar::fill(UInt_t rid)
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
    R3BCaloCrystalPar* cgd = (R3BCaloCrystalPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fId =  cgd->GetId();
    fIdAmcrys =  cgd->GetIdAmcrys();
    fEnergySource =  cgd->GetEnergySource();
    fAbsoluteLO =  cgd->GetAbsoluteLO();
    fLO =  cgd->GetLO();
    fEnergyResolution =  cgd->GetEnergyResolution();
    fOrigin =  cgd->GetOrigin();
    fLocation =  cgd->GetLocation();
    fObservation =  cgd->GetObservation();
  }

}


void R3BCaloCrystalPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloCrystalPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALOCRYSTALPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BCALOCRYSTALPAR").Data());
    sql_cmds.push_back(R3BCaloCrystalPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BCaloCrystalPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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
    cout << "-E- R3BCaloCrystalPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BCaloCrystalPar::Print()
{

  cout<<  " Crystal Id: " << fId << " Crystal Id Amcrys: "<< fIdAmcrys <<  endl;
  cout << " Crystal Energy Source : " << fEnergySource << endl;
  cout << " Crystal AbsoluteLO : " << fAbsoluteLO << endl;
  cout << " Crystal LO : " << fLO << endl;
  cout << " Crystal EnergyResolution : " << fEnergyResolution << endl;
  cout << " Crystal Origin : " << fOrigin << endl;
  cout << " Crystal Location : " << fLocation << endl;
  cout << " Crystal Observation : " << fObservation << endl;
}


Bool_t R3BCaloCrystalPar::Compare(const R3BCaloCrystalPar& that ) const {
  //  
  Bool_t test_h =      (fId          == that.fId)
 	               &&  (fIdAmcrys   == that.fIdAmcrys)
 	               &&  (fEnergySource   == that.fEnergySource)
 	               &&  (fAbsoluteLO   == that.fAbsoluteLO)
 	               &&  (fLO   == that.fLO)
 	               &&  (fEnergyResolution   == that.fEnergyResolution)
 	               &&  (fOrigin   == that.fOrigin)
		       &&  (fLocation   == that.fLocation)
		       &&  (fObservation   == that.fObservation);
  
 return (test_h); 
}


FairDbWriter<R3BCaloCrystalPar>* R3BCaloCrystalPar::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BCaloCrystalPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALOCRYSTALPAR") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BCALOCRYSTALPAR").Data());
	  sql_cmds.push_back(R3BCaloCrystalPar::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BCaloCrystalPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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

void R3BCaloCrystalPar::update(Int_t seqno)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloCrystalPar::Update()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  FairDbString command;
  command << "UPDATE R3BCALOCRYSTALPARVAL SET TIMEEND = NOW() WHERE SEQNO = " << seqno <<  ";\0";

  stmtDbn->Commit(command.c_str());
  if ( stmtDbn->PrintExceptions() ) {
   fail = true;
   cout << "-E- R3BCaloCrystalPar::Update() ******* Error Executing SQL commands ***********  " << endl;
  }

}
