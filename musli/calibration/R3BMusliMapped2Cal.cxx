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
#include "R3BMusliCalData.h"
#include "R3BMusliCalPar.h"
#include "R3BMusliMapped2Cal.h"
#include "R3BMusliMappedData.h"
#include "R3BEventHeader.h"

#include <iomanip>

// R3BMusliMapped2Cal: Default Constructor --------------------------
R3BMusliMapped2Cal::R3BMusliMapped2Cal()
    : R3BMusliMapped2Cal("R3BMusli Calibrator", 1)
{
}

// R3BMusliMapped2CalPar: Standard Constructor --------------------------
R3BMusliMapped2Cal::R3BMusliMapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNumSignals(MAX_NB_SIGNALS_MAP)
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
    fNumParamsEneFit = fCal_Par->GetNumParamsEneFit(); // Number of signals at Mapped
    fNumParamsPosFit = fCal_Par->GetNumParamsPosFit(); // Number of signals at Mapped
    fMaxMult = fCal_Par->GetMaxMult();

    Int_t array_ene = fNumGroupsAnodes * fNumParamsEneFit;
    fEneCalParams = new TArrayF();
    fEneCalParams->Set(array_ene);
    fEneCalParams = fCal_Par->GetEneCalParams();

    Int_t array_pos = fNumGroupsAnodes * fNumParamsPosFit;
    fPosCalParams = new TArrayF();
    fPosCalParams->Set(array_pos);
    fPosCalParams = fCal_Par->GetPosCalParams();
    
    Int_t array_mult = fNumGroupsAnodes * fNumParamsMultHit;
    fMultHitCalParams = new TArrayF();
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
    // Reset entries in output arrays, local arrays
    Reset();

    // Reading the Input -- Mapped Data --
    Int_t nHits = fMusliMappedDataCA->GetEntriesFast();
    if (nHits == 0)
        return;

    R3BMusliMappedData** mappedData = new R3BMusliMappedData*[nHits];
    UInt_t signalId = 0;
    Double_t pedestal = 0.;
    Double_t slope = 1.;
    Double_t pospar[fNumParamsPosFit];
    Double_t dt, e;

    for (Int_t i = 0; i < fNumSignals; i++)
    {
        mult_signalmap[i] = 0;
        for (Int_t j = 0; j < fMaxMult; j++)
        {
            signal[j][i] = 0;
            energy[j][i] = 0.;
            time[j][i] = 0.;
        }
    }

    for (Int_t i = 0; i < nHits; i++)
    {
        mappedData[i] = dynamic_cast<R3BMusliMappedData*>(fMusliMappedDataCA->At(i));
        signalId = mappedData[i]->GetSignal() - 1;

        if (0 <= signalId && signalId < fNumSignals)
        {
            signal[mult_signalmap[signalId]][signalId] = signalId + 1;
            energy[mult_signalmap[signalId]][signalId] = mappedData[i]->GetEnergy();
            time[mult_signalmap[signalId]][signalId] = mappedData[i]->GetTime();
            mult_signalmap[signalId]++;
        }
    }

    for (Int_t i = 0; i < fNumGroupsAnodes; i++)
    { 
        pedestal = fEneCalParams->GetAt(fNumParamsEneFit * i);
        slope = fEneCalParams->GetAt(fNumParamsEneFit * i + 1);
        if (mult_signalmap[16] == 1 && mult_signalmap[i] == 1)
	{
            for (Int_t j = 0; j < mult_signalmap[i]; j++)
            {
                dt = 0.;
                for (Int_t k = 0; k < fNumParamsPosFit; k++)
                {
                    pospar[k] = fPosCalParams->GetAt(fNumParamsPosFit * i + k);
                    dt += pospar[k] * pow(time[j][i] - time[0][16], k);
                }
                e = pedestal + slope * energy[j][i];
                if (e > 0.)
                    AddCalData(signal[j][i], dt, e);
            }
        }

	else if((mult_signalmap[16] > 1 || mult_signalmap[i] > 1) && fUseMultHit)
	{
	    Double_t good_t;
	    Int_t no_of_dt = 0;
	    for(Int_t j = 0; j < mult_signalmap[16]; j++)
	    {
		Double_t cfd_t = ((time[j][16]*25./256.) - fHeader->GetTStart());
		if(cfd_t > winL && cfd_t < winR)
		{
		    no_of_dt++;
		    good_t = time[j][16];
		}	
	    }
	    if(no_of_dt == 1)
	    {
		for(Int_t j = 0; j < mult_signalmap[i]; j++)
		{
		    Double_t diff_t = time[j][i] - good_t;
		    if(diff_t > fMultHitCalParams->GetAt(fNumParamsMultHit * i + 0) && diff_t < fMultHitCalParams->GetAt(fNumParamsMultHit * i + 1))
		    {
		        dt = 0.;
			for (Int_t k = 0; k < fNumParamsPosFit; k++)
			{
	    		    pospar[k] = fPosCalParams->GetAt(fNumParamsPosFit * i + k);
			    dt += pospar[k] * pow(diff_t, k);
			}
			e = pedestal + slope * energy[j][i];
			if (e > 0.)
	    		    AddCalData(signal[j][i], dt, e);
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
