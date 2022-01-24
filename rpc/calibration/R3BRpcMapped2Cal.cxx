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

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iomanip>

#include "R3BRpc.h"
#include "R3BRpcCalData.h"
#include "R3BRpcCalPar.h"
#include "R3BRpcMapped2Cal.h"
#include "R3BRpcMappedData.h"

// R3BRpcMapped2Cal: Constructor
R3BRpcMapped2Cal::R3BRpcMapped2Cal()
    : FairTask("R3B RPC Calibrator")
    , fNumChannels(0)
    , fCalParams(NULL)
    , fCal_Par(NULL)
    , fOnline(kFALSE)
    , fRpcMappedDataCA(NULL)
    , fRpcCalDataCA(NULL)
{
}

R3BRpcMapped2Cal::~R3BRpcMapped2Cal()
{
    LOG(INFO) << "R3BRpcMapped2Cal: Delete instance";

    /*
     * Note to whomever felt in neccessary to add: "delete fRpcMappedDataCA;"
     * This will cause a double free because  R3BRpcFebexReader (reasonably)
     * considers itself the owner of that object.
     * Short version: if your class did not allocate the object with new,
     * it also (typically) may not delete it. -- pklenze
     */
    if (fRpcCalDataCA)
        delete fRpcCalDataCA;
}

void R3BRpcMapped2Cal::SetParContainers()
{
    // Parameter Container
    // Reading RPCCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BRpcMapped2Cal:: FairRuntimeDb not opened";
    }

    fCal_Par = (R3BRpcCalPar*)rtdb->getContainer("RPCCalPar");
    if (!fCal_Par)
    {
        LOG(ERROR) << "R3BRpcMapped2Cal::Init() Couldn't get handle on RPCCalPar container";
    }
    else
    {
        LOG(INFO) << "R3BRpcMapped2Cal:: RPCCalPar container open";
    }
}

void R3BRpcMapped2Cal::SetParameter()
{
    // NB: the handling of calibration parameters should be redone from the scratch.
    // The par-file format  is not really human-readable (the root file even is worse).
    // Luckily, this does not matter because the choice to store the values in the
    // class using two one-dimensional array precludes any possibility of humans groking
    // it.
    // A sensible reimplementation might store the calibration in csv lines (with the crID
    // coming first) or go down the json rabbit hole.
    // For using the calibration here,  an std::vector of some struct {m, c, totAmp, totTime}
    // would be convenient. Nobody wants higher order polynomials for calibration.
    // Also, if you insist on crIDs starting from one (that ship has long sailed), then
    // please just leave the first array entry undefined and use params(id) == a[id], instead
    // of bothering with params(id)==a[id-1].
    // See also: https://github.com/R3BRootGroup/R3BRoot/issues/473
    // Just my 2 cents. -- pklenze

    //--- Parameter Container ---
    fNumChannels = fCal_Par->GetNumChannels(); // Number of Channels

    fCalParams = fCal_Par->GetCalParams(); // Array with the Cal parameters

    LOG(INFO) << "R3BRpcMapped2Cal:: Max Channel ID " << fNumChannels;
}

InitStatus R3BRpcMapped2Cal::Init()
{
    LOG(INFO) << "R3BRpcMapped2Cal::Init()";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(FATAL) << "R3BRpcMapped2Cal::FairRootManager not found";
        return kFATAL;
    }

    fRpcMappedDataCA = (TClonesArray*)rootManager->GetObject("RPCMappedData");
    if (!fRpcMappedDataCA)
    {
        LOG(FATAL) << "R3BRpcMapped2Cal::RpcMappedData not found";
        return kFATAL;
    }

    // OUTPUT DATA
    // Calibrated data
    fRpcCalDataCA = new TClonesArray("R3BRpcCalData", 50);

    rootManager->Register("RPCCalData", "RPC Cal", fRpcCalDataCA, !fOnline);

    SetParameter();
    return kSUCCESS;
}

InitStatus R3BRpcMapped2Cal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BRpcMapped2Cal::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    if (!fCal_Par)
    {
        LOG(WARNING) << "R3BRpcMapped2Cal::Parameter container not found";
    }

    // Reading the Input -- Mapped Data --
    Int_t nHits = fRpcMappedDataCA->GetEntries();
    if (!nHits)
        return;

    R3BRpcMappedData** mappedData;
    mappedData = new R3BRpcMappedData*[nHits];

    for (Int_t i = 0; i < nHits; i++)
    {
        mappedData[i] = (R3BRpcMappedData*)(fRpcMappedDataCA->At(i));
        Int_t channelId = mappedData[i]->GetChannelId();
        uint64_t time = mappedData[i]->GetTime();
        uint64_t wrts = mappedData[i]->GetWrts();
        Int_t side = mappedData[i]->GetSide();

        // DUMMY FILLING---> CALL HERE YOUR CALIB
        Double32_t energy = 0.;
        Double32_t TotCal = 0.;

        AddCalData(channelId, energy, wrts, TotCal);
    }

    if (mappedData)
        delete[] mappedData; // FTFY
    return;
}

void R3BRpcMapped2Cal::Finish() {}

void R3BRpcMapped2Cal::Reset()
{
    LOG(DEBUG) << "Clearing RPCCalData Structure";
    if (fRpcCalDataCA)
        fRpcCalDataCA->Clear();
}

R3BRpcCalData* R3BRpcMapped2Cal::AddCalData(Int_t id, Double_t energy, uint64_t wrts, Double_t tot_energy = 0)
{
    // It fills the R3BRpcCalData
    TClonesArray& clref = *fRpcCalDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BRpcCalData(id, energy, wrts, tot_energy);
}

ClassImp(R3BRpcMapped2Cal)
