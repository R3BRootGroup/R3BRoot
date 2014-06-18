#ifndef R3BDBLMDFILEINFO_H
#define R3BDBLMDFILEINFO_H

#include "FairParGenericSet.h"          // for FairParGenericSet
#include "DataType.h"                    // for EDataType::kData
#include "ValCondition.h"                 // for ValCondition
#include "ValTimeStamp.h"               // for ValTimeStamp
#include "db_detector_def.h"            // for Detector, etc

#include "Rtypes.h"                     // for Double_t, Int_t, UInt_t, etc

#include <iostream>                     // for operator<<, basic_ostream, etc
#include <string>                       // for string

#include "TObjArray.h"                  // Store for vertices 
#include "TVector3.h"                   // Vertex
#include "TGeoArb8.h"                   // Trap
#include "FairDbWriter.h"
#include "FairDbReader.h"


using namespace std;

class FairDbOutTableBuffer;
class FairDbResultPool;
class FairDbObjTableMap;
class FairDbValRecord;
class FairParamList;



class R3BDBLmdFileInfo : public FairParGenericSet
{

using TObject::Compare;

 public :
 static const std::string kNoFileName;  // = "<<NoFileName>>"
 static const std::string kNoFileLabel; // = "<<NoFileLabel>>"
 static const std::string kNoExpLabel;  // = "<<NoExpLabel>>"
 static const std::string kNoExpPhase;  // = "<<NoExpPhase>>"
 static const std::string kNoFileCom;   // = "<<NoFileCom>>"
 static const std::string kNoRunType;   // = "<<NoRunType>>"

  R3BDBLmdFileInfo (const char* name="R3BDBLMDFILEINFO",
				 const char* title="R3B DB File Info",
				 const char* context="TestDefaultContext",
				 Bool_t own=kTRUE);
    virtual ~R3BDBLmdFileInfo(void);
    void clear(void);
    void   putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print();

    /** Global Accessor functions **/
    Int_t GetComboNo()       const {return fCompId;}
    Int_t GetCompId()   const {return fCompId;}
    void SetCompId(Int_t x)  {SetComboNo(x); fCompId=x;}

    /** Local Accessor functions **/
    void   SetFileId(const Int_t& fid)      {fFileId = fid;}  
    const Int_t&  GetFileId()  const        {return fFileId;}

    void   SetRunNr(const Int_t& run)       {fRunNr = run;}  
    const Int_t&  GetRunNr()   const        {return fRunNr;}
    void   SetFileNr(const Int_t& run)      {fFileNr = run;}  
    const Int_t&  GetFileNr()  const        {return fFileNr;}
   
    void   SetExpLabel(const std::string& run) {fExpLabel = run;}  
	const std::string&  GetExpLabel()  const   {return fExpLabel;}
	void   SetExpPhase(const std::string& run) {fExpPhase = run;}  
	const std::string&  GetExpPhase()  const   {return fExpPhase;}
	void   SetRunType(const std::string& run) {fRunType = run;}  
	const std::string&  GetRunType()   const   {return fRunType;}


    void   SetStartTime(const ValTimeStamp& starttime) {fStartTime = starttime;}  
    void   SetEndTime(const ValTimeStamp& endtime)     {fEndTime = endtime;}  
    const ValTimeStamp&  GetStartTime() const          {return fStartTime;}  
    const ValTimeStamp&  GetEndTime() const            {return fEndTime;}  
      
    void   SetOffsetTime(const Int_t& offset)          {fOffsetTime = offset;}  
    const Int_t&  GetOffsetTime()   const              {return fOffsetTime;}
    void   SetType(const Int_t& type)                  {fType = type;}  
    const Int_t&  GetType()   const                    {return fType;}
    void   SetSubType(const Int_t& type)               {fSubType = type;}  
    const Int_t&  GetSubType()   const                 {return fSubType;}

