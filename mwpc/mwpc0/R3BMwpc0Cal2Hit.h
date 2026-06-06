/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------------
// -----                     R3BMwpc0Cal2Hit                    -----
// -----          Created 09/10/19 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------------

#pragma once

#include <FairTask.h>
#include <string>

#define Mw0PadsX 64
#define Mw0PadsY 64

class TClonesArray;
class R3BMwpcHitData;

class R3BMwpc0Cal2Hit : public FairTask
{
  public:
    // Default constructor
    R3BMwpc0Cal2Hit();

    // Standard constructor
    explicit R3BMwpc0Cal2Hit(const std::string& name, int iVerbose = 1);

    // Destructor
    virtual ~R3BMwpc0Cal2Hit();

    // Method Exec
    void Exec(Option_t* /*opt*/) override;

    // Virtual method Reset
    virtual void Reset();

    // Fair specific
    // Method Init
    InitStatus Init() override;

    // Method ReInit
    InitStatus ReInit() override;

    void SetOnline(bool opt = true) { fOnline = opt; }

  private:
    Double_t fSize = 200.; // Detector size in X and Y (mm)
    Double_t fwx = 3.125;  // Pad width in X (mm)
    Double_t fwy = 3.125;  // Pad width in Y (mm)
    Int_t fx[Mw0PadsX], fy[Mw0PadsY];

    bool fOnline = false; // Don't store data for online

    TClonesArray* fMwpcCalDataCA = nullptr; // Array with Cal input data
    TClonesArray* fMwpcHitDataCA = nullptr; // Array with Hit output data

    // Private method AddHitData
    // Adds a SofMwpcHitData to the MwpcHitCollection
    R3BMwpcHitData* AddHitData(Double_t x, Double_t y);

    // Private method to obtain the position X
    Double_t GetPositionX(Double_t qmax, Int_t padmax, Double_t qleft, Double_t qright);
    // Private method to obtain the position Y
    Double_t GetPositionY(Double_t qmax, Int_t padmax, Double_t qdown, Double_t qup);

  public:
    // Class definition
    ClassDefOverride(R3BMwpc0Cal2Hit, 1); // NOLINT
};
