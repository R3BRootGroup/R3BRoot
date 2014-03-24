#include "R3BCaloMediaPar.h"

#include "FairDbLogFormat.h"
#include "FairDbLogService.h"
#include "FairDbOutTableBuffer.h"         // for FairDbOutRowStream
#include "FairDbStatement.h"              // for FairDbStatement
#include "FairDbStreamer.h"               // for FairDbStreamer

#include "FairParamList.h"              // for FairParamList

#include "Riosfwd.h"                    // for ostream
#include "TString.h"                    // for TString

#include <stdlib.h>                     // for exit
#include <memory>                       // for auto_ptr, etc
#include <vector>                       // for vector, vector<>::iterator


using namespace std;

ClassImp(R3BCaloMediaPar);

#include "FairDbReader.tpl"
template class  FairDbReader<R3BCaloMediaPar>;

#include "FairDbWriter.tpl"
template class  FairDbWriter<R3BCaloMediaPar>;



R3BCaloMediaPar::R3BCaloMediaPar(const char* name, const char* title, const char* context, Bool_t own)
  : FairParGenericSet(name,title,context, own),
    fName(""), 
    fDensity(0.),
    fRadLen(0.),
    fIntLen(0.),
    fP(0.),
    fT(0.),    
    fN(0),         
    fCompId(0),
    fIDmixt(0),
    fAmixt(NULL),
	fZmixt(NULL),
    fWmixt(NULL),
    fDbEntry(0),  // Set the default Db Entry to the first slot
    fParam_Writer(NULL), //  Writer Meta-Class for SQL IO
    fParam_Reader(NULL), //  Reader Meta-Class for SQL IO
    fMultConn(FairDbTableInterfaceStore::Instance().fConnectionPool)
{
}


R3BCaloMediaPar::~R3BCaloMediaPar()
{

  if (fAmixt) delete[] fAmixt;
  if (fZmixt) delete[] fZmixt;
  if (fWmixt) delete[] fWmixt;
  
  if (fParam_Writer) {
    delete fParam_Writer;
    fParam_Writer=NULL;
  }

  if (fParam_Reader) {
    delete fParam_Reader;
    fParam_Reader=NULL;
  }

}


void R3BCaloMediaPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloMediaPar::putParams() called"<<std::endl;
  if(!list) { return; }
  list->add("mixt_name",  (Text_t*) fName.c_str());
  list->add("density",  fDensity);
  list->add("rad_len",  fRadLen);
  list->add("int_len",  fIntLen);
  list->add("pressure",  fP);
  list->add("temperature",  fT);
  list->add("nelements",  fN);
  list->add("comp_id",  fCompId);
  list->add("mixt_id", fIDmixt);
  list->add("A_List",  fAmixt, fN);
  list->add("Z_List",  fZmixt, fN);
  list->add("W_List",  fWmixt, fN);
}

Bool_t R3BCaloMediaPar::getParams(FairParamList* list)
{
  if (!list) { return kFALSE; }
  Text_t aName[80];
  if (!list->fill("mixt_name",    aName,80 )) { return kFALSE;}
  fName = aName;
  if (!list->fill("density",     &fDensity)) { return kFALSE;}
  if (!list->fill("rad_len",     &fRadLen )) { return kFALSE;}
  if (!list->fill("int_len",     &fIntLen )) { return kFALSE;}
  if (!list->fill("pressure",    &fP )) { return kFALSE;}
  if (!list->fill("temperature", &fT )) { return kFALSE;}
  if (!list->fill("nelements",   &fN )) { return kFALSE;}
  if (!list->fill("comp_id",     &fCompId)) { return kFALSE; }
  if (!list->fill("mixt_id",     &fIDmixt)) { return kFALSE; }
  if (!list->fill("A_List",      fAmixt , fN)) { return kFALSE;}
  if (!list->fill("Z_List",      fZmixt , fN)) { return kFALSE;}
  if (!list->fill("W_List",      fWmixt , fN)) { return kFALSE;}
  return kTRUE;
}