    void   SetLength(const Int_t& len)                 {fLength = len;}  
    const Int_t&  GetLength()   const                  {return fLength;}
    void   SetFragmentation(const Int_t& frag)         {fFragmented = frag;}  
    const Int_t&  GetFragmentation()   const           {return fFragmented;}

	void   SetFileLabel(const std::string& flab)       {fFileLabel = flab;}  
	const std::string&  GetFileLabel()   const         {return fFileLabel;}
	void   SetFileName(const std::string&  fname)      {fFileName = fname;}  
	const std::string&  GetFileName()   const          {return fFileName;}
	void   SetFileComment(const std::string& com)      {fFileComment = com;}  
	const std::string&  GetFileComment()   const       {return fFileComment;}
      


	const std::string&  GetTableName()   const       {return fTableName;}
    UInt_t GetIndex(UInt_t /*def*/) const { return fFileId; }
    Bool_t Compare(const R3BDBLmdFileInfo& that ) const; 
	
    // Add-ons: SQL descriptors for the parameter class
    virtual std::string GetTableDefinition(const char* Name = 0);
	
    virtual FairDbObjTableMap* CreateObjTableMap() const {
      return new R3BDBLmdFileInfo();
    }

    // Atomic IO (intrinsic)
    virtual void Fill(FairDbResultPool& res_in,
                      const FairDbValRecord* valrec);
    virtual void Store(FairDbOutTableBuffer& res_out,
                       const FairDbValRecord* valrec) const;
    // Global IO using run_id
    virtual void fill(UInt_t rid);
    virtual void store(UInt_t rid);

    // Validity frame definition
    virtual ValCondition GetContext(UInt_t rid) {
      return ValCondition(Detector::kUnknown,
                          DataType::kData,
                          ValTimeStamp(rid));
    }

    // SQL-IO Meta-Class Getters
    FairDbReader<R3BDBLmdFileInfo>* GetParamReader();
    FairDbWriter<R3BDBLmdFileInfo>* GetParamWriter();
	FairDbWriter<R3BDBLmdFileInfo>* ActivateWriter(Int_t rid);
 
	Bool_t Commit();

   Int_t        GetSeqNo() const { return CalcUniqueSeqNo(fFileId); }
   static Int_t  CalcUniqueSeqNo(Int_t run);

  private:
    Int_t          fCompId;
    Int_t          fFileId;
    Int_t          fRunNr;
    Int_t          fFileNr;
    std::string    fExpLabel;  
  	std::string    fExpPhase;
	std::string    fRunType;
    ValTimeStamp   fStartTime;   
    ValTimeStamp   fEndTime;   
    // Sync. time offset in [s]
    Int_t          fOffsetTime; 
    Int_t          fType;
    Int_t          fSubType;
    // Length in datawords units     
    Int_t          fLength; 
    Int_t          fFragmented;
    std::string    fFileLabel;
    std::string    fFileName;
    std::string    fFileComment; 
      

    // Database Pool Index
    Int_t fDbEntry; //!
	std::string fTableName; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbWriter<R3BDBLmdFileInfo>* fParam_Writer; //!
    // Parameter Container SQL Writer Meta-Class
    FairDbReader<R3BDBLmdFileInfo>* fParam_Reader; //!
    // Connection Pool
    FairDbConnectionPool* fMultConn;  //!

    ClassDef(R3BDBLmdFileInfo,1); // R3BDBLmdFileInfo Parameter Container example
};



inline  FairDbReader<R3BDBLmdFileInfo>* R3BDBLmdFileInfo::GetParamReader()
{
  if (fParam_Reader) { return fParam_Reader; }
  else {
    fParam_Reader = new  FairDbReader<R3BDBLmdFileInfo>();
    return fParam_Reader;
  }
}

inline FairDbWriter<R3BDBLmdFileInfo>* R3BDBLmdFileInfo::GetParamWriter()
{
  if (fParam_Writer) { return fParam_Writer; }
  else {
    fParam_Writer = new  FairDbWriter<R3BDBLmdFileInfo>();
    return fParam_Writer;
  }
}

#endif /* !R3BDBLMDFILEINFO_H*/
