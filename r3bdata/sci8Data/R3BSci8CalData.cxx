
#include "R3BSci8CalData.h"
#include "math.h"
#include "FairLogger.h"
/* for the data analysis of Sci8 detector.  
 */


R3BSci8CalData::R3BSci8CalData()
    : fDetector(0)
{
}

R3BSci8CalData::R3BSci8CalData(	UInt_t detector )
    : fDetector(detector)
    , fTimeV_r_ns( 0.0 / 0.0 ) // 0/0 produces a NAN. Macro NAN not available??
    , fTimeV_l_ns( 0.0 / 0.0 )
    , fTimeL_r_ns( 0.0 / 0.0 ) 
    , fTimeL_l_ns( 0.0 / 0.0 )
    , fTimeT_r_ns( 0.0 / 0.0 ) 
    , fTimeT_l_ns( 0.0 / 0.0 )
{	 
}

Double_t R3BSci8CalData::GetMeanTime()
{
	Double_t mean=0;
	Int_t num_times=0;
	if (!isnan(fTimeV_r_ns)) {mean+=fTimeV_r_ns;num_times++;}
	if (!isnan(fTimeV_l_ns)) {mean+=fTimeV_l_ns;num_times++;}
	if (!isnan(fTimeL_r_ns)) {mean+=fTimeL_r_ns;num_times++;}
	if (!isnan(fTimeL_l_ns)) {mean+=fTimeL_l_ns;num_times++;}
	if (!isnan(fTimeT_r_ns)) {mean+=fTimeT_r_ns;num_times++;}
	if (!isnan(fTimeT_l_ns)) {mean+=fTimeT_l_ns;num_times++;}	
		
	return num_times ? mean/num_times : NAN;
}
Double_t R3BSci8CalData::GetMeanTimeTAMEXL()
{
	Double_t mean=0;
	Int_t num_times=0;
	if (!isnan(fTimeL_r_ns)) {mean+=fTimeL_r_ns;num_times++;}
	if (!isnan(fTimeL_l_ns)) {mean+=fTimeL_l_ns;num_times++;}
		
	return num_times ? mean/num_times : NAN;
}

Double_t R3BSci8CalData::GetMeanTimeTAMEXT()
{
	Double_t mean=0;
	Int_t num_times=0;
	if (!isnan(fTimeT_r_ns)) {mean+=fTimeT_r_ns;num_times++;}
	if (!isnan(fTimeT_l_ns)) {mean+=fTimeT_l_ns;num_times++;}
		
	return num_times ? mean/num_times : NAN;
}


UInt_t R3BSci8CalData::GetTAMEXLNcha()
{
	Int_t num_times=0;
	if (!isnan(fTimeL_r_ns)) {num_times++;}
	if (!isnan(fTimeL_l_ns)) {num_times++;}
		
	return num_times ;
}

UInt_t R3BSci8CalData::GetTAMEXTNcha()
{
	Int_t num_times=0;
	if (!isnan(fTimeT_r_ns)) {num_times++;}
	if (!isnan(fTimeT_l_ns)) {num_times++;}
		
	return num_times ;
}


Double_t R3BSci8CalData::GetMeanTimeVFTX()
{
	Double_t mean=0;
	Int_t num_times=0;
	if (!isnan(fTimeV_r_ns)) {mean+=fTimeV_r_ns;num_times++;}
	if (!isnan(fTimeV_l_ns)) {mean+=fTimeV_l_ns;num_times++;}
		
		
	return num_times ? mean/num_times : NAN;
}
UInt_t R3BSci8CalData::GetVFTXNcha()
{
	Int_t num_times=0;
	if (!isnan(fTimeV_r_ns)) {num_times++;}
	if (!isnan(fTimeV_l_ns)) {num_times++;}
		
	return num_times ;
}
ClassImp(R3BSci8CalData)
