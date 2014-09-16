#include "R3BCaloDUCalPar.h"

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

ClassImp(R3BCaloDUCalPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BCaloDUCalPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BCaloDUCalPar>;



R3BCaloDUCalPar::R3BCaloDUCalPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
	fCompId(0),
	fDetectionUnit(0),
	fGammaParZero(0.),
	fGammaParOne(0.),
	fProtonParZero(0.),
	fProtonParOne(0.),
	fConversionFactor(0.)
	/*fExtraOne(0.),
	fExtraTwo(0.),
	fExtraThree(0.),
	fExtraFour(0.)*/
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


R3BCaloDUCalPar::~R3BCaloDUCalPar()
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


void R3BCaloDUCalPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloDUCalPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("comp_id",  fCompId);
  list->add("detection_unit", fDetectionUnit);
  list->add("gamma_parzero", fGammaParZero);
  list->add("gamma_parone", fGammaParOne);
  list->add("proton_parzero", fProtonParZero);
  list->add("proton_parone", fProtonParOne);
  list->add("conversion_factor", fConversionFactor);
  /*list->add("extra_one", fExtraOne);
  list->add("extra_two", fExtraTwo);
  list->add("extra_three", fExtraThree);
  list->add("extra_four", fExtraFour);*/
  
}

Bool_t R3BCaloDUCalPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("comp_id", &fCompId)) { return kFALSE; }
  if (!list->fill("detection_unit", &fDetectionUnit)) { return kFALSE; }
  if (!list->fill("gamma_parzero", &fGammaParZero)) { return kFALSE; }
  if (!list->fill("gamma_parone", &fGammaParOne)) { return kFALSE; }
  if (!list->fill("proton_parzero", &fProtonParZero)) { return kFALSE; }
  if (!list->fill("proton_parone", &fProtonParOne)) { return kFALSE; }
  if (!list->fill("conversion_factor", &fConversionFactor)) { return kFALSE; }
  /*if (!list->fill("extra_one", &fExtraOne)) { return kFALSE; }
  if (!list->fill("extra_two", &fExtraTwo)) { return kFALSE; }
  if (!list->fill("extra_three", &fExtraThree)) { return kFALSE; }
  if (!list->fill("extra_four", &fExtraFour)) { return kFALSE; }*/

  return kTRUE;
}

void R3BCaloDUCalPar::clear()
{
  fCompId=fDetectionUnit=0;
  fGammaParZero=fGammaParOne=fProtonParZero=fProtonParOne=fConversionFactor=/*fExtraOne=fExtraTwo=fExtraThree=fExtraFour=*/0.;

  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }

}


string R3BCaloDUCalPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BCALODUCALPAR"; }
  sql += "( SEQNO                 INT NOT NULL,";
  sql += "  ROW_ID                INT NOT NULL,";
  sql += "  COMP_ID               INT,";
  sql += "  DETECTION_UNIT        INT,";
  sql += "  GAMMA_PARZERO         DOUBLE,";
  sql += "  GAMMA_PARONE          DOUBLE,";
  sql += "  PROTON_PARZERO        DOUBLE,";
  sql += "  PROTON_PARONE         DOUBLE,";
  sql += "  CONVERSION_FACTOR     DOUBLE,";
  /*sql += "  EXTRA_ONE		  DOUBLE,";
  sql += "  EXTRA_TWO		  DOUBLE,";
  sql += "  EXTRA_THREE		  DOUBLE,";
  sql += "  EXTRA_FOUR		  DOUBLE,";*/
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BCaloDUCalPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  // Clear all structures
  clear();
  
  res_in >> fCompId  >> fDetectionUnit >> fGammaParZero  >> fGammaParOne >> fProtonParZero >> fProtonParOne 
  >> fConversionFactor /*>> fExtraOne >> fExtraTwo >> fExtraThree >> fExtraFour*/;

}

void R3BCaloDUCalPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{
  
  res_out << fCompId  << fDetectionUnit << fGammaParZero  << fGammaParOne << fProtonParZero << fProtonParOne 
  << fConversionFactor /*<< fExtraOne << fExtraTwo << fExtraThree << fExtraFour*/;

}


