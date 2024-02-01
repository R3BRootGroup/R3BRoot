/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "R3BLogger.h"

// Musli headers
#include "R3BEventHeader.h"
#include "R3BMusliCalData.h"
#include "R3BMusliCalPar.h"
#include "R3BMusliMapped2Cal.h"
#include "R3BMusliMappedData.h"

#include <iomanip>

// R3BMusliMapped2Cal: Default Constructor --------------------------
R3BMusliMapped2Cal::R3BMusliMapped2Cal()
    : R3BMusliMapped2Cal("R3BMusli Calibrator", 1)
{
}

// R3BMusliMapped2CalPar: Standard Constructor --------------------------
R3BMusliMapped2Cal::R3BMusliMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNumSignals(18)
    , fNumGroupsAnodes(15)
    , fNumParamsEneFit(2)
    , fNumParamsPosFit(2)
    , fNumParamsMultHit(2)
    , fMaxMult(20)
    , fEneCalParams(NULL)
    , fPosCalParams(NULL)
    , fMultHitCalParams(NULL)
    , fCal_Par(NULL)
    , fMusliMappedDataCA(NULL)
    , fMusliCalDataCA(NULL)
    , fOnline(kFALSE)
    , fHeader(NULL)
    , winL(0.)
    , winR(0.)
    , fUseMultHit(kFALSE)
{
}

// Virtual R3BMusliMapped2Cal: Destructor
R3BMusliMapped2Cal::~R3BMusliMapped2Cal()
{
    R3BLOG(debug1, "R3BMusliMapped2Cal: Delete instance");
    if (fMusliMappedDataCA)
        delete fMusliMappedDataCA;
    if (fMusliCalDataCA)
        delete fMusliCalDataCA;
}

void R3BMusliMapped2Cal::SetParContainers()
{
    // Parameter Container
    // Reading musicCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fCal_Par = dynamic_cast<R3BMusliCalPar*>(rtdb->getContainer("musliCalPar"));
    if (!fCal_Par)
    {
        R3BLOG(error, "R3BMusliMapped2Cal::SetParContainers() Couldn't get handle on musliCalPar container");
    }
    else
    {
        R3BLOG(info, "R3BMusliMapped2Cal::SetParContainers() musliCalPar container open");
    }
}

