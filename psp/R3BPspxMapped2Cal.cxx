// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxMapped2Cal                       -----
// -----                    Created  21-01-2016 by R. Plag                 -----
// -----		   renamed 09-03-2016 by I. Syndikus		   -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
/* Convert Mapped data to Cal data, meaning: apply (offset and) gain
 * to the energy values.
 */

#include <iostream>
using namespace std;

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

#include "R3BEventHeader.h"
#include "R3BPspxCalData.h"
#include "R3BPspxCalPar.h"
#include "R3BPspxMapped2Cal.h"
#include "R3BPspxMappedData.h"

R3BPspxMapped2Cal::R3BPspxMapped2Cal()
    : fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BPspxCalData"))
{
}

R3BPspxMapped2Cal::R3BPspxMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BPspxCalData"))
{
}

R3BPspxMapped2Cal::~R3BPspxMapped2Cal() {}

InitStatus R3BPspxMapped2Cal::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fMappedItems = (TClonesArray*)fMan->GetObject("PspxMappedData"); // = branch name in TTree
    if (!fMappedItems)
    {
        printf("Couldn't get handle on PSPX mapped items\n");
        return kFATAL;
    }

    // fCalItems = (TClonesArray*)fMan->GetObject("R3BPspxCalData");
    FairRootManager::Instance()->Register("PspxCalData", "Pspx", fCalItems, kTRUE);

    // fCalPar->printparams();

    // Initialisation of gain parameters
    gain.resize(fCalPar->GetPspxParDetector());
    for (Int_t i = 0; i < fCalPar->GetPspxParDetector(); i++)
    {
        if (fCalPar->GetPspxParOrientation().At(i) == 1 || fCalPar->GetPspxParOrientation().At(i) == 2)
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
    LOG(INFO) << "R3BPspxMapped2Cal :: Init() " << FairLogger::endl;
    for (Int_t i = 0; i < fCalPar->GetPspxParDetector(); i++)
    {
        for (Int_t j = 0; j < gain[i].size(); j++)
        {
            LOG(INFO) << "gain[" << i << "][" << j << "]=" << gain[i][j] << FairLogger::endl;
        }
    }

    // Initialisation of energy threshold parameters
    energythreshold.resize(fCalPar->GetPspxParDetector());
    for (Int_t i = 0; i < fCalPar->GetPspxParDetector(); i++)
    {
        if (fCalPar->GetPspxParOrientation().At(i) == 1 || fCalPar->GetPspxParOrientation().At(i) == 2)
        { // cathode cannel available
            energythreshold[i].resize(fCalPar->GetPspxParStrip().At(i) * 2 + 1);
        }
        else if (fCalPar->GetPspxParOrientation().At(i) == 3)
        {
            energythreshold[i].resize(fCalPar->GetPspxParStrip().At(i) * 4);
        }
    }
    start_detector = 0; // entries, not lines
    for (Int_t i = 0; i < energythreshold.size(); i++)
    { // detectors
        for (Int_t j = 0; j < energythreshold[i].size(); j++)
        { // channels
            energythreshold[i][j] = fCalPar->GetPspxParEnergyThreshold().At(start_detector + 3 + j * 2);
        }
        if (fCalPar->GetPspxParOrientation().At(i) == 1 || fCalPar->GetPspxParOrientation().At(i) == 2)
        { // cathode cannel available
            start_detector = start_detector + 2 + 2 * 2 * fCalPar->GetPspxParStrip().At(i) + 2;
        }
        else if (fCalPar->GetPspxParOrientation().At(i) == 3)
        {
            start_detector = start_detector + 2 + 2 * 4 * fCalPar->GetPspxParStrip().At(i);
        }
    }
    // LOG(INFO) << "R3BPspxMapped2Cal :: Init() " << FairLogger::endl;
    for (Int_t i = 0; i < fCalPar->GetPspxParDetector(); i++)
    {
        for (Int_t j = 0; j < energythreshold[i].size(); j++)
        {
            LOG(INFO) << "energythreshold[" << i << "][" << j << "]=" << energythreshold[i][j] << FairLogger::endl;
        }
    }

    return kSUCCESS;
}

