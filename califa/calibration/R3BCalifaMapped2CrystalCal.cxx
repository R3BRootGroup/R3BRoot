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
#include "TMath.h"
#include "TRandom.h"
#include <iomanip>

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaCrystalCalPar.h"
#include "R3BCalifaMapped2CrystalCal.h"
#include "R3BCalifaMappedData.h"
#include "R3BCalifaTotCalPar.h"
#include "R3BLogger.h"

// R3BCalifaMapped2CrystalCal::Constructor
R3BCalifaMapped2CrystalCal::R3BCalifaMapped2CrystalCal()
    : FairTask("R3BCalifaMapped2CrystalCal")
{
}


void R3BCalifaMapped2CrystalCal::SetParContainers()
{
    // Parameter Container
    // Reading califaCrystalCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fCal_Par = dynamic_cast<R3BCalifaCrystalCalPar*>(rtdb->getContainer("califaCrystalCalPar"));
    if (!fCal_Par)
    {
        R3BLOG(error, "Couldn't get handle on califaCrystalCalPar container");
    }
    else
    {
        R3BLOG(info, "califaCrystalCalPar container opened");
    }

    fTotCal_Par = dynamic_cast<R3BCalifaTotCalPar*>(rtdb->getContainer("CalifaTotCalPar"));
    if (!fTotCal_Par)
    {
        R3BLOG(warn, "Couldn't get handle on CalifaTotCalPar container");
    }
    else
    {
        R3BLOG(info, "CalifaTotCalPar container opened");
    }
}

void R3BCalifaMapped2CrystalCal::SetParameter()
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
    fNumCrystals = fCal_Par->GetNumCrystals();    // Number of Crystals
    fNumParams = fCal_Par->GetNumParametersFit(); // Number of Parameters

    fCalParams = fCal_Par->GetCryCalParams(); // Array with the Cal parameters
    assert(fCalParams->GetSize() >= fNumCrystals * fNumParams);

    R3BLOG(info, "Max Crystal ID " << fNumCrystals);
    R3BLOG(info, "Nb of parameters used in the fits " << fNumParams);

    //--- Parameter Container --- Tot
    fNumTotParams = fTotCal_Par->GetNumParametersFit(); // Number of Parameters

    fCalTotParams = fTotCal_Par->GetCryCalParams(); // Array with the Tot Cal parameters
    assert(fCalTotParams->GetSize() >= fNumCrystals * fNumTotParams);

    // handle old calibrations which mapped to barrel protons to crId+2432:
    // If you cal[id] is zero or nan (you wish),
    // And cal[id+2432] is nonzero,
    // Then make cal[id]=cal[id+2432] in case you are using the new unpacker
    // (where barrel is always in [1, 1952]) with an old calibration

    constexpr int offset = 2432;
    auto& cal = *fCalParams; // because (*ptr)[i] is ugly and error-prone
    auto& tot = *fCalTotParams;
    assert(fNumParams==2 && "Only pol1 is supported for calibration");
    if (fNumCrystals < 2 * offset)
    {
        R3BLOG(warn, "Not checking calibration in former proton range.");
        return;
    }
    auto invalid = [&cal](int id) {
        auto a = cal.GetAt(2 * (id - 1) + 0);
        auto b = cal.GetAt(2 * (id - 1) + 1);
        return (std::isnan(a) || a == 0.0) && (std::isnan(a) || a == 0.0);
    };

    int replaced{};
    for (int id = 1; id <= 1952; id++) // barrel range, formerly barrel gamma range
        if (invalid(id) && !invalid(id + offset))
        {
            ++replaced;
            // Note:  (*a)[n]=...
            for (int p = 0; p < fNumParams; p++)
                cal[fNumParams * (id - 1) + p] = cal[fNumParams * (id - 1 + offset) + p];
            for (int p = 0; p < fNumTotParams; p++)
                tot[fNumTotParams * (id - 1) + p] = tot[fNumTotParams * (id - 1 + offset) + p];
        }

    R3BLOG_IF(warn,
              replaced,
              replaced << " missing calibrations for crIDs in [1, 1952] have been copied over from the legacy proton "
                          "barrel range.");
}

InitStatus R3BCalifaMapped2CrystalCal::Init()
{
    R3BLOG(info, "");

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    assert(rootManager);

    fCalifaMappedData = rootManager->InitObjectAs<decltype(fCalifaMappedData)>(R3BCalifaMappedData::default_container_name);
    assert(fCalifaMappedData);
    
    // OUTPUT DATA
    // Calibrated data

    rootManager->RegisterAny(R3BCalifaCrystalCalData::default_container_name, fCalifaCryCalData, !fOnline);

    SetParameter();
    return kSUCCESS;
}

InitStatus R3BCalifaMapped2CrystalCal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BCalifaMapped2CrystalCal::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Overflow (R3BROOT-speech "Errors") handling:
    // If an error bit indicates that the data is invalid,
    // the correct approach is to set the invalid fields to NaN, imho

    // source: mbs f_user dir, struct_event_115a.h
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

    const uint32_t ANY_errorS = 0x061e;
    const uint32_t QPID_errorS = 0x1980 | ANY_errorS;
    const uint32_t EN_errorS = 0x0020 | ANY_errorS;

    auto validate_smear = [](uint16_t err_cond, double raw)
			  {
			    return err_cond ? NAN : raw + gRandom->Rndm() - 0.5;
			  };

    
    for (auto it: *fCalifaMappedData)
    {
        int crystalId=it.first;
	if ( crystalId<1  ||  crystalId > fNumCrystals)
	  continue;
	
	auto& mapped=it.second;
	auto ov=mapped.fOverFlow;
	
	double offset=fCalParams->GetAt(fNumParams * (crystalId - 1) + 1);
	double slope =fCalParams->GetAt(fNumParams * (crystalId - 1) + 1);
	
	auto res=fCalifaCryCalData->emplace(std::make_pair(crystalId, R3BCalifaCrystalCalData(crystalId)));
	assert(res.second && "duplicate crystal number");
	auto& h=(*res.first).second;
	
	h.fEnergy = slope * validate_smear(ov & EN_errorS, mapped.fEnergy) + offset;
	h.fNf = slope * validate_smear(ov & QPID_errorS, mapped.fNf );
	h.fNs = slope * validate_smear(ov & QPID_errorS, mapped.fNs );

	h.fWrts=mapped.fWrts;

        double TotCal {NAN}; // if we don't have a calibration, do not make some up.
        if (fCalTotParams)
        {
            double a0 = fCalTotParams->GetAt(fNumTotParams * (crystalId - 1));
            double a1 = fCalTotParams->GetAt(fNumTotParams * (crystalId - 1) + 1);
            TotCal = a0 * TMath::Exp(mapped.fTot / a1);
        }
	h.fToTEnergy=TotCal;
    }
}

void R3BCalifaMapped2CrystalCal::Reset()
{
    R3BLOG(debug, "Clearing CrystalCalData Structure");
    fCalifaCryCalData->clear();
}


ClassImp(R3BCalifaMapped2CrystalCal);
