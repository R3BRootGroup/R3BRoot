#ifndef R3BDBLMDUNPACK_H
#define R3BDBLMDUNPACK_H

#include "FairUnpack.h"
#include "ValTimeStamp.h"               // for ValTimeStamp

#include <iostream>
using namespace std; 

class LmdHeaderInfo : public TObject{
 public:
    // Constructor

  LmdHeaderInfo();
  ~LmdHeaderInfo(){};

  void Dump();
  
  
  std::string label;
  std::string file;
  std::string time;
  std::string exp;
  std::string comment;
  Int_t length;
  Int_t type;
  Int_t subtype;
  Int_t frag;
  Int_t used;
  Int_t buf;
  Int_t evt;
  Int_t stime; 
  Int_t runid; 
  Int_t swap;

    ClassDef(LmdHeaderInfo, 0)
};



class R3BDBLmdUnpack : public FairUnpack
{
  public:
    // Constructor
    R3BDBLmdUnpack();

    // Destructor
    virtual ~R3BDBLmdUnpack();

    // Fair specific
    virtual Bool_t Init();
    virtual Bool_t DoUnpack(Int_t* data, Int_t size);
    virtual void Reset();

	// Unpack specific.
	std::vector<ValTimeStamp*> GetStartTimes() { return fStartTimes;}
	std::vector<ValTimeStamp*> GetStopTimes()  { return fStopTimes;}
	std::vector<LmdHeaderInfo*> GetHeaderInfo()  { return fHeaderInfo;}

    ValTimeStamp* GetStartTimesAt(Int_t i ) {return fStartTimes[i];}
    ValTimeStamp* GetStopTimesAt(Int_t i ) {return fStopTimes[i];}
    LmdHeaderInfo* GetHeaderInfoAt(Int_t i ) {return fHeaderInfo[i];}

    Int_t GetNStartTimes() {return fStartTimes.size();}
    Int_t GetNStopTimes()  {return fStopTimes.size();}
	Int_t GetNHeaderInfo() {return fHeaderInfo.size();}



  protected:
    virtual void Register();
    

  private: 
    Int_t fCount;  

   	std::vector<ValTimeStamp*> fStartTimes;
	std::vector<ValTimeStamp*> fStopTimes;
	std::vector<LmdHeaderInfo*> fHeaderInfo;
 
    ClassDef(R3BDBLmdUnpack, 0)
};

#endif
