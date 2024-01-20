/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------------
// -----             R3BTttxMapped2Cal source file                     -----
// -----               Created 18/01/24  by XXX                        -----
// -------------------------------------------------------------------------

#pragma once

#include <FairTask.h>

#include "R3BTttxCalData.h"
#include "R3BTttxMappedData.h"

#include <Rtypes.h>
#include <TArrayF.h>
#include <TRandom.h>

class TClonesArray;
class R3BTttxStripCalPar;

class R3BTttxMapped2Cal : public FairTask
{
  public:
    // Default constructor
    R3BTttxMapped2Cal();

    // Standard constructor
    R3BTttxMapped2Cal(const TString& name, int iVerbose = 1);

    // Destructor
    ~R3BTttxMapped2Cal();

    // Method Exec
    void Exec(Option_t* /*option*/) override;

    // Method SetParContainers
    void SetParContainers() override;

    // Method Init
    InitStatus Init() override;

    // Method ReInit
    InitStatus ReInit() override;

    inline void SetOnline(Bool_t option) { fOnline = option; }

    /**
     * Method for setting the thresholds: Signal>sigma_strip*fTimesSigma
     */
    inline void SetThresholdSigma(double th) { fTimesSigma = th; }

  private:
    void Reset();
    void SetParameter();

    int NumDets = 0;
    int NumStrips = 0;
    int NumParams = 0;
    int MaxSigma = 3;
    double fTimesSigma = 3;
    TArrayF* CalParams;

    bool fOnline = false; // Don't store data for online

    R3BTttxStripCalPar* fCal_Par = nullptr;
    TClonesArray* fTttxMapData = nullptr;
    TClonesArray* fTttxCalData = nullptr;

    // Private method AddCalData
    R3BTttxCalData* AddCalData(uint8_t detid, uint8_t stripid, double time, double energy);

  public:
    ClassDefOverride(R3BTttxMapped2Cal, 1)
};
