// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                         R3BCalifaDUCalPar                         -----
// -----                 Created by P.Cabanelas 21/03/2017                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------


#include "R3BCalifaDUCalPar.h"

#include "FairDbLogFormat.h"
#include "FairDbLogService.h"
#include "FairDbOutTableBuffer.h"       // for FairDbOutRowStream
#include "FairDbStatement.h"            // for FairDbStatement
#include "FairDbStreamer.h"             // for FairDbStreamer

#include "FairParamList.h"              // for FairParamList

#include "Riosfwd.h"                    // for ostream
#include "TString.h"                    // for TString

#include <stdlib.h>                     // for exit
#include <memory>                       // for auto_ptr, etc
#include <vector>                       // for vector, vector<>::iterator


using namespace std;

ClassImp(R3BCalifaDUCalPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BCalifaDUCalPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BCalifaDUCalPar>;



R3BCalifaDUCalPar::R3BCalifaDUCalPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairDbObjTableMap(name,title,context, own),
    fCompId(0),
    fDetectionUnit(0),
    fGammaCal_offset(0.),
    fGammaCal_gain(0.), 
    fToTCal_par0(0.),
    fToTCal_par1(0.),
    fToTCal_par2(0.),
    fRangeCal_offset(0.),
    fRangeCal_gain(0.),
    fQuenchingFactor(0.),
    fPidGain(0.)
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
  //fMultConn=FairDbTableInterfaceStore::Instance().fConnectionPool;

}


R3BCalifaDUCalPar::~R3BCalifaDUCalPar()
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


void R3BCalifaDUCalPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCalifaDUCalPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("comp_id",  fCompId);
  list->add("detection_unit", fDetectionUnit);
  list->add("gammacal_offset", fGammaCal_offset);
  list->add("gammacal_gain", fGammaCal_gain);
  list->add("totcal_par0", fToTCal_par0);
  list->add("totcal_par1", fToTCal_par1);
  list->add("totcal_par2", fToTCal_par2);
  list->add("rangecal_offset", fRangeCal_offset);
  list->add("rangecal_gain", fRangeCal_gain);
  list->add("quenching_factor", fQuenchingFactor);
  list->add("pid_gain", fPidGain);

}

Bool_t R3BCalifaDUCalPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  if (!list->fill("comp_id", &fCompId)) { return kFALSE; }
  if (!list->fill("detection_unit", &fDetectionUnit)) { return kFALSE; }
  if (!list->fill("gammacal_offset", &fGammaCal_offset)) { return kFALSE; }
  if (!list->fill("gammacal_gain", &fGammaCal_gain)) { return kFALSE; }
  if (!list->fill("totcal_par0", &fToTCal_par0)) { return kFALSE; }
  if (!list->fill("totcal_par1", &fToTCal_par1)) { return kFALSE; }
  if (!list->fill("totcal_par2", &fToTCal_par2)) { return kFALSE; }
  if (!list->fill("rangecal_offset", &fRangeCal_offset)) { return kFALSE; }
  if (!list->fill("rangecal_gain", &fRangeCal_gain)) { return kFALSE; }
  if (!list->fill("quenching_factor", &fQuenchingFactor)) { return kFALSE; }
  if (!list->fill("pid_gain", &fPidGain)) { return kFALSE; }

  return kTRUE;
}

void R3BCalifaDUCalPar::clear()
{
  fCompId=fDetectionUnit=0;
  fGammaCal_offset = fGammaCal_gain = fToTCal_par0 = fToTCal_par1
     = fToTCal_par2 = fRangeCal_offset = fRangeCal_gain
     = fQuenchingFactor = fPidGain = 0.;
  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }

}


string R3BCalifaDUCalPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BCALODUCALPAR"; }
  sql += "( SEQNO                 INT NOT NULL,";
  sql += "  ROW_ID                INT NOT NULL,";
  sql += "  COMP_ID               INT,";
  sql += "  DETECTION_UNIT        INT,";
  sql += "  GAMMACAL_OFFSET       DOUBLE,";
  sql += "  GAMMACAL_GAIN         DOUBLE,";
  sql += "  TOTCAL_PAR0           DOUBLE,";
  sql += "  TOTCAL_PAR1           DOUBLE,";
  sql += "  TOTCAL_PAR2           DOUBLE,";
  sql += "  RANGECAL_OFFSET       DOUBLE,";
  sql += "  RANGECAL_GAIN         DOUBLE,";
  sql += "  QUENCHING_FACTOR      DOUBLE,";
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BCalifaDUCalPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{
  // Clear all structures
  clear();
  
  res_in >> fCompId  >> fDetectionUnit >> fGammaCal_offset >> fGammaCal_gain 
	 >> fToTCal_par0 >> fToTCal_par1 >> fToTCal_par2 >> fRangeCal_offset
         >> fRangeCal_gain >> fQuenchingFactor >> fPidGain;

}

