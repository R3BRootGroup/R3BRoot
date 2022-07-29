/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ----------------------------------------------------------------
// -----          R3BTwimMapped2Cal source file               -----
// -----    Created 24/11/19 by J.L. Rodriguez-Sanchez        -----
// ----------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// TWIM headers
#include "R3BLogger.h"
#include "R3BTimeStitch.h"
#include "R3BTwimCalData.h"
#include "R3BTwimCalPar.h"
#include "R3BTwimMapped2Cal.h"
#include "R3BTwimMappedData.h"

// R3BTwimMapped2Cal::Default Constructor --------------------------
R3BTwimMapped2Cal::R3BTwimMapped2Cal()
    : R3BTwimMapped2Cal("R3BTwimMapped2Cal", 1)
{
}

// R3BTwimMapped2CalPar::Standard Constructor --------------------------
R3BTwimMapped2Cal::R3BTwimMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNumSec(4)
    , fNumAnodes(16)    // 16 anodes
    , fNumAnodesRef(2)  // 2 anode for TREF
    , fNumAnodesTrig(2) // 2 anode for TRIG
    , fMaxMult(20)
    , fMinDT(-1000)
    , fMaxDT(1000)
    , fNumEParams(2)
    , fNumPosParams(3)
    , fCal_Par(NULL)
    , fTwimMappedDataCA(NULL)
    , fTwimCalDataCA(NULL)
    , fExpId(467)
    , fOnline(kFALSE)
{
    CalEParams.resize(fNumSec);
    PosParams.resize(fNumSec);
    for (Int_t s = 0; s < fNumSec; s++)
    {
        CalEParams[s] = NULL;
        PosParams[s] = NULL;
    }
}

// Virtual R3BTwimMapped2Cal::Destructor
R3BTwimMapped2Cal::~R3BTwimMapped2Cal()
{
    R3BLOG(DEBUG1, "");
    if (fTwimCalDataCA)
        delete fTwimCalDataCA;
}

void R3BTwimMapped2Cal::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(FATAL, !rtdb, "FairRuntimeDb not found");

    fCal_Par = (R3BTwimCalPar*)rtdb->getContainer("twimCalPar");
    if (!fCal_Par)
    {
        R3BLOG(ERROR, "Couldn't get handle on twimCalPar container");
    }
    else
    {
        R3BLOG(INFO, "twimCalPar container opened");
    }
    return;
}

