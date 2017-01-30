
#include "R3BLosCalData.h"
#include "math.h"

/* for the data analysis of the Los detector. 
 * Introduced by Ralf, Feb 2016
 *
 * March 2016
 * Rewrote the Cal structure to provide individual leafs for the
 * left, top, right and bottom signals. This allows to plot
 * the time differences via cbmsim->Draw(...) interactively (aka without
 * looping over all channels) which is crucial for a quick check of the
 * detector status during the experiment.
 *  
 * There is a fifth channel "ref" available which holds the master 
 * trigger, as time reference.
 *  
 */


R3BLosCalData::R3BLosCalData()
    : fDetector(0)
{
}

R3BLosCalData::R3BLosCalData(	UChar_t detector )
    : fDetector(detector)
    , fTime_r_ns( 0.0 / 0.0 ) // 0/0 produces a NAN. Macro NAN not available??
    , fTime_t_ns( 0.0 / 0.0 )
    , fTime_l_ns( 0.0 / 0.0 )
    , fTime_b_ns( 0.0 / 0.0 )
    , fTime_ref_ns( 0.0 / 0.0 )

    , fTime_cherenkov_l_ns(0.0 / 0.0)
    , fTime_cherenkov_r_ns(0.0 / 0.0)
{
}

Double_t R3BLosCalData::GetMeanTime()
{
	Double_t mean=0;
	Int_t num_times=0;
	if (!isnan(fTime_r_ns)) {mean+=fTime_r_ns;num_times++;}
	if (!isnan(fTime_l_ns)) {mean+=fTime_l_ns;num_times++;}
	if (!isnan(fTime_t_ns)) {mean+=fTime_t_ns;num_times++;}
	if (!isnan(fTime_b_ns)) {mean+=fTime_b_ns;num_times++;}
	return num_times ? mean/num_times : NAN;
}

ClassImp(R3BLosCalData)
