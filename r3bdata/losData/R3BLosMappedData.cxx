
#include "R3BLosMappedData.h"
#include "FairLogger.h"
#include "TMath.h"
#define IS_NAN(x) TMath::IsNaN(x)
#include <iostream>

using namespace std;

// for the data analysis of the Los detector. 
// Introduced by Ralf, Jan 2016
// fTimeLCoarse, fTimeLFine, fTimeTCoarse, fTimeTFine added by Aleksandra, Sept 2016
// Structure changed to detector, channel, type, fineTime, coarseTime Nov 2016 (Ralf & Aleksandra)


R3BLosMappedData::R3BLosMappedData()
    : fDetector(0)     // Detector number: 1...n
    , fChannel(0)      // Channel number:  1...n (n = 4 (old LOS), 8 (new LOS))
    , fType(0)         // Type: 0 = VFTX time, 1 = TAMEX leading edge, 2 = TAMEX trailing edge
    , fTimeFine(0)     // Fine time
    , fTimeCoarse(0)   // Coarse time
{
}

R3BLosMappedData::R3BLosMappedData(	UInt_t detector,
									UInt_t channel,
									UInt_t type,
									UInt_t timeFine,
									UInt_t timeCoarse)
    : fDetector(detector)
    , fChannel(channel)
    , fType(type)
    , fTimeFine(timeFine)
    , fTimeCoarse(timeCoarse)
   
{
//LOG(INFO)
//cout<<"R3BLosMappedData: chann. "<< channel <<", type "<<type<<", fine/coarse Times: "<<timeFine<<", "
//<<timeCoarse<<endl;
/* Everything fine here!
 
 if(timeFine <= 0 || IS_NAN(timeFine) ) LOG(INFO)<<"LosMappedData FineTime wrong value for channel: "<<channel<<" type: "<<type<<" value: "<<timeFine<<FairLogger::endl;
 if(timeCoarse < 0 || IS_NAN(timeCoarse)) LOG(INFO)<<"LosMappedData CoarseTime wrong value for channel: "<<channel<<" type: "<<type<<" value: "<<timeCoarse<<FairLogger::endl;
 
*/}

ClassImp(R3BLosMappedData)