void R3BCaloDUCalPar::fill(UInt_t rid)
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
    R3BCaloDUCalPar* cgd = (R3BCaloDUCalPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fCompId = cgd->GetCompId();
    fDetectionUnit =  cgd->GetDetectionUnit();
    fGammaParZero  =  cgd->GetGammaParZero();
    fGammaParOne  =  cgd->GetGammaParOne();
    fProtonParZero  =  cgd->GetProtonParZero();
    fProtonParOne  =  cgd->GetProtonParOne();
    fConversionFactor  =  cgd->GetConversionFactor();
    /*fExtraOne  =  cgd->GetExtraOne();
    fExtraTwo  =  cgd->GetExtraTwo();
    fExtraThree  =  cgd->GetExtraThree();
    fExtraFour  =  cgd->GetExtraFour();*/
  }

}


void R3BCaloDUCalPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloDUCalPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALODUCALPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BCALODUCALPAR").Data());
    sql_cmds.push_back(R3BCaloDUCalPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BCaloDUCalPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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
    cout << "-E- R3BCaloDUCalPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BCaloDUCalPar::Print()
{
  std::cout<<"   R3BCaloDUCalPar: Detection Unit Calibration Parameters: "<<std::endl;
  std::cout<<"   fCompId: "<<  fCompId <<  std::endl;
  std::cout<<"   fDetectionUnit: "<<  fDetectionUnit <<  std::endl;
  std::cout<<"   fGammaParZero: "<<  fGammaParZero <<  "   fGammaParOne: "<<  fGammaParOne <<  std::endl;
  std::cout<<"   fProtonParZero: "<<  fProtonParZero <<  "   fProtonParOne: "<<  fProtonParOne <<  std::endl;
  std::cout<<"   fConversionFactor: "<<  fConversionFactor <<  std::endl;
  /*std::cout<<"   fExtraOne: "<<  fExtraOne <<  std::endl;
  std::cout<<"   fExtraTwo: "<<  fExtraTwo <<  std::endl;
  std::cout<<"   fExtraThree: "<<  fExtraThree <<  std::endl;
  std::cout<<"   fExtraFour: "<<  fExtraFour <<  std::endl;*/
}


Bool_t R3BCaloDUCalPar::Compare(const R3BCaloDUCalPar& that ) const {
  //  
  Bool_t test_h =  (fCompId   == that.fCompId)
 	               &&  (fDetectionUnit   == that.fDetectionUnit)
		       &&  (fGammaParZero   == that.fGammaParZero)
		       &&  (fGammaParOne   == that.fGammaParOne)
		       &&  (fProtonParZero   == that.fProtonParZero)
		       &&  (fProtonParOne   == that.fProtonParOne)
		       &&  (fConversionFactor   == that.fConversionFactor);
		       /*&&  (fExtraOne   == that.fExtraOne)
		       &&  (fExtraTwo   == that.fExtraTwo)
		       &&  (fExtraThree   == that.fExtraThree)
		       &&  (fExtraFour   == that.fExtraFour);*/
  
  return (test_h); 
}



FairDbWriter<R3BCaloDUCalPar>* R3BCaloDUCalPar::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BCaloDUCalPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALODUCALPAR") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BCALODUCALPAR").Data());
	  sql_cmds.push_back(R3BCaloDUCalPar::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BCaloDUCalPar::Store() ******* Error Executing SQL commands ***********  " << endl;
	  }
	  
	}
	
	// Refresh list of tables in connected database
	// for the choosen DB entry
	fMultConn->GetConnection(GetDbEntry())->SetTableExists();
	
	// Writer Meta-Class Instance
	fParam_Writer = GetParamWriter();
		
	fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa Detection Unit Calib Pars");

    //ValTimeStamp tStart(rid);  
    //ValInterval valI(Detector::kCal, DataType::kData, tStart, ValTimeStamp::GetEOT(), "Validation interval");       
	//fParam_Writer->Activate(valI,GetComboNo(), GetVersion(),GetDbEntry(),"Califa Detection Unit Calib Pars");

	return fParam_Writer;
	
  }

  return NULL;
}
