
#include "R3BLosCalItem.h"

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


R3BLosCalItem::R3BLosCalItem()
    : fDetector(0)
{
}

R3BLosCalItem::R3BLosCalItem(	UChar_t detector )
    : fDetector(detector)
    , fTime_r_ns( 0.0 / 0.0 ) // 0/0 produces a NAN. Macro NAN not available??
    , fTime_t_ns( 0.0 / 0.0 )
    , fTime_l_ns( 0.0 / 0.0 )
    , fTime_b_ns( 0.0 / 0.0 )
    , fTime_ref_ns( 0.0 / 0.0 )
{
}

ClassImp(R3BLosCalItem)


