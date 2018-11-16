
#include "R3BBeamMonitorMappedData.h"
#include "FairLogger.h"
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
#include <iostream>

using namespace std;


R3BBeamMonitorMappedData::R3BBeamMonitorMappedData()
    : fIC(-1)           // IC
    , fSEETRAM(-1)      // SEETRAM
    , fTOFDOR(-1)       // TOFD or
{
}

R3BBeamMonitorMappedData::R3BBeamMonitorMappedData(	UInt_t IC,
									UInt_t SEETRAM,
									UInt_t TOFDOR)
    : fIC(IC)
    , fSEETRAM(SEETRAM)
    , fTOFDOR(TOFDOR)
{
/* 
 LOG(INFO)<<"LosBeamMonitorData: "<<IC<<",  "<<SEETRAM<<", "<<TOFDOR<<FairLogger::endl;
 
*/	 
}    
    
UInt_t R3BBeamMonitorMappedData::GetIC() const
{
  return fIC;
}

UInt_t R3BBeamMonitorMappedData::GetSEETRAM() const
{
  return fSEETRAM;
}

UInt_t R3BBeamMonitorMappedData::GetTOFDOR() const
{
  return fTOFDOR;
}

ClassImp(R3BBeamMonitorMappedData)
