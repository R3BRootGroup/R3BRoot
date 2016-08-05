// ------------------------------------------------------------
// -----                  R3BLosCal2Hit                   -----
// -----          Created Mar 10th 2016 by R.Plag         -----
// ------------------------------------------------------------

#include "R3BLosCal2Hit.h"
#include "R3BLosCalData.h"
#include "R3BLosHitData.h"

#include "FairLogger.h"

#include "TClonesArray.h"

#include "math.h"

R3BLosCal2Hit::R3BLosCal2Hit()
    : FairTask("LosCal2Hit", 1)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BLosHitData"))
    , fNofHitItems(0)
{
}

R3BLosCal2Hit::R3BLosCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BLosHitData"))
    , fNofHitItems(0)
{
}

R3BLosCal2Hit::~R3BLosCal2Hit()
{
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
	Double_t flosVeff    = 7.; // cm/ns
	Double_t flosOffsetX = -10.;
	Double_t flosOffsetY = -4.1;
	
    Int_t nDets = fCalItems->GetEntriesFast();

    for (Int_t i = 0; i < nDets; i++)
    {
       R3BLosCalData* calItem = (R3BLosCalData*)fCalItems->At(i);
       if (!calItem) continue; // can this happen?
            
       // missing times are NAN, hence t_hor or t_ver will also
       // be NAN if one time is missing.
       Double_t t_hor = calItem->fTime_r_ns + calItem->fTime_l_ns;
       Double_t t_ver = calItem->fTime_t_ns + calItem->fTime_b_ns;
       Double_t x_cm  = 0.0/0.0;
       Double_t y_cm  = 0.0/0.0;
       
       Double_t t_hit=0.0/0.0; // NAN
       if ((!isnan(t_hor)) && (!isnan(t_ver))) // have all four as it should be
       {
			t_hit=(t_hor+t_ver)/4;
			x_cm=(calItem->fTime_r_ns-calItem->fTime_l_ns)/2.*flosVeff-flosOffsetX;
			y_cm=(calItem->fTime_b_ns-calItem->fTime_t_ns)/2.*flosVeff-flosOffsetY;
	   }
	   else if (!isnan(t_hor))
	   {
			t_hit=t_hor/2;
			x_cm=(calItem->fTime_r_ns-calItem->fTime_l_ns)/2.*flosVeff-flosOffsetX;
	   }
	   else if (!isnan(t_ver))
	   {
			t_hit=t_ver/2;
			y_cm=(calItem->fTime_b_ns-calItem->fTime_t_ns)/2.*flosVeff-flosOffsetY;
	   }
       else
			continue;


			
       // what shall we do with the master trigger?
       Double_t t_diff2MT=t_hit - calItem->fTime_ref_ns; // time relative to the master trig
              
       new ((*fHitItems)[fNofHitItems]) R3BLosHitData(calItem->GetDetector(), t_hit , x_cm, y_cm, t_diff2MT);
       fNofHitItems += 1;
    }    
    
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
}

ClassImp(R3BLosCal2Hit)
