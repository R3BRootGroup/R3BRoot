// ------------------------------------------------------------
// -----                  R3BLosCal2Hit                   -----
// -----          Created Mar 10th 2016 by R.Plag         -----
// ------------------------------------------------------------

#include "R3BLosCal2Hit.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"
#include "R3BLosMapped2Cal.h"
#include "R3BLosMappedData.h"
#include "R3BTCalPar.h"
#include "R3BTCalEngine.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include <fstream>
#include <iostream>
#include <iomanip>
using namespace std;
#define IS_NAN(x) TMath::IsNaN(x)

R3BLosCal2Hit::R3BLosCal2Hit()
    : FairTask("LosCal2Hit", 1)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BLosHitData"))
    , fNofHitItems(0)
    , flosVeffX1(1.)
    , flosVeffY1(1.)    
    , flosOffsetX1(0.)
    , flosOffsetY1(0.)
    , flosVeffX2(1.)
    , flosVeffY2(1.)    
    , flosOffsetX2(0.)
    , flosOffsetY2(0.)    
    , flosVeffX1T(1.)
    , flosVeffY1T(1.)    
    , flosOffsetX1T(0.)
    , flosOffsetY1T(0.)
    , flosVeffX2T(1.)
    , flosVeffY2T(1.)    
    , flosOffsetX2T(0.)
    , flosOffsetY2T(0.)
    , flosVeffX1wc(1.)
    , flosVeffY1wc(1.)    
    , flosOffsetX1wc(0.)
    , flosOffsetY1wc(0.)
    , flosVeffX2wc(1.)
    , flosVeffY2wc(1.)    
    , flosOffsetX2wc(0.)
    , flosOffsetY2wc(0.) 
    , flosVeffX1Q(1.)
    , flosVeffY1Q(1.)    
    , flosOffsetX1Q(0.)
    , flosOffsetY1Q(0.)
    , flosVeffX2Q(1.)
    , flosVeffY2Q(1.)    
    , flosOffsetX2Q(0.)
    , flosOffsetY2Q(0.) 
    , flosVeffX1Qc(1.)
    , flosVeffY1Qc(1.)    
    , flosOffsetX1Qc(0.)
    , flosOffsetY1Qc(0.)
    , flosVeffX2Qc(1.)
    , flosVeffY2Qc(1.)    
    , flosOffsetX2Qc(0.)
    , flosOffsetY2Qc(0.)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{
	fhTres_M = NULL;
	fhTres_T = NULL;	
	fhTres_T_corr = NULL;	
	fhTres_M_corr = NULL;
	fhTres_MT_corr = NULL;	
	fhTres_M_evCh = NULL;
	fhTres_T_evCh = NULL;	
	fhTres_M_oddCh = NULL;
	fhTres_T_oddCh = NULL;
	fhTres_M_evCh_corr = NULL;
	fhTres_T_evCh_corr= NULL;	
	fhTres_M_oddCh_corr = NULL;
	fhTres_T_oddCh_corr = NULL;
	fhQ_L = NULL;
	fhQ_B = NULL;
	fhQ_R = NULL;
	fhQ_T = NULL;	
	fhQ_L_corr = NULL;
	fhQ_B_corr = NULL;
	fhQ_R_corr = NULL;
	fhQ_T_corr = NULL;
	fhQ_LT = NULL;
	fhQ_LB = NULL;
	fhQ_RB = NULL;
	fhQ_RT = NULL;		
	fhQ_LT_corr = NULL;
	fhQ_LB_corr = NULL;
	fhQ_RB_corr = NULL;
	fhQ_RT_corr = NULL;
	fhQ = NULL;
	fhQ_oddCh = NULL;
	fhQ_evCh = NULL;
	fhQ_oddCh_corr = NULL;
	fhQ_evCh_corr = NULL;
	fhQ_vs_X = NULL;
	fhQ_vs_Y = NULL;	
	fhQ_vs_X_corr = NULL;
	fhQ_vs_Y_corr = NULL;
	fhTM_vs_Q = NULL;
	fhTT_vs_Q = NULL;
	fhTM_vs_Q_corr = NULL;
	fhTT_vs_Q_corr = NULL;
	fhXY = NULL;	
	fhXYmean = NULL;	
	fhXY_corr = NULL;	
	fhXY_ToT = NULL;	
	fhXY_ToT_corr = NULL;
	fhXY1_ToT_corr = NULL;
	fhXY2_ToT_corr = NULL;
	fhXY1_corr = NULL;
	fhXY2_corr = NULL;
	fhXYproj = NULL;
	fhXY1 = NULL;
	fhXY2 = NULL;	
	fhXYT = NULL;
	fhXYT1 = NULL;
	fhXYT2 = NULL;	
	fhXYQ1 = NULL;
	fhXYQ2 = NULL;
	fhQ1_vs_Q5 = NULL;	
	fhQ1_vs_Q5_corr = NULL;
	fhQ2_vs_Q6 = NULL;	
	fhQ2_vs_Q6_corr = NULL;
	fhQ3_vs_Q7 = NULL;
	fhQ3_vs_Q7_corr = NULL;
	fhQ4_vs_Q8 = NULL;	
	fhQ4_vs_Q8_corr = NULL;
	fhQtest = NULL;
	fhTresX_M = NULL;
	fhTresY_M = NULL;
	fhTresX_M_corr= NULL;
	fhTresY_M_corr = NULL;	
	fhTresX_T = NULL;
	fhTresY_T = NULL;
	fhTresX_T_corr = NULL;
	fhTresY_T_corr = NULL;	
	fht1Q1M = NULL;			
	fht3Q3M = NULL;			
	fht5Q5M = NULL;			
	fht7Q7M = NULL;
	fht8Q8M = NULL;			
	fht2Q2M = NULL;			
	fht4Q4M = NULL;			
	fht6Q6M = NULL;
	fht1Q1T = NULL;			
	fht3Q3T = NULL;			
	fht5Q5T = NULL;			
	fht7Q7T = NULL;
	fht8Q8T = NULL;			
	fht2Q2T = NULL;			
	fht4Q4T = NULL;			
	fht6Q6T = NULL;		
	fht1Q1M_corr = NULL;			
	fht3Q3M_corr = NULL;			
	fht5Q5M_corr = NULL;			
	fht7Q7M_corr = NULL;
	fht8Q8M_corr = NULL;			
	fht2Q2M_corr = NULL;			
	fht4Q4M_corr = NULL;			
	fht6Q6M_corr = NULL;
	fht1Q1T_corr = NULL;			
	fht3Q3T_corr = NULL;			
	fht5Q5T_corr = NULL;			
	fht7Q7T_corr = NULL;
	fht8Q8T_corr = NULL;			
	fht2Q2T_corr = NULL;			
	fht4Q4T_corr = NULL;			
	fht6Q6T_corr = NULL;		
	fhQ1vsIcount = NULL;	
	fhQ3vsIcount = NULL;		
	fhQ5vsIcount = NULL;		
	fhQ7vsIcount = NULL;	
	fhQ2vsIcount = NULL;	
	fhQ4vsIcount = NULL;		
	fhQ6vsIcount = NULL;		
	fhQ8vsIcount = NULL;
	fhTresMvsIcount = NULL;	
	fhTresTvsIcount = NULL;	
	fhTreswcMvsIcount = NULL;	
	fhTreswcTvsIcount = NULL;	
	fhRW = NULL;
	fht1t5M = NULL;
	fht2t6M = NULL;	
	fht3t7M = NULL;	
	fht4t8M = NULL;	
	fht1t5M_corr = NULL;
	fht2t6M_corr = NULL;	
	fht3t7M_corr = NULL;	
	fht4t8M_corr = NULL;	
	fht1t5T = NULL;
	fht2t6T = NULL;	
	fht3t7T = NULL;	
	fht4t8T = NULL;	
	fht1t5T_corr = NULL;
	fht2t6T_corr = NULL;	
	fht3t7T_corr = NULL;	
	fht4t8T_corr = NULL;
	fhQpix = NULL;	
				
}

R3BLosCal2Hit::R3BLosCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BLosHitData"))
    , fNofHitItems(0)
    , flosVeffX1(1.)
    , flosVeffY1(1.)    
    , flosOffsetX1(0.)
    , flosOffsetY1(0.)
    , flosVeffX2(1.)
    , flosVeffY2(1.)    
    , flosOffsetX2(0.)
    , flosOffsetY2(0.)
    , flosVeffX1T(1.)
    , flosVeffY1T(1.)    
    , flosOffsetX1T(0.)
    , flosOffsetY1T(0.)
    , flosVeffX2T(1.)
    , flosVeffY2T(1.)    
    , flosOffsetX2T(0.)
    , flosOffsetY2T(0.)
    , flosVeffX1wc(1.)
    , flosVeffY1wc(1.)    
    , flosOffsetX1wc(0.)
    , flosOffsetY1wc(0.)
    , flosVeffX2wc(1.)
    , flosVeffY2wc(1.)    
    , flosOffsetX2wc(0.)
    , flosOffsetY2wc(0.) 
    , flosVeffX1Q(1.)
    , flosVeffY1Q(1.)    
    , flosOffsetX1Q(0.)
    , flosOffsetY1Q(0.)
    , flosVeffX2Q(1.)
    , flosVeffY2Q(1.)    
    , flosOffsetX2Q(0.)
    , flosOffsetY2Q(0.)
    , flosVeffX1Qc(1.)
    , flosVeffY1Qc(1.)    
    , flosOffsetX1Qc(0.)
    , flosOffsetY1Qc(0.)
    , flosVeffX2Qc(1.)
    , flosVeffY2Qc(1.)    
    , flosOffsetX2Qc(0.)
    , flosOffsetY2Qc(0.) 
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
{	
	fhTres_M = NULL;
	fhTres_T = NULL;	
	fhTres_T_corr = NULL;	
	fhTres_M_corr = NULL;
	fhTres_MT_corr = NULL;	
	fhTres_M_evCh = NULL;
	fhTres_T_evCh = NULL;	
	fhTres_M_oddCh = NULL;
	fhTres_T_oddCh = NULL;
	fhTres_M_evCh_corr = NULL;
	fhTres_T_evCh_corr= NULL;	
	fhTres_M_oddCh_corr = NULL;
	fhTres_T_oddCh_corr = NULL;
	fhQ_L = NULL;
	fhQ_B = NULL;
	fhQ_R = NULL;
	fhQ_T = NULL;	
	fhQ_L_corr = NULL;
	fhQ_B_corr = NULL;
	fhQ_R_corr = NULL;
	fhQ_T_corr = NULL;
	fhQ_LT = NULL;
	fhQ_LB = NULL;
	fhQ_RB = NULL;
	fhQ_RT = NULL;		
	fhQ_LT_corr = NULL;
	fhQ_LB_corr = NULL;
	fhQ_RB_corr = NULL;
	fhQ_RT_corr = NULL;
	fhQ = NULL;
	fhQ_oddCh = NULL;
	fhQ_evCh = NULL;
	fhQ_oddCh_corr = NULL;
	fhQ_evCh_corr = NULL;
	fhQ_vs_X = NULL;
	fhQ_vs_Y = NULL;	
	fhQ_vs_X_corr = NULL;
	fhQ_vs_Y_corr = NULL;
	fhTM_vs_Q = NULL;
	fhTT_vs_Q = NULL;	
	fhTM_vs_Q_corr = NULL;
	fhTT_vs_Q_corr = NULL;
	fhXY = NULL;	
	fhXYmean = NULL;	
	fhXY_corr = NULL;	
	fhXY_ToT = NULL;	
	fhXY_ToT_corr = NULL;
	fhXY1_ToT_corr = NULL;
	fhXY2_ToT_corr = NULL;
	fhXY1_corr = NULL;
	fhXY2_corr = NULL;
	fhXYproj = NULL;
	fhXY1 = NULL;
	fhXY2 = NULL;	
	fhXYT = NULL;
	fhXYT1 = NULL;
	fhXYT2 = NULL;	
	fhXYQ1 = NULL;
	fhXYQ2 = NULL;
	fhQ1_vs_Q5 = NULL;	
	fhQ1_vs_Q5_corr = NULL;
	fhQ2_vs_Q6 = NULL;	
	fhQ2_vs_Q6_corr = NULL;
	fhQ3_vs_Q7 = NULL;
	fhQ3_vs_Q7_corr = NULL;
	fhQ4_vs_Q8 = NULL;	
	fhQ4_vs_Q8_corr = NULL;
	fhQtest = NULL;
	fhTresX_M = NULL;
	fhTresY_M = NULL;
	fhTresX_M_corr= NULL;
	fhTresY_M_corr = NULL;	
	fhTresX_T = NULL;
	fhTresY_T = NULL;
	fhTresX_T_corr = NULL;
	fhTresY_T_corr = NULL;	
	fht1Q1M = NULL;			
	fht3Q3M = NULL;			
	fht5Q5M = NULL;			
	fht7Q7M = NULL;
	fht8Q8M = NULL;			
	fht2Q2M = NULL;			
	fht4Q4M = NULL;			
	fht6Q6M = NULL;
	fht1Q1T = NULL;			
	fht3Q3T = NULL;			
	fht5Q5T = NULL;			
	fht7Q7T = NULL;
	fht8Q8T = NULL;			
	fht2Q2T = NULL;			
	fht4Q4T = NULL;			
	fht6Q6T = NULL;		
	fht1Q1M_corr = NULL;			
	fht3Q3M_corr = NULL;			
	fht5Q5M_corr = NULL;			
	fht7Q7M_corr = NULL;
	fht8Q8M_corr = NULL;			
	fht2Q2M_corr = NULL;			
	fht4Q4M_corr = NULL;			
	fht6Q6M_corr = NULL;
	fht1Q1T_corr = NULL;			
	fht3Q3T_corr = NULL;			
	fht5Q5T_corr = NULL;			
	fht7Q7T_corr = NULL;
	fht8Q8T_corr = NULL;			
	fht2Q2T_corr = NULL;			
	fht4Q4T_corr = NULL;			
	fht6Q6T_corr = NULL;		
	fhQ1vsIcount = NULL;	
	fhQ3vsIcount = NULL;		
	fhQ5vsIcount = NULL;		
	fhQ7vsIcount = NULL;	
	fhQ2vsIcount = NULL;	
	fhQ4vsIcount = NULL;		
	fhQ6vsIcount = NULL;		
	fhQ8vsIcount = NULL;
	fhTresMvsIcount = NULL;	
	fhTresTvsIcount = NULL;	
	fhTreswcMvsIcount = NULL;	
	fhTreswcTvsIcount = NULL;	
	fhRW = NULL;
	fht1t5M = NULL;
	fht2t6M = NULL;	
	fht3t7M = NULL;	
	fht4t8M = NULL;	
	fht1t5M_corr = NULL;
	fht2t6M_corr = NULL;	
	fht3t7M_corr = NULL;	
	fht4t8M_corr = NULL;	
	fht1t5T = NULL;
	fht2t6T = NULL;	
	fht3t7T = NULL;	
	fht4t8T = NULL;	
	fht1t5T_corr = NULL;
	fht2t6T_corr = NULL;	
	fht3t7T_corr = NULL;	
	fht4t8T_corr = NULL;
	fhQpix = NULL;	
}

