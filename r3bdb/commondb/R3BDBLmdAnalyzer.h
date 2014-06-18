#ifndef R3BDBLMDANALYZER_H
#define R3BDBLMDANALYZER_H

#include "FairLmdSource.h"

#include "ValTimeStamp.h"

#include <vector>


//struct LmdHeaderInfo{
//  LmdHeaderInfo(){};
//  ~LmdHeaderInfo(){};
//  void Init(){label=0;
//  Int_t label;
//  Int_t time;  
//
//}


class R3BDBLmdAnalyzer : public FairLmdSource
{
  public:
    R3BDBLmdAnalyzer();
    R3BDBLmdAnalyzer(const R3BDBLmdAnalyzer& source);
    virtual ~R3BDBLmdAnalyzer();

    virtual Bool_t Init();
    virtual Int_t ReadEvent();
    virtual void Close();

	std::vector<ValTimeStamp*> GetStartTimes() { return fStartTimes;}
	std::vector<ValTimeStamp*> GetStopTimes()  { return fStopTimes;}
    ValTimeStamp* GetStartTimesAt(Int_t i ) {return fStartTimes[i];}
    ValTimeStamp* GetStopTimesAt(Int_t i ) {return fStopTimes[i];}
    Int_t GetNStartTimes() {return fStartTimes.size();}
    Int_t GetNStopTimes() {return fStopTimes.size();}
    
    void PrintHeaderInfo();


  protected:
    Int_t fNEvent;
	Int_t fCurrentEvent;
	std::vector<ValTimeStamp*> fStartTimes;
	std::vector<ValTimeStamp*> fStopTimes;

  public:
    ClassDef(R3BDBLmdAnalyzer, 0)
};


#endif
