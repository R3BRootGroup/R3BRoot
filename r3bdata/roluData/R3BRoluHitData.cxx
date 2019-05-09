
#include "R3BRoluHitData.h"

// for the data analysis of ROLU following LOS. 

R3BRoluHitData::R3BRoluHitData()
    : fDetector(0)
{
}

R3BRoluHitData::R3BRoluHitData(	UChar_t detector,
							Double_t tot_r,
							Double_t tot_o,
							Double_t tot_l,
							Double_t tot_u
								)

    : fDetector(detector)
    , ftot_r( tot_r ) 
    , ftot_o( tot_o )
    , ftot_l( tot_l )
    , ftot_u( tot_u )

{
}

ClassImp(R3BRoluHitData)



