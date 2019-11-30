// -------------------------------------------------------------
// -----         R3BMusicCal2Hit source file               -----
// -----    Created 30/11/19  by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iomanip>

// Music headers
#include "R3BMusicCal2Hit.h"
#include "R3BMusicCalData.h"
#include "R3BMusicHitData.h"
#include "R3BMusicHitPar.h"

// R3BMusicCal2Hit: Default Constructor --------------------------
R3BMusicCal2Hit::R3BMusicCal2Hit()
    : FairTask("R3B Music Hit Calibrator", 1)
    , fNumAnodes(8)
    , fNumParams(2)
    , CalParams(NULL)
    , fCal_Par(NULL)
    , fMusicHitDataCA(NULL)
    , fMusicCalDataCA(NULL)
    , fOnline(kFALSE)
{
}

// R3BMusicCal2HitPar: Standard Constructor --------------------------
R3BMusicCal2Hit::R3BMusicCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNumAnodes(8)
    , fNumParams(2)
    , CalParams(NULL)
    , fCal_Par(NULL)
    , fMusicHitDataCA(NULL)
    , fMusicCalDataCA(NULL)
    , fOnline(kFALSE)
{
}

// Virtual R3BMusicCal2Hit: Destructor
R3BMusicCal2Hit::~R3BMusicCal2Hit()
{
    LOG(INFO) << "R3BMusicCal2Hit: Delete instance";
    if (fMusicHitDataCA)
        delete fMusicHitDataCA;
    if (fMusicCalDataCA)
        delete fMusicCalDataCA;
}

void R3BMusicCal2Hit::SetParContainers()
{

    // Parameter Container
    // Reading MusicCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "FairRuntimeDb not opened!";
    }

    fCal_Par = (R3BMusicHitPar*)rtdb->getContainer("musicHitPar");
    if (!fCal_Par)
    {
        LOG(ERROR) << "R3BMusicCal2HitPar::Init() Couldn't get handle on musicHitPar container";
    }
    else
    {
        LOG(INFO) << "R3BMusicCal2HitPar:: musicHitPar container open";
    }
}

void R3BMusicCal2Hit::SetParameter()
{

    //--- Parameter Container ---
    fNumParams = fCal_Par->GetNumParametersFit(); // Number of Parameters

    LOG(INFO) << "R3BMusicCal2Hit: Nb parameters from pedestal fit: " << fNumParams;

    CalParams = new TArrayF();
    Int_t array_size = fNumParams;
    CalParams->Set(array_size);
    CalParams = fCal_Par->GetDetectorHitParams(); // Array with the Cal parameters

    // Parameters detector
    LOG(INFO) << "R3BMusicCal2Hit Parameters:" << CalParams->GetAt(0) << " : " << CalParams->GetAt(1);
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BMusicCal2Hit::Init()
{
    LOG(INFO) << "R3BMusicCal2Hit: Init";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        return kFATAL;
    }

    fMusicCalDataCA = (TClonesArray*)rootManager->GetObject("MusicCalData");
    if (!fMusicCalDataCA)
    {
        return kFATAL;
    }

    // OUTPUT DATA
    fMusicHitDataCA = new TClonesArray("R3BMusicHitData", 10);
    if (!fOnline)
    {
        rootManager->Register("MusicHitData", "Music Hit", fMusicHitDataCA, kTRUE);
    }
    else
    {
        rootManager->Register("MusicHitData", "Music Hit", fMusicHitDataCA, kFALSE);
    }

    SetParameter();
    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BMusicCal2Hit::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BMusicCal2Hit::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    if (!fCal_Par)
    {
        LOG(ERROR) << "NO Container Parameter!!";
    }

    Int_t nHits = fMusicCalDataCA->GetEntries();
    if (!nHits)
        return;

    // R3BMusicCalData* CalDat;
    R3BMusicCalData** CalDat = new R3BMusicCalData*[nHits];

    Int_t secId, anodeId;
    Double_t energyperanode[fNumAnodes];
    Int_t nbdet = 0;

    for (Int_t j = 0; j < fNumAnodes; j++)
        energyperanode[j] = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        CalDat[i] = (R3BMusicCalData*)(fMusicCalDataCA->At(i));
        anodeId = CalDat[i]->GetAnodeID();
        energyperanode[anodeId] = CalDat[i]->GetEnergy();
    }

    Double_t nba = 0, a0 = 0., a1 = 0., theta = 0., charge = 0.;

    // calculate truncated dE from 8 anodes, MUSIC
    for (Int_t j = 0; j < fNumAnodes; j++)
    {
        if (energyperanode[j] > 0.)
        {
            charge = charge + energyperanode[j];
            nba++;
        }
    }

    if (nba > 0 && charge / nba > 0.)
    {
        a0 = CalParams->GetAt(0);
        a1 = CalParams->GetAt(1);
        if ((a0 + a1 * charge / nba) > 0)
            AddHitData(theta, a0 + a1 * charge / nba);
    }

    if (CalDat)
        delete CalDat;
    return;
}

// -----   Protected method Finish   --------------------------------------------
void R3BMusicCal2Hit::Finish() {}

// -----   Public method Reset   ------------------------------------------------
void R3BMusicCal2Hit::Reset()
{
    LOG(DEBUG) << "Clearing R3BMusicHitData Structure";
    if (fMusicHitDataCA)
        fMusicHitDataCA->Clear();
}

// -----   Private method AddHitData  --------------------------------------------
R3BMusicHitData* R3BMusicCal2Hit::AddHitData(Double_t theta, Double_t charge_z)
{
    // It fills the R3BMusicHitData
    TClonesArray& clref = *fMusicHitDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BMusicHitData(theta, charge_z);
}
