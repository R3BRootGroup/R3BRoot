// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxMapped2Precal                    -----
// -----                    Created  13-03-2017 by I. Syndikus		   -----
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
#include "R3BPspxMapped2Precal.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxPrecalData.h"
#include "R3BPspxPrecalPar.h"

R3BPspxMapped2Precal::R3BPspxMapped2Precal()
    : fMappedItems(NULL)
    , fPrecalItems(new TClonesArray("R3BPspxPrecalData"))
{
}

R3BPspxMapped2Precal::R3BPspxMapped2Precal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fPrecalItems(new TClonesArray("R3BPspxPrecalData"))
{
}

R3BPspxMapped2Precal::~R3BPspxMapped2Precal() {}

InitStatus R3BPspxMapped2Precal::Init()
{
    /**
     * Initialize output data. Read input data and parameters.
     * The parameters get saved in dedicated arrays.
     * Print parameters, if verbosity is set to INFO.
     */

    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fMappedItems = (TClonesArray*)fMan->GetObject("PspxMappedData"); // = branch name in TTree
    if (!fMappedItems)
    {
        printf("Couldn't get handle on PSPX mapped items\n");
        return kFATAL;
    }

    // fPrecalItems = (TClonesArray*)fMan->GetObject("R3BPspxPrecalData");
    FairRootManager::Instance()->Register("PspxPrecalData", "Pspx", fPrecalItems, kTRUE);

    // fPrecalPar->printparams();

    // Initialisation of gain parameters
    gain.resize(fPrecalPar->GetPspxParDetector());
    for (Int_t i = 0; i < fPrecalPar->GetPspxParDetector(); i++)
    {
        if (fPrecalPar->GetPspxParOrientation().At(i) == 1 || fPrecalPar->GetPspxParOrientation().At(i) == 2)
        { // strips on 1 side
            gain[i].resize(fPrecalPar->GetPspxParStrip().At(i));
        }
        else if (fPrecalPar->GetPspxParOrientation().At(i) == 3)
        { // strips on 2 side1
            gain[i].resize(fPrecalPar->GetPspxParStrip().At(i) * 2);
        }
    }
    Int_t start_detector = 0; // entries, not lines
    for (Int_t i = 0; i < gain.size(); i++)
    { // detectors
        for (Int_t j = 0; j < gain[i].size(); j++)
        { // strips
            gain[i][j] = fPrecalPar->GetPspxParGain().At(start_detector + 3 + j * 2);
        }
        start_detector = start_detector + 2 + 2 * fPrecalPar->GetPspxParStrip().At(i);
    }
    LOG(INFO) << "R3BPspxMapped2Precal :: Init() " << FairLogger::endl;
    for (Int_t i = 0; i < fPrecalPar->GetPspxParDetector(); i++)
    {
        for (Int_t j = 0; j < gain[i].size(); j++)
        {
            LOG(INFO) << "gain[" << i << "][" << j << "]=" << gain[i][j] << FairLogger::endl;
        }
    }

    // Initialisation of energy threshold parameters
    energythreshold.resize(fPrecalPar->GetPspxParDetector());
    for (Int_t i = 0; i < fPrecalPar->GetPspxParDetector(); i++)
    {
        if (fPrecalPar->GetPspxParOrientation().At(i) == 1 || fPrecalPar->GetPspxParOrientation().At(i) == 2)
        { // cathode cannel available
            energythreshold[i].resize(fPrecalPar->GetPspxParStrip().At(i) * 2 + 1);
        }
        else if (fPrecalPar->GetPspxParOrientation().At(i) == 3)
        {
            energythreshold[i].resize(fPrecalPar->GetPspxParStrip().At(i) * 4);
        }
    }
    start_detector = 0; // entries, not lines
    for (Int_t i = 0; i < energythreshold.size(); i++)
    { // detectors
        for (Int_t j = 0; j < energythreshold[i].size(); j++)
        { // channels
            energythreshold[i][j] = fPrecalPar->GetPspxParEnergyThreshold().At(start_detector + 3 + j * 2);
        }
        if (fPrecalPar->GetPspxParOrientation().At(i) == 1 || fPrecalPar->GetPspxParOrientation().At(i) == 2)
        { // cathode cannel available
            start_detector = start_detector + 2 + 2 * 2 * fPrecalPar->GetPspxParStrip().At(i) + 2;
        }
        else if (fPrecalPar->GetPspxParOrientation().At(i) == 3)
        {
            start_detector = start_detector + 2 + 2 * 4 * fPrecalPar->GetPspxParStrip().At(i);
        }
    }
    // LOG(INFO) << "R3BPspxMapped2Precal :: Init() " << FairLogger::endl;
    for (Int_t i = 0; i < fPrecalPar->GetPspxParDetector(); i++)
    {
        for (Int_t j = 0; j < energythreshold[i].size(); j++)
        {
            LOG(INFO) << "energythreshold[" << i << "][" << j << "]=" << energythreshold[i][j] << FairLogger::endl;
        }
    }

    return kSUCCESS;
}