void R3BMusliMapped2Cal::SetParameters()
{
    //--- Parameter Container ---
    fNumSignals = fCal_Par->GetNumSignals();           // Number of signals at Mapped
    fNumGroupsAnodes = fCal_Par->GetNumGroupsAnodes(); // Number of groups of anodes
    fNumParamsEneFit = fCal_Par->GetNumParamsEneFit(); // Number of ene calib params per group of anodes
    fNumParamsPosFit = fCal_Par->GetNumParamsPosFit(); // Number of position calib params per group of anodes
    fMaxMult = fCal_Par->GetMaxMult();                 // Maximum number of data per output channel

    Int_t array_ene = fNumGroupsAnodes * fNumParamsEneFit;
    fEneCalParams = new TArrayF(); // energy calib params array for all groups of anodes
    fEneCalParams->Set(array_ene);
    fEneCalParams = fCal_Par->GetEneCalParams();

    Int_t array_pos = fNumGroupsAnodes * fNumParamsPosFit;
    fPosCalParams = new TArrayF(); // position calib params array for all groups of anodes
    fPosCalParams->Set(array_pos);
    fPosCalParams = fCal_Par->GetPosCalParams();

    Int_t array_mult = fNumGroupsAnodes * fNumParamsMultHit;
    fMultHitCalParams = new TArrayF(); // raw drift time limits array for all groups of anodes to select single hit
    fMultHitCalParams->Set(array_mult);
    fMultHitCalParams = fCal_Par->GetMultHitCalParams();
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BMusliMapped2Cal::Init()
{
    R3BLOG(info, "R3BMusliMapped2Cal::Init()");

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        R3BLOG(fatal, "R3BMusliMapped2Cal::Init() FairRootManager not found.");
        return kFATAL;
    }

    fHeader = dynamic_cast<R3BEventHeader*>(rootManager->GetObject("EventHeader."));

    fMusliMappedDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("MusliMappedData"));
    if (!fMusliMappedDataCA)
    {
        R3BLOG(fatal, "R3BMusliMapped2Cal::Init() MusliMappedData not found.");
        return kFATAL;
    }

    // OUTPUT DATA
    fMusliCalDataCA = new TClonesArray("R3BMusliCalData");
    rootManager->Register("MusliCalData", "MUSLI Cal", fMusliCalDataCA, !fOnline);
    fMusliCalDataCA->Clear();

    SetParameters();
    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BMusliMapped2Cal::ReInit()
{
    SetParContainers();
    SetParameters();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BMusliMapped2Cal::Exec(Option_t* option)
{
    // --- Reset entries in output arrays, local arrays
    Reset();

    // --- Variables needed to read mapped data
    UInt_t sig;
    UInt_t map_mult[fNumSignals];
    UInt_t map_sig[fNumSignals][fMaxMult];
    Double_t map_eraw[fNumSignals][fMaxMult];
    Double_t map_traw[fNumSignals][fMaxMult];
    for (Int_t i = 0; i < fNumSignals; i++)
    {
        map_mult[i] = 0;
        for (Int_t j = 0; j < fMaxMult; j++)
        {
            map_sig[i][j] = 0;
            map_eraw[i][j] = 0.;
            map_traw[i][j] = 0.;
        }
    }

    // --- Reading the musli mapped data input
    UInt_t nHits = fMusliMappedDataCA->GetEntriesFast();
    if (nHits == 0)
        return;
    for (UInt_t i = 0; i < nHits; i++)
    {

        R3BMusliMappedData* hit = (R3BMusliMappedData*)fMusliMappedDataCA->At(i);
        if (!hit)
            continue;

        sig = hit->GetSignal() - 1; // sig is 0-based: 0..7, 8..11, 12, 13, 14, 16, 17
        if (0 <= sig && sig < fNumSignals)
        {
            if (map_mult[sig] < fMaxMult)
            {
                map_sig[sig][map_mult[sig]] = hit->GetSignal();
                map_eraw[sig][map_mult[sig]] = hit->GetEnergy();
                map_traw[sig][map_mult[sig]] = hit->GetTime();
            }
            else
            {
                LOG(info) << " R3BMusliMapped2Cal::Exec() : WARNING Signal " << sig + 1
                          << " with mult = " << map_mult[sig] + 1;
                LOG(info) << "                                      signal discard in case of multi-hit analysis ";
            }
            map_mult[sig]++;
        }
        else
        {
            LOG(info) << "WARNING in R3BMusliMapped2Cal::Exec() ; sig = " << sig;
        }
    }

    // --- Variables needed to calibrate map data and fill cal data
    Double_t enepar[fNumParamsEneFit];
    Double_t pospar[fNumParamsPosFit];
    Double_t ecal;  // ecal = calibrated energy per group of anodes
    Double_t dtraw; // dtraw = raw drift time in [100ps]
    Double_t dtcal; // dtcal = calibrated drift time

    // --- Loop over the number of output groups of anodes
    if (fNumGroupsAnodes <= fNumSignals)
    {
        for (Int_t i = 0; i < fNumGroupsAnodes; i++) // i = [0..14]
        {
            if (map_mult[i] >= fMaxMult)
            {
                continue;
            }
            else if (map_mult[i] == 1)
            {
                // if Tref, dtraw calculation and calibration
                dtcal = 0;
                if (map_mult[16] == 1)
                {
                    dtraw = map_traw[i][0] - map_traw[16][0];
                    for (Int_t k = 0; k < fNumParamsPosFit; k++)
                    {
                        pospar[k] = fPosCalParams->GetAt(fNumParamsPosFit * i + k);
                        dtcal += pospar[k] * pow(dtraw, k);
                    }
                }
                // eraw calibration
                ecal = 0.;
                for (Int_t k = 0; k < fNumParamsEneFit; k++)
                {
                    enepar[k] = fEneCalParams->GetAt(fNumParamsEneFit * i + k);
                    ecal += enepar[k] * pow(map_eraw[i][0], k);
                }
                if (ecal > 0)
                    AddCalData(map_sig[i][0], dtcal, ecal);
            }
            // but in case of several hits ...
            else if (map_mult[i] > 1 && map_mult[i] < fMaxMult && fUseMultHit)
            {
                Double_t good_tref = -1;
                UInt_t n_tref = 0;
                if (map_mult[16] == 1)
                {
                    good_tref = map_traw[16][0];
                    n_tref = 1;
                }
                // else if (map_mult[16] > 1 && map_mult[16] < fMaxMult)
                //{
                //     n_tref = 0;
                //     // loop over the number of Tref
                //     for (Int_t j = 0; j < map_mult[16]; j++)
                //     {
                //         // TO DO : NEED AN EXPLANATION OF THIS cfd_t CRITERIA
                //         if (fHeader)
                //         {
                //             Double_t cfd_t = map_traw[16][j] * 25. / 256. - fHeader->GetTStart();
                //             if (cfd_t > winL && cfd_t < winR)
                //             {
                //                 n_tref++;
                //                 good_tref = map_traw[16][j];
                //             }
                //         }
                //     }
                // }
                //   good tref is found
                if (n_tref == 1 && good_tref > 0)
                {
                    UInt_t n_goodhit = 0;
                    Int_t good_hitindex = -1;
                    // loop over the output groups of anodes
                    for (Int_t j = 0; j < map_mult[i]; j++)
                    {
                        // dtraw = map_traw[i][j] - good_tref;
                        // if (dtraw > fMultHitCalParams->GetAt(fNumParamsMultHit * i + 0) &&
                        //     dtraw < fMultHitCalParams->GetAt(fNumParamsMultHit * i + 1))
                        //{
                        //     n_goodhit++;
                        //     good_hitindex = j;
                        // }
                    }
                    if (n_goodhit == 1 && good_hitindex >= 0)
                    {
                        // raw drift time calibration
                        dtraw = map_traw[i][good_hitindex] - good_tref;
                        dtcal = 0.;
                        for (Int_t k = 0; k < fNumParamsPosFit; k++)
                        {
                            pospar[k] = fPosCalParams->GetAt(fNumParamsPosFit * i + k);
                            dtcal += pospar[k] * pow(dtraw, k);
                        }
                        // raw energy calibration
                        ecal = 0.;
                        for (Int_t k = 0; k < fNumParamsEneFit; k++)
                        {
                            enepar[k] = fEneCalParams->GetAt(fNumParamsEneFit * i + k);
                            ecal += enepar[k] * pow(map_eraw[i][good_hitindex], k);
                        }
                        if (ecal > 0)
                            AddCalData(map_sig[i][good_hitindex], dtcal, ecal);
                    }
                }
            } // end of if multi hits
        }
    } // end of for (i) loop over the array of groups of anodes
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BMusliMapped2Cal::Reset()
{
    R3BLOG(debug1, "Clearing MusliCalData Structure");
    if (fMusliCalDataCA)
        fMusliCalDataCA->Clear();
}

// -----   Private method AddCalData  --------------------------------------------
R3BMusliCalData* R3BMusliMapped2Cal::AddCalData(UInt_t sig, Double_t dt, Double_t e)
{
    // It fills the R3BMusliCalData
    TClonesArray& clref = *fMusliCalDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BMusliCalData(sig, dt, e);
}

ClassImp(R3BMusliMapped2Cal);
