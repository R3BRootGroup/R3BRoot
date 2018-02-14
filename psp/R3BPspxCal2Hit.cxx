// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxCal2Hit                          -----
// -----	     created 09-03-2016 by I. Syndikus		           -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
#include <limits>
#include <cmath>

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

#include "R3BEventHeader.h"
#include "R3BPspxCal2Hit.h"
#include "R3BPspxCalData.h"
#include "R3BPspxHitData.h"
#include "R3BPspxHitPar.h"

using namespace std;

R3BPspxCal2Hit::R3BPspxCal2Hit()
    : fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BPspxHitData"))
{
}

R3BPspxCal2Hit::R3BPspxCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BPspxHitData"))
{
}

R3BPspxCal2Hit::~R3BPspxCal2Hit() {}

InitStatus R3BPspxCal2Hit::Init()
{
    /**
     * Initialize output data. Read input data and parameters.
     * The parameters get saved in dedicated arrays.
     * Print parameters, if verbosity is set to INFO.
     */

    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fCalItems = (TClonesArray*)fMan->GetObject("PspxCalData"); // = branch name in TTree
    if (!fCalItems)
    {
        printf("Couldnt get handle on PSPX cal items\n");
        return kFATAL;
    }
    // fHitItems = (TClonesArray*)fMan->GetObject("R3BPspxHitData");
    FairRootManager::Instance()->Register("PspxHitData", "Pspx", fHitItems, kTRUE);

    // fHitPar->printparams();

    // Initialisation of gain parameters
    offset.resize(fHitPar->GetPspxParDetector());
    slope.resize(fHitPar->GetPspxParDetector());
    for (Int_t i = 0; i < fHitPar->GetPspxParDetector(); i++)
    {
        if (fHitPar->GetPspxParOrientation().At(i) == 1 || fHitPar->GetPspxParOrientation().At(i) == 2 ||
            fHitPar->GetPspxParOrientation().At(i) == 0)
        { // strips on 1 side
            offset[i].resize(fHitPar->GetPspxParStrip().At(i));
	    slope[i].resize(fHitPar->GetPspxParStrip().At(i));
        }
        else if (fHitPar->GetPspxParOrientation().At(i) == 3)
        { // strips on 2 side1
            offset[i].resize(fHitPar->GetPspxParStrip().At(i) * 2);
	    slope[i].resize(fHitPar->GetPspxParStrip().At(i) * 2);
        }
    }
    Int_t start_detector = 0; // entries, not lines
    for (Int_t i = 0; i < offset.size(); i++)
    { // detectors
        for (Int_t j = 0; j < offset[i].size(); j++)
        { // strips
            offset[i][j] = fHitPar->GetPspxParLinearParam().At(start_detector + 3 + j * 3);
            slope[i][j] = fHitPar->GetPspxParLinearParam().At(start_detector + 4 + j * 3);
        }
        start_detector = start_detector + 2 + 3 * fHitPar->GetPspxParStrip().At(i);
    }
    LOG(INFO) << "R3BPspxCal2Hit :: Init() " << FairLogger::endl;
    for (Int_t i = 0; i < fHitPar->GetPspxParDetector(); i++)
    {
        for (Int_t j = 0; j < offset[i].size(); j++)
        {
            LOG(INFO) << "offset[" << i << "][" << j << "]=" << offset[i][j] << FairLogger::endl;
            LOG(INFO) << "slope[" << i << "][" << j << "]=" << slope[i][j] << FairLogger::endl;
        }
    }
    // remain from initcalibration
    sign_x.resize(fHitPar->GetPspxParDetector());
    sign_y.resize(fHitPar->GetPspxParDetector());
    for (UInt_t i = 0; i < fHitPar->GetPspxParDetector(); i++)
    {
        sign_x[i] = fHitPar->GetPspxParOrientationXSign().At(i);
        sign_y[i] = fHitPar->GetPspxParOrientationYSign().At(i);
    }

    for (Int_t i = 0; i < fHitPar->GetPspxParDetector(); i++)
    {
        LOG(INFO) << "sign_x[" << i << "]=" << sign_x[i] << FairLogger::endl;
        LOG(INFO) << "sign_y[" << i << "]=" << sign_y[i] << FairLogger::endl;
    }

    return kSUCCESS;
}