void R3BCaloMediaPar::clear()
{
  fName="";

  fCompId=fIDmixt=fN=0;
  fDensity=fRadLen=fIntLen=fP=fT=0.;

  for(Int_t i=0; i<fN;i++){
      fAmixt[i]=0.;fZmixt[i]=0.;fWmixt[i]=0.;
  }

  if (fParam_Writer) { fParam_Writer->Reset(); }
  if (fParam_Reader) { fParam_Reader->Reset(); }
}


string R3BCaloMediaPar::GetTableDefinition(const char* Name)
{

  string sql("create table ");
  if ( Name ) { sql += Name; }
  else { sql += "R3BCALOMEDIAPAR"; }
  sql += "( SEQNO          INT NOT NULL,";
  sql += "  ROW_ID         INT NOT NULL,";
  sql += "  NAME           TEXT,";
  sql += "  DENSITY        DOUBLE,";
  sql += "  RADLEN         DOUBLE,";
  sql += "  INTLEN         DOUBLE,";
  sql += "  P              DOUBLE,";
  sql += "  T              DOUBLE,";
  sql += "  N              INT,";
  sql += "  COMP_ID        INT,";
  sql += "  MIXT_ID        INT,";
  sql += "  A_MIXT         TEXT,";
  sql += "  Z_MIXT         TEXT,";
  sql += "  W_MIXT         TEXT,";
  sql += "  primary key(SEQNO,ROW_ID))";
  return sql;
}



void R3BCaloMediaPar::Fill(FairDbResultPool& res_in,
                        const FairDbValRecord* valrec)
{

  // Allocate Buffers
  if (!fAmixt) fAmixt = new Double_t[MAX_MEDIA];
  if (!fZmixt) fZmixt = new Double_t[MAX_MEDIA];
  if (!fWmixt) fWmixt = new Double_t[MAX_MEDIA];

  FairDbStreamer Amixt(fAmixt,MAX_MEDIA);
  FairDbStreamer Zmixt(fZmixt,MAX_MEDIA);
  FairDbStreamer Wmixt(fWmixt,MAX_MEDIA);
 
  res_in >> fName >> fDensity >> fRadLen  >> fIntLen 
         >> fP >> fT >> fN >> fCompId >> fIDmixt 
         >> Amixt >> Zmixt >> Wmixt;
     
  // Update arrays
  Amixt.Fill(fAmixt);
  Zmixt.Fill(fZmixt);
  Wmixt.Fill(fWmixt);

}

void R3BCaloMediaPar::Store(FairDbOutTableBuffer& res_out,
                         const FairDbValRecord* valrec) const
{
  // Check first if arrays are defined
  if (!fAmixt || !fZmixt || !fWmixt ) {
	cout << "-E- R3BCaloMediaPar::Store() SQL I/O  : Array Structure not defined " << endl;
    exit(0); 
  }

  FairDbStreamer Amixt(fAmixt,fN);
  FairDbStreamer Zmixt(fZmixt,fN);
  FairDbStreamer Wmixt(fWmixt,fN);

  res_out << fName << fDensity << fRadLen  << fIntLen 
          << fP << fT << fN << fCompId << fIDmixt 
          << Amixt << Zmixt << Wmixt;
}


void R3BCaloMediaPar::fill(UInt_t rid)
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
    R3BCaloMediaPar* cgd = (R3BCaloMediaPar*) fParam_Reader->GetRow(i);
    if (!cgd) { continue; }
    fName = cgd->GetMixtName(); 
    fDensity = cgd->GetDensity();
    fRadLen = cgd->GetRadLen();
    fIntLen = cgd->GetIntLen();
    fP = cgd->GetPressure();
    fT = cgd->GetTemp();    
    fN = cgd->GetN();         
    fCompId = cgd->GetCompId(); 
    fIDmixt = cgd->GetID();
    fAmixt = cgd->GetAmixt();
    fZmixt = cgd->GetZmixt();
    fWmixt = cgd->GetWmixt();
  }

}


