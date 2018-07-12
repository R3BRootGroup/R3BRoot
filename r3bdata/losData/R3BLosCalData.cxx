
#include "R3BLosCalData.h"
#include "math.h"
#include "FairLogger.h"
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
 * September 2016
 * TAMEX readout of MCFD amplitudes added by Aleksandra.
 * 
 * November 2016
 * Data structure changed and adapted to new LOS detector (with 8 PMs)  
 * 
 * There is a fifth channel "ref" available which holds the master 
 * trigger, as time reference. Not used (since Sept. 2016). 
 *  
 */


R3BLosCalData::R3BLosCalData()
    : fDetector(0)
{
}
/*
R3BLosCalData::R3BLosCalData()
    : fVFTXNcha(0)
{
}
R3BLosCalData::R3BLosCalData()
    : fVFTXNcha(0)
{
}
*/
R3BLosCalData::R3BLosCalData(	UInt_t detector )
    : fDetector(detector)
    , fTimeV_r_ns( 0.0 / 0.0 ) // 0/0 produces a NAN. Macro NAN not available??
    , fTimeV_t_ns( 0.0 / 0.0 )
    , fTimeV_l_ns( 0.0 / 0.0 )
    , fTimeV_b_ns( 0.0 / 0.0 )
    , fTimeL_r_ns( 0.0 / 0.0 ) 
    , fTimeL_t_ns( 0.0 / 0.0 )
    , fTimeL_l_ns( 0.0 / 0.0 )
    , fTimeL_b_ns( 0.0 / 0.0 )    
    , fTimeT_r_ns( 0.0 / 0.0 ) 
    , fTimeT_t_ns( 0.0 / 0.0 )
    , fTimeT_l_ns( 0.0 / 0.0 )
    , fTimeT_b_ns( 0.0 / 0.0 )    
    , fTimeV_rt_ns( 0.0 / 0.0 )
    , fTimeV_rb_ns( 0.0 / 0.0 )
    , fTimeV_lt_ns( 0.0 / 0.0 )
    , fTimeV_lb_ns( 0.0 / 0.0 )
    , fTimeL_rt_ns( 0.0 / 0.0 )
    , fTimeL_rb_ns( 0.0 / 0.0 )
    , fTimeL_lt_ns( 0.0 / 0.0 )
    , fTimeL_lb_ns( 0.0 / 0.0 )
    , fTimeT_rt_ns( 0.0 / 0.0 )
    , fTimeT_rb_ns( 0.0 / 0.0 )
    , fTimeT_lt_ns( 0.0 / 0.0 )
    , fTimeT_lb_ns( 0.0 / 0.0 ) 
    , fTimeV_ref_ns( 0.0 / 0.0 ) 
    , fTimeL_ref_ns( 0.0 / 0.0 ) 
    , fTimeT_ref_ns( 0.0 / 0.0 )
    , fTimeV_pix_ns( 0.0 / 0.0 ) // Signal from Pixel detector
    , fTimeL_pix_ns( 0.0 / 0.0 ) 
    , fTimeT_pix_ns( 0.0 / 0.0 )
{	 
}