// ----  Initialisation  ----------------------------------------------
void R3BPspxCal2Hit::SetParContainers()
{
    /**
     * Initialize/Reads parameter file for conversion.
     */
    LOG(INFO) << "R3BPspxCal2Hit :: SetParContainers() " << FairLogger::endl;

    fHitPar = (R3BPspxHitPar*)FairRuntimeDb::instance()->getContainer("R3BPspxHitPar");

    // Get Base Container
    // FairRunAna* ana = FairRunAna::Instance();
    // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    // fHitPar = (R3BPspxHitPar*) (rtdb->getContainer("R3BPspxHitPar"));

    if (!fHitPar)
    {
        LOG(ERROR) << "Could not get access to R3BPspxHitPar-Container." << FairLogger::endl;
        return;
    }

    fHitPar->printparams();
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus R3BPspxCal2Hit::ReInit()
{
    /**
     * Initialize/Reads parameter file for conversion.
     */

    LOG(INFO) << " R3BPspxCal2Hit :: ReInit() " << FairLogger::endl;

    // FairRunAna* ana = FairRunAna::Instance();
    // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    // fHitPar = (R3BPspxHitPar*) (rtdb->getContainer("R3BPspxHitPar"));

    fHitPar = (R3BPspxHitPar*)FairRuntimeDb::instance()->getContainer("R3BPspxHitPar");

    if (!fHitPar)
    {
        LOG(ERROR) << "Could not get access to R3BPspxHitPar-Container." << FairLogger::endl;
        return kFATAL;
    }

    return kSUCCESS;
}

void R3BPspxCal2Hit::Exec(Option_t* option)
{
    /**
     * Does the conversion from Cal to Hit level. It is called for every event.
     * It determines the multiplicities for the energy (cathode/back) channel and the channels of the x and y side of
     * the detector.
     * For events with multiplicity 2 or 4 in x/y direction, it calculated the detector internal (u/v) positions
     * acccording to
     * position_au = (E1-E2)/(E1+E2) for multiplicity 2
     * position_au = (E1_1+E1_2 - (E2_1+E2_2))/(E1_1+E1_2+E2_1+E2_2) for multiplicity 4.
     * Using the provided parameters the x/y position is calculated from the u/v positons acccording to
     * position_cm = sign * (offset + slope * position_au)
     * The sign is used to give the final positons in a general coordinate system.
     * The qualitiy of the x/y positons is given by sigma. At the moment sigma = 0 for construction with the explained
     * method and sigma = 1 for events, for which only the strip number could be used to determine the position.
     * The total energy is either determined from the cathode (back, X1) or from the sum of anode channels (strips, X5,
     * but only for multiplicity 2 or 4).
     */

    if (!fCalItems)
    {
        printf("Cannot access PSPX cal items\n");
        return;
    }

    // Initialize
    Float_t u;
    Float_t v;
    Float_t x;
    Float_t y;
    Float_t sigma_x;
    Float_t sigma_y;
    Float_t energy;

    UShort_t x_mult[fHitPar->GetPspxParDetector()];
    UShort_t y_mult[fHitPar->GetPspxParDetector()];
    UShort_t e_mult[fHitPar->GetPspxParDetector()];

    UShort_t nstrips;
    UShort_t strip;
    UShort_t strip1;
    UShort_t strip2;

    for (UShort_t j = 0; j < fHitPar->GetPspxParDetector(); j++)
    {
        x_mult[j] = std::numeric_limits<UShort_t>::quiet_NaN();
        y_mult[j] = std::numeric_limits<UShort_t>::quiet_NaN();
        e_mult[j] = std::numeric_limits<UShort_t>::quiet_NaN();
    }

    Int_t nMapped = fCalItems->GetEntries();

    // calculationg multiplicities
    for (Int_t i = 0; i < nMapped; i++)
    {
        strip = std::numeric_limits<UShort_t>::quiet_NaN();

        R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);
        strip = mItem->GetStrip();
        UInt_t detector = mItem->GetDetector();

        // calculating multiplicity
        if (strip > 0 && strip < fHitPar->GetPspxParStrip().At(detector - 1) + 1)
        {
            if (mItem->GetEnergy1() != 0&& !std::isnan(mItem->GetEnergy1()) )
            {
                y_mult[detector - 1]++;
            }
            if (mItem->GetEnergy2() != 0&& !std::isnan(mItem->GetEnergy2()) )
            {
                y_mult[detector - 1]++;
            }
        }
        else if (strip > fHitPar->GetPspxParStrip().At(detector - 1) &&
                 strip < fHitPar->GetPspxParStrip().At(detector - 1) * 2 + 1)
        {
            if (mItem->GetEnergy1() != 0&& !std::isnan(mItem->GetEnergy1()) )
            {
                x_mult[detector - 1]++;
            }
            if (mItem->GetEnergy2() != 0&& !std::isnan(mItem->GetEnergy2()) )
            {
                x_mult[detector - 1]++;
            }
        }
        else if (strip == fHitPar->GetPspxParStrip().At(detector - 1) * 2 + 1)
        {
            if (mItem->GetEnergy1() != 0&& !std::isnan(mItem->GetEnergy1()) )
            {
                e_mult[detector - 1]++;
            }
        }
    }

    // calculating positions for each detector
    for (UShort_t detector = 0; detector < fHitPar->GetPspxParDetector(); detector++)
    {
        if (x_mult[detector] == 0 && y_mult[detector] == 0 && e_mult[detector] == 0)
            continue;

        u = std::numeric_limits<Float_t>::quiet_NaN();
        v = std::numeric_limits<Float_t>::quiet_NaN();
        x = std::numeric_limits<Float_t>::quiet_NaN();
        y = std::numeric_limits<Float_t>::quiet_NaN();
        sigma_x = std::numeric_limits<Float_t>::quiet_NaN();
        sigma_y = std::numeric_limits<Float_t>::quiet_NaN();
        energy = std::numeric_limits<Float_t>::quiet_NaN();

        nstrips = std::numeric_limits<UShort_t>::quiet_NaN();
        strip = std::numeric_limits<UShort_t>::quiet_NaN();
        strip1 = std::numeric_limits<UShort_t>::quiet_NaN();
        strip2 = std::numeric_limits<UShort_t>::quiet_NaN();

        nstrips = fHitPar->GetPspxParStrip().At(detector);

        // energy from cathode/back
        if (e_mult[detector] == 1)
        {
            for (Int_t i = 0; i < nMapped; i++)
            {
                R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);
                strip = mItem->GetStrip();

                if (detector + 1 == mItem->GetDetector() && strip == nstrips * 2 + 1)
                {
                    energy = mItem->GetEnergy1();
                }
            }
        }

        // 1 strip x direction
        if (x_mult[detector] == 2)
        {
            for (Int_t i = 0; i < nMapped; i++)
            {
                R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);

                if (detector + 1 == mItem->GetDetector())
                {
                    strip = mItem->GetStrip();

                    // energy
                    if (e_mult[detector] == 0 && strip > nstrips && strip < nstrips * 2 + 1)
                    {
                        energy = mItem->GetEnergy1() + mItem->GetEnergy2();
                    }

                    // position
                    if (strip > nstrips && strip < nstrips * 2 + 1)
                    {
			u = (mItem->GetEnergy1() - mItem->GetEnergy2()) / (mItem->GetEnergy1() + mItem->GetEnergy2());		
			
			if(fHitPar->GetPspxParOrientation().At(detector) == 3){
		            x = sign_x[detector] *
				(offset[detector][strip - 1] + slope[detector][strip - 1] * u);
			} else if(fHitPar->GetPspxParOrientation().At(detector) == 2) {
			    x = sign_x[detector] *
				(offset[detector][strip - nstrips - 1] + slope[detector][strip - nstrips - 1] * u);
			
			}
                        sigma_x = 0; // ToDo  

                        if (y_mult[detector] != 2 && y_mult[detector] != 4)
                        {
                            v = (strip - 1.5 * nstrips - 0.5) / (nstrips / 2.);
                            y = sign_y[detector] * v * fHitPar->GetPspxParLength().At(detector) / 2.;
                            sigma_y = 1; // ToDo
                        }
                    }
                }
            }
        }

        // 1 strip y direction
        if (y_mult[detector] == 2)
        {
            for (Int_t i = 0; i < nMapped; i++)
            {
                R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);

                if (detector + 1 == mItem->GetDetector())
                {
                    strip = mItem->GetStrip();

                    // energy
                    if (e_mult[detector] == 0 && strip > 0 && strip < nstrips + 1)
                    {
                        energy = mItem->GetEnergy1() + mItem->GetEnergy2();
                    }

                    // position
                    if (strip > 0 && strip < nstrips + 1)
                    {
                        v = (mItem->GetEnergy1() - mItem->GetEnergy2()) / (mItem->GetEnergy1() + mItem->GetEnergy2());
                        y = sign_y[detector] * (offset[detector][strip - 1] + slope[detector][strip - 1] * v);
                        sigma_y = 0; // ToDo

                        if (x_mult[detector] != 2 && x_mult[detector] != 4)
                        {
                            u = (strip - 0.5 * nstrips - 0.5) / (nstrips / 2.);
                            x = sign_x[detector] * u * fHitPar->GetPspxParLength().At(detector) / 2.;
                            sigma_x = 1; // ToDo
                        }
                    }
                }
            }
        }

        // 2 strips x direction
        if (x_mult[detector] == 4)
        {
            for (Int_t i = 0; i < nMapped; i++)
            {
                R3BPspxCalData* mItem1 = (R3BPspxCalData*)fCalItems->At(i);

                if (detector + 1 == mItem1->GetDetector())
                {
                    strip1 = mItem1->GetStrip();

                    // energy
                    if (e_mult[detector] == 0 && strip1 > nstrips && strip1 < nstrips * 2 + 1)
                    {
                        for (Int_t j = i + 1; j < nMapped; j++)
                        { // start with i+1 to avoid double counting
                            R3BPspxCalData* mItem2 = (R3BPspxCalData*)fCalItems->At(j);
                            strip2 = mItem2->GetStrip();

                            if (detector + 1 == mItem2->GetDetector() && strip2 > nstrips && strip2 < nstrips * 2 + 1 &&
                                (strip1 == strip2 + 1 || strip1 == strip2 - 1))
                            {
                                energy = mItem1->GetEnergy1() + mItem1->GetEnergy2() + mItem2->GetEnergy1() +
                                         mItem2->GetEnergy2();
                            }
                        }
                    }

                    // position
                    if (strip1 > nstrips && strip1 < nstrips * 2 + 1)
                    {
                        for (Int_t j = i + 1; j < nMapped; j++)
                        { // start with i+1 to avoid double counting
                            R3BPspxCalData* mItem2 = (R3BPspxCalData*)fCalItems->At(j);
                            strip2 = mItem2->GetStrip();

                            if (detector + 1 == mItem2->GetDetector() && strip2 > nstrips && strip2 < nstrips * 2 + 1 &&
                                (strip1 == strip2 + 1 || strip1 == strip2 - 1))
                            {
                                u = ((mItem1->GetEnergy1() + mItem2->GetEnergy1()) -
                                     (mItem1->GetEnergy2() + mItem2->GetEnergy2())) /
                                    (mItem1->GetEnergy1() + mItem2->GetEnergy1() + mItem1->GetEnergy2() +
                                     mItem2->GetEnergy2());
				    
				if(fHitPar->GetPspxParOrientation().At(detector) == 3){
				    x = sign_x[detector] *
                                    ((offset[detector][strip1 - 1] + offset[detector][strip2 - 1]) /
                                         2. +
                                     (slope[detector][strip1 - 1] + slope[detector][strip2 - 1]) /
                                         2. * u);
				} else if(fHitPar->GetPspxParOrientation().At(detector) == 2) {
				    x = sign_x[detector] *
                                    ((offset[detector][strip1 - nstrips - 1] + offset[detector][strip2 - nstrips - 1]) /
                                         2. +
                                     (slope[detector][strip1 - nstrips - 1] + slope[detector][strip2 - nstrips - 1]) /
                                         2. * u);
				}
				sigma_x = 0; // ToDo

                                if (y_mult[detector] != 2 && y_mult[detector] != 4)
                                {
                                    v = ((strip1 + strip2) / 2. - 1.5 * nstrips - 0.5) / (nstrips / 2.);
                                    y = sign_y[detector] * v * fHitPar->GetPspxParLength().At(detector) / 2.;
                                    sigma_y = 1; // ToDo
                                }
                            }
                        }
                    }
                }
            }
        }

        // 2 strips y direction
        if (y_mult[detector] == 4)
        {
            for (Int_t i = 0; i < nMapped; i++)
            {
                R3BPspxCalData* mItem1 = (R3BPspxCalData*)fCalItems->At(i);

                if (detector + 1 == mItem1->GetDetector())
                {
                    strip1 = mItem1->GetStrip();

                    // energy
                    if (e_mult[detector] == 0 && strip1 > 0 && strip1 < nstrips + 1)
                    {
                        for (Int_t j = i + 1; j < nMapped; j++)
                        { // start with i+1 to avoid double counting
                            R3BPspxCalData* mItem2 = (R3BPspxCalData*)fCalItems->At(j);
                            strip2 = mItem2->GetStrip();

                            if (detector + 1 == mItem2->GetDetector() && strip2 > 0 && strip2 < nstrips + 1 &&
                                (strip1 == strip2 + 1 || strip1 == strip2 - 1))
                            {
                                energy = mItem1->GetEnergy1() + mItem1->GetEnergy2() + mItem2->GetEnergy1() +
                                         mItem2->GetEnergy2();
                            }
                        }
                    }

                    // position
                    if (strip1 > 0 && strip1 < nstrips + 1)
                    {
                        for (Int_t j = i + 1; j < nMapped; j++)
                        { // start with i+1 to avoid double counting
                            R3BPspxCalData* mItem2 = (R3BPspxCalData*)fCalItems->At(j);
                            strip2 = mItem2->GetStrip();

                            if (detector + 1 == mItem2->GetDetector() && strip2 > 0 && strip2 < nstrips + 1 &&
                                (strip1 == strip2 + 1 || strip1 == strip2 - 1))
                            {
                                v = ((mItem1->GetEnergy1() + mItem2->GetEnergy1()) -
                                     (mItem1->GetEnergy2() + mItem2->GetEnergy2())) /
                                    (mItem1->GetEnergy1() + mItem2->GetEnergy1() + mItem1->GetEnergy2() +
                                     mItem2->GetEnergy2());
                                y = sign_y[detector] *
                                    ((offset[detector][strip1 - 1] + offset[detector][strip2 - 1]) / 2. +
                                     (slope[detector][strip1 - 1] + slope[detector][strip2 - 1]) / 2. * v);
                                sigma_y = 0; // ToDo

                                if (x_mult[detector] != 2 && x_mult[detector] != 4)
                                {
                                    u = ((strip1 + strip2) / 2. - 0.5 * nstrips - 0.5) / (nstrips / 2.);
                                    x = sign_x[detector] * u * fHitPar->GetPspxParLength().At(detector) / 2.;
                                    sigma_x = 1; // ToDo
                                }
                            }
                        }
                    }
                }
            }
        }

        new ((*fHitItems)[fHitItems->GetEntriesFast()])
            R3BPspxHitData(detector + 1,
                           u,
                           v,
                           x,
                           y,
                           sigma_x,
                           sigma_y,
                           energy,
                           x_mult[detector] + y_mult[detector] + e_mult[detector],
                           x_mult[detector],
                           y_mult[detector]);
    }
}

void R3BPspxCal2Hit::FinishEvent() { fHitItems->Clear(); }

void R3BPspxCal2Hit::FinishTask() {}

// void R3BPspxCal2Hit::WriteHistos() {}

ClassImp(R3BPspxCal2Hit)
