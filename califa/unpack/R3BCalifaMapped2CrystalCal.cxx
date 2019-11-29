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

#include "R3BCalifa.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaCrystalCalPar.h"
#include "R3BCalifaMapped2CrystalCal.h"
#include "R3BCalifaMappedData.h"

// R3BCalifaMapped2CrystalCal: Constructor
R3BCalifaMapped2CrystalCal::R3BCalifaMapped2CrystalCal()
    : FairTask("R3B CALIFA Calibrator")
    , NumCrystals(0)
    , NumParams(0)
    , fCalParams(NULL)
    , fCal_Par(NULL)
    , fOnline(kFALSE)
    , fCalifaMappedDataCA(NULL)
    , fCalifaCryCalDataCA(NULL)
{
}

R3BCalifaMapped2CrystalCal::~R3BCalifaMapped2CrystalCal()
{
    LOG(INFO) << "R3BCalifaMapped2CrystalCal: Delete instance";
    if (fCalifaMappedDataCA)
        delete fCalifaMappedDataCA;
    if (fCalifaCryCalDataCA)
        delete fCalifaCryCalDataCA;
}

void R3BCalifaMapped2CrystalCal::SetParContainers()
{
    // Parameter Container
    // Reading califaCrystalCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BCalifaMapped2CrystalCal:: FairRuntimeDb not opened!";
    }

    fCal_Par = (R3BCalifaCrystalCalPar*)rtdb->getContainer("califaCrystalCalPar");
    if (!fCal_Par)
    {
        LOG(ERROR) << "R3BCalifaMapped2CrystalCal::Init() Couldn't get handle on califaCrystalCalPar container";
    }
    if (fCal_Par)
    {
        LOG(INFO) << "R3BCalifaMapped2CrystalCal:: califaCrystalCalPar container open";
    }
}

void R3BCalifaMapped2CrystalCal::SetParameter()
{
    //--- Parameter Container ---
    NumCrystals = fCal_Par->GetNumCrystals();    // Number of Crystals
    NumParams = fCal_Par->GetNumParametersFit(); // Number of Parameters

    fCalParams = new TArrayF();
    fCalParams = fCal_Par->GetCryCalParams(); // Array with the Cal parameters
    assert(fCalParams->GetSize() >= NumCrystals * NumParams);

    LOG(INFO) << "R3BCalifaMapped2CrystalCal:: Max Crystal ID " << NumCrystals;
    LOG(INFO) << "R3BCalifaMapped2CrystalCal:: Nb of parameters used in the fits " << NumParams;
}

InitStatus R3BCalifaMapped2CrystalCal::Init()
{
    LOG(INFO) << "R3BCalifaMapped2CrystalCal::Init()";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        return kFATAL;
    }

    fCalifaMappedDataCA = (TClonesArray*)rootManager->GetObject("CalifaMappedData");
    if (!fCalifaMappedDataCA)
    {
        return kFATAL;
    }

    // OUTPUT DATA
    // Calibrated data
    fCalifaCryCalDataCA = new TClonesArray("R3BCalifaCrystalCalData", 50);
    if (!fOnline)
    {
        rootManager->Register("CalifaCrystalCalData", "CALIFA Crystal Cal", fCalifaCryCalDataCA, kTRUE);
    }
    else
    {
        rootManager->Register("CalifaCrystalCalData", "CALIFA Crystal Cal", fCalifaCryCalDataCA, kFALSE);
    }

    SetParameter();
    return kSUCCESS;
}

InitStatus R3BCalifaMapped2CrystalCal::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BCalifaMapped2CrystalCal::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    if (!fCal_Par)
    {
        LOG(WARNING) << "R3BCalifaMapped2CrystalCal::NO Container Parameter!!";
    }

    // Reading the Input -- Mapped Data --
    Int_t nHits = fCalifaMappedDataCA->GetEntries();
    if (!nHits)
        return;

    R3BCalifaMappedData** mappedData;
    mappedData = new R3BCalifaMappedData*[nHits];

    // Overflow (R3BROOT-speech "Errors") handling:
    // If an error bit indicates that the data is invalid,
    // the correct approach is to set the invalid fields to NaN, imho

    //  source: mbs f_user dir, struct_event_115a.h
    // bit      name        invalidates

    //   0      CFD         nothing, trigger branch overflow
    //   1      Baseline    everything
    //   2      MAU         everything
    //   3      MWD         everything

    //   4      PeakSensing everything????
    //   5      E->EvntBuf  Energy
    //   6      Trace->EBuf nothing, traces are not handled by R3BROOT
    //   7      Nf->EvntBuf Nf

    //   8      Ns->EvntBuf Ns
    //   9      ADC         everything
    //   a      ADC underfl everything
    //   b      QPID Nf     Nf

    //   c      QPID Ns     Ns
    const uint16_t ANY_ERRORS = 0x061e;
    const uint16_t QPID_ERRORS = 0x1980 | ANY_ERRORS;
    const uint16_t EN_ERRORS = 0x0020 | ANY_ERRORS;

    for (Int_t i = 0; i < nHits; i++)
    {
        mappedData[i] = (R3BCalifaMappedData*)(fCalifaMappedDataCA->At(i));
        auto crystalId = mappedData[i]->GetCrystalId();
        auto Time = mappedData[i]->GetTime();
        auto Erro = mappedData[i]->GetError();
        auto Tot = mappedData[i]->GetTot();
        auto validate_smear = [](uint16_t err_cond, double raw) {
            return err_cond ? NAN : raw + gRandom->Rndm() - 0.5;
        };
        enum id
        {
            en = 0,
            Nf = 1,
            Ns = 2
        };
        double raw[3];
        raw[en] = validate_smear(Erro & EN_ERRORS, mappedData[i]->GetEnergy());
        raw[Nf] = validate_smear(Erro & QPID_ERRORS, mappedData[i]->GetNf());
        raw[Ns] = validate_smear(Erro & QPID_ERRORS, mappedData[i]->GetNs());
        double cal[3] = { 0, 0, 0 };

        if (0 < crystalId && crystalId <= NumCrystals)
            for (int idx = 0; idx < 3; idx++)
                for (int p = 0; p < NumParams; p++)
                {
                    cal[idx] +=
                        pow(raw[idx], (NumParams == 1) ? 1 : p) * fCalParams->GetAt(NumParams * (crystalId - 1) + p);
                }
        else
            for (int idx; idx < 3; idx++)
                cal[idx] = NAN;

        double TotCal = Tot; // TODO
        AddCalData(crystalId, cal[en], cal[Nf], cal[Ns], Time, TotCal);
    }

    if (mappedData)
        delete mappedData;
    return;
}

void R3BCalifaMapped2CrystalCal::Finish() {}

void R3BCalifaMapped2CrystalCal::Reset()
{
    LOG(DEBUG) << "Clearing CrystalCalData Structure";
    if (fCalifaCryCalDataCA)
        fCalifaCryCalDataCA->Clear();
}

R3BCalifaCrystalCalData* R3BCalifaMapped2CrystalCal::AddCalData(Int_t id,
                                                                Double_t energy,
                                                                Double_t Nf,
                                                                Double_t Ns,
                                                                ULong64_t time,
                                                                Double_t tot_energy = 0)
{
    // It fills the R3BCalifaCrystalCalData
    TClonesArray& clref = *fCalifaCryCalDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BCalifaCrystalCalData(id, energy, Nf, Ns, time, tot_energy);
}

ClassImp(R3BCalifaMapped2CrystalCal)
