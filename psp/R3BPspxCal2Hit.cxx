// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxCal2Hit                          -----
// -----	     created 09-03-2016 by I. Syndikus		           -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <cmath>
#include <iostream>
#include <limits>

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

R3BPspxCal2Hit::R3BPspxCal2Hit(const char* name, Int_t iVerbose, Float_t range)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BPspxHitData"))
{
    rangeE = range;
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
    sign_pos_x.resize(fHitPar->GetPspxParDetector());
    sign_pos_y.resize(fHitPar->GetPspxParDetector());
    sign_strip_x.resize(fHitPar->GetPspxParDetector());
    sign_strip_y.resize(fHitPar->GetPspxParDetector());
    for (UInt_t i = 0; i < fHitPar->GetPspxParDetector(); i++)
    {
        sign_pos_x[i] = fHitPar->GetPspxParOrientationXPosition().At(i);
        sign_pos_y[i] = fHitPar->GetPspxParOrientationYPosition().At(i);
        sign_strip_x[i] = fHitPar->GetPspxParOrientationXStrip().At(i);
        sign_strip_y[i] = fHitPar->GetPspxParOrientationYStrip().At(i);
    }

    for (Int_t i = 0; i < fHitPar->GetPspxParDetector(); i++)
    {
        LOG(INFO) << "sign_pos_x[" << i << "]=" << sign_pos_x[i] << FairLogger::endl;
        LOG(INFO) << "sign_pos_y[" << i << "]=" << sign_pos_y[i] << FairLogger::endl;
        LOG(INFO) << "sign_strip_x[" << i << "]=" << sign_strip_x[i] << FairLogger::endl;
        LOG(INFO) << "sign_strip_y[" << i << "]=" << sign_strip_y[i] << FairLogger::endl;
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
     * The total energy is either determined from the cathode (back, X1) or from the mean of the sum of the front and
     * the back channels after a check whether these two values are roughly the same (strips, X5).
     */

    if (!fCalItems)
    {
        printf("Cannot access PSPX cal items\n");
        return;
    }

    // initialize
    Float_t u;
    Float_t v;
    Float_t x;
    Float_t y;
    Float_t sigma_x;
    Float_t sigma_y;
    Float_t energy;

    Float_t energy_x[fHitPar->GetPspxParDetector()];
    Float_t energy_y[fHitPar->GetPspxParDetector()];
    UShort_t mult_x[fHitPar->GetPspxParDetector()];
    UShort_t mult_y[fHitPar->GetPspxParDetector()];
    UShort_t mult_e[fHitPar->GetPspxParDetector()];

    UShort_t nstrips;
    UShort_t strip;
    UShort_t strip1;
    UShort_t strip2;

    for (UShort_t j = 0; j < fHitPar->GetPspxParDetector(); j++)
    {
        energy_x[j] = 0;
        energy_y[j] = 0;
        mult_x[j] = 0;
        mult_y[j] = 0;
        mult_e[j] = 0;
    }

    Int_t nCal = fCalItems->GetEntries();

    // calculating multiplicities
    for (Int_t i = 0; i < nCal; i++)
    {
        strip = std::numeric_limits<UShort_t>::quiet_NaN();

        R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);
        strip = mItem->GetStrip();
        UInt_t detector = mItem->GetDetector();

        if (strip > 0 && strip < fHitPar->GetPspxParStrip().At(detector - 1) + 1)
        {
            if (mItem->GetEnergy1() != 0 && !std::isnan(mItem->GetEnergy1()))
            {
                mult_y[detector - 1]++;
                energy_y[detector - 1] += mItem->GetEnergy1();
            }
            if (mItem->GetEnergy2() != 0 && !std::isnan(mItem->GetEnergy2()))
            {
                mult_y[detector - 1]++;
                energy_y[detector - 1] += mItem->GetEnergy2();
            }
        }
        else if (strip > fHitPar->GetPspxParStrip().At(detector - 1) &&
                 strip < fHitPar->GetPspxParStrip().At(detector - 1) * 2 + 1)
        {
            if (mItem->GetEnergy1() != 0 && !std::isnan(mItem->GetEnergy1()))
            {
                mult_x[detector - 1]++;
                energy_x[detector - 1] += mItem->GetEnergy1();
            }
            if (mItem->GetEnergy2() != 0 && !std::isnan(mItem->GetEnergy2()))
            {
                mult_x[detector - 1]++;
                energy_x[detector - 1] += mItem->GetEnergy2();
            }
        }
        else if (strip == fHitPar->GetPspxParStrip().At(detector - 1) * 2 + 1)
        {
            if (mItem->GetEnergy1() != 0 && !std::isnan(mItem->GetEnergy1()))
            {
                mult_e[detector - 1]++;
            }
        }
    }

    // calculating position and energy for each detector
    for (UShort_t detector = 0; detector < fHitPar->GetPspxParDetector(); detector++)
    {
        if (mult_x[detector] == 0 && mult_y[detector] == 0 && mult_e[detector] == 0)
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

        // energy
        if (mult_e[detector] == 1) // detector type X1
        {
            for (Int_t i = 0; i < nCal; i++)
            {
                R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);
                strip = mItem->GetStrip();

                if (detector + 1 == mItem->GetDetector() && strip == nstrips * 2 + 1)
                {
                    energy = mItem->GetEnergy1();
                }
            }
        }
        else if (mult_e[detector] == 0 && mult_x[detector] != 0 && mult_y[detector] != 0) // detector type X5
        {
            // be carefull: energy_x is negative
            if ((std::abs(energy_x[detector] + energy_y[detector]) < rangeE * energy_y[detector]) ||
                (std::abs(energy_x[detector] + energy_y[detector]) <
                 rangeE * energy_x[detector])) // energy on front and back have the same energy
            {
                // LOG(INFO) << "R3BPspxCal2Hit " << detector << " " << -energy_x[detector] << " " << energy_y[detector]
                // << FairLogger::endl;
                energy = (-energy_x[detector] + energy_y[detector]) / 2.;
            }
            else
            {
                LOG(WARNING) << "R3BPspxCal2Hit: Energy on front (" << energy_y[detector] << ") and back ("
                             << -energy_x[detector] << ") don't match." << FairLogger::endl;
                continue;
            }
        }
        else
        {
            LOG(ERROR) << "R3BPspxCal2Hit: Something went terribly wrong... or its just noise?" << FairLogger::endl;
            continue;
        }

        // position x direction
        if (mult_x[detector] == 2) // 1 strip hit
        {
            for (Int_t i = 0; i < nCal; i++)
            {
                R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);

                if (detector + 1 == mItem->GetDetector())
                {
                    strip = mItem->GetStrip();

                    if (strip > nstrips && strip < nstrips * 2 + 1)
                    {
                        u = (mItem->GetEnergy1() - mItem->GetEnergy2()) / (mItem->GetEnergy1() + mItem->GetEnergy2());

                        if (fHitPar->GetPspxParOrientation().At(detector) == 3)
                        {
                            x = sign_pos_x[detector] * (offset[detector][strip - 1] + slope[detector][strip - 1] * u);
                        }
                        else if (fHitPar->GetPspxParOrientation().At(detector) == 2)
                        {
                            x = sign_pos_x[detector] *
                                (offset[detector][strip - nstrips - 1] + slope[detector][strip - nstrips - 1] * u);
                        }
                        sigma_x = 0; // TODO how do we get the uncertainty for the calclated value?

                        // position y direction for cases in which it cannot be calculated precisely
                        if (mult_y[detector] != 2 && mult_y[detector] != 4)
                        {
                            v = (strip - 1.5 * nstrips - 0.5) / (nstrips / 2.);
                            y = sign_strip_y[detector] * v * fHitPar->GetPspxParLength().At(detector) / 2.;
                            sigma_y = 1; // TODO rms=width/sqrt(12), set width or read from parameter file?
                        }
                    }
                }
            }
        }
        else if (mult_x[detector] == 4) // 2 strips hit
        {
            for (Int_t i = 0; i < nCal; i++)
            {
                R3BPspxCalData* mItem1 = (R3BPspxCalData*)fCalItems->At(i);

                if (detector + 1 == mItem1->GetDetector())
                {
                    strip1 = mItem1->GetStrip();

                    if (strip1 > nstrips && strip1 < nstrips * 2 + 1)
                    {
                        for (Int_t j = i + 1; j < nCal; j++) // start with i+1 to avoid double counting
                        {
                            R3BPspxCalData* mItem2 = (R3BPspxCalData*)fCalItems->At(j);
                            strip2 = mItem2->GetStrip();

                            if (detector + 1 == mItem2->GetDetector() && strip2 > nstrips && strip2 < nstrips * 2 + 1 &&
                                (strip1 == strip2 + 1 || strip1 == strip2 - 1)) // neighboring strips
                            {
                                u = ((mItem1->GetEnergy1() + mItem2->GetEnergy1()) -
                                     (mItem1->GetEnergy2() + mItem2->GetEnergy2())) /
                                    (mItem1->GetEnergy1() + mItem2->GetEnergy1() + mItem1->GetEnergy2() +
                                     mItem2->GetEnergy2());

                                if (fHitPar->GetPspxParOrientation().At(detector) == 3)
                                {
                                    x = sign_pos_x[detector] *
                                        ((offset[detector][strip1 - 1] + offset[detector][strip2 - 1]) / 2. +
                                         (slope[detector][strip1 - 1] + slope[detector][strip2 - 1]) / 2. * u);
                                }
                                else if (fHitPar->GetPspxParOrientation().At(detector) == 2)
                                {
                                    x = sign_pos_x[detector] * ((offset[detector][strip1 - nstrips - 1] +
                                                                 offset[detector][strip2 - nstrips - 1]) /
                                                                    2. +
                                                                (slope[detector][strip1 - nstrips - 1] +
                                                                 slope[detector][strip2 - nstrips - 1]) /
                                                                    2. * u);
                                }
                                sigma_x = 0; // TODO how do we get the uncertainty for the calclated value?

                                // position y direction for cases in which it cannot be calculated precisely
                                if (mult_y[detector] != 2 && mult_y[detector] != 4)
                                {
                                    v = ((strip1 + strip2) / 2. - 1.5 * nstrips - 0.5) / (nstrips / 2.);
                                    y = sign_strip_y[detector] * v * fHitPar->GetPspxParLength().At(detector) / 2.;
                                    sigma_y =
                                        1; // TODO how precisely do we know the position for a hit between 2 strips
                                }
                            }
                        }
                    }
                }
            }
        }

        // position y direction
        if (mult_y[detector] == 2) // 1 strip hit
        {
            for (Int_t i = 0; i < nCal; i++)
            {
                R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);

                if (detector + 1 == mItem->GetDetector())
                {
                    strip = mItem->GetStrip();

                    if (strip > 0 && strip < nstrips + 1)
                    {
                        v = (mItem->GetEnergy1() - mItem->GetEnergy2()) / (mItem->GetEnergy1() + mItem->GetEnergy2());
                        y = sign_pos_y[detector] * (offset[detector][strip - 1] + slope[detector][strip - 1] * v);
                        sigma_y = 0; // TODO how do we get the uncertainty for the calclated value?

                        // position x direction for cases in which it cannot be calculated precisely
                        if (mult_x[detector] != 2 && mult_x[detector] != 4)
                        {
                            u = (strip - 0.5 * nstrips - 0.5) / (nstrips / 2.);
                            x = sign_strip_x[detector] * u * fHitPar->GetPspxParLength().At(detector) / 2.;
                            sigma_x = 1; // TODO sigma=width/sqrt(12), set width or read from parameter file?
                        }
                    }
                }
            }
        }
        else if (mult_y[detector] == 4) // 2 strip hit
        {
            for (Int_t i = 0; i < nCal; i++)
            {
                R3BPspxCalData* mItem1 = (R3BPspxCalData*)fCalItems->At(i);

                if (detector + 1 == mItem1->GetDetector())
                {
                    strip1 = mItem1->GetStrip();

                    if (strip1 > 0 && strip1 < nstrips + 1)
                    {
                        for (Int_t j = i + 1; j < nCal; j++) // start with i+1 to avoid double counting
                        {
                            R3BPspxCalData* mItem2 = (R3BPspxCalData*)fCalItems->At(j);
                            strip2 = mItem2->GetStrip();

                            if (detector + 1 == mItem2->GetDetector() && strip2 > 0 && strip2 < nstrips + 1 &&
                                (strip1 == strip2 + 1 || strip1 == strip2 - 1)) // neighboring strips
                            {
                                v = ((mItem1->GetEnergy1() + mItem2->GetEnergy1()) -
                                     (mItem1->GetEnergy2() + mItem2->GetEnergy2())) /
                                    (mItem1->GetEnergy1() + mItem2->GetEnergy1() + mItem1->GetEnergy2() +
                                     mItem2->GetEnergy2());
                                y = sign_pos_y[detector] *
                                    ((offset[detector][strip1 - 1] + offset[detector][strip2 - 1]) / 2. +
                                     (slope[detector][strip1 - 1] + slope[detector][strip2 - 1]) / 2. * v);
                                sigma_y = 0; // TODO how do we get the uncertainty for the calclated value?

                                // position x direction for cases in which it cannot be calculated precisely
                                if (mult_x[detector] != 2 && mult_x[detector] != 4)
                                {
                                    u = ((strip1 + strip2) / 2. - 0.5 * nstrips - 0.5) / (nstrips / 2.);
                                    x = sign_strip_x[detector] * u * fHitPar->GetPspxParLength().At(detector) / 2.;
                                    sigma_x =
                                        1; // TODO how precisely do we know the position for a hit between 2 strips
                                }
                            }
                        }
                    }
                }
            }
        }

        // TODO what to do, if mult=4 but not 2 neighboring strips? how to calculate energy? two particles?

        new ((*fHitItems)[fHitItems->GetEntriesFast()])
            R3BPspxHitData(detector + 1,
                           u,
                           v,
                           x,
                           y,
                           sigma_x,
                           sigma_y,
                           energy,
                           mult_x[detector] + mult_y[detector] + mult_e[detector],
                           mult_x[detector],
                           mult_y[detector]);
    }
}

void R3BPspxCal2Hit::FinishEvent() { fHitItems->Clear(); }

void R3BPspxCal2Hit::FinishTask() {}

// void R3BPspxCal2Hit::WriteHistos() {}

ClassImp(R3BPspxCal2Hit)