Double_t R3BLosCalData::GetMeanTime()
{
	Double_t mean=0;
	Int_t num_times=0;
	if (!isnan(fTimeV_r_ns)) {mean+=fTimeV_r_ns;num_times++;}
	if (!isnan(fTimeV_l_ns)) {mean+=fTimeV_l_ns;num_times++;}
	if (!isnan(fTimeV_t_ns)) {mean+=fTimeV_t_ns;num_times++;}
	if (!isnan(fTimeV_b_ns)) {mean+=fTimeV_b_ns;num_times++;}
	if (!isnan(fTimeL_r_ns)) {mean+=fTimeL_r_ns;num_times++;}
	if (!isnan(fTimeL_l_ns)) {mean+=fTimeL_l_ns;num_times++;}
	if (!isnan(fTimeL_t_ns)) {mean+=fTimeL_t_ns;num_times++;}
	if (!isnan(fTimeL_b_ns)) {mean+=fTimeL_b_ns;num_times++;}	
	if (!isnan(fTimeT_r_ns)) {mean+=fTimeT_r_ns;num_times++;}
	if (!isnan(fTimeT_l_ns)) {mean+=fTimeT_l_ns;num_times++;}
	if (!isnan(fTimeT_t_ns)) {mean+=fTimeT_t_ns;num_times++;}
	if (!isnan(fTimeT_b_ns)) {mean+=fTimeT_b_ns;num_times++;}	
	if (!isnan(fTimeV_rt_ns)) {mean+=fTimeV_rt_ns;num_times++;}
	if (!isnan(fTimeV_rb_ns)) {mean+=fTimeV_rb_ns;num_times++;}
	if (!isnan(fTimeV_lt_ns)) {mean+=fTimeV_lt_ns;num_times++;}
	if (!isnan(fTimeV_lb_ns)) {mean+=fTimeV_lb_ns;num_times++;}
	if (!isnan(fTimeL_rt_ns)) {mean+=fTimeL_rt_ns;num_times++;}
	if (!isnan(fTimeL_rb_ns)) {mean+=fTimeL_rb_ns;num_times++;}
	if (!isnan(fTimeL_lt_ns)) {mean+=fTimeL_lt_ns;num_times++;}
	if (!isnan(fTimeL_lb_ns)) {mean+=fTimeL_lb_ns;num_times++;}	
	if (!isnan(fTimeT_rt_ns)) {mean+=fTimeT_rt_ns;num_times++;}
	if (!isnan(fTimeT_rb_ns)) {mean+=fTimeT_rb_ns;num_times++;}
	if (!isnan(fTimeT_lt_ns)) {mean+=fTimeT_lt_ns;num_times++;}
	if (!isnan(fTimeT_lb_ns)) {mean+=fTimeT_lb_ns;num_times++;}	
	if (!isnan(fTimeV_ref_ns)) {mean+=fTimeV_ref_ns;num_times++;}
	if (!isnan(fTimeL_ref_ns)) {mean+=fTimeL_ref_ns;num_times++;}
	if (!isnan(fTimeT_ref_ns)) {mean+=fTimeT_ref_ns;num_times++;}
	
		
	return num_times ? mean/num_times : NAN;
}
Double_t R3BLosCalData::GetMeanTimeTAMEXL()
{
	Double_t mean=0;
	Int_t num_times=0;
	if (!isnan(fTimeL_r_ns)) {mean+=fTimeL_r_ns;num_times++;}
	if (!isnan(fTimeL_l_ns)) {mean+=fTimeL_l_ns;num_times++;}
	if (!isnan(fTimeL_t_ns)) {mean+=fTimeL_t_ns;num_times++;}
	if (!isnan(fTimeL_b_ns)) {mean+=fTimeL_b_ns;num_times++;}	
	if (!isnan(fTimeL_rt_ns)) {mean+=fTimeL_rt_ns;num_times++;}
	if (!isnan(fTimeL_rb_ns)) {mean+=fTimeL_rb_ns;num_times++;}
	if (!isnan(fTimeL_lt_ns)) {mean+=fTimeL_lt_ns;num_times++;}
	if (!isnan(fTimeL_lb_ns)) {mean+=fTimeL_lb_ns;num_times++;}	
		
	return num_times ? mean/num_times : NAN;
}

