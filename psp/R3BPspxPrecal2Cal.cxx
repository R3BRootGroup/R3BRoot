// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxPrecal2Cal                       -----
// -----                    Created  20-03-2017 by I. Syndikus		   -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
#include <limits>

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

#include "R3BEventHeader.h"
#include "R3BPspxCalData.h"
#include "R3BPspxCalPar.h"
#include "R3BPspxPrecal2Cal.h"
#include "R3BPspxPrecalData.h"

R3BPspxPrecal2Cal::R3BPspxPrecal2Cal()
    : fPrecalItems(NULL)
    , fCalItems(new TClonesArray("R3BPspxCalData"))
{
}

R3BPspxPrecal2Cal::R3BPspxPrecal2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fPrecalItems(NULL)
    , fCalItems(new TClonesArray("R3BPspxCalData"))
{
}

R3BPspxPrecal2Cal::~R3BPspxPrecal2Cal() {}

InitStatus R3BPspxPrecal2Cal::Init()
{
    /**
     * Initialize output data. Read input data and parameters.
     * The parameters get saved in dedicated arrays.
     * Print parameters, if verbosity is set to INFO.
     */

    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fPrecalItems = (TClonesArray*)fMan->GetObject("PspxPrecalData"); // = branch name in TTree
    if (!fPrecalItems)
    {
        printf("Couldn't get handle on PSPX precal items\n");
        return kFATAL;
    }

    // fCalItems = (TClonesArray*)fMan->GetObject("R3BPspxCalData");
    FairRootManager::Instance()->Register("PspxCalData", "Pspx", fCalItems, kTRUE);

    // fCalPar->printparams();

    // Initialisation of gain parameters
    gain.resize(fCalPar->GetPspxParDetector());
    for (Int_t i = 0; i < fCalPar->GetPspxParDetector(); i++)
    {
        if (fCalPar->GetPspxParOrientation().At(i) == 1 || fCalPar->GetPspxParOrientation().At(i) == 2 ||
            fCalPar->GetPspxParOrientation().At(i) == 0)
        { // strips on 1 side
            gain[i].resize(fCalPar->GetPspxParStrip().At(i));
        }
        else if (fCalPar->GetPspxParOrientation().At(i) == 3)
        { // strips on 2 side1
            gain[i].resize(fCalPar->GetPspxParStrip().At(i) * 2);
        }
    }
    Int_t start_detector = 0; // entries, not lines
    for (Int_t i = 0; i < gain.size(); i++)
    { // detectors
        for (Int_t j = 0; j < gain[i].size(); j++)
        { // strips
            gain[i][j] = fCalPar->GetPspxParGain().At(start_detector + 3 + j * 2);
        }
        start_detector = start_detector + 2 + 2 * fCalPar->GetPspxParStrip().At(i);
    }
    LOG(INFO) << "R3BPspxPrecal2Cal :: Init() " << FairLogger::endl;
    for (Int_t i = 0; i < fCalPar->GetPspxParDetector(); i++)
    {
        for (Int_t j = 0; j < gain[i].size(); j++)
        {
            LOG(INFO) << "gain[" << i << "][" << j << "]=" << gain[i][j] << FairLogger::endl;
        }
    }

    return kSUCCESS;
}

void R3BPspxPrecal2Cal::SetParContainers()
{
    /**
     * Initialize/Reads parameter file for conversion.
     */

    LOG(INFO) << "R3BPspxPrecal2Cal :: SetParContainers() " << FairLogger::endl;

    fCalPar = (R3BPspxCalPar*)FairRuntimeDb::instance()->getContainer("R3BPspxCalPar");

    // Get Base Container
    // FairRunAna* ana = FairRunAna::Instance();
    // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    // fCalPar = (R3BPspxCalPar*) (rtdb->getContainer("R3BPspxCalPar"));

    if (!fCalPar)
    {
        LOG(ERROR) << "Could not get access to R3BPspxCalPar-Container." << FairLogger::endl;
        return;
    }

    fCalPar->printparams();
}

InitStatus R3BPspxPrecal2Cal::ReInit()
{
    /**
     * Initialize/Reads parameter file for conversion.
     */

    LOG(INFO) << " R3BPspxPrecal2Cal :: ReInit() " << FairLogger::endl;

    // FairRunAna* ana = FairRunAna::Instance();
    // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    // fCalPar = (R3BPspxCalPar*) (rtdb->getContainer("R3BPspxCalPar"));

    fCalPar = (R3BPspxCalPar*)FairRuntimeDb::instance()->getContainer("R3BPspxCalPar");

    if (!fCalPar)
    {
        LOG(ERROR) << "Could not get access to R3BPspxCalPar-Container." << FairLogger::endl;
        return kFATAL;
    }

    return kSUCCESS;
}

void R3BPspxPrecal2Cal::Exec(Option_t* option)
{
    /**
     * Does the conversion from Precal to Cal level. It is called for every event.
     * Applies (strip specific) gains to the energy entries of every strip. This is necessary 
     * for energy calibration.
     */

    if (!fPrecalItems)
    {
        printf("Cannot access PSPX precal items\n");
        return;
    }

    UShort_t detector;
    UShort_t strip;
    Float_t energy1;
    Float_t energy2;

    Int_t nPrecal = fPrecalItems->GetEntries();

    // Calculating strip and energys
    for (Int_t i = 0; i < nPrecal; i++)
    {
        detector = std::numeric_limits<UShort_t>::quiet_NaN();
        strip = std::numeric_limits<UShort_t>::quiet_NaN();
        energy1 = std::numeric_limits<Float_t>::quiet_NaN();
        energy2 = std::numeric_limits<Float_t>::quiet_NaN();

        R3BPspxPrecalData* mItem = (R3BPspxPrecalData*)fPrecalItems->At(i);
        detector = mItem->GetDetector();

        if (fCalPar->GetPspxParStrip().At(detector - 1) == 0)
            continue;

        strip = mItem->GetStrip();
        energy1 = mItem->GetEnergy1();
        energy2 = mItem->GetEnergy2();

        if (strip != fCalPar->GetPspxParStrip().At(detector - 1) * 2 + 1)
        { // strip
            if (fCalPar->GetPspxParOrientation().At(detector - 1) == 1 ||
                fCalPar->GetPspxParOrientation().At(detector - 1) == 3)
            {
                energy1 = energy1 * gain[detector - 1][strip - 1];
                energy2 = energy2 * gain[detector - 1][strip - 1];
            }
            else if (fCalPar->GetPspxParOrientation().At(detector - 1) == 2)
            {
                energy1 = energy1 * gain[detector - 1][strip - 1 - fCalPar->GetPspxParStrip().At(detector - 1)];
                energy2 = energy2 * gain[detector - 1][strip - 1 - fCalPar->GetPspxParStrip().At(detector - 1)];
            }
        }

        new ((*fCalItems)[fCalItems->GetEntriesFast()]) R3BPspxCalData(detector, strip, energy1, energy2);
    }
}

void R3BPspxPrecal2Cal::FinishEvent() { fCalItems->Clear(); }

void R3BPspxPrecal2Cal::FinishTask() {}

// void R3BPspxPrecal2Cal::WriteHistos() {}

ClassImp(R3BPspxPrecal2Cal)