void R3BTwimMapped2Cal::SetParameter()
{
    //--- Parameter Container ---
    fNumSec = fCal_Par->GetNumSec();               // Number of sections
    fNumAnodesRef = fCal_Par->GetNumAnodesTRef();  // Anodes for TREF
    fNumAnodesTrig = fCal_Par->GetNumAnodesTrig(); // Anodes for Trig
    fMaxMult = fCal_Par->GetMaxMult();
    fMinDT = fCal_Par->GetMinDT();
    fMaxDT = fCal_Par->GetMaxDT();
    fNumAnodes = fCal_Par->GetNumAnodes();          // Number of anodes per section
    fNumEParams = fCal_Par->GetNumParamsEFit();     // Number of parameters for energy calibration
    fNumPosParams = fCal_Par->GetNumParamsPosFit(); // Number of parameters for position calibration

    R3BLOG(INFO, "Nb sections: " << fNumSec);
    R3BLOG(INFO, "Nb anodes: " << fNumAnodes);
    R3BLOG(INFO, "Nb of twim Ref anodes: " << fNumAnodesRef);
    R3BLOG(INFO, "Nb of twim Trigger anodes: " << fNumAnodesTrig);
    R3BLOG(INFO, "Nb of twim Max. multiplicity per anode: " << fMaxMult);
    R3BLOG(INFO, "Twim Min Drift Time accepted: " << fMinDT);
    R3BLOG(INFO, "Twim Max Drift Time accepted: " << fMaxDT);
    R3BLOG(INFO, "Nb parameters from energy cal fit: " << fNumEParams);
    R3BLOG(INFO, "Nb parameters from position cal fit: " << fNumPosParams);

    CalEParams.resize(fNumSec);
    PosParams.resize(fNumSec);
    for (Int_t s = 0; s < fNumSec; s++)
    {
        // Energy cal parameters
        CalEParams[s] = new TArrayF();
        Int_t array_e = fNumAnodes * fNumEParams;
        CalEParams[s]->Set(array_e);
        CalEParams[s] = fCal_Par->GetAnodeCalEParams(s + 1);
        // Position cal parameters
        PosParams[s] = new TArrayF();
        Int_t array_pos = fNumAnodes * fNumPosParams;
        PosParams[s]->Set(array_pos);
        PosParams[s] = fCal_Par->GetAnodeCalPosParams(s + 1);
    }

    // Count the number of dead anodes or not used
    for (Int_t s = 0; s < fNumSec; s++)
    {
        Int_t numdeadanodes = 0;
        for (Int_t i = 0; i < fNumAnodes; i++)
            if (CalEParams[s]->GetAt(fNumEParams * i + 1) == -1 || fCal_Par->GetInUse(s + 1, i + 1) == 0)
                numdeadanodes++;
        R3BLOG(INFO, "Dead (or not used) anodes in section " << s + 1 << ": " << numdeadanodes);
    }
    return;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BTwimMapped2Cal::Init()
{
    R3BLOG(INFO, "");

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        R3BLOG(FATAL, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fTwimMappedDataCA = (TClonesArray*)rootManager->GetObject("TwimMappedData");
    if (!fTwimMappedDataCA)
    {
        R3BLOG(FATAL, "TwimMappedData not found");
        return kFATAL;
    }

    // OUTPUT DATA
    fTwimCalDataCA = new TClonesArray("R3BTwimCalData");
    rootManager->Register("TwimCalData", "TWIM_Cal", fTwimCalDataCA, !fOnline);
    fTwimCalDataCA->Clear();

    // Definition of a time stich object to correlate VFTX times
    fTimeStitch = new R3BTimeStitch();

    SetParameter();
    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BTwimMapped2Cal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BTwimMapped2Cal::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Reading the Input -- Mapped Data --
    Int_t nHits = fTwimMappedDataCA->GetEntriesFast();
    if (nHits == 0)
        return;

    R3BTwimMappedData** mappedData = new R3BTwimMappedData*[nHits];
    Int_t secId = 0;
    Int_t anodeId = 0;
    Double_t pedestal = 0.;
    Double_t slope = 1.;

    for (Int_t s = 0; s < fNumSec; s++)
        for (Int_t i = 0; i < (fNumAnodes + fNumAnodesRef + fNumAnodesTrig); i++)
        {
            mulanode[s][i] = 0;
            for (Int_t j = 0; j < fMaxMult; j++)
            {
                fE[s][j][i] = 0.;
                fDT[s][j][i] = 0.;
            }
        }

    for (Int_t i = 0; i < nHits; i++)
    {
        mappedData[i] = (R3BTwimMappedData*)(fTwimMappedDataCA->At(i));
        secId = mappedData[i]->GetSecID() - 1;
        anodeId = mappedData[i]->GetAnodeID() - 1;

        if (anodeId < fNumAnodes && fCal_Par->GetInUse(secId + 1, anodeId + 1) == 1)
        {
            pedestal = CalEParams[secId]->GetAt(fNumEParams * anodeId);
            slope = CalEParams[secId]->GetAt(fNumEParams * anodeId + 1);

            fE[secId][mulanode[secId][anodeId]][anodeId] = pedestal + slope * mappedData[i]->GetEnergy();
            fDT[secId][mulanode[secId][anodeId]][anodeId] = mappedData[i]->GetTime();
            mulanode[secId][anodeId]++;
        }
        else if (anodeId >= fNumAnodes)
        {
            fDT[secId][mulanode[secId][anodeId]][anodeId] = mappedData[i]->GetTime(); // Ref. Time
            mulanode[secId][anodeId]++;
        }
    }

    // Fill data only if there is TREF signal
    for (Int_t s = 0; s < fNumSec; s++)
        if (mulanode[s][fNumAnodes] == 1)
        {
            for (Int_t i = 0; i < fNumAnodes; i++)
            {
                Int_t ii = fNumPosParams * i;
                Float_t a0 = PosParams[s]->GetAt(ii);
                Float_t a1 = PosParams[s]->GetAt(ii + 1);
                Float_t a2 = 0.0;
                if (fNumPosParams > 2)
                    a2 = PosParams[s]->GetAt(ii + 2);
                for (Int_t j = 0; j < mulanode[s][fNumAnodes]; j++)
                    for (Int_t k = 0; k < mulanode[s][i]; k++)
                    {
                        if (fExpId == 444 || fExpId == 467)
                        {
                            // s444 and s467: 2020
                            // anodes 1 to 16 : energy and time
                            // anode 17 and 18 : reference time --> will be changed to 17 only when the full Twin-MUSIC
                            // will be cabled anode 19 and 20 : trigger time   --> will be changed to 18 only when the
                            // full Twin-MUSIC will be cabled
                            if (i < 8) // Check if fNumAnodes+1 and +2 values
                            {
                                if (fE[s][k][i] > 0.)
                                    AddCalData(s + 1,
                                               i + 1,
                                               a0 + a1 * fTimeStitch->GetTime(
                                                             fDT[s][k][i] - fDT[s][j][fNumAnodes + 1], "vftx", "vftx"),
                                               fE[s][k][i]);
                            }
                            else
                            {
                                if (fE[s][k][i] > 0.)
                                    AddCalData(s + 1,
                                               i + 1,
                                               a0 + a1 * fTimeStitch->GetTime(
                                                             fDT[s][k][i] - fDT[s][j][fNumAnodes + 2], "vftx", "vftx"),
                                               fE[s][k][i]);
                            }
                        }
                        else if (fExpId == 455 && fE[s][k][i] > 0.)
                        {
                            auto dtime = a0 + a1 * (fDT[s][k][i] - fDT[s][j][fNumAnodes]) +
                                         a2 * pow((fDT[s][k][i] - fDT[s][j][fNumAnodes]), 2);
                            if (dtime > fMinDT && dtime < fMaxDT)
                            {
                                AddCalData(s + 1, i + 1, dtime, fE[s][k][i]);
                            }
                        }
                    }
            }
        }

    if (mappedData)
        delete[] mappedData;
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BTwimMapped2Cal::Reset()
{
    R3BLOG(DEBUG1, "Clearing TwimCalData Structure");
    if (fTwimCalDataCA)
        fTwimCalDataCA->Clear();
}

// -----   Private method AddCalData  --------------------------------------------
R3BTwimCalData* R3BTwimMapped2Cal::AddCalData(Int_t secID, Int_t anodeID, Double_t dtime, Double_t energy)
{
    // It fills the R3BTwimCalData
    TClonesArray& clref = *fTwimCalDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BTwimCalData(secID, anodeID, dtime, energy);
}

ClassImp(R3BTwimMapped2Cal);
