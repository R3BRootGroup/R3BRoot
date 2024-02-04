/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------
// -----               R3BAlpideMapped2Cal                 -----
// -----     Created 09/02/22 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

#pragma once

#include <FairTask.h>

#include "R3BAlpideCalData.h"

#include <Rtypes.h>

class TClonesArray;
class R3BAlpideMappingPar;

class R3BAlpideMapped2Cal : public FairTask
{
  public:
    /** Default constructor **/
    R3BAlpideMapped2Cal();

    /** Standard constructor **/
    R3BAlpideMapped2Cal(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BAlpideMapped2Cal() override;

    /** Method Exec **/
    void Exec(Option_t*) override;

    /** Virtual method Reset **/
    virtual void Reset();

    void SetParContainers() override;

    /** Method Init **/
    InitStatus Init() override;

    /** Method ReInit **/
    InitStatus ReInit() override;

    // Method to setup online mode
    inline void SetOnline(bool option) { fOnline = option; }

  private:
    void SetParameter();
    int GetCol(int reg, int dcol, int ads);
    int GetRow(int ads);

    bool fOnline = false; // Don't store data for online

    R3BAlpideMappingPar* fMap_Par = nullptr;   /**< Parameter container. >*/
    TClonesArray* fAlpideMappedData = nullptr; // Array with Alpide Mapped input data
    TClonesArray* fAlpideCalData = nullptr;    // Array with Alpide Cal output data

    // Private method AddCalData
    R3BAlpideCalData* AddCalData(uint16_t senId, uint16_t row, uint16_t col);

  public:
    // Class definition
    ClassDefOverride(R3BAlpideMapped2Cal, 1)
};