R3BLosCal2Hit::~R3BLosCal2Hit()
{	
	if(fhTres_M) delete(fhTres_M) ;
	if(fhTres_T) delete(fhTres_T) ;	
	if(fhTres_T_corr) delete(fhTres_T_corr) ;	
	if(fhTres_M_corr) delete(fhTres_M_corr) ;
	if(fhTres_MT_corr) delete(fhTres_MT_corr);	
	if(fhTres_M_evCh) delete(fhTres_M_evCh) ;
	if(fhTres_T_evCh) delete(fhTres_T_evCh) ;	
	if(fhTres_T_evCh_corr) delete(fhTres_T_evCh_corr) ;	
	if(fhTres_M_evCh_corr) delete(fhTres_M_evCh_corr) ;
	if(fhTres_M_oddCh) delete(fhTres_M_oddCh) ;
	if(fhTres_T_oddCh) delete(fhTres_T_oddCh) ;	
	if(fhTres_T_oddCh_corr) delete(fhTres_T_oddCh_corr) ;	
	if(fhTres_M_oddCh_corr) delete(fhTres_M_oddCh_corr) ;
	if(fhQ_L) delete(fhQ_L) ;
	if(fhQ_B) delete(fhQ_B) ;
	if(fhQ_R) delete(fhQ_R) ;
	if(fhQ_T) delete(fhQ_T) ;	
	if(fhQ_L_corr) delete(fhQ_L_corr) ;
	if(fhQ_B_corr) delete(fhQ_B_corr) ;
	if(fhQ_R_corr) delete(fhQ_R_corr) ;
	if(fhQ_T_corr ) delete(fhQ_T_corr );
	if(fhQ_LT) delete(fhQ_LT) ;
	if(fhQ_LB) delete(fhQ_LB) ;
	if(fhQ_RB) delete(fhQ_RB) ;
	if(fhQ_RT) delete(fhQ_RT) ;		
	if(fhQ_LT_corr) delete(fhQ_LT_corr) ;
	if(fhQ_LB_corr) delete( fhQ_LB_corr);
	if(fhQ_RB_corr) delete(fhQ_RB_corr) ;
	if(fhQ_RT_corr) delete( fhQ_RT_corr);
	if(fhQ) delete(fhQ) ;
	if(fhQ_evCh) delete(fhQ_evCh);
    if(fhQ_oddCh) delete(fhQ_oddCh);
    if(fhQ_evCh_corr) delete(fhQ_evCh_corr);
    if(fhQ_oddCh_corr) delete(fhQ_oddCh_corr);
    if(fhQtest) delete(fhQtest) ;
	if(fhQ_vs_X) delete(fhQ_vs_X) ;
	if(fhQ_vs_Y) delete( fhQ_vs_Y);	
	if(fhQ_vs_X_corr) delete(fhQ_vs_X_corr) ;
	if(fhQ_vs_Y_corr) delete(fhQ_vs_Y_corr) ;
	if(fhTM_vs_Q) delete(fhTM_vs_Q) ;
	if(fhTT_vs_Q) delete(fhTT_vs_Q) ;
	if(fhTM_vs_Q_corr) delete(fhTM_vs_Q_corr) ;
	if(fhTT_vs_Q_corr) delete(fhTT_vs_Q_corr) ;	
	if(fhXY) delete(fhXY) ;	
	if(fhXYmean) delete(fhXYmean) ;	
	if(fhXY_corr) delete(fhXY_corr) ;	
	if(fhXY_ToT) delete(fhXY_ToT);	
	if(fhXY_ToT_corr) delete(fhXY_ToT_corr) ;
	if(fhXY1_ToT_corr) delete(fhXY1_ToT_corr) ;
	if(fhXY2_ToT_corr) delete(fhXY2_ToT_corr) ;
	if(fhXY1_corr) delete(fhXY1_corr) ;
	if(fhXY2_corr) delete(fhXY2_corr) ;
	if(fhXYproj) delete(fhXYproj) ;
	if(fhXY1 ) delete(fhXY1 );
	if(fhXY2) delete(fhXY2) ;	
	if(fhXYT ) delete(fhXYT );
	if(fhXYT1) delete(fhXYT1) ;
	if(fhXYT2) delete(fhXYT2) ;	
	if(fhXYQ1) delete(fhXYQ1) ;
	if(fhXYQ2) delete(fhXYQ2) ;
	if(fhQ1_vs_Q5) delete(fhQ1_vs_Q5) ;	
	if(fhQ1_vs_Q5_corr) delete(fhQ1_vs_Q5_corr) ;
	if(fhQ2_vs_Q6) delete(fhQ2_vs_Q6) ;	
	if(fhQ2_vs_Q6_corr) delete(fhQ2_vs_Q6_corr) ;
	if(fhQ3_vs_Q7) delete(fhQ3_vs_Q7) ;
	if(fhQ3_vs_Q7_corr) delete(fhQ3_vs_Q7_corr) ;
	if(fhQ4_vs_Q8) delete(fhQ4_vs_Q8) ;	
	if(fhQ4_vs_Q8_corr) delete(fhQ4_vs_Q8_corr) ;
	if(fhTresX_M) delete(fhTresX_M) ;
	if(fhTresY_M) delete(fhTresY_M) ;
	if(fhTresX_M_corr) delete(fhTresX_M_corr) ;
	if(fhTresY_M_corr) delete(fhTresY_M_corr) ;	
	if(fhTresX_T) delete(fhTresX_T) ;
	if(fhTresY_T) delete(fhTresY_T) ;
	if(fhTresX_T_corr) delete(fhTresX_T_corr) ;
	if(fhTresY_T_corr) delete(fhTresY_T_corr) ;	
	if(fht1Q1M) delete(fht1Q1M) ;			
	if(fht3Q3M) delete(fht3Q3M) ;			
	if(fht5Q5M) delete(fht5Q5M) ;			
	if(fht7Q7M) delete(fht7Q7M) ;
	if(fht8Q8M) delete(fht8Q8M) ;			
	if(fht2Q2M) delete(fht2Q2M) ;			
	if(fht4Q4M) delete(fht4Q4M) ;			
	if(fht6Q6M) delete(fht6Q6M) ;
	if(fht1Q1T) delete(fht1Q1T) ;			
	if(fht3Q3T) delete(fht3Q3T) ;			
	if(fht5Q5T) delete(fht5Q5T) ;			
	if(fht7Q7T) delete(fht7Q7T);
	if(fht8Q8T) delete(fht8Q8T) ;			
	if(fht2Q2T) delete(fht2Q2T) ;			
	if(fht4Q4T) delete(fht4Q4T) ;			
	if(fht6Q6T) delete(fht6Q6T) ;		
	if(fht1Q1M_corr) delete(fht1Q1M_corr) ;			
	if(fht3Q3M_corr) delete(fht3Q3M_corr) ;			
	if(fht5Q5M_corr) delete(fht5Q5M_corr) ;			
	if(fht7Q7M_corr) delete(fht7Q7M_corr) ;
	if(fht8Q8M_corr) delete(fht8Q8M_corr) ;			
	if(fht2Q2M_corr) delete(fht2Q2M_corr) ;			
	if(fht4Q4M_corr) delete(fht4Q4M_corr);			
	if(fht6Q6M_corr) delete(fht6Q6M_corr) ;
	if(fht1Q1T_corr) delete(fht1Q1T_corr) ;			
	if(fht3Q3T_corr) delete(fht3Q3T_corr) ;			
	if(fht5Q5T_corr) delete(fht5Q5T_corr) ;			
	if(fht7Q7T_corr) delete(fht7Q7T_corr) ;
	if(fht8Q8T_corr) delete(fht8Q8T_corr);			
	if(fht2Q2T_corr) delete(fht2Q2T_corr) ;			
	if(fht4Q4T_corr) delete(fht4Q4T_corr) ;			
	if(fht6Q6T_corr) delete(fht6Q6T_corr) ;		
	if(fhQ1vsIcount) delete(fhQ1vsIcount) ;	
	if(fhQ3vsIcount) delete(fhQ3vsIcount) ;		
	if(fhQ5vsIcount) delete(fhQ5vsIcount) ;		
	if(fhQ7vsIcount) delete(fhQ7vsIcount) ;	
	if(fhQ2vsIcount) delete(fhQ2vsIcount) ;	
	if(fhQ4vsIcount) delete(fhQ4vsIcount);		
	if(fhQ6vsIcount) delete(fhQ6vsIcount) ;		
	if(fhQ8vsIcount) delete(fhQ8vsIcount) ;
	if(fhTresMvsIcount) delete(fhTresMvsIcount) ;	
	if(fhTresTvsIcount) delete(fhTresTvsIcount) ;	
	if(fhTreswcMvsIcount) delete(fhTreswcMvsIcount) ;	
	if(fhTreswcTvsIcount) delete(fhTreswcTvsIcount) ;		
	if(fhRW) delete(fhRW);
	if(fht1t5M) delete(fht1t5M);
	if(fht2t6M) delete(fht2t6M);	
	if(fht3t7M) delete(fht3t7M);	
	if(fht4t8M) delete(fht4t8M);	
	if(fht1t5M_corr) delete(fht1t5M_corr);
	if(fht2t6M_corr) delete(fht2t6M_corr);	
	if(fht3t7M_corr) delete(fht3t7M_corr);	
	if(fht4t8M_corr) delete(fht4t8M_corr);	
	if(fht1t5T) delete(fht1t5T);
	if(fht2t6T) delete(fht2t6T);	
	if(fht3t7T) delete(fht3t7T);	
	if(fht4t8T) delete(fht4t8T);	
	if(fht1t5T_corr) delete(fht1t5T_corr);
	if(fht2t6T_corr) delete(fht2t6T_corr);	
	if(fht3t7T_corr) delete(fht3t7T_corr);	
	if(fht4t8T_corr) delete(fht4t8T_corr);
	if(fhQpix) delete(fhQpix);	
	
    if (fHitItems)
    {
        delete fHitItems;
        fHitItems = NULL;
    }
}

InitStatus R3BLosCal2Hit::Init()
{
	// get access to Cal data
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        
    fCalItems = (TClonesArray*)mgr->GetObject("LosCal");
    if (NULL == fCalItems)
        FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "Branch LosCal not found");

	// request storage of Hit data in output tree
    mgr->Register("LosHit", "Land", fHitItems, kTRUE);
    
    Icount = 0;
    
 // file with walk-correction parameters  
   ifstream infile(fwalk_param_file.c_str());
   if (infile.is_open())
  {
    for(Int_t ivec = 0; ivec < 16; ivec++)  
   {    
/*  
 ************* Parameters 0-7 MCFD  **********************
 ************* Parameters 8-15 TAMEX *********************
*/ 	   
	   
     infile >> walk_par[ivec][0] >> walk_par[ivec][1] >> walk_par[ivec][2] >> walk_par[ivec][3] >> walk_par[ivec][4] >>
               walk_par[ivec][5] >> walk_par[ivec][6] >> walk_par[ivec][7] >> walk_par[ivec][8] >> walk_par[ivec][9] >>
               walk_par[ivec][10];
               
 /* 
      cout<<setprecision(10)<< ivec<<", "<< walk_par[ivec][0] <<", "<< walk_par[ivec][1] <<", "<< walk_par[ivec][2] <<", "<< 
                          walk_par[ivec][3] <<", "<< walk_par[ivec][4] <<", "<< walk_par[ivec][5] <<", "<<
                          walk_par[ivec][6] <<", "<< walk_par[ivec][7] <<", "<< walk_par[ivec][8] <<", "<<
                          walk_par[ivec][9] <<", "<< walk_par[ivec][10] <<endl;       
  */             
   }
  } 
  else //cerr << "Unable to open file \""<<fwalk_param_file<<"\" with walk parameters!"<<endl;
  {
	cout << "*****************************************************************"<<endl;  
	cout << "UNABLE TO OPEN FILE WITH WALK PARAMETERS! Parameters set to zero!"<<endl;
	cout << "*****************************************************************"<<endl;
	for(Int_t ivec = 0; ivec < 16; ivec++) 
	{
	 walk_par[ivec][0] = 10.;
	 walk_par[ivec][1] = 1000.;
	  for(Int_t iv = 2; iv < 11; iv++) 
	  {
	    walk_par[ivec][iv] = 0.;;
	   }
	 }  	  
   }	  
// file with tot-correction parameters
     ifstream infile1(ftot_param_file.c_str());

   if (infile1.is_open())
  {
    for(Int_t ivec = 0; ivec < 8; ivec++)  
   {    
     infile1 >> tot_par[ivec][0] >> tot_par[ivec][1] >> tot_par[ivec][2] >> tot_par[ivec][3] ;
                        
   }
  } 
  else //cerr << "Unable to open file \""<<ftot_param_file<<"\" with tot parameters!"<<endl;  
  {
	cout << "****************************************************************"<<endl;
	cout << "UNABLE TO OPEN FILE WITH ToT PARAMETERS! Parameters set to zero!"<<endl;
	cout << "****************************************************************"<<endl;
	for(Int_t ivec = 0; ivec < 8; ivec++) 
	{
	 tot_par[ivec][0] = 0.;
	 tot_par[ivec][1] = 0.;  
	 tot_par[ivec][2] = 0.;
	 tot_par[ivec][3] = 1.; // Normalization factor
	} 
   }	   

  
  //cout << "R3BLosCal2Hit::Init END" << endl;
  return kSUCCESS;
}


InitStatus R3BLosCal2Hit::ReInit()
{
    return kSUCCESS;
}

/* Calculate a single hit time for each LOS detector
 * 
 * Remember: The times of individual channels depend on the position of
 * the particle on the scintillator. To obtain a precise time of the 
 * particle, we need to average either over all four signals (right, top,
 * left, bottom) or over two opposite signals (left+right or top+bottom).
 */
