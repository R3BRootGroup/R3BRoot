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

// -----------------------------------------------------------------
// -----         R3BFootMapped2StripCal source file            -----
// -----      Created 08/09/21 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// FOOT headers
#include "R3BFootCalData.h"
#include "R3BFootCalPar.h"
#include "R3BFootMapped2StripCal.h"
#include "R3BFootMappedData.h"
#include "R3BLogger.h"

// R3BFootMapped2StripCal: Default Constructor --------------------------
R3BFootMapped2StripCal::R3BFootMapped2StripCal()
    : R3BFootMapped2StripCal("R3BFootMapped2StripCal", 1)
{
}

// R3BFootMapped2StripCalPar: Standard Constructor --------------------------
R3BFootMapped2StripCal::R3BFootMapped2StripCal(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , NumDets(1)
    , NumStrips(640)
    , NumParams(2)
    , MaxSigma(5)
    , fTimesSigma(3.)
    , CalParams(NULL)
    , fCal_Par(NULL)
    , fFootMappedData(NULL)
    , fFootCalData(NULL)
    , fOnline(kFALSE)
{
}

// Virtual R3BFootMapped2StripCal: Destructor
R3BFootMapped2StripCal::~R3BFootMapped2StripCal()
{
    R3BLOG(DEBUG1, "");
    if (fFootCalData)
        delete fFootCalData;
}

void R3BFootMapped2StripCal::SetParContainers()
{
    // Parameter Container
    // Reading footCalPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(ERROR, !rtdb, "FairRuntimeDb not found");

    fCal_Par = (R3BFootCalPar*)rtdb->getContainer("footCalPar");
    if (!fCal_Par)
    {
        R3BLOG(ERROR, "footCalPar container not found");
    }
    else
    {
        R3BLOG(INFO, "footCalPar found");
    }
}

void R3BFootMapped2StripCal::SetParameter()
{
    //--- Parameter Container ---
    NumDets = fCal_Par->GetNumDets();      // Number of Detectors
    NumStrips = fCal_Par->GetNumStrips();  // Number of Strips
    NumParams = fCal_Par->GetNumParsFit(); // Number of Parameters

    LOG(INFO) << "R3BFootMapped2StripCal::Nb detectors: " << NumDets;
    LOG(INFO) << "R3BFootMapped2StripCal::Nb strips: " << NumStrips;
    LOG(INFO) << "R3BFootMapped2StripCal::Nb parameters from pedestal fit: " << NumParams;

    CalParams = new TArrayF();
    Int_t array_size = NumDets * NumStrips * NumParams;
    CalParams->Set(array_size);
    CalParams = fCal_Par->GetStripCalParams(); // Array with the Cal parameters

    // Count the number of dead strips per AMS detector
    for (Int_t d = 0; d < NumDets; d++)
    {
        Int_t numdeadstrips = 0;
        for (Int_t i = 0; i < NumStrips; i++)
        {
            if (CalParams->GetAt(NumParams * i + 1 + NumStrips * d * NumParams) == -1)
                numdeadstrips++;
        }
        LOG(INFO) << "R3BFootMapped2StripCal::Nb of dead strips in FOOT detector " << d + 1 << ": " << numdeadstrips;
    }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BFootMapped2StripCal::Init()
{
    R3BLOG(INFO, "");

    FairRootManager* rmg = FairRootManager::Instance();
    if (!rmg)
    {
        R3BLOG(FATAL, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fFootMappedData = (TClonesArray*)rmg->GetObject("FootMappedData");
    if (!fFootMappedData)
    {
        R3BLOG(FATAL, "FootMappedData not found");
        return kFATAL;
    }

    // OUTPUT DATA
    fFootCalData = new TClonesArray("R3BFootCalData");
    rmg->Register("FootCalData", "FOOT strip Cal", fFootCalData, !fOnline);
    fFootCalData->Clear();

    SetParameter();
    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BFootMapped2StripCal::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BFootMapped2StripCal::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Reading the Input -- Mapped Data --
    Int_t nHits = fFootMappedData->GetEntries();
    if (!nHits)
        return;

    R3BFootMappedData** mappedData = new R3BFootMappedData*[nHits];
    Int_t detId;
    Int_t stripId;
    Double_t energy;
    Double_t pedestal = 0.;
    Double_t sigma = 0.;
    Int_t StripNAve[(Int_t)fCal_Par->GetNumDets()]; //Number of Strips to compute average for each detector
    Double_t Ave[(Int_t)fCal_Par->GetNumDets()]; //Average ADC value for each detector
    Double_t StripNAveASIC[(Int_t)fCal_Par->GetNumDets()][10]; //Number of Strips to compute average for each ASICS
    Double_t AveASIC[(Int_t)fCal_Par->GetNumDets()][10]; //Average ADC value for each ASICS
    Int_t StripCounter[(Int_t)fCal_Par->GetNumDets()]; //Counter to disregard events with baseline jumps
    for (Int_t i = 0; i < fCal_Par->GetNumDets(); i++){
      StripCounter[i] = 0;
      Ave[i] = 0.;
      StripNAve[i] = 0;
      for (Int_t j = 0; j < 10; j++){
        AveASIC[i][j] = 0.;
        StripNAveASIC[i][j] = 0;
      }
    }

    //Pedestal substraction
    for (Int_t i = 0; i < nHits; i++)
    {
        mappedData[i] = (R3BFootMappedData*)(fFootMappedData->At(i));
        detId = mappedData[i]->GetDetId() - 1;
        stripId = mappedData[i]->GetStripId() - 1;

        if (CalParams)
        {
            pedestal = CalParams->GetAt(NumParams * stripId + detId * NumParams * NumStrips);
            sigma = CalParams->GetAt(NumParams * stripId + 1 + detId * NumParams * NumStrips);
        }
        
        energy = mappedData[i]->GetEnergy() - pedestal;

        //only use strips with signal below threshold for correction
        if(energy < fTimesSigma * sigma){
          Ave[detId] += energy;
          StripNAve[detId]++;
        }
    }
    
    for (Int_t i = 0; i < fCal_Par->GetNumDets(); i++){
      Ave[i] = Ave[i]/StripNAve[i];
    }             

    //Average correction 
    for (Int_t i = 0; i < nHits; i++){

      detId = mappedData[i]->GetDetId() - 1;
      stripId = mappedData[i]->GetStripId() - 1;

      if (CalParams)
        {
          pedestal = CalParams->GetAt(NumParams * stripId + detId * NumParams * NumStrips);
          sigma = CalParams->GetAt(NumParams * stripId + 1 + detId * NumParams * NumStrips);
        }

      energy = mappedData[i]->GetEnergy() - pedestal - Ave[detId];

      //only use strips with signal below threshold for correction
      if(energy < fTimesSigma * sigma){
        Int_t ASIC = (Double_t)stripId/64.;
        AveASIC[detId][ASIC] += energy;
        StripNAveASIC[detId][ASIC]++;
      }
    }

    for (Int_t i = 0; i < fCal_Par->GetNumDets(); i++){
      for (Int_t j = 0; j < 10; j++){
        AveASIC[i][j] = AveASIC[i][j]/StripNAveASIC[i][j];
      }
    }    

    //ASIC Average correction
    for (Int_t i = 0; i < nHits; i++){

      detId = mappedData[i]->GetDetId() - 1;
      stripId = mappedData[i]->GetStripId() - 1;

      if (CalParams)
        {
          pedestal = CalParams->GetAt(NumParams * stripId + detId * NumParams * NumStrips);
          sigma = CalParams->GetAt(NumParams * stripId + 1 + detId * NumParams * NumStrips);
        }

      Int_t ASIC = (Double_t)stripId/64.;

      energy = mappedData[i]->GetEnergy() - pedestal - fTimesSigma * sigma - Ave[detId] - AveASIC[detId][ASIC];

      if (energy > 0. && pedestal != -1)
        {
          StripCounter[detId]++;
        }
    }

    for (Int_t i = 0; i < nHits; i++){

      detId = mappedData[i]->GetDetId() - 1;
      stripId = mappedData[i]->GetStripId() - 1;

      if (CalParams)
        {
          pedestal = CalParams->GetAt(NumParams * stripId + detId * NumParams * NumStrips);
          sigma = CalParams->GetAt(NumParams * stripId + 1 + detId * NumParams * NumStrips);
        }

      Int_t ASIC = (Double_t)stripId/64.;

      energy = mappedData[i]->GetEnergy() - pedestal - fTimesSigma * sigma - Ave[detId] - AveASIC[detId][ASIC];
      //energy = mappedData[i]->GetEnergy() - pedestal + 500.;
      
      if (energy > 0. && pedestal != -1 && StripCounter[detId] < 100)
        {
          AddCalData(detId + 1, stripId + 1, energy);
        }
    }    
    if (mappedData)
      delete mappedData;
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BFootMapped2StripCal::Reset()
{
    R3BLOG(DEBUG1, "Clearing StripCalData Structure");
    if (fFootCalData)
        fFootCalData->Clear();
}

// -----   Private method AddCalData  --------------------------------------------
R3BFootCalData* R3BFootMapped2StripCal::AddCalData(Int_t detid, Int_t stripid, Double_t energy)
{
    // It fills the R3BFootCalData
    TClonesArray& clref = *fFootCalData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BFootCalData(detid, stripid, energy);
}

ClassImp(R3BFootMapped2StripCal);
