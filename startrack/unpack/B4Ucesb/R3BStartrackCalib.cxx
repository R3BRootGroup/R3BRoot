/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           from R3BCaloCalib                       -----
// -----                 Created 18/07/2014 by H. Alvarez Pol              -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iomanip>

// Califa headers
#include "R3BStartrackCalib.h"
#include "R3BStartrackerDigitHit.h"

// R3BStartrackCalib: Constructor
R3BStartrackCalib::R3BStartrackCalib()
    : FairTask("R3B Startracker Calibrator")
    , fRawHitCA(0)
    , fSiDetHitCA(new TClonesArray("R3BStartrackHit"))
    , fStartrackCalibPar(0)
{
}

// Virtual R3BStartrackCalib: Public method
R3BStartrackCalib::~R3BStartrackCalib()
{
    LOG(info) << "R3BStartrackCalib: Delete instance";
    delete fRawHitCA;
    delete fSiDetHitCA;
}

// Init: Public method
InitStatus R3BStartrackCalib::Init()
{
    Register();
    return kSUCCESS;
}

void R3BStartrackCalib::SetParContainers()
{
    // Get run and runtime database
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
        LOG(fatal) << "R3BStartrackCalib::SetParContainers: No analysis run";

    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    if (!rtdb)
        LOG(fatal) << "R3BStarTraCalib::SetParContainers: No runtime database";

    fStartrackCalibPar = (R3BStartrackCalibPar*)(rtdb->getContainer("R3BStartrackCalibPar"));

    if (fVerbose && fStartrackCalibPar)
    {
        LOG(info) << "R3BStarTraCalib::SetParContainers() ";
        LOG(info) << "Container R3BStarTraCalibPar loaded ";
    }
}

// Register: Protected method
void R3BStartrackCalib::Register()
{
    LOG(debug) << "Registering";
    FairRootManager* fMan = FairRootManager::Instance();
    if (!fMan)
    {
        LOG(fatal) << "Init: No FairRootManager";
        return;
    }
    fRawHitCA = (TClonesArray*)fMan->GetObject("StartrackRawHit");

    fMan->Register("SiDetHit", "Calibrated data from Startracker", fSiDetHitCA, kTRUE);
}

// -----   Public method ReInit   --------------------------------------------
InitStatus R3BStartrackCalib::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

// DoCalib: Public method
void R3BStartrackCalib::Exec(Option_t* opt)
{

    LOG(debug) << "Calibring Startracker Raw Data";

    R3BStartrackRawHit** rawHit;
    Int_t module_id = 0;
    Int_t side = 0;
    Int_t asic_id = 0;
    Int_t strip_id = 0;
    Double32_t energy = 0;
    Double32_t time = 0;

    Int_t rawHits; // Nb of RawHits in current event
    rawHits = fRawHitCA->GetEntries();
    if (rawHits > 0)
    {
        rawHit = new R3BStartrackRawHit*[rawHits];
        for (Int_t i = 0; i < rawHits; i++)
        {
            rawHit[i] = new R3BStartrackRawHit;
            rawHit[i] = (R3BStartrackRawHit*)fSiDetHitCA->At(i);

            module_id = MapModuleID(rawHit[i]);
            side = MapSide(rawHit[i]);
            asic_id = MapAsicID(rawHit[i]);
            strip_id = MapStripID(rawHit[i]);
            energy = CalibrateEnergy(rawHit[i]);
            time = CalibrateTime(rawHit[i]);

            new ((*fSiDetHitCA)[i]) R3BStartrackerDigitHit(module_id, asic_id, side, strip_id, energy, time);
        }
    }

    return;
}

void R3BStartrackCalib::Finish() {}

// Reset: Public method
void R3BStartrackCalib::Reset()
{
    LOG(debug) << "Clearing Si tracker Hit Data Structure";
    if (fSiDetHitCA)
        fSiDetHitCA->Clear();
}

Int_t R3BStartrackCalib::MapModuleID(R3BStartrackRawHit* chit)
{
    LOG(debug) << "Mapping Module ID in R3BStarTraCalib";

    // Implement here the mapping from the unpacker to physical strip numbers
    return chit->GetModuleId();
}

Int_t R3BStartrackCalib::MapSide(R3BStartrackRawHit* chit)
{
    LOG(debug) << "Mapping Side ID in R3BStarTraCalib";

    // Implement here the mapping from the unpacker to physical strip numbers
    return chit->GetSide();
}

Int_t R3BStartrackCalib::MapAsicID(R3BStartrackRawHit* chit)
{
    LOG(debug) << "Mapping Asic ID in R3BStarTraCalib";

    // Implement here the mapping from the unpacker to physical strip numbers
    return chit->GetAsicId();
}

Int_t R3BStartrackCalib::MapStripID(R3BStartrackRawHit* chit)
{
    LOG(debug) << "Mapping Strip ID in R3BStarTraCalib";

    // Implement here the mapping from the unpacker to physical strip numbers
    return chit->GetStripId();
}

Double32_t R3BStartrackCalib::CalibrateEnergy(R3BStartrackRawHit* chit)
{
    LOG(debug) << "Calibrating Energies in R3BStarTraCalib";

    // Implement here the energy calibration based on the parameters
    return chit->GetADCdata();
}

Double32_t R3BStartrackCalib::CalibrateTime(R3BStartrackRawHit* chit)
{
    LOG(debug) << "Calibrating Time in R3BStarTraCalib";

    // Implement here the time calibration based on the parameters
    return chit->GetTimelb();
}

ClassImp(R3BStartrackCalib)