void R3BCalifaDUCalPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{
  
  res_out << fCompId  << fDetectionUnit << fGammaCal_offset << fGammaCal_gain 
	  << fToTCal_par0 << fToTCal_par1 << fToTCal_par2 << fRangeCal_offset
          << fRangeCal_gain << fQuenchingFactor << fPidGain;

}


void R3BCalifaDUCalPar::fill(UInt_t rid)
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
    R3BCalifaDUCalPar* cgd = (R3BCalifaDUCalPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fCompId = cgd->GetCompId();
    fDetectionUnit =  cgd->GetDetectionUnit();
    fGammaCal_offset  =  cgd->GetGammaCal_offset();
    fGammaCal_gain  =  cgd->GetGammaCal_gain();
    fToTCal_par0  =  cgd->GetToTCal_par0();
    fToTCal_par1  =  cgd->GetToTCal_par1();
    fToTCal_par2  =  cgd->GetToTCal_par2();
    fRangeCal_offset  =  cgd->GetRangeCal_offset();
    fRangeCal_gain  =  cgd->GetRangeCal_gain();
    fQuenchingFactor  =  cgd->GetQuenchingFactor();
    fPidGain = cgd->GetPidGain();
  }

}


void R3BCalifaDUCalPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCalifaDUCalPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALIFADUCALPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BCALIFADUCALPAR").Data());
    sql_cmds.push_back(R3BCalifaDUCalPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BCalifaDUCalPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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
    cout << "-E- R3BCalifaDUCalPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BCalifaDUCalPar::Print()
{
  std::cout<<"   R3BCalifaDUCalPar: Detection Unit Calibration Parameters: "<<std::endl;
  std::cout<<"   fCompId: "<<  fCompId <<  std::endl;
  std::cout<<"   fDetectionUnit: "<<  fDetectionUnit <<  std::endl;
  std::cout<<"   fGammaCal_offset: "<<  fGammaCal_offset <<  "   fGammaCal_gain: "<<  fGammaCal_gain <<  std::endl;
  std::cout<<"   fToTCal_par0: "<<  fToTCal_par0 <<"   fToTCal_par1: "<<  fToTCal_par1 <<"   fToTCal_par2: "<<  fToTCal_par2 <<  std::endl;
  std::cout<<"   fRangeCal_offset: "<<  fRangeCal_offset <<  "   fRangeCal_gain: "<<  fRangeCal_gain <<  std::endl;
  std::cout<<"   fQuenchingFactor: "<<  fQuenchingFactor <<  std::endl;
  std::cout<<"   fPidGain: "<<  fPidGain <<  std::endl;
 
}


Bool_t R3BCalifaDUCalPar::Compare(const R3BCalifaDUCalPar& that ) const {
  //  
  Bool_t test_h =  (fCompId   == that.fCompId)
 	               &&  (fDetectionUnit   == that.fDetectionUnit)
	               &&  (fGammaCal_offset   == that.fGammaCal_offset)
		           &&  (fGammaCal_gain   == that.fGammaCal_gain)  
		           &&  (fToTCal_par0   == that.fToTCal_par0)   
		           &&  (fToTCal_par1   == that.fToTCal_par1)   
   		           &&  (fToTCal_par2   == that.fToTCal_par2)   
                   &&  (fRangeCal_offset   == that.fRangeCal_offset)
		           &&  (fRangeCal_gain   == that.fRangeCal_gain)
		           &&  (fQuenchingFactor   == that.fQuenchingFactor)
                           && (fPidGain == that.fPidGain);
  
  return (test_h); 
}



FairDbWriter<R3BCalifaDUCalPar>* R3BCalifaDUCalPar::ActivateWriter(Int_t rid)
{
  // delete if already existing
  if (fParam_Writer) { delete fParam_Writer; fParam_Writer=NULL; }

  else {
	// Create according to IoV
	Bool_t fail= kFALSE;
	
	// Create a unique statement on choosen DB entry
	auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
	if ( ! stmtDbn.get() ) {
	  cout << "-E-  R3BCalifaDUCalPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
		   << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
	  exit(1);
	}
	
	// Check if for this DB entry the table already exists.
	// If not call the corresponding Table Definition Function
	std::vector<std::string> sql_cmds;
	TString atr(GetName());
	atr.ToUpper();
	
	if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALIFADUCALPAR") ) {
	  sql_cmds.push_back(FairDb::GetValDefinition("R3BCALIFADUCALPAR").Data());
	  sql_cmds.push_back(R3BCalifaDUCalPar::GetTableDefinition());
	}
	
	// Packed SQL commands executed internally via SQL processor
	std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
	while( itr != itrEnd ) {
	  std::string& sql_cmd(*itr++);
	  stmtDbn->Commit(sql_cmd.c_str());
	  if ( stmtDbn->PrintExceptions() ) {
		fail = true;
		cout << "-E- R3BCalifaDUCalPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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