// ----  Initialisation  ----------------------------------------------
void R3BPspxMapped2Cal::SetParContainers()
{
    LOG(INFO) << "R3BPspxMapped2Cal :: SetParContainers() " << FairLogger::endl;

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
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus R3BPspxMapped2Cal::ReInit()
{

    LOG(INFO) << " R3BPspxMapped2Cal :: ReInit() " << FairLogger::endl;

    // FairRunAna* ana = FairRunAna::Instance();
    // FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    // fDigiPar = (FairTutorialDet2DigiPar*) (rtdb->getContainer("FairTutorialDet2DigiPar"));

    fCalPar = (R3BPspxCalPar*)FairRuntimeDb::instance()->getContainer("R3BPspxCalPar");

    if (!fCalPar)
    {
        LOG(ERROR) << "Could not get access to R3BPspxCalPar-Container." << FairLogger::endl;
        return kFATAL;
    }

    return kSUCCESS;
}

void R3BPspxMapped2Cal::Exec(Option_t* option)
{

    if (!fMappedItems)
    {
        printf("Cannot access PSPX mapped items\n");
        return;
    }

    Int_t detector1;
    Int_t detector2;
    Int_t strip1;
    Int_t strip2;
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
        detector1 = 0;
        detector2 = 0;
        strip1 = 0;
        strip2 = 0;
        energy1 = 0;
        energy2 = 0;

        R3BPspxMappedData* mItem1 = (R3BPspxMappedData*)fMappedItems->At(i);
        detector1 = mItem1->GetDetector();
        if (fCalPar->GetPspxParStrip().At(detector1 - 1) == 0)
            continue;

        // calculating stripnumber
        if (mItem1->GetChannel() > 0 && mItem1->GetChannel() < fCalPar->GetPspxParStrip().At(detector1 - 1) * 4 + 1)
        { // strips
            strip1 = (mItem1->GetChannel() + 1) / 2;
        }
        else if (mItem1->GetChannel() == fCalPar->GetPspxParStrip().At(detector1 - 1) * 4 + 1)
        { // cathode
            strip1 = fCalPar->GetPspxParStrip().At(detector1 - 1) * 2 + 1;
        }

        // setting energy(s) for strips
        if (strip1 > 0 && strip1 < fCalPar->GetPspxParStrip().At(detector1 - 1) * 2 + 2 && used[i] == false)
        {
            if (strip1 == fCalPar->GetPspxParStrip().At(detector1 - 1) * 2 + 1)
            { // cathode
                if (mItem1->GetEnergy() <
                    energythreshold[detector1 - 1][fCalPar->GetPspxParStrip().At(detector1 - 1) * 2])
                    continue;
                energy1 = mItem1->GetEnergy();
                energy2 = 0;
            }
            else
            { // strips
                if (fCalPar->GetPspxParOrientation().At(detector1 - 1) == 1 ||
                    fCalPar->GetPspxParOrientation().At(detector1 - 1) == 3)
                { // vertical strips or strips in both directions
                    if (mItem1->GetEnergy() < energythreshold[detector1 - 1][mItem1->GetChannel() - 1])
                        continue;
                }
                else if (fCalPar->GetPspxParOrientation().At(detector1 - 1) == 2)
                { // horizontal strips
                    if (mItem1->GetEnergy() <
                        energythreshold[detector1 - 1]
                                       [mItem1->GetChannel() - 1 - 2 * fCalPar->GetPspxParStrip().At(detector1 - 1)])
                        continue; // different counting of array and in root file + counting from #Strips+1 to #Strips*2
                                  // for horizontal strips
                }
                if (mItem1->GetChannel() % 2 == 1)
                {
                    energy1 = mItem1->GetEnergy();
                    energy2 = 0;
                }
                else
                {
                    energy1 = 0;
                    energy2 = mItem1->GetEnergy();
                }

                // searching for second entry of strip
                for (Int_t j = i + 1; j < nMapped; j++)
                {
                    R3BPspxMappedData* mItem2 = (R3BPspxMappedData*)fMappedItems->At(j);
                    detector2 = mItem2->GetDetector();
                    if (fCalPar->GetPspxParStrip().At(detector2 - 1) == 0)
                        continue;

                    // calculating stripnumber
                    if (mItem2->GetChannel() > 0 &&
                        mItem2->GetChannel() < fCalPar->GetPspxParStrip().At(detector2 - 1) * 4 + 1)
                    { // strips
                        strip2 = (mItem2->GetChannel() + 1) / 2;
                    }
                    else if (mItem2->GetChannel() == fCalPar->GetPspxParStrip().At(detector2 - 1) * 4 + 1)
                    { // cathode
                        strip2 = fCalPar->GetPspxParStrip().At(detector2 - 1) * 2 + 1;
                    }

                    if (detector1 == detector2 && strip1 == strip2)
                    {
                        used[j] = true; // to avoid that the same entry is read a second time in loop i
                        if (fCalPar->GetPspxParOrientation().At(detector2 - 1) == 1 ||
                            fCalPar->GetPspxParOrientation().At(detector1 - 1) == 3)
                        { // vertical strips or strips in both directions
                            if (mItem2->GetEnergy() < energythreshold[detector2 - 1][mItem2->GetChannel() - 1])
                                continue;
                        }
                        else if (fCalPar->GetPspxParOrientation().At(detector2 - 1) == 2)
                        { // horizontal strips
                            if (mItem2->GetEnergy() <
                                energythreshold[detector2 - 1][mItem2->GetChannel() - 1 -
                                                               2 * fCalPar->GetPspxParStrip().At(detector1 - 1)])
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
                        if (fCalPar->GetPspxParOrientation().At(detector1 - 1) == 1 ||
                            fCalPar->GetPspxParOrientation().At(detector1 - 1) == 3)
                        { // vertical strips or strips in both directions
                            energy2 = energy2 *
                                      gain[detector1 - 1][strip1 - 1]; // different counting of array and in root file
                        }
                        else if (fCalPar->GetPspxParOrientation().At(detector1 - 1) == 2)
                        { // horizontal strips
                            energy2 =
                                energy2 *
                                gain[detector1 - 1]
                                    [strip1 - 1 - fCalPar->GetPspxParStrip().At(detector1 - 1)]; // different counting
                                                                                                 // of array and in root
                                                                                                 // file + counting from
                                                                                                 // #Strips+1 to
                                                                                                 // #Strips*2 for
                                                                                                 // horizontal strips
                        }
                    }
                }
            }

            new ((*fCalItems)[fCalItems->GetEntriesFast()]) R3BPspxCalData(detector1, strip1, energy1, energy2);
        }
    }
}

void R3BPspxMapped2Cal::FinishEvent() { fCalItems->Clear(); }

void R3BPspxMapped2Cal::FinishTask() {}

// void R3BPspxMapped2Cal::WriteHistos() {}

ClassImp(R3BPspxMapped2Cal)