void R3BLosCal2Hit::Exec(Option_t* option)
{ 
	//cout << "R3BLosCal2Hit::Exec BEGIN: " << Icount << endl;
	   Icount++;
 // ofstream myFile("data_483.dat",ios_base::out|ios_base::app);
     

       // missing times are NAN, hence other times will also
       // be NAN if one time is missing.
       Double_t time_r_V = 0.0/0.0;   
       Double_t time_t_V = 0.0/0.0;       
       Double_t time_l_V = 0.0/0.0;       
       Double_t time_b_V = 0.0/0.0;  
       Double_t time_ref_V = 0.0/0.0; 
       Double_t time_pix_V = 0.0/0.0;      
       Double_t time_r_V_off = 0.0/0.0;   
       Double_t time_t_V_off = 0.0/0.0;       
       Double_t time_l_V_off = 0.0/0.0;       
       Double_t time_b_V_off = 0.0/0.0;          
       Double_t time_r_V_corr = 0.0/0.0;   
       Double_t time_t_V_corr = 0.0/0.0;       
       Double_t time_l_V_corr = 0.0/0.0;       
       Double_t time_b_V_corr = 0.0/0.0;          
       Double_t time_rb_V_corr = 0.0/0.0;   
       Double_t time_rt_V_corr = 0.0/0.0;       
       Double_t time_lt_V_corr = 0.0/0.0;       
       Double_t time_lb_V_corr = 0.0/0.0;           
       Double_t time_rb_L_corr = 0.0/0.0;   
       Double_t time_rt_L_corr = 0.0/0.0;       
       Double_t time_lt_L_corr = 0.0/0.0;       
       Double_t time_lb_L_corr = 0.0/0.0;       
       Double_t time_r_L = 0.0/0.0;     
       Double_t time_ref_L = 0.0/0.0;  
       Double_t time_pix_L = 0.0/0.0;  
       Double_t time_r_L_corr = 0.0/0.0;
       Double_t time_r_T = 0.0/0.0;
       Double_t time_ref_T = 0.0/0.0;
       Double_t time_pix_T = 0.0/0.0;
       Double_t time_l_L = 0.0/0.0;       
       Double_t time_l_L_corr = 0.0/0.0;
       Double_t time_l_T = 0.0/0.0;
       Double_t time_t_L = 0.0/0.0;      
       Double_t time_t_L_corr = 0.0/0.0;
       Double_t time_t_T = 0.0/0.0;
       Double_t time_b_L = 0.0/0.0;       
       Double_t time_b_L_corr = 0.0/0.0;
       Double_t time_b_T = 0.0/0.0;  
       Double_t time_rt_V = 0.0/0.0;   
       Double_t time_lt_V = 0.0/0.0;       
       Double_t time_lb_V = 0.0/0.0;       
       Double_t time_rb_V = 0.0/0.0;           
       Double_t time_rt_L = 0.0/0.0;
       Double_t time_rt_T = 0.0/0.0;
       Double_t time_lb_L = 0.0/0.0;
       Double_t time_lb_T = 0.0/0.0;
       Double_t time_lt_L = 0.0/0.0;
       Double_t time_lt_T = 0.0/0.0;
       Double_t time_rb_L = 0.0/0.0;
       Double_t time_rb_T = 0.0/0.0;              
       Double_t t_hor_M = 0.0/0.0;
       Double_t t_ver_M = 0.0/0.0;       
       Double_t t_hor_M_corr = 0.0/0.0;
       Double_t t_ver_M_corr = 0.0/0.0;
       Double_t t_45_M = 0.0/0.0;
       Double_t t_135_M = 0.0/0.0;
       Double_t t_hor_T = 0.0/0.0;
       Double_t t_ver_T = 0.0/0.0;       
       Double_t t_hor_T_corr = 0.0/0.0;
       Double_t t_ver_T_corr = 0.0/0.0;
       Double_t t_45_T = 0.0/0.0;
       Double_t t_135_T = 0.0/0.0;
       Double_t timeLosM = 0.0/0.0;
       Double_t LosTresM = 0.0/0.0;       
       Double_t LosTresM_corr = 0.0/0.0;
       Double_t timeLosT = 0.0/0.0;
       Double_t LosTresT = 0.0/0.0;
       Double_t LosTresT_corr = 0.0/0.0;
       Double_t x_cm  = 0.0/0.0;
       Double_t y_cm  = 0.0/0.0;      
       Double_t x_cm_corr  = 0.0/0.0;
       Double_t y_cm_corr  = 0.0/0.0;
       Double_t x1_cm  = 0.0/0.0;
       Double_t y1_cm  = 0.0/0.0;       
       Double_t x1_cm_corr  = 0.0/0.0;
       Double_t y1_cm_corr  = 0.0/0.0;
       Double_t x2_cm  = 0.0/0.0;
       Double_t y2_cm  = 0.0/0.0;         
       Double_t x2_cm_corr  = 0.0/0.0;
       Double_t y2_cm_corr  = 0.0/0.0;     
       Double_t xT_cm  = 0.0/0.0;
       Double_t yT_cm  = 0.0/0.0;
       Double_t x1T_cm  = 0.0/0.0;
       Double_t y1T_cm  = 0.0/0.0;     
       Double_t xT_cm_corr  = 0.0/0.0;
       Double_t yT_cm_corr  = 0.0/0.0;
       Double_t x1T_cm_corr  = 0.0/0.0;
       Double_t y1T_cm_corr  = 0.0/0.0;
       Double_t x2T_cm  = 0.0/0.0;
       Double_t y2T_cm  = 0.0/0.0;
       Double_t xToT = 0.0/0.0;
       Double_t yToT = 0.0/0.0;
       Double_t x1ToT_cm = 0.0/0.0;
       Double_t y1ToT_cm = 0.0/0.0;
       Double_t x2ToT_cm = 0.0/0.0;
       Double_t y2ToT_cm = 0.0/0.0;    
       Double_t xmean_cm=0.0/0.0, ymean_cm=0.0/0.0;     
       Double_t y1real= 0.0/0.0,x1real= 0.0/0.0,y2real= 0.0/0.0,x2real= 0.0/0.0,xreal= 0.0/0.0,yreal= 0.0/0.0;
       Double_t  tott_scal= 0.0/0.0,totl_scal= 0.0/0.0,totb_scal= 0.0/0.0,totr_scal= 0.0/0.0,
                 totlt_scal= 0.0/0.0,totlb_scal= 0.0/0.0,totrb_scal= 0.0/0.0,totrt_scal= 0.0/0.0;
       Double_t Z = 0.0;     
       Double_t t_hit=0.0/0.0; // NAN
       Double_t tot=0./0.0; 
       Double_t totr = 0.0/0.0;
       Double_t totl = 0.0/0.0;
       Double_t tott = 0.0/0.0;
       Double_t totb = 0.0/0.0; 
       Double_t totrt = 0.0/0.0;
       Double_t totlt = 0.0/0.0;
       Double_t totlb = 0.0/0.0;
       Double_t totrb = 0.0/0.0;
       Double_t tot_45 = 0.0/0.0;
       Double_t tot_135 = 0.0/0.0;
       Double_t tot_hor = 0.0/0.0;
       Double_t tot_ver = 0.0/0.0;              
       Double_t totl_corr = 0.0/0.0;
       Double_t totb_corr = 0.0/0.0;           		      	        
       Double_t totr_corr = 0.0/0.0;           
       Double_t tott_corr = 0.0/0.0;           
       Double_t totlt_corr = 0.0/0.0;
       Double_t totlb_corr = 0.0/0.0;           		      	        
       Double_t totrb_corr = 0.0/0.0;           
       Double_t totrt_corr = 0.0/0.0;		   
       Double_t tot_corr = 0.0/0.0; 
       Double_t totpix = 0.0/0.0;                 
       Double_t ref_timeM=0.;
	   Double_t ref_timeT=0.; 		   
	   Double_t rw_sum=0.0;
       Double_t rw_t = 0.0, rw_l = 0.0, rw_b = 0.0, rw_r = 0.0; // Winkel
       Double_t rw_lt = 0.0, rw_lb = 0.0, rw_rb = 0.0, rw_rt = 0.0; // Winkel      
       Double_t rr_t = 0.0, rr_l = 0.0, rr_b = 0.0, rr_r = 0.0; // Radius
       Double_t rr_lt = 0.0, rr_lb = 0.0, rr_rb = 0.0, rr_rt = 0.0; // Radius 
       Double_t tot_ref[8][2];  
       Double_t rl_t = 0.0, rl_l = 0.0, rl_b = 0.0, rl_r = 0.0; // Absorption length
       Double_t rl_lt = 0.0, rl_lb = 0.0, rl_rb = 0.0, rl_rt = 0.0; // Absorption length
       Double_t xvar, yvar, xvar1, yvar1, l1, l2;    
       Double_t dx1 = 0.0/0.0, dy1 = 0.0/0.0, dx2 = 0.0/0.0, dy2 = 0.0/0.0; 
       Double_t dt1 = 0.0/0.0, dt2 = 0.0/0.0, dt3 = 0.0/0.0, dt4 = 0.0/0.0;  
// Sci shape:
// 0 - Large (10x10) squared, with 2inch PM
// 1 - Small octagonal, with 1inch PM
// 2 - Small (5x5) squared, with 2inch PM  
// 3 - Large octagonal, with 2inch PM     
// 4 - Large (10x10) squared, with 1inch PM   
// 5 - Large octagonal, with 4 2inch and 4 1inch PMs
// 6 - Large octagonal, with 8 1inch PMs
       
       Double_t l_PM, l_PM1, l_PM2 ;  //PM diameter in cm
       Double_t l_SC ;  // Sci length    
       Double_t l_c; // Side-length of Sci   

       if(iSciType == 0)
       {
		l_PM = 2.*2.54;
		l_SC = 10.0;  
	   }
      if(iSciType == 1)
       {
		l_PM = 2.54;
		l_SC = 2.7*(1.+sqrt(2.));  
	   }	   
       if(iSciType == 2)
       {
		l_PM = 2.*2.54;
		l_SC = 5.;  
	   }
      if(iSciType == 3)
       {
		l_PM = 2.*2.54;
		l_SC = 5.2*(1.+sqrt(2.));  
	   }	       
	   if(iSciType == 4)
       {
		l_PM = 2.54;
		l_SC = 10.0;  
	   }   	  
	   if(iSciType == 5)
       {
		l_PM1 = 2.*2.54;
		l_PM2 = 2.54;
		l_c = 5.4;
		l_SC = l_c*(1.+sqrt(2.));  
	   }	 	  
	   if(iSciType == 6)
       {
		l_PM1 = 2.54;
		l_PM2 = 2.54;
		l_c = 5.4;
		l_SC = l_c*(1.+sqrt(2.));  
	   }	         
//        cout<<"SciType "<<iSciType<<endl;
        
        
       Double_t fhdTxmin = 0.;
       Double_t fhdTxmax = 100.;
       Int_t    fhdTxbin = 20;
     // min,max,Nbins for time spectra  
       Double_t fhTmin = -20.; //-5.; //-10
       Double_t fhTmax = 20.; //5.;  // 10
       Int_t    fhTbin = 2000; //10000; // 20000
     // min,max,Nbins for ToT spectra        
       Double_t fhQmin = 0.;
       Double_t fhQmax = 200; //300.; //150
       Int_t    fhQbin = 4000; //0; //3000; //1500
     // min,max,Nbins for X and Y spectra
       Double_t fhXmin = -5.; //-5
       Double_t fhXmax = 5.; // 5
       Int_t    fhXbin = 2000; // 1000
      // min,max,Nbins for radius spectra
       Double_t fhRmin = -2.;//0.;
       Double_t fhRmax = 2.;//10.;
       Int_t    fhRbin = 1000;
       
          if (NULL == fhTres_M )
          {
              char strName[255];
              sprintf(strName, "LOS_dt_MCFD");
              fhTres_M = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }	 
                     
          if (NULL == fhTres_M_evCh)
          {
              char strName[255];
              sprintf(strName, "LOS_dt_MCFD_evenCh");
              fhTres_M_evCh= new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }	 
                        
          if (NULL == fhTres_M_oddCh)
          {
              char strName[255];
              sprintf(strName, "LOS_dt_MCFD_oddCh");
              fhTres_M_oddCh= new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }
                                            
           if (NULL == fhTres_M_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_dt_MCFD_wc");
              fhTres_M_corr = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }
          
           if (NULL == fhTres_M_evCh_corr)
          {
              char strName[255];
              sprintf(strName, "LOS_dt_MCFD_evenCh_wc");
              fhTres_M_evCh_corr= new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }	 
                        
          if (NULL == fhTres_M_oddCh_corr)
          {
              char strName[255];
              sprintf(strName, "LOS_dt_MCFD_oddCh_wc");
              fhTres_M_oddCh_corr= new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }
                                            
          if (NULL == fhTres_T )
          {
              char strName[255];
              sprintf(strName, "LOS_dt_TAMEX");
              fhTres_T = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }	
          
          if (NULL == fhTres_T_evCh )
          {
              char strName[255];
              sprintf(strName, "LOS_dt_TAMEX_evenCh");
              fhTres_T_evCh = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }	
          
          if (NULL == fhTres_T_oddCh )
          {
              char strName[255];
              sprintf(strName, "LOS_dt_TAMEX_oddCh");
              fhTres_T_oddCh = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }	
                    
                              
          if (NULL == fhTres_T_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_dt_TAMEX_wc");
              fhTres_T_corr = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }         
          
          if (NULL == fhTres_T_evCh_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_dt_TAMEX_evenCh_wc");
              fhTres_T_evCh_corr = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }	
          
          if (NULL == fhTres_T_oddCh_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_dt_TAMEX_oddCh_wc");
              fhTres_T_oddCh_corr = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          } 
          
          if (NULL == fhTres_MT_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_dt_MCFD_TAMEX_wc");
              fhTres_MT_corr = new TH1F(strName, "", fhTbin, fhTmin, fhTmax);              
          }
          
          if (NULL == fhQ_L )
          {
              char strName[255];
              sprintf(strName, "LOS_QL_vs_X1");
              fhQ_L = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);              
          }	

         if (NULL == fhQ_T )
          {
              char strName[255];
              sprintf(strName, "LOS_QT_vs_Y1");
              fhQ_T = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
                   
         if (NULL == fhQ_B )
          {
              char strName[255];
              sprintf(strName, "LOS_QB_vs_Y1");
              fhQ_B = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ_R )
          {
              char strName[255];
              sprintf(strName, "LOS_QR_vs_X1");
              fhQ_R = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
          
          
         if (NULL == fhQ_L_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_QL_vs_X1_corr");
              fhQ_L_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax, fhQbin, fhQmin, fhQmax);              
          }	

         if (NULL == fhQ_T_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_QT_vs_Y1_corr");
              fhQ_T_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
                   
         if (NULL == fhQ_B_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_QB_vs_Y1_corr");
              fhQ_B_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ_R_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_QR_vs_X1_corr");
              fhQ_R_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
                    
          if (NULL == fhQ_LB )
          {
              char strName[255];
              sprintf(strName, "LOS_QLB_vs_Y2");
              fhQ_LB = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }	

         if (NULL == fhQ_LT )
          {
              char strName[255];
              sprintf(strName, "LOS_QLT_vs_X2");
              fhQ_LT = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
                   
         if (NULL == fhQ_RB )
          {
              char strName[255];
              sprintf(strName, "LOS_QRB_vs_X2");
              fhQ_RB = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ_RT )
          {
              char strName[255];
              sprintf(strName, "LOS_QRT_vs_Y2");
              fhQ_RT = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
          
                    
          if (NULL == fhQ_LB_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_QLB_vs_Y2_corr");
              fhQ_LB_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }	

         if (NULL == fhQ_LT_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_QLT_vs_X2_corr");
              fhQ_LT_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
                   
         if (NULL == fhQ_RB_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_QRB_vs_X2_corr");
              fhQ_RB_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ_RT_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_QRT_vs_Y2_corr");
              fhQ_RT_corr = new TH2F(strName, "", fhRbin, fhRmin, fhRmax,fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ_oddCh )
          {
              char strName[255];
              sprintf(strName, "LOS_Q_oddCh");
              fhQ_oddCh = new TH1F(strName, "", fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ_evCh )
          {
              char strName[255];
              sprintf(strName, "LOS_Q_evenCh");
              fhQ_evCh = new TH1F(strName, "", fhQbin, fhQmin, fhQmax);              
          }          
          
          if (NULL == fhQ_oddCh_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_Q_oddCh_corr");
              fhQ_oddCh_corr = new TH1F(strName, "", fhQbin, fhQmin, fhQmax);              
          }          
          
          if (NULL == fhQ_evCh_corr )
          {
              char strName[255];
              sprintf(strName, "LOS_Q_evenCh_corr");
              fhQ_evCh_corr = new TH1F(strName, "", fhQbin, fhQmin, fhQmax);              
          }          
          
         if (NULL == fhQ )
          {
              char strName[255];
              sprintf(strName, "LOS_Q");
              fhQ = new TH1F(strName, "", 10000, fhQmin, fhQmax);              
          } 
          
          if (NULL == fhQtest )
          {
              char strName[255];
              sprintf(strName, "LOS_Q_test");
              fhQtest = new TH1F(strName, "", 10000, fhQmin, fhQmax);              
          } 
          
          if (NULL == fhQpix )
          {
              char strName[255];
              sprintf(strName, "LOS_Qpix");
              fhQpix = new TH1F(strName, "", fhQbin, fhQmin, fhQmax);              
          } 
                             
         if (NULL == fhTM_vs_Q )
          {
              char strName[255];
              sprintf(strName, "TMCFD_vs_Q");
              fhTM_vs_Q = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
          
          if (NULL == fhTT_vs_Q )
          {
              char strName[255];
              sprintf(strName, "TTAMEX_vs_Q");
              fhTT_vs_Q = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }         

         if (NULL == fhTM_vs_Q_corr )
          {
              char strName[255];
              sprintf(strName, "TMCFD_vs_Q_wc");
              fhTM_vs_Q_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          

         if (NULL == fhTT_vs_Q_corr )
          {
              char strName[255];
              sprintf(strName, "TTAMEX_vs_Q_wc");
              fhTT_vs_Q_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          } 
          
         if (NULL == fhTresX_M )
          {
              char strName[255];
              sprintf(strName, "Tres_vs_X_MCFD");
              fhTresX_M = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhTbin, fhTmin, fhTmax);              
          }          
          
         if (NULL == fhTresY_M )
          {
              char strName[255];
              sprintf(strName, "Tres_vs_Y_MCFD");
              fhTresY_M = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhTbin, fhTmin, fhTmax);              
          }          
          
         if (NULL == fhTresX_M_corr )
          {
              char strName[255];
              sprintf(strName, "Tres_vs_X_MCFD_wc");
              fhTresX_M_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhTbin, fhTmin, fhTmax);              
          }          
          
         if (NULL == fhTresY_M_corr )
          {
              char strName[255];
              sprintf(strName, "Tres_vs_Y_MCFD_wc");
              fhTresY_M_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhTbin, fhTmin, fhTmax);              
          }          
          
         if (NULL == fhTresX_T )
          {
              char strName[255];
              sprintf(strName, "Tres_vs_X_TAMEX");
              fhTresX_T = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhTbin, fhTmin, fhTmax);              
          }          
          
         if (NULL == fhTresY_T )
          {
              char strName[255];
              sprintf(strName, "Tres_vs_Y_TAMEX");
              fhTresY_T = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhTbin, fhTmin, fhTmax);              
          }          

         if (NULL == fhTresX_T_corr)
          {
              char strName[255];
              sprintf(strName, "Tres_vs_X_TAMEX_wc");
              fhTresX_T_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhTbin, fhTmin, fhTmax);              
          }          
          
         if (NULL == fhTresY_T_corr )
          {
              char strName[255];
              sprintf(strName, "Tres_vs_Y_TAMEX_wc");
              fhTresY_T_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhTbin, fhTmin, fhTmax);              
          } 
          
         if (NULL == fhXY )
          {
              char strName[255];
              sprintf(strName, "X_vs_Y");
              fhXY = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          }        
          
          if (NULL == fhXYT )
          {
              char strName[255];
              sprintf(strName, "X_vs_Y_TAMEX");
              fhXYT = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          }  
          
          if (NULL == fhXYmean )
          {
              char strName[255];
              sprintf(strName, "X_vs_Y_mean");
              fhXYmean = new TH2F(strName, "", 4000, fhXmin, fhXmax,4000, fhXmin, fhXmax);              
          }

         if (NULL == fhXY_corr )
          {
              char strName[255];
              sprintf(strName, "X_vs_Y_wc");
              fhXY_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          }          
          
          if (NULL == fhXY1_corr )
          {
              char strName[255];
              sprintf(strName, "X1_vs_Y1_wc");
              fhXY1_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          } 
          
          if (NULL == fhXY2_corr )
          {
              char strName[255];
              sprintf(strName, "X2_vs_Y2_wc");
              fhXY2_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          }
          
         if (NULL == fhXY_ToT )
          {
              char strName[255];
              sprintf(strName, "X_vs_Y_ToT");
              fhXY_ToT = new TH2F(strName, "", 4000, fhXmin, fhXmax, 4000, fhXmin, fhXmax);              
          }       
          
          if (NULL == fhXY_ToT_corr )
          {
              char strName[255];
              sprintf(strName, "X_vs_Y_ToT_corr");
              fhXY_ToT_corr = new TH2F(strName, "", 4000, fhXmin, fhXmax,4000, fhXmin, fhXmax);              
          }      
          
           if (NULL == fhXY1_ToT_corr )
          {
              char strName[255];
              sprintf(strName, "X1_vs_Y1_ToT_corr");
              fhXY1_ToT_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          }      
          
          if (NULL == fhXY2_ToT_corr )
          {
              char strName[255];
              sprintf(strName, "X2_vs_Y2_ToT_corr");
              fhXY2_ToT_corr = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          }      
               
          if (NULL == fhXYproj )
          {
              char strName[255];
              sprintf(strName, "X2_vs_Y2_proj");
              fhXYproj = new TH2F(strName, "", 4000, fhXmin, fhXmax,4000, fhXmin, fhXmax);              
          }  
          
         if (NULL == fhXY1 )
          {
              char strName[255];
              sprintf(strName, "X1_vs_Y1");
              fhXY1 = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          } 
          
         if (NULL == fhXY2 )
          {
              char strName[255];
              sprintf(strName, "X2_vs_Y2");
              fhXY2 = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          }
           
         if (NULL == fhXYT1 )
          {
              char strName[255];
              sprintf(strName, "X1_vs_Y1_TAMEX");
              fhXYT1 = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          } 
          
         if (NULL == fhXYT2 )
          {
              char strName[255];
              sprintf(strName, "X2_vs_Y2_TAMEX");
              fhXYT2 = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          }
          
         if (NULL == fhXYQ1 )
          {
              char strName[255];
              sprintf(strName, "X1_vs_Y1_ToT");
              fhXYQ1 = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          } 
          
         if (NULL == fhXYQ2 )
          {
              char strName[255];
              sprintf(strName, "X2_vs_Y2_ToT");
              fhXYQ2 = new TH2F(strName, "", fhXbin, fhXmin, fhXmax,fhXbin, fhXmin, fhXmax);              
          }
           
          if (NULL == fhQ_vs_X )
          {
              char strName[255];
              sprintf(strName, "Q_vs_X");
              fhQ_vs_X = new TH2F(strName, "",fhXbin, fhXmin, fhXmax,fhQbin, fhQmin, fhQmax);              
          }     
      
          if (NULL == fhQ_vs_Y )
          {
              char strName[255];
              sprintf(strName, "Q_vs_Y");
              fhQ_vs_Y = new TH2F(strName, "",fhXbin, fhXmin, fhXmax,fhQbin, fhQmin, fhQmax);              
          }          
          
          if (NULL == fhQ_vs_X_corr )
          {
              char strName[255];
              sprintf(strName, "Qcorr_vs_X");
              fhQ_vs_X_corr = new TH2F(strName, "",fhXbin, fhXmin, fhXmax,fhQbin, fhQmin, fhQmax);              
          }     
      
          if (NULL == fhQ_vs_Y_corr )
          {
              char strName[255];
              sprintf(strName, "Qcorr_vs_Y");
              fhQ_vs_Y_corr = new TH2F(strName, "",fhXbin, fhXmin, fhXmax,fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ1_vs_Q5 )
          {
              char strName[255];
              sprintf(strName, "QPM1_vs_QPM5");
              fhQ1_vs_Q5 = new TH2F(strName, "",fhQbin, fhQmin, fhQmax,fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ1_vs_Q5_corr )
          {
              char strName[255];
              sprintf(strName, "QPM1_vs_QPM5_corr");
              fhQ1_vs_Q5_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax,fhQbin, fhQmin, fhQmax);              
          }
                    
          if (NULL == fhQ2_vs_Q6 )
          {
              char strName[255];
              sprintf(strName, "QPM2_vs_QPM6");
              fhQ2_vs_Q6 = new TH2F(strName, "",fhQbin, fhQmin, fhQmax,fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ2_vs_Q6_corr )
          {
              char strName[255];
              sprintf(strName, "QPM2_vs_QPM6_corr");
              fhQ2_vs_Q6_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax,fhQbin, fhQmin, fhQmax);              
          }
                    
          if (NULL == fhQ3_vs_Q7 )
          {
              char strName[255];
              sprintf(strName, "QPM3_vs_QPM7");
              fhQ3_vs_Q7 = new TH2F(strName, "",fhQbin, fhQmin, fhQmax,fhQbin, fhQmin, fhQmax);              
          }
          
           if (NULL == fhQ3_vs_Q7_corr )
          {
              char strName[255];
              sprintf(strName, "QPM3_vs_QPM7_corr");
              fhQ3_vs_Q7_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax,fhQbin, fhQmin, fhQmax);              
          }
                   
          if (NULL == fhQ4_vs_Q8 )
          {
              char strName[255];
              sprintf(strName, "QPM4_vs_QPM8");
              fhQ4_vs_Q8 = new TH2F(strName, "",fhQbin, fhQmin, fhQmax,fhQbin, fhQmin, fhQmax);              
          }
          
          if (NULL == fhQ4_vs_Q8_corr )
          {
              char strName[255];
              sprintf(strName, "QPM4_vs_QPM8_corr");
              fhQ4_vs_Q8_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax,fhQbin, fhQmin, fhQmax);              
          }
          
         if (NULL == fht1Q1M )
          {
              char strName[255];
              sprintf(strName, "t1_vs_Q1_M");
              fht1Q1M = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht3Q3M )
          {
              char strName[255];
              sprintf(strName, "t3_vs_Q3_M");
              fht3Q3M = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht5Q5M )
          {
              char strName[255];
              sprintf(strName, "t5_vs_Q5_M");
              fht5Q5M = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht7Q7M )
          {
              char strName[255];
              sprintf(strName, "t7_vs_Q7_M");
              fht7Q7M = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }                  
          
          if (NULL == fht1Q1M_corr )
          {
              char strName[255];
              sprintf(strName, "t1_vs_Q1_M_wc");
              fht1Q1M_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht3Q3M_corr )
          {
              char strName[255];
              sprintf(strName, "t3_vs_Q3_M_wc");
              fht3Q3M_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht5Q5M_corr )
          {
              char strName[255];
              sprintf(strName, "t5_vs_Q5_M_wc");
              fht5Q5M_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht7Q7M_corr )
          {
              char strName[255];
              sprintf(strName, "t7_vs_Q7_M_wc");
              fht7Q7M_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          

         if (NULL == fht8Q8M )
          {
              char strName[255];
              sprintf(strName, "t8_vs_Q8_M");
              fht8Q8M = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht2Q2M)
          {
              char strName[255];
              sprintf(strName, "t2_vs_Q2_M");
              fht2Q2M = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht4Q4M )
          {
              char strName[255];
              sprintf(strName, "t4_vs_Q4_M");
              fht4Q4M = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht6Q6M )
          {
              char strName[255];
              sprintf(strName, "t6_vs_Q6_M");
              fht6Q6M = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          } 

         if (NULL == fht8Q8M_corr )
          {
              char strName[255];
              sprintf(strName, "t8_vs_Q8_M_wc");
              fht8Q8M_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht2Q2M_corr)
          {
              char strName[255];
              sprintf(strName, "t2_vs_Q2_M_wc");
              fht2Q2M_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht4Q4M_corr )
          {
              char strName[255];
              sprintf(strName, "t4_vs_Q4_M_wc");
              fht4Q4M_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht6Q6M_corr)
          {
              char strName[255];
              sprintf(strName, "t6_vs_Q6_M_wc");
              fht6Q6M_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          } 
 
          if (NULL == fht1Q1T )
          {
              char strName[255];
              sprintf(strName, "t1_vs_Q1_T");
              fht1Q1T = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht3Q3T )
          {
              char strName[255];
              sprintf(strName, "t3_vs_Q3_T");
              fht3Q3T = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht5Q5T )
          {
              char strName[255];
              sprintf(strName, "t5_vs_Q5_T");
              fht5Q5T = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht7Q7T )
          {
              char strName[255];
              sprintf(strName, "t7_vs_Q7_T");
              fht7Q7T = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }                  
          
          if (NULL == fht1Q1T_corr )
          {
              char strName[255];
              sprintf(strName, "t1_vs_Q1_T_wc");
              fht1Q1T_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht3Q3T_corr )
          {
              char strName[255];
              sprintf(strName, "t3_vs_Q3_T_wc");
              fht3Q3T_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht5Q5T_corr )
          {
              char strName[255];
              sprintf(strName, "t5_vs_Q5_T_wc");
              fht5Q5T_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht7Q7T_corr )
          {
              char strName[255];
              sprintf(strName, "t7_vs_Q7_T_wc");
              fht7Q7T_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          

         if (NULL == fht8Q8T )
          {
              char strName[255];
              sprintf(strName, "t8_vs_Q8_T");
              fht8Q8T = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht2Q2T)
          {
              char strName[255];
              sprintf(strName, "t2_vs_Q2_T");
              fht2Q2T = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht4Q4T )
          {
              char strName[255];
              sprintf(strName, "t4_vs_Q4_T");
              fht4Q4T = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht6Q6T )
          {
              char strName[255];
              sprintf(strName, "t6_vs_Q6_T");
              fht6Q6T = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          } 

         if (NULL == fht8Q8T_corr )
          {
              char strName[255];
              sprintf(strName, "t8_vs_Q8_T_wc");
              fht8Q8T_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht2Q2T_corr)
          {
              char strName[255];
              sprintf(strName, "t2_vs_Q2_T_wc");
              fht2Q2T_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht4Q4T_corr )
          {
              char strName[255];
              sprintf(strName, "t4_vs_Q4_T_wc");
              fht4Q4T_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          }          
                    
         if (NULL == fht6Q6T_corr)
          {
              char strName[255];
              sprintf(strName, "t6_vs_Q6_T_wc");
              fht6Q6T_corr = new TH2F(strName, "",fhQbin, fhQmin, fhQmax, fhTbin, fhTmin, fhTmax);              
          } 

                    
         if(NULL == fhQ1vsIcount)
         {
              char strName[255];
              sprintf(strName, "Q1_vs_Icount");
              fhQ1vsIcount = new TH2F(strName, "",1500,0,15000000,fhQbin, fhQmin, fhQmax);  			 
	     }          
                                        
         if(NULL == fhQ3vsIcount)
         {
              char strName[255];
              sprintf(strName, "Q3_vs_Icount");
              fhQ3vsIcount = new TH2F(strName, "",1500,0,15000000,fhQbin, fhQmin, fhQmax);  			 
	     }
	     
         if(NULL == fhQ5vsIcount)
         {
              char strName[255];
              sprintf(strName, "Q5_vs_Icount");
              fhQ5vsIcount = new TH2F(strName, "",1500,0,15000000,fhQbin, fhQmin, fhQmax);  			 
	     }
	     
         if(NULL == fhQ7vsIcount)
         {
              char strName[255];
              sprintf(strName, "Q7_vs_Icount");
              fhQ7vsIcount = new TH2F(strName, "",1500,0,15000000,fhQbin, fhQmin, fhQmax);  			 
	     }	

         if(NULL == fhQ2vsIcount)
         {
              char strName[255];
              sprintf(strName, "Q2_vs_Icount");
              fhQ2vsIcount = new TH2F(strName, "",1500,0,15000000,fhQbin, fhQmin, fhQmax);  			 
	     } 
	     	          
	     if(NULL == fhQ4vsIcount)
         {
              char strName[255];
              sprintf(strName, "Q4_vs_Icount");
              fhQ4vsIcount = new TH2F(strName, "",1500,0,15000000,fhQbin, fhQmin, fhQmax);  			 
	     } 
	     
	     
	     if(NULL == fhQ6vsIcount)
         {
              char strName[255];
              sprintf(strName, "Q6_vs_Icount");
              fhQ6vsIcount = new TH2F(strName, "",1500,0,15000000,fhQbin, fhQmin, fhQmax);  			 
	     } 
	     
	     if(NULL == fhQ8vsIcount)
         {
              char strName[255];
              sprintf(strName, "Q8_vs_Icount");
              fhQ8vsIcount = new TH2F(strName, "",1500,0,15000000,fhQbin, fhQmin, fhQmax);  			 
	     } 
	     
	     	         	               
         if(NULL == fhTresMvsIcount)
         {
              char strName[255];
              sprintf(strName, "TresM_vs_Icount");
              fhTresMvsIcount = new TH2F(strName, "",1500,0,15000000,fhTbin, fhTmin, fhTmax);  			 
	     }	         
	                           
	     if(NULL == fhTreswcMvsIcount)
         {
              char strName[255];
              sprintf(strName, "TreswcM_vs_Icount");
              fhTreswcMvsIcount = new TH2F(strName, "",1500,0,15000000,fhTbin, fhTmin, fhTmax);  			 
	     }
	     	     
         if(NULL == fhTresTvsIcount)
         {
              char strName[255];
              sprintf(strName, "TresT_vs_Icount");
              fhTresTvsIcount = new TH2F(strName, "",1500,0,15000000,fhTbin, fhTmin, fhTmax);  			 
	     }
	     	     
         if(NULL == fhTreswcTvsIcount)
         {
              char strName[255];
              sprintf(strName, "TreswcT_vs_Icount");
              fhTreswcTvsIcount = new TH2F(strName, "",1500,0,15000000,fhTbin, fhTmin, fhTmax);  			 
	     }
	     	     
         if(NULL == fhRW)
         {
              char strName[255];
              sprintf(strName, "RaumWinkel");
              fhRW = new TH1F(strName, "",9000,90,390);  			 
	     }
	     
        if(NULL == fht1t5M )
         {
              char strName[255];
              sprintf(strName, "t1t5_M");
              fht1t5M  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	     
        if(NULL == fht2t6M )
         {
              char strName[255];
              sprintf(strName, "t2t6_M");
              fht2t6M  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	     	     
        if(NULL == fht3t7M )
         {
              char strName[255];
              sprintf(strName, "t3t7_M");
              fht3t7M  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	    	     
        if(NULL == fht4t8M )
         {
              char strName[255];
              sprintf(strName, "t4t8_M");
              fht4t8M  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	    	     
        if(NULL == fht1t5M_corr )
         {
              char strName[255];
              sprintf(strName, "t1t5_M_wc");
              fht1t5M_corr  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	     
        if(NULL == fht2t6M_corr)
         {
              char strName[255];
              sprintf(strName, "t2t6_M_wc");
              fht2t6M_corr  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	     	     
        if(NULL == fht3t7M_corr )
         {
              char strName[255];
              sprintf(strName, "t3t7_M_wc");
              fht3t7M_corr  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	    	     
        if(NULL == fht4t8M_corr )
         {
              char strName[255];
              sprintf(strName, "t4t8_M_wc");
              fht4t8M_corr  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	    	     
        if(NULL == fht1t5T )
         {
              char strName[255];
              sprintf(strName, "t1t5_T");
              fht1t5T  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	     
        if(NULL == fht2t6T )
         {
              char strName[255];
              sprintf(strName, "t2t6_T");
              fht2t6T  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	     	     
        if(NULL == fht3t7T )
         {
              char strName[255];
              sprintf(strName, "t3t7_T");
              fht3t7T  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	    	     
        if(NULL == fht4t8T )
         {
              char strName[255];
              sprintf(strName, "t4t8_T");
              fht4t8T  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	    	     
        if(NULL == fht1t5T_corr )
         {
              char strName[255];
              sprintf(strName, "t1t5_T_wc");
              fht1t5T_corr  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	     
        if(NULL == fht2t6T_corr)
         {
              char strName[255];
              sprintf(strName, "t2t6_T_wc");
              fht2t6T_corr  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	     	     
        if(NULL == fht3t7T_corr )
         {
              char strName[255];
              sprintf(strName, "t3t7_T_wc");
              fht3t7T_corr  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	    	     
        if(NULL == fht4t8T_corr )
         {
              char strName[255];
              sprintf(strName, "t4t8_T_wc");
              fht4t8T_corr  = new TH1F(strName, "",fhTbin, fhTmin, fhTmax);  			 
	     }	     
	    	     
	     
    Int_t nDet = 0;    
    Int_t nHits = fCalItems->GetEntries();   
      
        
    for (Int_t ihit = 0; ihit < nHits; ihit++)  
    {   	
      
	  R3BLosCalData *calItem = (R3BLosCalData*)fCalItems->At(ihit);

      nDet = calItem->GetDetector();

/*  
 cout<<" VFTX   1-4 "<<ihit<<", "<<calItem->fTimeV_t_ns<<", "<<calItem->fTimeV_lt_ns<<", "<<calItem->fTimeV_l_ns<<", "<<calItem->fTimeV_lb_ns<<endl;
 cout<<"TAMEX L 1-4 "<<ihit<<", "<<calItem->fTimeL_t_ns<<", "<<calItem->fTimeL_lt_ns<<", "<<calItem->fTimeL_l_ns<<", "<<calItem->fTimeL_lb_ns<<endl;
 cout<<"TAMEX T 1-4 "<<ihit<<", "<<calItem->fTimeT_t_ns<<", "<<calItem->fTimeT_lt_ns<<", "<<calItem->fTimeT_l_ns<<", "<<calItem->fTimeT_lb_ns<<endl;
 cout<<" VFTX   5-8 "<<ihit<<", "<<calItem->fTimeV_b_ns<<", "<<calItem->fTimeV_rb_ns<<", "<<calItem->fTimeV_r_ns<<", "<<calItem->fTimeV_rt_ns<<endl;
 cout<<"TAMEX L 5-8 "<<ihit<<", "<<calItem->fTimeL_b_ns<<", "<<calItem->fTimeL_rb_ns<<", "<<calItem->fTimeL_r_ns<<", "<<calItem->fTimeL_rt_ns<<endl;
 cout<<"TAMEX T 5-8 "<<ihit<<", "<<calItem->fTimeT_b_ns<<", "<<calItem->fTimeT_rb_ns<<", "<<calItem->fTimeT_r_ns<<", "<<calItem->fTimeT_rt_ns<<endl;
*/
         if(ihit == 0) 
         {
 // VFTX: 
           if(!(IS_NAN(calItem->fTimeV_r_ns))) time_r_V = calItem->fTimeV_r_ns;
           if(!(IS_NAN(calItem->fTimeV_t_ns))) time_t_V = calItem->fTimeV_t_ns;
           if(!(IS_NAN(calItem->fTimeV_l_ns))) time_l_V = calItem->fTimeV_l_ns;
           if(!(IS_NAN(calItem->fTimeV_b_ns))) time_b_V = calItem->fTimeV_b_ns;  
           if(!(IS_NAN(calItem->fTimeV_rt_ns))) time_rt_V = calItem->fTimeV_rt_ns;
           if(!(IS_NAN(calItem->fTimeV_lt_ns))) time_lt_V = calItem->fTimeV_lt_ns;
           if(!(IS_NAN(calItem->fTimeV_lb_ns))) time_lb_V = calItem->fTimeV_lb_ns;
           if(!(IS_NAN(calItem->fTimeV_rb_ns))) time_rb_V = calItem->fTimeV_rb_ns; 
          
 // cout<<"VFTX in "<<calItem->fTimeV_t_ns<<", "<<calItem->fTimeV_lt_ns<<", "<<calItem->fTimeV_l_ns<<", "<<calItem->fTimeV_lb_ns<<", "<<calItem->fTimeV_b_ns<<", "<<calItem->fTimeV_rb_ns<<", "<<calItem->fTimeV_r_ns<<", "<<calItem->fTimeV_rt_ns<<endl;          
	    // }
	   //  else if(ihit == 1)
	    // {                 
 // TAMEX:         
           if(!(IS_NAN(calItem->fTimeL_r_ns))) time_r_L = calItem->fTimeL_r_ns;
           if(!(IS_NAN(calItem->fTimeL_t_ns))) time_t_L = calItem->fTimeL_t_ns;
           if(!(IS_NAN(calItem->fTimeL_l_ns))) time_l_L = calItem->fTimeL_l_ns;
           if(!(IS_NAN(calItem->fTimeL_b_ns))) time_b_L = calItem->fTimeL_b_ns;
           if(!(IS_NAN(calItem->fTimeL_rt_ns))) time_rt_L = calItem->fTimeL_rt_ns;
           if(!(IS_NAN(calItem->fTimeL_lt_ns))) time_lt_L = calItem->fTimeL_lt_ns;
           if(!(IS_NAN(calItem->fTimeL_lb_ns))) time_lb_L = calItem->fTimeL_lb_ns;
           if(!(IS_NAN(calItem->fTimeL_rb_ns))) time_rb_L = calItem->fTimeL_rb_ns;
 //cout<<"TAMEX L in "<<calItem->fTimeL_t_ns<<", "<<calItem->fTimeL_lt_ns<<", "<<calItem->fTimeL_l_ns<<", "<<calItem->fTimeL_lb_ns<<", "<<calItem->fTimeL_b_ns<<", "<<calItem->fTimeL_rb_ns<<", "<<calItem->fTimeL_r_ns<<", "<<calItem->fTimeL_rt_ns<<endl;
	              
           if(!(IS_NAN(calItem->fTimeT_r_ns))) time_r_T = calItem->fTimeT_r_ns;
           if(!(IS_NAN(calItem->fTimeT_t_ns))) time_t_T = calItem->fTimeT_t_ns;  
           if(!(IS_NAN(calItem->fTimeT_l_ns))) time_l_T = calItem->fTimeT_l_ns; 
           if(!(IS_NAN(calItem->fTimeT_b_ns))) time_b_T = calItem->fTimeT_b_ns;  
           if(!(IS_NAN(calItem->fTimeT_rt_ns))) time_rt_T = calItem->fTimeT_rt_ns;
           if(!(IS_NAN(calItem->fTimeT_lt_ns))) time_lt_T = calItem->fTimeT_lt_ns;  
           if(!(IS_NAN(calItem->fTimeT_lb_ns))) time_lb_T = calItem->fTimeT_lb_ns; 
           if(!(IS_NAN(calItem->fTimeT_rb_ns))) time_rb_T = calItem->fTimeT_rb_ns;  
           
 
// cout<<"TAMEX T in "<<calItem->fTimeT_t_ns<<", "<<calItem->fTimeT_lt_ns<<", "<<calItem->fTimeT_l_ns<<", "<<calItem->fTimeT_lb_ns<<", "<<calItem->fTimeT_b_ns<<", "<<calItem->fTimeT_rb_ns<<", "<<calItem->fTimeT_r_ns<<", "<<calItem->fTimeT_rt_ns<<endl;         
	     }   
                   
         if (!calItem) 
         {
		   cout<<" !calItem"<<endl;	 
           continue; // can this happen?
         }
  
     }
// Determine which type of detector is analyzed (square or octagonal):
      UInt_t iLOSTypeMCFD = 0;
	  UInt_t iLOSTypeTAMEX = 0;
	  UInt_t iLOSType = 0;
	  UInt_t Igood_event = 0;
	  	 
      if(time_r_V > 0. && !(IS_NAN(time_r_V)) && time_t_V > 0. && !(IS_NAN(time_t_V)) && time_l_V > 0. && !(IS_NAN(time_l_V)) && time_b_V > 0. && !(IS_NAN(time_b_V)) 
      && IS_NAN(time_rt_V) && IS_NAN(time_rb_V) && IS_NAN(time_lt_V) && IS_NAN(time_lb_V))
      {        
         iLOSTypeMCFD = 1; // 4PM LOS
      }
      if(time_r_V > 0. && !(IS_NAN(time_r_V)) && time_t_V > 0. && !(IS_NAN(time_t_V)) && time_l_V > 0. && !(IS_NAN(time_l_V)) && time_b_V > 0. && !(IS_NAN(time_b_V)) 
      && !(IS_NAN(time_rt_V)) && !(IS_NAN(time_rb_V)) && !(IS_NAN(time_lt_V)) && !(IS_NAN(time_lb_V))
      && time_rt_V > 0. && time_lt_V > 0. && time_lb_V > 0. && time_rb_V > 0.)
      {        
         iLOSTypeMCFD = 2; // 8PM LOS
      } 
      if(time_r_L > 0. && !(IS_NAN(time_r_L)) && time_t_L > 0. && !(IS_NAN(time_t_L)) && time_l_L > 0. && !(IS_NAN(time_l_L)) && time_b_L > 0. && !(IS_NAN(time_b_L))
      && time_r_T > 0. && !(IS_NAN(time_r_T)) && time_t_T > 0. && !(IS_NAN(time_t_T)) && time_l_T > 0. && !(IS_NAN(time_l_T)) && time_b_T > 0. && !(IS_NAN(time_b_T))
      && IS_NAN(time_rt_L) && IS_NAN(time_rb_L) && IS_NAN(time_lt_L) && IS_NAN(time_lb_L))
      {        
         iLOSTypeTAMEX = 1;  // 4PM LOS
      }
      if(time_r_L > 0. && !(IS_NAN(time_r_L)) && time_t_L > 0. && !(IS_NAN(time_t_L)) && time_l_L > 0. && !(IS_NAN(time_l_L)) && time_b_L > 0. && !(IS_NAN(time_b_L))
      && time_r_T > 0. && !(IS_NAN(time_r_T)) && time_t_T > 0. && !(IS_NAN(time_t_T)) && time_l_T > 0. && !(IS_NAN(time_l_T)) && time_b_T > 0. && !(IS_NAN(time_b_T))
      && !(IS_NAN(time_rt_L)) && !(IS_NAN(time_rb_L)) && !(IS_NAN(time_lt_L)) && !(IS_NAN(time_lb_L))
      && time_rt_L > 0. && time_lt_L > 0. && time_lb_L > 0. && time_rb_L > 0.
      && !(IS_NAN(time_rt_T)) && !(IS_NAN(time_rb_T)) && !(IS_NAN(time_lt_T)) && !(IS_NAN(time_lb_T))
      && time_rt_T > 0. && time_lt_T > 0. && time_lb_T > 0. && time_rb_T > 0.)
      {        
         iLOSTypeTAMEX = 2; // 8PM LOS       
      }     

      
// We will consider only events in which booth MCFD and TAMEX see same number of channels:
      if(iLOSTypeTAMEX == 1 && iLOSTypeMCFD == 1) iLOSType = 1;
      if(iLOSTypeTAMEX == 2 && iLOSTypeMCFD == 2) iLOSType = 2;
   //   if(iLOSTypeMCFD == 1) iLOSType = 1;
   //   if(iLOSTypeMCFD == 2) iLOSType = 2;           
      
//    cout<<"ILOSType: "<< iLOSTypeMCFD<<", "<<iLOSTypeTAMEX<<endl;      
//    cout<<"VFTX 1-8 "<<time_t_V<<", "<<time_lt_V<<", "<<time_l_V<<", "<<time_lb_V<<", "<<time_b_V<<", "<<time_rb_V<<", "<<time_r_V<<", "<<time_rt_V<<endl;          
//    cout<<"TAMEX L 1-8 "<<time_t_L<<", "<<time_lt_L<<", "<<time_l_L<<", "<<time_lb_L<<", "<<time_b_L<<", "<<time_rb_L<<", "<<time_r_L<<", "<<time_rt_L<<endl;     
//    cout<<"TAMEX T 1-8 "<<time_t_T<<", "<<time_lt_T<<", "<<time_l_T<<", "<<time_lb_T<<", "<<time_b_T<<", "<<time_rb_T<<", "<<time_r_T<<", "<<time_rt_T<<endl;  
             
     
     // Calculate ToT and walk corrections for channels 1=T, 3=L, 5=B, 7=R               	      
         if(iLOSType == 1 || iLOSType == 2) 
         { 
		// calculate time over threshold and check if clock counter went out of range          
		    while(time_r_T - time_r_L <0.) {
	          time_r_T=time_r_T+2048.*fClockFreq; 
	        }  
            while(time_l_T - time_l_L <0.) {
	          time_l_T=time_l_T+2048.*fClockFreq; 
	        }	      
            while(time_t_T - time_t_L <0.) {
	          time_t_T=time_t_T+2048.*fClockFreq; 
	        }	                 
            while(time_b_T - time_b_L <0.) {
	          time_b_T=time_b_T+2048.*fClockFreq; 
	        }
	         totr = time_r_T - time_r_L;
             totl = time_l_T - time_l_L;
             tott = time_t_T - time_t_L;
             totb = time_b_T - time_b_L;            

             
             tot_ver = (tott+totb)/2.;
             tot_hor = (totr+totl)/2.;         

             	    
          } // end if(iLOSType == 1 || iLOSType == 2)
           
     // Calculate ToT for channels 2=LT, 4=LB, 6=RB, 8=RT  
         if(iLOSType == 2) 
         {
		// calculate time over threshold and check if clock counter went out of range  
			while(time_rt_T - time_rt_L <0.) {
	          time_rt_T=time_rt_T+2048.*fClockFreq; 
	        }  
            while(time_lt_T - time_lt_L <0.) {
	          time_lt_T=time_lt_T+2048.*fClockFreq; 
	        }	      
            while(time_rb_T - time_rb_L <0.) {
	          time_rb_T=time_rb_T+2048.*fClockFreq; 
	        }	                 
            while(time_lb_T - time_lb_L <0.) {
	          time_lb_T=time_lb_T+2048.*fClockFreq; 
	        }
	        if(!(IS_NAN(time_pix_L))){ 	        
	         while(time_pix_T - time_pix_L <0.) {
	           time_pix_T=time_pix_T+2048.*fClockFreq; 
	         }
	        }
	        totrb = time_rb_T - time_rb_L;
            totlt = time_lt_T - time_lt_L;
            totrt = time_rt_T - time_rt_L;
            totlb = time_lb_T - time_lb_L; 
            totpix = time_pix_T - time_pix_L;
            fhQpix->Fill(totpix);
             
        
            tot_135 = (totrt+totlb)/2.;
            tot_45 =  (totlt+totrb)/2.; 
          
		    
 // Spectra for performing walk corrections are filled:
 // For squared detector: 4 PMs signal + 4 reference signals
 // Difference between measured time of a single PM and a reference signal
 //  is plotted as a function of ToT of a given PM.
 // For octagonal detector, two runs are  made - once ref signal is put to channel 1 and once to channel 8.
 	           
         if(iSciType == 0 || iSciType ==2 || iSciType ==4)
         {
           ref_timeM=(time_rb_V + time_lt_V +time_rt_V + time_lb_V)/4.+33.;
		   ref_timeT=(time_rb_L + time_lt_L+time_rt_L + time_lb_L)/4.+27.;
         }
         else
         {
			ref_timeM=time_ref_V+43.; 
			ref_timeT=time_ref_L+43.;
			
			//ref_timeM=time_ref_V+48.; 
			//ref_timeT=time_ref_L+48.;			
		  }	 
  
      /*           
           if(tott>walk_par[0][0] && totlt>walk_par[1][0] && totl>walk_par[2][0] && 
              totlb>walk_par[3][0] && totb>walk_par[4][0] && totrb>walk_par[5][0] && 
              totr>walk_par[6][0] && totrt>walk_par[7][0]) Igood_event = 1;
       */       
              
           Igood_event = 1;  
              
  // This is for 12C beam:
          //if((tot_ver+tot_hor+tot_45+tot_135)/4. > 53.4 && (tot_ver+tot_hor+tot_45+tot_135)/4. < 59.1) Igood_event = 1;
         //   if(tott>18.5 && totlt>18.2 && totl>20.5 && totlb>18.2 && totb>18.2 && totrb>21.0 && totr>19.0 && totrt>17. &&
         //      tott<23.2 && totlt<24.5 && totl<24.0 && totlb<24.0 && totb<22.4 && totrb<24.5 && totr<22.6 && totrt<23.) Igood_event = 1;
              
		           		
		// MCFD 1,3,5,7 
		   fht1Q1M->Fill(tott,(time_t_V-time_b_V));
	       fht3Q3M->Fill(totl,(time_l_V-time_r_V));
	       fht5Q5M->Fill(totb,(time_b_V-time_t_V));	  
		   fht7Q7M->Fill(totr,(time_r_V-time_l_V));
	    // MCFD 2,4,6,8   
		   fht2Q2M->Fill(totlt,(time_lt_V-time_rb_V));
		   fht4Q4M->Fill(totlb,(time_lb_V-time_rt_V));	  
		   fht6Q6M->Fill(totrb,(time_rb_V-time_lt_V));
		   fht8Q8M->Fill(totrt,(time_rt_V-time_lb_V));
		// TAMEX 1,3,5,7 
		   fht1Q1T->Fill(tott,(time_t_L-ref_timeT));
	       fht3Q3T->Fill(totl,(time_l_L-ref_timeT));
	       fht5Q5T->Fill(totb,(time_b_L-ref_timeT));	  
		   fht7Q7T->Fill(totr,(time_r_L-ref_timeT));
	    // TAMEX 2,4,6,8   
		   fht2Q2T->Fill(totlt,(time_lt_L-ref_timeT));
		   fht4Q4T->Fill(totlb,(time_lb_L-ref_timeT));	  
		   fht6Q6T->Fill(totrb,(time_rb_L-ref_timeT));
		   fht8Q8T->Fill(totrt,(time_rt_L-ref_timeT));
		     
/*
      	 if(!(IS_NAN(ref_timeM)) && !(IS_NAN(ref_timeT)))
      	 {
			myFile<<setprecision(10)<<"1 "<<time_t_V<<" "<<time_t_L<<" "<<tott<<endl;
			myFile<<setprecision(10)<<"2 "<<time_lt_V<<" "<<time_lt_L<<" "<<totlt<<endl;
			myFile<<setprecision(10)<<"3 "<<time_l_V<<" "<<time_l_L<<" "<<totl<<endl;
			myFile<<setprecision(10)<<"4 "<<time_lb_V<<" "<<time_lb_L<<" "<<totlb<<endl;
			myFile<<setprecision(10)<<"5 "<<time_b_V<<" "<<time_b_L<<" "<<totb<<endl;
			myFile<<setprecision(10)<<"6 "<<time_rb_V<<" "<<time_rb_L<<" "<<totrb<<endl;
	 one really produces data so it's ok to throw all
        * detector hits into the same list (and hence traversing a longer
        * list than strictly necessary for the reconstruction)		myFile<<setprecision(10)<<"7 "<<time_r_V<<" "<<time_r_L<<" "<<totr<<endl;
			myFile<<setprecision(10)<<"8 "<<time_rt_V<<" "<<time_rt_L<<" "<<totrt<<endl;
			myFile<<setprecision(10)<<"9 "<<ref_timeM<<" "<<ref_timeT<<" "<<
			      (tott+totlt+totl+totlb+totb+totrb+totr+totrt)/8. <<endl;
          }
*/
            	        	       
      } // end if(iLOSType == 2) 	
 
 
 
 
 // Next, times are corrected for walk.
 
     if(iLOSType == 1 )  // Squared scintillator
     {   
           
        //  if(tott>walk_par[0][0] && totl>walk_par[2][0] && totb>walk_par[4][0] && totr>walk_par[6][0]) Igood_event = 1;
                           
          Igood_event = 1;  
                   
  // Walk correction for MCFD
   // PM1 
             
             time_t_V_corr = time_t_V - walk(0,tott); 
                      
   // PM2 
                                                     
             time_l_V_corr = time_l_V - walk(2,totl);
             
   // PM3                                                                                            
             time_b_V_corr = time_b_V - walk(4,totb);
                                      
   // PM4             
                                                    
             time_r_V_corr = time_r_V - walk(6,totr);     
		     
		     
 // Walk correction for TAMEX
	      
   // PM1 
                                                    
             time_t_L_corr = time_t_L - walk(8,tott); 
                                   
   // PM2 
             time_l_L_corr = time_l_L - walk(10,totl);
                          
   // PM3            
             time_b_L_corr = time_b_L - walk(12,totb);
             
   // PM4             
             time_r_L_corr = time_r_L - walk(14,totr);
		     
	     
    // Walk-corrected position, for test purposes:    
    
         dx1 = (time_l_V_corr-time_r_V_corr) ;         
         dy1 = (time_b_V_corr-time_t_V_corr) ;       
         
         
         if(tott < walk_par[0][0] || tott > walk_par[0][1]) dy1 = 0.0/0.0; 		     
         if(totl < walk_par[2][0] || totl > walk_par[2][1]) dx1 = 0.0/0.0;
               	       
      } // end if(iLOSType == 1)               

    if(iLOSType == 2)  // Octogonal scintillator
    {	           
                            
  // Walk correction for MCFD
    // PM1             
             time_t_V_corr = time_t_V - walk(0,tott); 
                       
    // PM2             
             time_lt_V_corr = time_lt_V - walk(1,totlt);  
                          
    // PM3                                                     
             time_l_V_corr = time_l_V - walk(2,totl);
             
    // PM4                                                                                             
             time_lb_V_corr = time_lb_V - walk(3,totlb);
             
    // PM5                                                                
             time_b_V_corr = time_b_V - walk(4,totb);
                          
    // PM6                                                                 
             time_rb_V_corr = time_rb_V - walk(5,totrb);
             
    // PM7                                                                 
             time_r_V_corr = time_r_V - walk(6,totr);
             
    // PM8                                                                 
             time_rt_V_corr = time_rt_V - walk(7,totrt);	     
		     
		     
 // Walk correction for TAMEX
	      
    // PM1                                                     
             time_t_L_corr = time_t_L - walk(8,tott); 
                                   
    // PM2                                                      
             time_lt_L_corr = time_lt_L - walk(9,totlt);               
                    
    // PM3                                                                                 
             time_l_L_corr = time_l_L - walk(10,totl);
                          
    // PM4                                                                        
             time_lb_L_corr = time_lb_L - walk(11,totlb);
                      
    // PM5                          
             time_b_L_corr = time_b_L - walk(12,totb);
                          
    // PM6                         
             time_rb_L_corr = time_rb_L - walk(13,totrb);                          
             
    // PM7                          
             time_r_L_corr = time_r_L - walk(14,totr);
		         
    // PM8                          
             time_rt_L_corr = time_rt_L - walk(15,totrt);
		     
	     
    // Walk-corrected position, for test purposes:    
    
         dx1 = (time_l_V_corr-time_r_V_corr) ;         
         dy1 = (time_b_V_corr-time_t_V_corr) ;       
         dx2 = (time_lt_V_corr-time_rb_V_corr) ;         
         dy2 = (time_lb_V_corr-time_rt_V_corr) ;
         
         if(tott < walk_par[0][0] || tott > walk_par[0][1]) dy1 = 0.0/0.0; 		     
         if(totl < walk_par[2][0] || totl > walk_par[2][1]) dx1 = 0.0/0.0;
         if(totlb < walk_par[3][0] || totlb > walk_par[3][1]) dy2 = 0.0/0.0;
         if(totlt < walk_par[1][0] || totlt > walk_par[1][1]) dx2 = 0.0/0.0;

         
        if(Igood_event == 1){  
			         
		// MCFD 1,3,5,7	 
 	      fht1Q1M_corr->Fill(tott,time_t_V_corr-ref_timeM);
		  fht3Q3M_corr->Fill(totl,time_l_V_corr-ref_timeM);
		  fht5Q5M_corr->Fill(totb,time_b_V_corr-ref_timeM);
	      fht7Q7M_corr->Fill(totr,time_r_V_corr-ref_timeM);
	      
		 // MCFD 2,4,6,8 
		  fht2Q2M_corr->Fill(totlt,time_lt_V_corr-ref_timeM);
		  fht4Q4M_corr->Fill(totlb,time_lb_V_corr-ref_timeM);
		  fht6Q6M_corr->Fill(totrb,time_rb_V_corr-ref_timeM); 
		  fht8Q8M_corr->Fill(totrt,time_rt_V_corr-ref_timeM);

		// TAMEX 1,3,5,7	 
 	      fht1Q1T_corr->Fill(tott,time_t_L_corr-ref_timeT);
		  fht3Q3T_corr->Fill(totl,time_l_L_corr-ref_timeT);
		  fht5Q5T_corr->Fill(totb,time_b_L_corr-ref_timeT);
	      fht7Q7T_corr->Fill(totr,time_r_L_corr-ref_timeT);
	      
		 // TAMEX 2,4,6,8 
		  fht2Q2T_corr->Fill(totlt,time_lt_L_corr-ref_timeT);
		  fht4Q4T_corr->Fill(totlb,time_lb_L_corr-ref_timeT);
		  fht6Q6T_corr->Fill(totrb,time_rb_L_corr-ref_timeT); 
		  fht8Q8T_corr->Fill(totrt,time_rt_L_corr-ref_timeT);
		  	
		// Sum of times
		   fht1t5M->Fill((time_t_V+time_b_V)/2.-ref_timeM);
		   fht2t6M->Fill((time_lt_V+time_rb_V)/2.-ref_timeM);
		   fht3t7M->Fill((time_r_V+time_l_V)/2.-ref_timeM);
		   fht4t8M->Fill((time_rt_V+time_lb_V)/2.-ref_timeM);
		   
		   fht1t5T->Fill((time_t_L+time_b_L)/2.-ref_timeT);
		   fht2t6T->Fill((time_lt_L+time_rb_L)/2.-ref_timeT);
		   fht3t7T->Fill((time_r_L+time_l_L)/2.-ref_timeT);
		   fht4t8T->Fill((time_rt_L+time_lb_L)/2.-ref_timeT);
		   		
		   fht1t5M_corr->Fill((time_t_V_corr+time_b_V_corr)/2.-ref_timeM);
		   fht2t6M_corr->Fill((time_lt_V_corr+time_rb_V_corr)/2.-ref_timeM);
		   fht3t7M_corr->Fill((time_r_V_corr+time_l_V_corr)/2.-ref_timeM);
		   fht4t8M_corr->Fill((time_rt_V_corr+time_lb_V_corr)/2.-ref_timeM);
		  
		   fht1t5T_corr->Fill((time_t_L_corr+time_b_L_corr)/2.-ref_timeT);
		   fht2t6T_corr->Fill((time_lt_L_corr+time_rb_L_corr)/2.-ref_timeT);
		   fht3t7T_corr->Fill((time_r_L_corr+time_l_L_corr)/2.-ref_timeT);
		   fht4t8T_corr->Fill((time_rt_L_corr+time_lb_L_corr)/2.-ref_timeT);
           }  
            	        	       
      } // end if(iLOSType == 2) 	
	            	  
   if(iLOSType == 1 || iLOSType == 2 ) 
     {
	// TAMEX:		  
		   t_hor_T = (time_r_L + time_l_L)/2.;       		  
           t_ver_T = (time_t_L + time_b_L)/2.; 
           
 		   t_hor_T_corr = (time_r_L_corr + time_l_L_corr)/2.;       		  
           t_ver_T_corr = (time_t_L_corr + time_b_L_corr)/2.;          
           
		   x1T_cm=(time_l_L-time_r_L-flosOffsetX1T)*flosVeffX1T;
		   y1T_cm=(time_b_L-time_t_L-flosOffsetY1T)*flosVeffY1T;
		   		   
		   x1T_cm_corr=(time_l_L_corr-time_r_L_corr);
		   y1T_cm_corr=(time_b_L_corr-time_t_L_corr);	
		   
	// MCFD:	           
           t_hor_M = (time_r_V + time_l_V)/2.; 		  
	       t_ver_M = (time_t_V + time_b_V)/2.;
           t_hor_M_corr = (time_r_V_corr + time_l_V_corr)/2.; 		  
	       t_ver_M_corr = (time_t_V_corr + time_b_V_corr)/2.;
	       
	       Double_t  x_cm_temp,y_cm_temp;
	       x1_cm=(time_l_V-time_r_V-flosOffsetX1)*flosVeffX1;
	 	   y1_cm=(time_b_V-time_t_V-flosOffsetY1)*flosVeffY1;  
	 	    
/*	 	   
	 	   y1_cm = y_cm_temp*cos(0.1714)-x_cm_temp*sin(0.1714);
		   x1_cm = x_cm_temp*cos(0.1714)+y_cm_temp*sin(0.1714);   	       
*/            	   
           if(!(IS_NAN(dx1)) && !(IS_NAN(dy1))){ 
            x1_cm_corr=(dx1-flosOffsetX1wc)*flosVeffX1wc; 
		    y1_cm_corr=(dy1-flosOffsetY1wc)*flosVeffY1wc;
	       }

		  }	  
 
     if(iLOSType == 2 ) 
     {
	// TAMEX:		  
            t_45_T = (time_rb_L + time_lt_L)/2.; 		  
	        t_135_T = (time_rt_L + time_lb_L)/2.; 
    		x2T_cm=(time_lt_L-time_rb_L-flosOffsetX2T)*flosVeffX2T;
		    y2T_cm=(time_lb_L-time_rt_L-flosOffsetY2T)*flosVeffY2T;	
		    

    // MCFD:	        
	        t_45_M  = (time_rb_V + time_lt_V)/2.; 		  
	        t_135_M = (time_rt_V + time_lb_V)/2.; 
	        
            x2_cm=(time_lt_V-time_rb_V-flosOffsetX2)*flosVeffX2;
            y2_cm=(time_lb_V-time_rt_V-flosOffsetY2)*flosVeffY2;
                       
            if(!(IS_NAN(dx2)) && !(IS_NAN(dy2))){        
 		   	 x2_cm_corr=(dx2-flosOffsetX2wc)*flosVeffX2wc;
		     y2_cm_corr=(dy2-flosOffsetY2wc)*flosVeffY2wc;
		    }	   		    	
    		
	  }	 
	      
	   if(iLOSType == 1) 
       {
         // TAMEX 
          timeLosT = (t_hor_T + t_ver_T)/2.;
          LosTresT = (time_r_L + time_l_L)/2.-(time_t_L + time_b_L)/2.; 		 
          LosTresT_corr = (time_r_L_corr + time_l_L_corr)/2.-(time_t_L_corr + time_b_L_corr)/2.;		 
          xT_cm = x1T_cm;
          yT_cm = y1T_cm;
          xT_cm_corr = x1T_cm_corr;
          yT_cm_corr = y1T_cm_corr;          
         // MCFD 
          timeLosM = (time_r_V + time_l_V + time_t_V + time_b_V)/4.;
          LosTresM = (time_r_V + time_l_V)/2.-(time_t_V + time_b_V)/2.;		  
          LosTresM_corr = (time_r_V_corr + time_l_V_corr)/2.-(time_t_V_corr + time_b_V_corr)/2.;
          x_cm = x1_cm;
	      y_cm = y1_cm;	
          x_cm_corr = x1_cm_corr;
	      y_cm_corr = y1_cm_corr;


  // Raumwinkel und Radius

           xvar = x_cm;
           yvar = y_cm;

           rr_b = sqrt((l_SC/2.+yvar)*(l_SC/2.+yvar)+xvar*xvar);
           rr_t = sqrt((l_SC/2.-yvar)*(l_SC/2.-yvar)+xvar*xvar);   
           rr_l = sqrt((l_SC/2.-xvar)*(l_SC/2.-xvar)+yvar*yvar); 
           rr_r = sqrt((l_SC/2.+xvar)*(l_SC/2.+xvar)+yvar*yvar);              		   		   
           rw_t = atan((abs(xvar)+l_PM/2.)/(l_SC/2.-yvar))-atan((abs(xvar)-l_PM/2.)/(l_SC/2.-yvar));           
           rw_b = atan((abs(xvar)+l_PM/2.)/(l_SC/2.+yvar))-atan((abs(xvar)-l_PM/2.)/(l_SC/2.+yvar));
           rw_l = atan((abs(yvar)+l_PM/2.)/(l_SC/2.-xvar))-atan((abs(yvar)-l_PM/2.)/(l_SC/2.-xvar));           
           rw_r = atan((abs(yvar)+l_PM/2.)/(l_SC/2.+xvar))-atan((abs(yvar)-l_PM/2.)/(l_SC/2.+xvar));       

           totl_corr = satu(2,totl);
           totr_corr = satu(6,totr);           
           totb_corr = satu(4,totb); 
           tott_corr = satu(0,tott);            
                      
    	   Double_t tot_ver_corr = (tott_corr+totb_corr)/2.;
           Double_t tot_hor_corr = (totr_corr+totl_corr)/2.;


		   rw_sum = (rw_t+rw_l+rw_b+rw_r)*180./3.14159265;	   
		   fhRW->Fill(rw_sum);             	

		if(!(IS_NAN(tott_corr)) && !(IS_NAN(totl_corr)) &&
           !(IS_NAN(totb_corr)) && !(IS_NAN(totr_corr)) && 
           tott_corr > 0. && totl_corr > 0. && totb_corr > 0. && 
           totr_corr > 0. )
        {  		    
           tot_corr = (tott_corr +totl_corr + totb_corr + totr_corr)/4.;                       
        } 	  
           x1ToT_cm = ((totl-totr)/tot-flosOffsetX1Q)*flosVeffX1Q;
           y1ToT_cm = ((tott-totb)/tot-flosOffsetY1Q)*flosVeffY1Q;
           xToT = x1ToT_cm;
           yToT = y1ToT_cm;
	   }  
	    	   
       if(iLOSType == 2)           
       {          

     // in case of reference signal on channels 2, 4, 6, 8:	
          
       if(iSciType == 0 || iSciType == 2 || iSciType == 4)
       {                	   
          tot = (tot_ver+tot_hor)/2.;
         // TAMEX 
          timeLosT = (t_hor_T + t_ver_T)/2.;
          LosTresT = (time_r_L + time_l_L)/2.-(time_t_L + time_b_L)/2.; 		 
          LosTresT_corr = (time_r_L_corr + time_l_L_corr)/2.-(time_t_L_corr + time_b_L_corr)/2.;		 
          xT_cm = x1T_cm;
          yT_cm = y1T_cm;
          xT_cm_corr = x1T_cm_corr;
          yT_cm_corr = y1T_cm_corr;          
         // MCFD 
          timeLosM = (time_r_V + time_l_V + time_t_V + time_b_V)/4.;
          LosTresM = (time_r_V + time_l_V)/2.-(time_t_V + time_b_V)/2.;		  
          LosTresM_corr = (time_r_V_corr + time_l_V_corr)/2.-(time_t_V_corr + time_b_V_corr)/2.;
          x_cm = x1_cm;
	      y_cm = y1_cm;	
          x_cm_corr = x1_cm_corr;
	      y_cm_corr = y1_cm_corr;


  // Raumwinkel und Radius

           xvar = x_cm;
           yvar = y_cm;

           rr_b = sqrt((l_SC/2.+yvar)*(l_SC/2.+yvar)+xvar*xvar);
           rr_t = sqrt((l_SC/2.-yvar)*(l_SC/2.-yvar)+xvar*xvar);   
           rr_l = sqrt((l_SC/2.-xvar)*(l_SC/2.-xvar)+yvar*yvar); 
           rr_r = sqrt((l_SC/2.+xvar)*(l_SC/2.+xvar)+yvar*yvar);              		   		   
           rw_t = atan((abs(xvar)+l_PM/2.)/(l_SC/2.-yvar))-atan((abs(xvar)-l_PM/2.)/(l_SC/2.-yvar));           
           rw_b = atan((abs(xvar)+l_PM/2.)/(l_SC/2.+yvar))-atan((abs(xvar)-l_PM/2.)/(l_SC/2.+yvar));
           rw_l = atan((abs(yvar)+l_PM/2.)/(l_SC/2.-xvar))-atan((abs(yvar)-l_PM/2.)/(l_SC/2.-xvar));           
           rw_r = atan((abs(yvar)+l_PM/2.)/(l_SC/2.+xvar))-atan((abs(yvar)-l_PM/2.)/(l_SC/2.+xvar));       
 
           totl_corr = satu(2,totl);
           totr_corr = satu(6,totr);           
           totb_corr = satu(4,totb); 
           tott_corr = satu(0,tott);         
                      
    	   Double_t tot_ver_corr = (tott_corr+totb_corr)/2.;
           Double_t tot_hor_corr = (totr_corr+totl_corr)/2.;


		   rw_sum = (rw_t+rw_l+rw_b+rw_r)*180./3.14159265;	   
		   fhRW->Fill(rw_sum);             	

 		if(!(IS_NAN(tott_corr)) && !(IS_NAN(totl_corr)) &&
           !(IS_NAN(totb_corr)) && !(IS_NAN(totr_corr)) && 
           tott_corr > 0. && totl_corr > 0. && totb_corr > 0. &&  totr_corr > 0. )
        {  		    
           tot_corr = (tott_corr +totl_corr + totb_corr + totr_corr)/4.;                       
        }  
           x1ToT_cm = ((totl-totr)/tot-flosOffsetX1Q)*flosVeffX1Q;
           y1ToT_cm = ((tott-totb)/tot-flosOffsetY1Q)*flosVeffY1Q;
           xToT = x1ToT_cm;
           yToT = y1ToT_cm;
           
        }   
  // end squared
               
       if(iSciType == 1 || iSciType == 3 || iSciType == 5 || iSciType == 6)
       { 		  
// if octagon  
		          
          // TAMEX:                 
          timeLosT = (t_hor_T + t_ver_T + t_45_T + t_135_T)/4.;
          LosTresT = (time_r_L  + time_l_L  + time_t_L  +  time_b_L)/4.-
                          (time_rb_L + time_lt_L + time_rt_L + time_lb_L)/4.;         
        if(time_r_L_corr > 0. && !(IS_NAN(time_r_L_corr)) && time_t_L_corr > 0. && !(IS_NAN(time_t_L_corr)) && time_l_L_corr > 0. 
            && !(IS_NAN(time_l_L_corr)) && time_b_L_corr > 0. && !(IS_NAN(time_b_L_corr)) 
            && !(IS_NAN(time_rt_L_corr)) && !(IS_NAN(time_rb_L_corr)) && !(IS_NAN(time_lt_L_corr)) && !(IS_NAN(time_lb_L_corr))
            && time_rt_L_corr > 0. && time_lt_L_corr > 0. && time_lb_L_corr > 0. && time_rb_L_corr > 0.)
      {         
          LosTresT_corr = (time_r_L_corr  + time_l_L_corr  + time_t_L_corr  +  time_b_L_corr)/4.-
                          (time_rb_L_corr + time_lt_L_corr + time_rt_L_corr+ time_lb_L_corr)/4.; 
                       
      }
      //    xT_cm = (x1T_cm + (x2T_cm/sqrt(2.)-y2T_cm/sqrt(2.)))/2.;
      //    yT_cm = (y1T_cm + (x2T_cm/sqrt(2.)+y2T_cm/sqrt(2.)))/2.;
          
  // New coordinate system since KVI (transformation for 22.5 deg):  
  /*      
          xT_cm = (x1T_cm*cos(3.141593/8.) + y1T_cm*sin(3.141593/8.) + 
                   x2T_cm*cos(3.141593/8.) - y2T_cm*sin(3.141593/8.))/2.;
                   
          yT_cm = (y1T_cm*cos(3.141593/8.) - x1T_cm*sin(3.141593/8.) +
                   x2T_cm*sin(3.141593/8.) + y2T_cm*cos(3.141593/8.))/2.;    
                   
    */               
          xT_cm = (time_l_L+time_lb_L)/2.-(time_r_L+time_rt_L)/2.;                  
          yT_cm = (time_b_L+time_rb_L)/2.-(time_t_L+time_lt_L)/2.;
          
          xT_cm=(xT_cm-1.347)*0.742;
	   	  yT_cm=(yT_cm-0.139)*0.911;
          			
         if(!(IS_NAN(tott)) && !(IS_NAN(totlt)) && !(IS_NAN(totl)) && !(IS_NAN(totlb)) &&
           !(IS_NAN(totb)) && !(IS_NAN(totrb)) && !(IS_NAN(totr)) && !(IS_NAN(totrt)) &&
           tott > 0. && totlt > 0. && totl > 0. && totlb > 0. && totb > 0. && totrb > 0. &&
           totr > 0. && totrt > 0.)
         {			
           tot = (tot_ver + tot_hor + tot_45 + tot_135)/4.;
         }	
         
          // MCFD:
          timeLosM = (t_hor_M + t_ver_M + t_45_M + t_135_M)/4.;
          LosTresM = (t_hor_M + t_ver_M)/2. - (t_45_M + t_135_M)/2.; 
          
         if(time_r_V_corr > 0. && !(IS_NAN(time_r_V_corr)) && time_t_V_corr > 0. && !(IS_NAN(time_t_V_corr)) && time_l_V_corr > 0. 
            && !(IS_NAN(time_l_V_corr)) && time_b_V_corr > 0. && !(IS_NAN(time_b_V_corr)) 
            && !(IS_NAN(time_rt_V_corr)) && !(IS_NAN(time_rb_V_corr)) && !(IS_NAN(time_lt_V_corr)) && !(IS_NAN(time_lb_V_corr))
            && time_rt_V_corr > 0. && time_lt_V_corr > 0. && time_lb_V_corr > 0. && time_rb_V_corr > 0.)
      {         
          LosTresM_corr = (time_r_V_corr  + time_l_V_corr  + time_t_V_corr  +  time_b_V_corr)/4.-
                          (time_rb_V_corr + time_lt_V_corr + time_rt_V_corr + time_lb_V_corr)/4.;
                       
      }

          Double_t y_cm_temp, x_cm_temp;

    //      x_cm_temp = (x1_cm + (x2_cm/sqrt(2.)-y2_cm/sqrt(2.)))/2.;
    //      y_cm_temp = (y1_cm + (x2_cm/sqrt(2.)+y2_cm/sqrt(2.)))/2.;
          
    //      y_cm = y_cm_temp;
    //      x_cm = x_cm_temp;
       
  // New coordinate system since KVI (transformation for 22.5 deg):         
          /*
          x_cm = (x1_cm*cos(3.141593/8.)+y1_cm*sin(3.141593/8.) + 
                  x2_cm*cos(3.141593/8.)-y2_cm*sin(3.141593/8.))/2.;
          y_cm = (y1_cm*cos(3.141593/8.)-x1_cm*sin(3.141593/8.) +
                  x2_cm*sin(3.141593/8.)+y2_cm*cos(3.141593/8.))/2.;             
           */
                  
          x_cm = (time_l_V+time_lb_V)/2.-(time_r_V+time_rt_V)/2.;                  
          y_cm = (time_b_V+time_rb_V)/2.-(time_t_V+time_lt_V)/2.;
       
          x_cm=(x_cm-0.0222)*1.870;
		  y_cm=(y_cm+0.194063)*2.238;
          
                   
  //        x_cm_corr = (x1_cm_corr + (x2_cm_corr/sqrt(2.)-y2_cm_corr/sqrt(2.)))/2.;
  //        y_cm_corr = (y1_cm_corr + (x2_cm_corr/sqrt(2.)+y2_cm_corr/sqrt(2.)))/2.;
          
          x_cm_corr = (x1_cm_corr*cos(3.141593/8.)+y1_cm_corr*sin(3.141593/8.) + 
                       x2_cm_corr*cos(3.141593/8.)-y2_cm_corr*sin(3.141593/8.))/2.;
          y_cm_corr = (y1_cm_corr*cos(3.141593/8.)-x1_cm_corr*sin(3.141593/8.) +
                       x2_cm_corr*sin(3.141593/8.)+y2_cm_corr*cos(3.141593/8.))/2.;  
          
   /*       
          if(IS_NAN(tott))  cout<<"IS_NAN(tott)"<<endl;
          if(IS_NAN(totlt)) cout<<"IS_NAN(totlt)"<<endl;
          if(IS_NAN(totl))  cout<<"IS_NAN(totl)"<<endl;          
          if(IS_NAN(totlb)) cout<<"IS_NAN(totlb)"<<endl;          
          if(IS_NAN(totb))  cout<<"IS_NAN(totb)"<<endl;          
          if(IS_NAN(totrb)) cout<<"IS_NAN(totrb)"<<endl;          
          if(IS_NAN(totr))  cout<<"IS_NAN(totr)"<<endl;          
          if(IS_NAN(totrt)) cout<<"IS_NAN(totrt)"<<endl;          
          
          if(tott == 0.0)  cout<<"tott=0"<<endl;
          if(totlt == 0.0) cout<<"totlt=0"<<endl;
          if(totl == 0.0)  cout<<"totl=0"<<endl;          
          if(totlb == 0.0) cout<<"totlb=0"<<endl;          
          if(totb == 0.0)  cout<<"totb=0"<<endl;          
          if(totrb == 0.0) cout<<"totrb=0"<<endl;          
          if(totr == 0.0)  cout<<"totr=0"<<endl;          
          if(totrt == 0.0) cout<<"totrt=0"<<endl;          
    */      
        
          
        if(!(IS_NAN(tott)) && !(IS_NAN(totlt)) && !(IS_NAN(totl)) && !(IS_NAN(totlb)) &&
           !(IS_NAN(totb)) && !(IS_NAN(totrb)) && !(IS_NAN(totr)) && !(IS_NAN(totrt)) &&
           tott > 0. && totlt > 0. && totl > 0. && totlb > 0. && totb > 0. && totrb > 0. &&
           totr > 0. && totrt > 0.)
        {
			
           tot = (tot_ver + tot_hor + tot_45 + tot_135)/4.;
     
           x1ToT_cm = ((totr-totl)/(totl+totr)-flosOffsetX1Q)*flosVeffX1Q;
           y1ToT_cm = ((tott-totb)/(tott+totb)-flosOffsetY1Q)*flosVeffY1Q;
           
           x2ToT_cm = ((totrb-totlt)/(totlt+totrb)-flosOffsetX2Q)*flosVeffX2Q; 
           y2ToT_cm = ((totrt-totlb)/(totrt+totlb)-flosOffsetY2Q)*flosVeffY2Q;                  
                    
    //       xToT = (x1ToT_cm + (x2ToT_cm/sqrt(2.)-y2ToT_cm/sqrt(2.)))/2.;
    //       yToT = (y1ToT_cm + (x2ToT_cm/sqrt(2.)+y2ToT_cm/sqrt(2.)))/2.; 
 
 // New coordinate system since KVI (transformation for 22.5 deg): 
 /*
          xToT = (x1ToT_cm*cos(3.141593/8.)+y1ToT_cm*sin(3.141593/8.) + 
                  x2ToT_cm*cos(3.141593/8.)-y2ToT_cm*sin(3.141593/8.))/2.;
          yToT = (y1ToT_cm*cos(3.141593/8.)-x1ToT_cm*sin(3.141593/8.) +
                  x2ToT_cm*sin(3.141593/8.)+y2ToT_cm*cos(3.141593/8.))/2.;    
*/

           xToT=((totr+totrt)/2.-(totl+totlb)/2.)/((totl+totlb+totr+totrt)/4.);
           yToT=((tott+totlt)/2.-(totb+totrb)/2.)/((tott+totlt+totrb+totb)/4.);
    
           xToT = (xToT-0.0044)*4.510;
           yToT = (yToT+0.02423)*4.492;


  // Raumwinkel und Radius           
  
           xvar = x_cm;
           yvar = y_cm;
           
           xvar1 = xvar/sqrt(2.)+yvar/sqrt(2.);
           yvar1 = -xvar/sqrt(2.)+yvar/sqrt(2.);           
                              
           rr_t  = sqrt((l_SC/2.-yvar)*(l_SC/2.-yvar)+xvar*xvar); 
           rr_l  = sqrt((l_SC/2.-xvar)*(l_SC/2.-xvar)+yvar*yvar); 
           rr_b  = sqrt((l_SC/2.+yvar)*(l_SC/2.+yvar)+xvar*xvar);
           rr_r  = sqrt((l_SC/2.+xvar)*(l_SC/2.+xvar)+yvar*yvar);  
           rr_rt  = sqrt((l_SC/2.-yvar1)*(l_SC/2.-yvar1)+xvar1*xvar1); 
           rr_lt  = sqrt((l_SC/2.-xvar1)*(l_SC/2.-xvar1)+yvar1*yvar1); 
           rr_lb  = sqrt((l_SC/2.+yvar1)*(l_SC/2.+yvar1)+xvar1*xvar1);
           rr_rb  = sqrt((l_SC/2.+xvar1)*(l_SC/2.+xvar1)+yvar1*yvar1);             

           if(iSciType == 1 || iSciType == 3)
           {      		   		   
            rw_t  = atan((abs(xvar)+l_PM/2.)/(l_SC/2.-yvar))-atan((abs(xvar)-l_PM/2.)/(l_SC/2.-yvar));           
            rw_b  = atan((abs(xvar)+l_PM/2.)/(l_SC/2.+yvar))-atan((abs(xvar)-l_PM/2.)/(l_SC/2.+yvar));         
            rw_l  = atan((abs(yvar)+l_PM/2.)/(l_SC/2.-xvar))-atan((abs(yvar)-l_PM/2.)/(l_SC/2.-xvar));           
            rw_r  = atan((abs(yvar)+l_PM/2.)/(l_SC/2.+xvar))-atan((abs(yvar)-l_PM/2.)/(l_SC/2.+xvar));             
            rw_rt = atan((abs(xvar)+l_PM/2.)/(l_SC/2.-yvar))-atan((abs(xvar)-l_PM/2.)/(l_SC/2.-yvar));          
            rw_lb = atan((abs(xvar)+l_PM/2.)/(l_SC/2.+yvar))-atan((abs(xvar)-l_PM/2.)/(l_SC/2.+yvar));
            rw_lt = atan((abs(yvar)+l_PM/2.)/(l_SC/2.-xvar))-atan((abs(yvar)-l_PM/2.)/(l_SC/2.-xvar));
            rw_rb = atan((abs(yvar)+l_PM/2.)/(l_SC/2.+xvar))-atan((abs(yvar)-l_PM/2.)/(l_SC/2.+xvar)); 
           }
           else
           {              		   		   
            rw_t  = atan((abs(xvar)+l_PM1/2.)/(l_SC/2.-yvar))-atan((abs(xvar)-l_PM1/2.)/(l_SC/2.-yvar));           
            rw_b  = atan((abs(xvar)+l_PM1/2.)/(l_SC/2.+yvar))-atan((abs(xvar)-l_PM1/2.)/(l_SC/2.+yvar));         
            rw_l  = atan((abs(yvar)+l_PM1/2.)/(l_SC/2.-xvar))-atan((abs(yvar)-l_PM1/2.)/(l_SC/2.-xvar));           
            rw_r  = atan((abs(yvar)+l_PM1/2.)/(l_SC/2.+xvar))-atan((abs(yvar)-l_PM1/2.)/(l_SC/2.+xvar));
                         
            rw_rt  = atan((abs(xvar1)+l_PM2/2.)/(l_SC/2.-yvar1))-atan((abs(xvar1)-l_PM2/2.)/(l_SC/2.-yvar1));           
            rw_lb  = atan((abs(xvar1)+l_PM2/2.)/(l_SC/2.+yvar1))-atan((abs(xvar1)-l_PM2/2.)/(l_SC/2.+yvar1));         
            rw_lt  = atan((abs(yvar1)+l_PM2/2.)/(l_SC/2.-xvar1))-atan((abs(yvar1)-l_PM2/2.)/(l_SC/2.-xvar1));           
            rw_rb  = atan((abs(yvar1)+l_PM2/2.)/(l_SC/2.+xvar1))-atan((abs(yvar1)-l_PM2/2.)/(l_SC/2.+xvar1)); 			   
		   }		  
		   
		   rw_sum = (rw_t+rw_lt+rw_l+rw_lb+rw_b+rw_rb+rw_r+rw_rt)*180./3.14159265;	   
		   fhRW->Fill(rw_sum);
		   		  
 // Correction for the PMs saturation:
		  tott_corr  = satu(0,tott);
		  totlt_corr = satu(1,totlt);
		  totl_corr  = satu(2,totl);
		  totlb_corr = satu(3,totlb);		  		  
		  totb_corr  = satu(4,totb);
		  totrb_corr = satu(5,totrb);
		  totr_corr  = satu(6,totr);
		  totrt_corr = satu(7,totrt);
                          		   
        x1real = ((totl_corr-totr_corr)/(totl_corr+totr_corr)-flosOffsetX1Qc)*flosVeffX1Qc;            
        y1real = ((tott_corr-totb_corr)/(tott_corr+totb_corr)-flosOffsetY1Qc)*flosVeffY1Qc;
   
        
        x2real = ((totlt_corr-totrb_corr)/(totlt_corr+totrb_corr)-flosOffsetX2Qc)*flosVeffX2Qc;            
        y2real = ((totrt_corr-totlb_corr)/(totrt_corr+totlb_corr)-flosOffsetY2Qc)*flosVeffY2Qc; 

  //      xreal = (x1real + (x2real/sqrt(2.)-y2real/sqrt(2.)))/2.;
  //      yreal = (y1real + (x2real/sqrt(2.)+y2real/sqrt(2.)))/2.; 
        
        
  // New coordinate system since KVI (transformation for 22.5 deg): 
/*
          xreal = (x1real*cos(3.141593/8.)+y1real*sin(3.141593/8.) + 
                  x2real*cos(3.141593/8.)-y2real*sin(3.141593/8.))/2.;
          yreal = (y1real*cos(3.141593/8.)-x1real*sin(3.141593/8.) +
                  x2real*sin(3.141593/8.)+y2real*cos(3.141593/8.))/2.; 
*/
          xreal=((totr_corr+totrt_corr)/2.-(totl_corr+totlb_corr)/2.)/((totl_corr+totlb_corr+totr_corr+totrt_corr)/4.);
          yreal=((tott_corr+totlt_corr)/2.-(totb_corr+totrb_corr)/2.)/((tott_corr+totlt_corr+totrb_corr+totb_corr)/4.);                
          xreal = (xreal-0.0044)*4.510;
          yreal = (yreal+0.02423)*4.492;



          xvar = xreal;
          yvar = yreal;
           
          xvar1 = xvar/sqrt(2.)+yvar/sqrt(2.);
          yvar1 = -xvar/sqrt(2.)+yvar/sqrt(2.);  
 

		
		if(!(IS_NAN(tott_corr)) && !(IS_NAN(totlt_corr)) && !(IS_NAN(totl_corr)) && !(IS_NAN(totlb_corr)) &&
           !(IS_NAN(totb_corr)) && !(IS_NAN(totrb_corr)) && !(IS_NAN(totr_corr)) && !(IS_NAN(totrt_corr))&&
           tott_corr > 0. && totlt_corr > 0. && totl_corr > 0. && totlb_corr > 0. && totb_corr > 0. && 
           totrb_corr > 0. && totr_corr > 0. && totrt_corr > 0.)
        {  		    
           tot_corr = (totl_corr + totlt_corr + totlb_corr + tott_corr
                     + totr_corr + totrb_corr + totb_corr + totrt_corr)/8.;                       
        } 
               
            
           xmean_cm = (xreal+xT_cm)/2.;
           ymean_cm = (yreal+yT_cm)/2.;	
         }
  
       }   // end octagon
       
     } // end LOSTYPE=2
     
                 
      if(iLOSType == 1 || iLOSType == 2 ) 
      {
        Z = tot;
        t_hit = timeLosM; 
       }
       
 // Fill the spectra:
       
     if(iLOSType == 2 ) 
      {                    
      
       if(Igood_event == 1)
       {
			 
			  
  // MCFD times:    
          fhTres_M->Fill(LosTresM);          
          fhTres_M_oddCh->Fill((t_45_M - t_135_M));    
          fhTres_M_evCh->Fill((t_hor_M - t_ver_M));               
          fhTresMvsIcount->Fill(Icount,LosTresM);
  // MCFD walk corrected times:			 
          fhTres_M_corr->Fill(LosTresM_corr);		  
          fhTres_M_evCh_corr->Fill((time_r_V_corr  + time_l_V_corr)/2. - (time_t_V_corr  +  time_b_V_corr)/2.); 	  
          fhTres_M_oddCh_corr->Fill((time_rb_V_corr + time_lt_V_corr)/2. - (time_rt_V_corr + time_lb_V_corr)/2.);	
          fhTreswcMvsIcount->Fill(Icount,LosTresM_corr);          
   // TAMEX times:       
	      fhTres_T->Fill(LosTresT); 
	      
	   //   cout<<"TAMEX t,tot: "<<LosTresT<<", "<<tot<<endl;
	      
          fhTres_T_oddCh->Fill((t_45_T - t_135_T));    
          fhTres_T_evCh->Fill((t_hor_T - t_ver_T));               
          fhTresMvsIcount->Fill(Icount,LosTresM);
  // TAMEX walk corrected times:			 
          fhTres_T_corr->Fill(LosTresT_corr);		  
          fhTres_T_oddCh_corr->Fill((time_rb_L_corr  + time_lt_L_corr)/2. - (time_rt_L_corr  +  time_lb_L_corr)/2.); 	  
          fhTres_T_evCh_corr->Fill((time_b_L_corr + time_t_L_corr)/2. - (time_r_L_corr + time_l_L_corr)/2.);	
          fhTreswcTvsIcount->Fill(Icount,LosTresT_corr);
          
   // dt combined:
          fhTres_MT_corr->Fill((LosTresM_corr+LosTresT_corr)/2.);	        
                    	                   
  // x,y from MCFD:          
          fhXY->Fill(x_cm,y_cm);         
          fhXY1->Fill(x1_cm,y1_cm);
          fhXY2->Fill(x2_cm,y2_cm); 
      // Average of three:          
          fhXYproj->Fill(((xreal+x_cm+xT_cm)/3.),(yreal+y_cm+yT_cm)/3.);                      
      // Mean value between TAMEX and ToT:  
          fhXYmean->Fill(xmean_cm,ymean_cm);         
          
  // x,y from MCFD walk corrected: 
          fhXY_corr->Fill(x_cm_corr,y_cm_corr);         
          fhXY1_corr->Fill(x1_cm_corr,y1_cm_corr);
          fhXY2_corr->Fill(x2_cm_corr,y2_cm_corr);
  // x,y from TAMEX:	                     
          fhXYT->Fill(xT_cm,yT_cm); 
          fhXYT1->Fill(x1T_cm,y1T_cm);
          fhXYT2->Fill(x2T_cm,y2T_cm);
          
  // x,y from ToT:            
          fhXY_ToT->Fill(xToT,yToT);         
          fhXYQ1->Fill(x1ToT_cm,y1ToT_cm);
          fhXYQ2->Fill(x2ToT_cm,y2ToT_cm);                   
        		  	 
 // x,y from ToT corrected:             
          fhXY_ToT_corr->Fill(xreal,yreal); 
          fhXY1_ToT_corr->Fill(x1real,y1real);
          fhXY2_ToT_corr->Fill(x2real,y2real);         
          
 // ToT
  	      fhQ->Fill(tot);
          fhQtest->Fill(tot_corr);  
                 
          fhQ1vsIcount->Fill(Icount,totlt);        
          fhQ3vsIcount->Fill(Icount,totlb);   
          fhQ5vsIcount->Fill(Icount,totrb);             
          fhQ7vsIcount->Fill(Icount,totrt);            
          fhQ2vsIcount->Fill(Icount,totl);        
          fhQ4vsIcount->Fill(Icount,totb);   
          fhQ6vsIcount->Fill(Icount,totr);             
          fhQ8vsIcount->Fill(Icount,tott);	                     
          
          fhQ_oddCh->Fill((totlt + totlb + totrb + totrt )/4.);
          fhQ_evCh->Fill((tott + totb + totr + totl)/4.);
		  fhQ_oddCh_corr->Fill((totlt_corr + totlb_corr + totrb_corr + totrt_corr )/4.);
		  fhQ_evCh_corr->Fill((tott_corr + totb_corr + totr_corr + totl_corr)/4.);
		  		  
		  fhQ1_vs_Q5->Fill(totlt,totrb);
		  fhQ3_vs_Q7->Fill(totlb,totrt);	          	
		  fhQ2_vs_Q6->Fill(totl,totr);
		  fhQ4_vs_Q8->Fill(totb,tott);
 		  fhQ1_vs_Q5_corr->Fill(totlt_corr,totrb_corr);
		  fhQ3_vs_Q7_corr->Fill(totlb_corr,totrt_corr);           
		  fhQ2_vs_Q6_corr->Fill(totl_corr,totr_corr);
		  fhQ4_vs_Q8_corr->Fill(totb_corr,tott_corr);

	  
		  fhQ_T->Fill(y_cm,tott); 		  
		  fhQ_L->Fill(x_cm,totl);
 	      fhQ_B->Fill(y_cm,totb); 
		  fhQ_R->Fill(x_cm,totr);   		      
		  
		  fhQ_RT->Fill(y_cm,totrt);		  
          fhQ_LT->Fill(x_cm,totlt);  		  
		  fhQ_LB->Fill(y_cm,totlb);
		  fhQ_RB->Fill(x_cm,totrb);	
		  
		  fhQ_T_corr->Fill(y_cm,tott_corr); 		  
		  fhQ_L_corr->Fill(x_cm,totl_corr);
 	      fhQ_B_corr->Fill(y_cm,totb_corr); 
		  fhQ_R_corr->Fill(x_cm,totr_corr);   		      
		  
		  fhQ_RT_corr->Fill(y_cm,totrt_corr);		  
          fhQ_LT_corr->Fill(x_cm,totlt_corr);  		  
		  fhQ_LB_corr->Fill(y_cm,totlb_corr);
		  fhQ_RB_corr->Fill(x_cm,totrb_corr);	



 // Correlations:         
          fhTresX_M->Fill(x_cm,LosTresM);
          fhTresY_M->Fill(y_cm,LosTresM);
          fhTresX_M_corr->Fill(x_cm,LosTresM_corr);
          fhTresY_M_corr->Fill(y_cm,LosTresM_corr);          
          
          fhTresX_T->Fill(x_cm,LosTresT);
          fhTresY_T->Fill(y_cm,LosTresT);
          fhTresX_T_corr->Fill(x_cm,LosTresT_corr);
          fhTresY_T_corr->Fill(y_cm,LosTresT_corr);          
        
          fhTM_vs_Q->Fill(tot,LosTresM);
          fhTM_vs_Q_corr->Fill(tot,LosTresM_corr);
          fhTT_vs_Q->Fill(tot,LosTresT);     
          fhTT_vs_Q_corr->Fill(tot,LosTresT_corr);
              
          fhQ_vs_X->Fill(x_cm,tot);
          fhQ_vs_Y->Fill(y_cm,tot); 
               
          fhQ_vs_X_corr->Fill(x_cm,tot_corr);
          fhQ_vs_Y_corr->Fill(y_cm,tot_corr);
          		  	
		            
	     }

	   }  // end of LOSTYPE = 2
	   
	 if(iLOSType == 1 ) 
      {                    
      
       if(Igood_event == 1)
       {
			 		  
  // MCFD times:    
          fhTres_M->Fill(LosTresM);   
          fhTresMvsIcount->Fill(Icount,LosTresM);
  // MCFD walk corrected times:			 
          fhTres_M_corr->Fill(LosTresM_corr);		  
          fhTreswcMvsIcount->Fill(Icount,LosTresM_corr);          
   // TAMEX times:       
	      fhTres_T->Fill(LosTresT);     
          fhTresMvsIcount->Fill(Icount,LosTresM);
  // TAMEX walk corrected times:			 
          fhTres_T_corr->Fill(LosTresT_corr);		  
          fhTreswcTvsIcount->Fill(Icount,LosTresT_corr);
          
   // dt combined:
          fhTres_MT_corr->Fill((LosTresM_corr+LosTresT_corr)/2.);	        
                    	                   
  // x,y from MCFD:
          fhXY->Fill(x_cm,y_cm);
      // Average of two:             
          fhXYproj->Fill(((xreal+x_cm+xT_cm)/3.),(yreal+y_cm+yT_cm)/3.);
  // x,y from MCFD walk corrected: 
          fhXY_corr->Fill(x_cm_corr,y_cm_corr);         
  // x,y from TAMEX:	           
          fhXYT->Fill(xT_cm,yT_cm);   
  // x,y from ToT:            
          fhXY_ToT->Fill(xToT,yToT);                         
 // Mean value between MCFD and ToT:  
          fhXYmean->Fill(xmean_cm,ymean_cm);       		  	 
 // x,y from ToT corrected:             
          fhXY_ToT_corr->Fill(xreal,yreal); 
         
          
 // ToT
  	      fhQ->Fill(tot);
          fhQtest->Fill(tot_corr);  
                 
          fhQ1vsIcount->Fill(Icount,tott);        
          fhQ3vsIcount->Fill(Icount,totl);   
          fhQ5vsIcount->Fill(Icount,totb);             
          fhQ7vsIcount->Fill(Icount,totr);                             
          		  		  
		  fhQ1_vs_Q5->Fill(tott,totb);
		  fhQ3_vs_Q7->Fill(totl,totr);	          	
 		  fhQ1_vs_Q5_corr->Fill(tott_corr,totb_corr);
		  fhQ3_vs_Q7_corr->Fill(totr_corr,totl_corr);           
	  
		  fhQ_T->Fill(yT_cm,tott); 		  
		  fhQ_L->Fill(xT_cm,totl);
 	      fhQ_B->Fill(yT_cm,totb); 
		  fhQ_R->Fill(xT_cm,totr);   		      
		  
	  
		  fhQ_T_corr->Fill(yT_cm,tott_corr); 		  
		  fhQ_L_corr->Fill(xT_cm,totl_corr);
 	      fhQ_B_corr->Fill(yT_cm,totb_corr); 
		  fhQ_R_corr->Fill(xT_cm,totr_corr);   		      

 // Correlations:         
          fhTresX_M->Fill(xT_cm,LosTresM);
          fhTresY_M->Fill(yT_cm,LosTresM);
          fhTresX_M_corr->Fill(xT_cm,LosTresM_corr);
          fhTresY_M_corr->Fill(yT_cm,LosTresM_corr);          
          
          fhTresX_T->Fill(xT_cm,LosTresT);
          fhTresY_T->Fill(yT_cm,LosTresT);
          fhTresX_T_corr->Fill(xT_cm,LosTresT_corr);
          fhTresY_T_corr->Fill(yT_cm,LosTresT_corr);          
        
          fhTM_vs_Q->Fill(tot,LosTresM);
          fhTM_vs_Q_corr->Fill(tot,LosTresM_corr);
          fhTT_vs_Q->Fill(tot,LosTresT);     
          fhTT_vs_Q_corr->Fill(tot,LosTresT_corr);
              
          fhQ_vs_X->Fill(xT_cm,tot);
          fhQ_vs_Y->Fill(yT_cm,tot); 
               
          fhQ_vs_X_corr->Fill(xT_cm,tot_corr);
          fhQ_vs_Y_corr->Fill(yT_cm,tot_corr);
          		  	
		            
	     }

	   }  // end of LOSTYPE=1   	  
	      	  
		         
	      	                
       new ((*fHitItems)[fNofHitItems]) R3BLosHitData(nDet, t_hit , x_cm, y_cm, Z);
       fNofHitItems += 1;
        
 //  myFile.close();
        //cout << "R3BLosCal2Hit::Exec END: " << Icount << endl;
}

void R3BLosCal2Hit::FinishEvent()
{

    if (fHitItems)
    {
        fHitItems->Clear();
        fNofHitItems = 0;
        
    }

}

void R3BLosCal2Hit::FinishTask()
{
	if(fhTres_M) fhTres_M->Write() ;
	if(fhTres_T) fhTres_T->Write() ;	
	if(fhTres_M_oddCh) fhTres_M_oddCh->Write() ;
	if(fhTres_T_oddCh) fhTres_T_oddCh->Write() ;	
	if(fhTres_M_evCh) fhTres_M_evCh->Write() ;
	if(fhTres_T_evCh) fhTres_T_evCh->Write() ;	
	if(fhQ) fhQ->Write() ;
	if(fhQ_evCh) fhQ_evCh->Write();
    if(fhQ_oddCh) fhQ_oddCh->Write();
	if(fhQ_vs_X) fhQ_vs_X->Write() ;
	if(fhQ_vs_Y) fhQ_vs_Y->Write();	
	if(fhTM_vs_Q) fhTM_vs_Q->Write() ;
	if(fhTT_vs_Q) fhTT_vs_Q->Write() ;	    	
	if(fhXY) fhXY->Write() ;	
	if(fhXYmean) fhXYmean->Write() ;	
	if(fhXY_ToT) fhXY_ToT->Write();
	if(fhXYproj) fhXYproj->Write() ;
	if(fhXY1) fhXY1->Write();
	if(fhXY2) fhXY2->Write() ;	
	if(fhXYT) fhXYT->Write();
	if(fhXYT1) fhXYT1->Write() ;
	if(fhXYT2) fhXYT2->Write() ;	
	if(fhXYQ1) fhXYQ1->Write() ;
	if(fhXYQ2) fhXYQ2->Write() ;
	if(fhQ1_vs_Q5) fhQ1_vs_Q5->Write() ;	
	if(fhQ2_vs_Q6) fhQ2_vs_Q6->Write() ;	
	if(fhQ3_vs_Q7) fhQ3_vs_Q7->Write() ;
	if(fhQ4_vs_Q8) fhQ4_vs_Q8->Write() ;
	if(fhTresX_M) fhTresX_M->Write() ;
	if(fhTresY_M) fhTresY_M->Write() ;
	if(fhTresX_T) fhTresX_T->Write() ;
	if(fhTresY_T) fhTresY_T->Write() ;	
	
			
	if(1 == 1){					
	if(fhTres_T_corr) fhTres_T_corr->Write() ;	
	if(fhTres_M_corr) fhTres_M_corr->Write() ;
	if(fhTres_MT_corr) fhTres_MT_corr->Write();
	if(fhTres_T_oddCh_corr) fhTres_T_oddCh_corr->Write() ;	
	if(fhTres_M_oddCh_corr) fhTres_M_oddCh_corr->Write() ;	
	if(fhTres_T_evCh_corr) fhTres_T_evCh_corr->Write() ;	
	if(fhTres_M_evCh_corr) fhTres_M_evCh_corr->Write() ;	
	if(fhQ_L) fhQ_L->Write() ;
	if(fhQ_B) fhQ_B->Write() ;
	if(fhQ_R) fhQ_R->Write() ;
	if(fhQ_T) fhQ_T->Write() ;	
	if(fhQ_L_corr) fhQ_L_corr->Write() ;
	if(fhQ_B_corr) fhQ_B_corr->Write() ;
	if(fhQ_R_corr) fhQ_R_corr->Write() ;
	if(fhQ_T_corr ) fhQ_T_corr->Write();
	if(fhQ_LT) fhQ_LT->Write() ;
	if(fhQ_LB) fhQ_LB->Write() ;
	if(fhQ_RB) fhQ_RB->Write() ;
	if(fhQ_RT) fhQ_RT->Write() ;		
	if(fhQ_LT_corr) fhQ_LT_corr->Write() ;
	if(fhQ_LB_corr)  fhQ_LB_corr->Write();
	if(fhQ_RB_corr) fhQ_RB_corr->Write() ;
	if(fhQ_RT_corr) fhQ_RT_corr->Write();
    if(fhQ_evCh_corr) fhQ_evCh_corr->Write();
    if(fhQ_oddCh_corr) fhQ_oddCh_corr->Write();
	if(fhQ_vs_X_corr) fhQ_vs_X_corr->Write() ;
	if(fhQ_vs_Y_corr) fhQ_vs_Y_corr->Write() ;	
	if(fhTM_vs_Q_corr) fhTM_vs_Q_corr->Write() ;
	if(fhTT_vs_Q_corr) fhTT_vs_Q_corr->Write() ;
	if(fhXY_corr) fhXY_corr->Write() ;		
	if(fhXY_ToT_corr) fhXY_ToT_corr->Write() ;
	if(fhXY1_ToT_corr) fhXY1_ToT_corr->Write() ;
	if(fhXY2_ToT_corr) fhXY2_ToT_corr->Write() ;
	if(fhXY1_corr) fhXY1_corr->Write() ;
	if(fhXY2_corr) fhXY2_corr->Write() ;
	if(fhQ1_vs_Q5_corr) fhQ1_vs_Q5_corr->Write() ;
	if(fhQ2_vs_Q6_corr) fhQ2_vs_Q6_corr->Write() ;
	if(fhQ3_vs_Q7_corr) fhQ3_vs_Q7_corr->Write() ;	
	if(fhQ4_vs_Q8_corr) fhQ4_vs_Q8_corr->Write() ;
	if(fhQtest) fhQtest->Write() ;
	if(fhTresX_M_corr) fhTresX_M_corr->Write() ;
	if(fhTresY_M_corr) fhTresY_M_corr->Write() ;	
	if(fhTresX_T_corr) fhTresX_T_corr->Write() ;
	if(fhTresY_T_corr) fhTresY_T_corr->Write() ;	
	if(fht1Q1M) fht1Q1M->Write() ;			
	if(fht3Q3M) fht3Q3M->Write() ;			
	if(fht5Q5M) fht5Q5M->Write() ;			
	if(fht7Q7M) fht7Q7M->Write() ;
	if(fht8Q8M) fht8Q8M->Write() ;			
	if(fht2Q2M) fht2Q2M->Write() ;			
	if(fht4Q4M) fht4Q4M->Write() ;			
	if(fht6Q6M) fht6Q6M->Write() ;
	if(fht1Q1T) fht1Q1T->Write() ;			
	if(fht3Q3T) fht3Q3T->Write() ;			
	if(fht5Q5T) fht5Q5T->Write() ;			
	if(fht7Q7T) fht7Q7T->Write();
	if(fht8Q8T) fht8Q8T->Write() ;			
	if(fht2Q2T) fht2Q2T->Write() ;			
	if(fht4Q4T) fht4Q4T->Write() ;			
	if(fht6Q6T) fht6Q6T->Write() ;		
	if(fht1Q1M_corr) fht1Q1M_corr->Write() ;			
	if(fht3Q3M_corr) fht3Q3M_corr->Write() ;			
	if(fht5Q5M_corr) fht5Q5M_corr->Write() ;			
	if(fht7Q7M_corr) fht7Q7M_corr->Write() ;
	if(fht8Q8M_corr) fht8Q8M_corr->Write() ;			
	if(fht2Q2M_corr) fht2Q2M_corr->Write() ;			
	if(fht4Q4M_corr) fht4Q4M_corr->Write();			
	if(fht6Q6M_corr) fht6Q6M_corr->Write() ;
	if(fht1Q1T_corr) fht1Q1T_corr->Write() ;			
	if(fht3Q3T_corr) fht3Q3T_corr->Write() ;			
	if(fht5Q5T_corr) fht5Q5T_corr->Write() ;			
	if(fht7Q7T_corr) fht7Q7T_corr->Write() ;
	if(fht8Q8T_corr) fht8Q8T_corr->Write();			
	if(fht2Q2T_corr) fht2Q2T_corr->Write() ;			
	if(fht4Q4T_corr) fht4Q4T_corr->Write() ;			
	if(fht6Q6T_corr) fht6Q6T_corr->Write() ;		
	if(fhQ1vsIcount) fhQ1vsIcount->Write() ;	
	if(fhQ3vsIcount) fhQ3vsIcount->Write() ;		
	if(fhQ5vsIcount) fhQ5vsIcount->Write() ;		
	if(fhQ7vsIcount) fhQ7vsIcount->Write() ;	
	if(fhQ2vsIcount) fhQ2vsIcount->Write() ;	
	if(fhQ4vsIcount) fhQ4vsIcount->Write();		
	if(fhQ6vsIcount) fhQ6vsIcount->Write() ;		
	if(fhQ8vsIcount) fhQ8vsIcount->Write() ;
	if(fhTresMvsIcount) fhTresMvsIcount->Write() ;	
	if(fhTresTvsIcount) fhTresTvsIcount->Write() ;	
	if(fhTreswcMvsIcount) fhTreswcMvsIcount->Write() ;	
	if(fhTreswcTvsIcount) fhTreswcTvsIcount->Write() ;		
	if(fhRW) fhRW->Write();	
	if(fht1t5M) fht1t5M->Write();
	if(fht2t6M) fht2t6M->Write();	
	if(fht3t7M) fht3t7M->Write();	
	if(fht4t8M) fht4t8M->Write();	
	if(fht1t5M_corr) fht1t5M_corr->Write();
	if(fht2t6M_corr) fht2t6M_corr->Write();	
	if(fht3t7M_corr) fht3t7M_corr->Write();	
	if(fht4t8M_corr) fht4t8M_corr->Write();	
	if(fht1t5T) fht1t5T->Write();
	if(fht2t6T) fht2t6T->Write();	
	if(fht3t7T) fht3t7T->Write();	
	if(fht4t8T) fht4t8T->Write();	
	if(fht1t5T_corr) fht1t5T_corr->Write();
	if(fht2t6T_corr) fht2t6T_corr->Write();	
	if(fht3t7T_corr) fht3t7T_corr->Write();	
	if(fht4t8T_corr) fht4t8T_corr->Write();	
	if(fhQpix) fhQpix->Write();
   }
		
}

Double_t R3BLosCal2Hit::walk(Int_t inum, Double_t tot)
{

    Double_t y=0./0., ysc=0./0., term[8]={0.};
    Double_t x;
    
                               
    x = tot;
    term[0]=x;
    for (Int_t i = 0; i < 7; i++)
    {
	 term[i+1] = term[i] * x ;	
    }		

    
    ysc = walk_par[inum][2] + walk_par[inum][3]*term[0] + walk_par[inum][4]*term[1] + 
                              walk_par[inum][5]*term[2] + walk_par[inum][6]*term[3] +
                              walk_par[inum][7]*term[4] + walk_par[inum][8]*term[5] +
                              walk_par[inum][9]*term[6] + walk_par[inum][10]*term[7];

	if(tot < walk_par[inum][0] || tot > walk_par[inum][1]) ysc = 0.0/0.0;  	
                                                                                
   /*  
      cout<< inum<<", "<< walk_par[inum][0] <<", "<< walk_par[inum][1] <<", "<< walk_par[inum][2] <<", "<< 
                          walk_par[inum][3] <<", "<< walk_par[inum][4] <<", "<< walk_par[inum][5] <<", "<<
                          walk_par[inum][6] <<", "<< walk_par[inum][7] <<", "<< walk_par[inum][8] <<", "<<
                          walk_par[inum][9] <<", "<< walk_par[inum][10] <<endl; 
   */ 
    return ysc;
}

Double_t R3BLosCal2Hit::satu(Int_t inum, Double_t tot)
{

    Double_t ysc=0./0. ;
    
    ysc = tot;
    
    if(tot_par[inum][0] > 0.)  
    ysc  = (tot_par[inum][0]*tot+tot_par[inum][1])/(tot_par[inum][2]-tot)*tot_par[inum][3] ;
      
                                                                                
 /*   
    cout<< inum<<", "<< tot_par[inum][0] <<", "<< tot_par[inum][1] <<", "<< tot_par[inum][2]<<", "<< tot_par[inum][3] <<endl; 
    cout<<tot<<", "<<ysc<<endl;    
 */ 
    return ysc;
}

ClassImp(R3BLosCal2Hit)