void R3BPspxMapped2Precal::SetParContainers()
{
    /**
     * Initialize/Reads parameter file for conversion.
     */

    LOG(INFO) << "R3BPspxMapped2Precal :: SetParContainers() " << FairLogger::endl;

    fPrecalPar = (R3BPspxPrecalPar*)FairRuntimeDb::instance()->getContainer("R3BPspxPrecalPar");

    // Get Base Container
    // FairRunAna* ana = FairRunAna::Instance();
    // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    // fPrecalPar = (R3BPspxPrecalPar*) (rtdb->getContainer("R3BPspxPrecalPar"));

    if (!fPrecalPar)
    {
        LOG(ERROR) << "Could not get access to R3BPspxPrecalPar-Container." << FairLogger::endl;
        return;
    }

    fPrecalPar->printparams();
}

InitStatus R3BPspxMapped2Precal::ReInit()
{
    /**
     * Initialize/Reads parameter file for conversion.
     */

    LOG(INFO) << " R3BPspxMapped2Precal :: ReInit() " << FairLogger::endl;

    // FairRunAna* ana = FairRunAna::Instance();
    // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    // fPrecalPar = (R3BPspxPrecalPar*) (rtdb->getContainer("R3BPspxPrecalPar"));

    fPrecalPar = (R3BPspxPrecalPar*)FairRuntimeDb::instance()->getContainer("R3BPspxPrecalPar");

    if (!fPrecalPar)
    {
        LOG(ERROR) << "Could not get access to R3BPspxPrecalPar-Container." << FairLogger::endl;
        return kFATAL;
    }

    return kSUCCESS;
}