Double_t R3BLosCalData::GetMeanTimeTAMEXT()
{
	Double_t mean=0;
	Int_t num_times=0;
	if (!isnan(fTimeT_r_ns)) {mean+=fTimeT_r_ns;num_times++;}
	if (!isnan(fTimeT_l_ns)) {mean+=fTimeT_l_ns;num_times++;}
	if (!isnan(fTimeT_t_ns)) {mean+=fTimeT_t_ns;num_times++;}
	if (!isnan(fTimeT_b_ns)) {mean+=fTimeT_b_ns;num_times++;}	
	if (!isnan(fTimeT_rt_ns)) {mean+=fTimeT_rt_ns;num_times++;}
	if (!isnan(fTimeT_rb_ns)) {mean+=fTimeT_rb_ns;num_times++;}
	if (!isnan(fTimeT_lt_ns)) {mean+=fTimeT_lt_ns;num_times++;}
	if (!isnan(fTimeT_lb_ns)) {mean+=fTimeT_lb_ns;num_times++;}	

		
	return num_times ? mean/num_times : NAN;
}


UInt_t R3BLosCalData::GetTAMEXLNcha()
{
	Int_t num_times=0;
	if (!isnan(fTimeL_r_ns)) {num_times++;}
	if (!isnan(fTimeL_l_ns)) {num_times++;}
	if (!isnan(fTimeL_t_ns)) {num_times++;}
	if (!isnan(fTimeL_b_ns)) {num_times++;}	
	if (!isnan(fTimeL_rt_ns)) {num_times++;}
	if (!isnan(fTimeL_rb_ns)) {num_times++;}
	if (!isnan(fTimeL_lt_ns)) {num_times++;}
	if (!isnan(fTimeL_lb_ns)) {num_times++;}	
		
	return num_times ;
}

UInt_t R3BLosCalData::GetTAMEXTNcha()
{
	Int_t num_times=0;
	if (!isnan(fTimeT_r_ns)) {num_times++;}
	if (!isnan(fTimeT_l_ns)) {num_times++;}
	if (!isnan(fTimeT_t_ns)) {num_times++;}
	if (!isnan(fTimeT_b_ns)) {num_times++;}	
	if (!isnan(fTimeT_rt_ns)) {num_times++;}
	if (!isnan(fTimeT_rb_ns)) {num_times++;}
	if (!isnan(fTimeT_lt_ns)) {num_times++;}
	if (!isnan(fTimeT_lb_ns)) {num_times++;}	

		
	return num_times ;
}


Double_t R3BLosCalData::GetMeanTimeVFTX()
{
	Double_t mean=0;
	Int_t num_times=0;
	if (!isnan(fTimeV_r_ns)) {mean+=fTimeV_r_ns;num_times++;}
	if (!isnan(fTimeV_l_ns)) {mean+=fTimeV_l_ns;num_times++;}
	if (!isnan(fTimeV_t_ns)) {mean+=fTimeV_t_ns;num_times++;}
	if (!isnan(fTimeV_b_ns)) {mean+=fTimeV_b_ns;num_times++;}
	if (!isnan(fTimeV_rt_ns)) {mean+=fTimeV_rt_ns;num_times++;}
	if (!isnan(fTimeV_rb_ns)) {mean+=fTimeV_rb_ns;num_times++;}
	if (!isnan(fTimeV_lt_ns)) {mean+=fTimeV_lt_ns;num_times++;}
	if (!isnan(fTimeV_lb_ns)) {mean+=fTimeV_lb_ns;num_times++;}
		
		
	return num_times ? mean/num_times : NAN;
}
UInt_t R3BLosCalData::GetVFTXNcha()
{
	Int_t num_times=0;
	if (!isnan(fTimeV_r_ns)) {num_times++;}
	if (!isnan(fTimeV_l_ns)) {num_times++;}
	if (!isnan(fTimeV_t_ns)) {num_times++;}
	if (!isnan(fTimeV_b_ns)) {num_times++;}
	if (!isnan(fTimeV_rt_ns)) {num_times++;}
	if (!isnan(fTimeV_rb_ns)) {num_times++;}
	if (!isnan(fTimeV_lt_ns)) {num_times++;}
	if (!isnan(fTimeV_lb_ns)) {num_times++;}
		
		
	return num_times ;
}
ClassImp(R3BLosCalData)
