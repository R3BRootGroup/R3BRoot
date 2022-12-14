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

// ---------------------------------------------------------------
// -----         R3BMwpc0Mapped2Cal source file              -----
// -----    Created 07/10/19 by J.L. Rodriguez-Sanchez       -----
// ---------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// R3B headers
#include "R3BLogger.h"
#include "R3BMwpc0CalPar.h"
#include "R3BMwpc0Mapped2Cal.h"
#include "R3BMwpcCalData.h"
#include "R3BMwpcMappedData.h"

// R3BMwpc0Mapped2Cal: Default Constructor --------------------------
R3BMwpc0Mapped2Cal::R3BMwpc0Mapped2Cal()
    : R3BMwpc0Mapped2Cal("R3BMwpc0Mapped2Cal", 1)
{
}

// R3BMwpc0Mapped2Cal: Standard Constructor --------------------------
R3BMwpc0Mapped2Cal::R3BMwpc0Mapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , NumPadX(0)
    , NumPadY(0)
    , NumParams(0)
    , CalParams(NULL)
    , fCal_Par(NULL)
    , fMwpcMappedDataCA(NULL)
    , fMwpcCalDataCA(NULL)
    , fOnline(kFALSE)
{
}

// Virtual R3BMwpc0Mapped2Cal: Destructor
R3BMwpc0Mapped2Cal::~R3BMwpc0Mapped2Cal()
{
    R3BLOG(DEBUG1, "Destructor");
    if (fMwpcCalDataCA)
        delete fMwpcCalDataCA;
}

void R3BMwpc0Mapped2Cal::SetParContainers()
{

    // Parameter Container
    // Reading padCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(ERROR, !rtdb, "FairRuntimeDb not found");

    fCal_Par = (R3BMwpc0CalPar*)rtdb->getContainer("mwpc0CalPar");
    if (!fCal_Par)
    {
        R3BLOG(ERROR, "Couldn't get handle on mwpc0CalPar container.");
    }
    else
    {
        R3BLOG(INFO, "mwpc0CalPar container found.");
    }
    return;
}

void R3BMwpc0Mapped2Cal::SetParameter()
{
    //--- Parameter Container ---
    NumPadX = fCal_Par->GetNumPadsX();           // Number of Pads in X
    NumPadY = fCal_Par->GetNumPadsY();           // Number of Pads in Y
    NumParams = fCal_Par->GetNumParametersFit(); // Number of parameters in the Fit

    R3BLOG(INFO, "NumPadX: " << NumPadX);
    R3BLOG(INFO, "NumPadY: " << NumPadY);
    R3BLOG(INFO, "Number of fit parameters: " << NumParams);

    CalParams = new TArrayF();
    Int_t array_size = (NumPadX + NumPadY) * NumParams;
    CalParams->Set(array_size);
    CalParams = fCal_Par->GetPadCalParams(); // Array with the Cal parameters
    return;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BMwpc0Mapped2Cal::Init()
{
    R3BLOG(INFO, "");

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        R3BLOG(FATAL, "FairRootManager not found");
        return kFATAL;
    }

    fMwpcMappedDataCA = (TClonesArray*)rootManager->GetObject("Mwpc0MappedData");
    if (!fMwpcMappedDataCA)
    {
        R3BLOG(FATAL, "Mwpc0MappedData not found");
        return kFATAL;
    }

    // OUTPUT DATA
    // Calibrated data
    fMwpcCalDataCA = new TClonesArray("R3BMwpcCalData");
    rootManager->Register("Mwpc0CalData", "MWPC0 Cal", fMwpcCalDataCA, !fOnline);
    fMwpcCalDataCA->Clear();

    SetParameter();
    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BMwpc0Mapped2Cal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BMwpc0Mapped2Cal::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Reading the Input -- Mapped Data --
    Int_t nHits = fMwpcMappedDataCA->GetEntriesFast();
    if (nHits > (NumPadX + NumPadY) && nHits > 0)
    {
        R3BLOG(WARNING, "nHits>(NumPadX+NumPadY)");
    }
    if (nHits==0)
        return;

    R3BMwpcMappedData** mappedData;
    mappedData = new R3BMwpcMappedData*[nHits];
    Int_t planeId = 0;
    Int_t padId = 0;
    Float_t charge = 0.0;
    Float_t pedestal = 0.0;
    Int_t nbpad = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        mappedData[i] = dynamic_cast<R3BMwpcMappedData*>((fMwpcMappedDataCA->At(i)));
        planeId = mappedData[i]->GetPlane();
        padId = mappedData[i]->GetPad() - 1;
        if (planeId == 1)
            nbpad = padId * NumParams;
        else if (planeId == 3)
            nbpad = (padId + NumPadX) * NumParams;
        else
            R3BLOG(ERROR, "Plane " << planeId << " does not exist in MWPC0");

        pedestal = CalParams->GetAt(nbpad);
        charge = mappedData[i]->GetQ() - pedestal;

        // We accept the hit if the charge is larger than zero
        if (charge > 0)
        {
            AddCalData(planeId, padId + 1, charge);
        }
    }
    if (mappedData)
        delete[] mappedData;
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BMwpc0Mapped2Cal::Reset()
{
    R3BLOG(DEBUG1, "Clearing Mwpc0CalData Structure");
    if (fMwpcCalDataCA)
        fMwpcCalDataCA->Clear();
}

// -----   Private method AddCalData  --------------------------------------------
R3BMwpcCalData* R3BMwpc0Mapped2Cal::AddCalData(Int_t plane, Int_t pad, Float_t charge)
{
    // It fills the R3BMwpcCalData
    TClonesArray& clref = *fMwpcCalDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BMwpcCalData(plane, pad, charge);
}

ClassImp(R3BMwpc0Mapped2Cal);