void R3BPspxMapped2Precal::Exec(Option_t* option)
{
    /**
     * Does the conversion from Mapped to Precal level. It is called for every event.
     * Energies, which are below a channel specific threshold, will be ignored.
     * Applies (strip specific) gains to the energy entries of even channels. This is
     * the first calibration step for the position reconstruction.
     */

    if (!fMappedItems)
    {
        printf("Cannot access PSPX mapped items\n");
        return;
    }

    UShort_t detector1;
    UShort_t detector2;
    UShort_t strip1;
    UShort_t strip2;
    Float_t energy1;
    Float_t energy2;

    Int_t nMapped = fMappedItems->GetEntries();

    // Temp array to avoid double counting
    Bool_t used[nMapped];
    for (Int_t k = 0; k < nMapped; k++)
    {
        used[k] = false;
    }

    // Calculating strip and energys
    for (Int_t i = 0; i < nMapped; i++)
    {
        detector1 = std::numeric_limits<UShort_t>::quiet_NaN();
        detector2 = std::numeric_limits<UShort_t>::quiet_NaN();
        strip1 = std::numeric_limits<UShort_t>::quiet_NaN();
        strip2 = std::numeric_limits<UShort_t>::quiet_NaN();
        energy1 = std::numeric_limits<Float_t>::quiet_NaN();
        energy2 = std::numeric_limits<Float_t>::quiet_NaN();

        R3BPspxMappedData* mItem1 = (R3BPspxMappedData*)fMappedItems->At(i);
        detector1 = mItem1->GetDetector();
        if (fPrecalPar->GetPspxParStrip().At(detector1 - 1) == 0)
            continue;

	// get rif of error message from Febex
	if(mItem1->GetEnergy()==-3075811 || mItem1->GetEnergy()==-3075810)
            continue;

        // calculating stripnumber, only for valid channel number
        if (mItem1->GetChannel() > 0 && mItem1->GetChannel() < fPrecalPar->GetPspxParStrip().At(detector1 - 1) * 4 + 1)
        { // strips
            strip1 = (mItem1->GetChannel() + 1) / 2;
        }
        else if (mItem1->GetChannel() == fPrecalPar->GetPspxParStrip().At(detector1 - 1) * 4 + 1)
        { // cathode
            strip1 = fPrecalPar->GetPspxParStrip().At(detector1 - 1) * 2 + 1;
        }

        // setting energy(s) for strips
        if (strip1 > 0 && strip1 < fPrecalPar->GetPspxParStrip().At(detector1 - 1) * 2 + 2 && used[i] == false)
        {
            if (strip1 == fPrecalPar->GetPspxParStrip().At(detector1 - 1) * 2 + 1)
            { // cathode
                if (TMath::Abs(mItem1->GetEnergy()) <
                    energythreshold[detector1 - 1][fPrecalPar->GetPspxParStrip().At(detector1 - 1) * 2])
                    continue;
                energy1 = mItem1->GetEnergy();
                energy2 = std::numeric_limits<Float_t>::quiet_NaN();
            }
            else
            { // strips
                if (fPrecalPar->GetPspxParOrientation().At(detector1 - 1) == 1 ||
                    fPrecalPar->GetPspxParOrientation().At(detector1 - 1) == 3)
                { // vertical strips or strips in both directions
                    if (TMath::Abs(mItem1->GetEnergy()) < energythreshold[detector1 - 1][mItem1->GetChannel() - 1])
                        continue;
                }
                else if (fPrecalPar->GetPspxParOrientation().At(detector1 - 1) == 2)
                { // horizontal strips
                    if (TMath::Abs(mItem1->GetEnergy()) <
                        energythreshold[detector1 - 1]
                                       [mItem1->GetChannel() - 1 - 2 * fPrecalPar->GetPspxParStrip().At(detector1 - 1)])
                        continue; // different counting of array and in root file + counting from #Strips+1 to #Strips*2
                                  // for horizontal strips
                }
                if (mItem1->GetChannel() % 2 == 1)
                {
                    energy1 = mItem1->GetEnergy();
                    energy2 = std::numeric_limits<Float_t>::quiet_NaN();
                }
                else
                {
                    energy1 = std::numeric_limits<Float_t>::quiet_NaN();
                    energy2 = mItem1->GetEnergy();
                }

                // searching for second entry of strip
                for (Int_t j = i + 1; j < nMapped; j++)
                {
                    R3BPspxMappedData* mItem2 = (R3BPspxMappedData*)fMappedItems->At(j);
                    detector2 = mItem2->GetDetector();
                    if (fPrecalPar->GetPspxParStrip().At(detector2 - 1) == 0)
                        continue;

		    // get rif of error message from Febex
		    if(mItem2->GetEnergy()==-3075811 || mItem2->GetEnergy()==-3075810)
		        continue;

                    // calculating stripnumber
                    if (mItem2->GetChannel() > 0 &&
                        mItem2->GetChannel() < fPrecalPar->GetPspxParStrip().At(detector2 - 1) * 4 + 1)
                    { // strips
                        strip2 = (mItem2->GetChannel() + 1) / 2;
                    }
                    else if (mItem2->GetChannel() == fPrecalPar->GetPspxParStrip().At(detector2 - 1) * 4 + 1)
                    { // cathode
                        strip2 = fPrecalPar->GetPspxParStrip().At(detector2 - 1) * 2 + 1;
                    }

                    if (detector1 == detector2 && strip1 == strip2)
                    {
                        used[j] = true; // to avoid that the same entry is read a second time in loop i
                        if (fPrecalPar->GetPspxParOrientation().At(detector2 - 1) == 1 ||
                            fPrecalPar->GetPspxParOrientation().At(detector1 - 1) == 3)
                        { // vertical strips or strips in both directions
                            if (TMath::Abs(mItem2->GetEnergy()) < energythreshold[detector2 - 1][mItem2->GetChannel() - 1])
                                continue;
                        }
                        else if (fPrecalPar->GetPspxParOrientation().At(detector2 - 1) == 2)
                        { // horizontal strips
                            if (TMath::Abs(mItem2->GetEnergy()) <
                                energythreshold[detector2 - 1][mItem2->GetChannel() - 1 -
                                                               2 * fPrecalPar->GetPspxParStrip().At(detector1 - 1)])
                                continue; // different counting of array and in root file + counting from #Strips+1 to
                                          // #Strips*2 for horizontal strips
                        }
                        if (mItem2->GetChannel() % 2 == 0 && mItem1->GetChannel() % 2 == 1)
                        {
                            energy2 = mItem2->GetEnergy();
                        }
                        else
                        {
                            energy1 = mItem2->GetEnergy();
                        }
                        if (fPrecalPar->GetPspxParOrientation().At(detector1 - 1) == 1 ||
                            fPrecalPar->GetPspxParOrientation().At(detector1 - 1) == 3)
                        { // vertical strips or strips in both directions
                            energy2 = energy2 *
                                      gain[detector1 - 1][strip1 - 1]; // different counting of array and in root file
                        }
                        else if (fPrecalPar->GetPspxParOrientation().At(detector1 - 1) == 2)
                        { // horizontal strips
                            energy2 = energy2 *
                                      gain[detector1 - 1]
                                          [strip1 - 1 -
                                           fPrecalPar->GetPspxParStrip().At(detector1 - 1)]; // different counting
                            // of array and in root file + counting from #Strips+1 to #Strips*2 for horizontal strips
                        }
                    }
                }
            }

            new ((*fPrecalItems)[fPrecalItems->GetEntriesFast()])
                R3BPspxPrecalData(detector1, strip1, energy1, energy2);
        }
    }
}

void R3BPspxMapped2Precal::FinishEvent() { fPrecalItems->Clear(); }

void R3BPspxMapped2Precal::FinishTask() {}

// void R3BPspxMapped2Precal::WriteHistos() {}

ClassImp(R3BPspxMapped2Precal)