void R3BCaloMediaPar::store(UInt_t rid)
{

  // Boolean IO test variable
  Bool_t fail= kFALSE;

  // Create a unique statement on choosen DB entry
  auto_ptr<FairDbStatement> stmtDbn(fMultConn->CreateStatement(GetDbEntry()));
  if ( ! stmtDbn.get() ) {
    cout << "-E-  R3BCaloMediaPar::Store()  Cannot create statement for Database_id: " << GetDbEntry()
         << "\n    Please check the FAIRDB_TSQL_* environment.  Quitting ... " << endl;
    exit(1);
  }

  // Check if for this DB entry the table already exists.
  // If not call the corresponding Table Definition Function
  std::vector<std::string> sql_cmds;
  TString atr(GetName());
  atr.ToUpper();

  if (! fMultConn->GetConnection(GetDbEntry())->TableExists("R3BCALOMEDIAPAR") ) {
    sql_cmds.push_back(FairDb::GetValDefinition("R3BCALOMEDIAPAR").Data());
    sql_cmds.push_back(R3BCaloMediaPar::GetTableDefinition());
  }

  // Packed SQL commands executed internally via SQL processor
  std::vector<std::string>::iterator itr(sql_cmds.begin()), itrEnd(sql_cmds.end());
  while( itr != itrEnd ) {
    std::string& sql_cmd(*itr++);
    stmtDbn->Commit(sql_cmd.c_str());
    if ( stmtDbn->PrintExceptions() ) {
      fail = true;
      cout << "-E- R3BCaloMediaPar::Store() ******* Error Executing SQL commands ***********  " << endl;
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

  fParam_Writer->Activate(GetValInterval(rid),GetComboNo(), GetVersion(),GetDbEntry(),"Califa media parameter");

  // Object to Table mapping
  *fParam_Writer<< (*this);

  // Check for eventual IO problems
  if ( !fParam_Writer->Close() ) {
    fail = true;
    cout << "-E- R3BCaloMediaPar::Store() ******** Cannot do IO on class: " << GetName() <<  endl;
  }

}

void R3BCaloMediaPar::Print()
{
  std::cout<<"   Medium SQL Parameter <Mixture_Id> "<< fIDmixt <<  " <comp_Id> " << fCompId << std::endl;
  std::cout<<"   Name        = " << fName  << std::endl;
  std::cout<<"   Density     = " << fDensity  << std::endl;
  std::cout<<"   RadLen      = " << fRadLen  << std::endl;
  std::cout<<"   IntLen      = " << fIntLen  << std::endl;
  std::cout<<"   P           = " << fP << std::endl;
  std::cout<<"   T           = " << fT  << std::endl;
  std::cout<<"   Nelements   = " << fN  << std::endl;
  for(Int_t i=0; i<fN; i++) {
	std::cout<<"   A:   = " << fAmixt[i]  << std::endl;
    std::cout<<"   Z:   = " << fZmixt[i]  << std::endl;
    std::cout<<"   W:   = " << fWmixt[i]  << std::endl;
  }
}

void R3BCaloMediaPar::SetN(Int_t nele){
  fN=nele;
  if(!fAmixt) fAmixt = new Double_t[fN];
  if(!fZmixt) fZmixt = new Double_t[fN];
  if(!fWmixt) fWmixt = new Double_t[fN];
}  

Bool_t R3BCaloMediaPar::Compare(const R3BCaloMediaPar& that ) const {
  
  Bool_t test_h =  
	    (fIDmixt   == that.fIDmixt)
	&&  (fCompId   == that.fCompId)
    &&  (fName.compare(that.fName)==0)
    &&  (fDensity  == that.fDensity)
    &&  (fRadLen  == that.fRadLen)
    &&  (fIntLen  == that.fIntLen)
    &&  (fP  == that.fP)
    &&  (fT  == that.fT);
  
  
  Bool_t test_d=kTRUE;
  for(Int_t i=0; i<fN;i++){
	if ( 
		(fAmixt[i]!=that.fAmixt[i]) 
		||
		(fZmixt[i]!=that.fZmixt[i]) 
		||
		(fWmixt[i]!=that.fWmixt[i])
		 ) test_d = kFALSE;
  }

  return (test_h && test_d); 
}




