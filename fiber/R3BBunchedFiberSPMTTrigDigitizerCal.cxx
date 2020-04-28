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

#include "R3BBunchedFiberSPMTTrigDigitizerCal.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

// includes for modeling
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGeoShapeAssembly.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "R3BFibPoint.h"
#include "R3BMCTrack.h"

using std::cout;
using std::endl;

R3BBunchedFiberSPMTTrigDigitizerCal::R3BBunchedFiberSPMTTrigDigitizerCal()
    : FairTask("R3B Fiber single PMT Digitization scheme ", 1)
    , fFiberTriggerSingleCals(new TClonesArray("R3BBunchedFiberCalData"))

{
}

R3BBunchedFiberSPMTTrigDigitizerCal::~R3BBunchedFiberSPMTTrigDigitizerCal() 
{
    delete fFiberTriggerSingleCals;
}

InitStatus R3BBunchedFiberSPMTTrigDigitizerCal::Init()
{
    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(fatal) << "Init:No FairRootManager";
    fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");
    if (NULL == fMCTrack)
        LOG(fatal) << "Branch MCTrack not found";

    // Register output array 
    ioman->Register("BunchedFiberSPMTTrigCal", "Digital response of single PMT of fiber", 
    fFiberTriggerSingleCals, kTRUE);

    return kSUCCESS;
}

void R3BBunchedFiberSPMTTrigDigitizerCal::Exec(Option_t* opt)
{

    Reset();
	
	for(Int_t j = 0; j< 3; j++){

        new ((*fFiberTriggerSingleCals)[fFiberTriggerSingleCals->GetEntriesFast()])
          R3BBunchedFiberCalData(1, j+1, 1, 0.);
	}
	
}
// -------------------------------------------------------------------------

void R3BBunchedFiberSPMTTrigDigitizerCal::Reset()
{
    if (fFiberTriggerSingleCals)
        fFiberTriggerSingleCals->Clear();
}

void R3BBunchedFiberSPMTTrigDigitizerCal::Finish() {}

ClassImp(R3BBunchedFiberSPMTTrigDigitizerCal)
