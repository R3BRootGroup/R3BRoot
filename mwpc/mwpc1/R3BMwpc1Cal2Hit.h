/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------------
// -----                       R3BMwpc1 header file                  -----
// -----                 Created 06/10/19 by JL Rodriguez            -----
// -----                  s455 method 17/06/22 by Antia GG           -----
// -----------------------------------------------------------------------

#pragma once

#include <FairTask.h>
#include <TH1F.h>
#include <TRandom.h>

#include "R3BMwpcCalData.h"
#include "R3BMwpcHitData.h"

#define Mw1PadsX 64
#define Mw1PadsY 40

using namespace std;

class TClonesArray;
class R3BEventHeader;

class R3BMwpc1Cal2Hit : public FairTask
{

  public:
    /** Default constructor **/
    R3BMwpc1Cal2Hit();

    /** Standard constructor **/
    explicit R3BMwpc1Cal2Hit(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BMwpc1Cal2Hit() override;

    /** Method Exec **/
    void Exec(Option_t* option) override;

    /** Virtual method Reset **/
    virtual void Reset();

    // Fair specific
    /** Method Init **/
    InitStatus Init() override;

    /** Method ReInit **/
    InitStatus ReInit() override;

    void SetOnline(bool option = true) { fOnline = option; }

    void SetExpId(Int_t exp)
    {
        fExpId = exp;
    } // Mutator to set fExpId manually. It should be globally defined by EventHeader.

  private:
    /** Private method Experiment s455 **/
    virtual void S455();
    /** Private method Experiment s467 **/
    virtual void S467();

    R3BEventHeader* header = nullptr; // Event header

    Double_t fSize = 200.0; // Detector size in X and Y in mm
    Double_t fwx = 5.0;     // Pad width in X in mm
    Double_t fwy = 3.125;   // Pad width in Y in mm
    Int_t fx[Mw1PadsX], fx_p1[Mw1PadsX], fx_p2[Mw1PadsX], fy[Mw1PadsY];

    bool fOnline = false; // Don't store data for online if it is true
    Int_t fExpId = 0;

    TClonesArray* fMwpcCalDataCA = nullptr; // Array with Cal input data
    TClonesArray* fMwpcHitDataCA = nullptr; // Array with Hit output data

    /** Private method AddHitData **/
    // Adds a MwpcHitData to the MwpcHitCollection
    R3BMwpcHitData* AddHitData(Double_t x, Double_t y, Int_t plane = 1);

    /** Private method to obtain the position X **/
    Double_t GetPositionX(Double_t qmax, Int_t padmax, Double_t qleft, Double_t qright);
    /** Private method to obtain the position Y **/
    Double_t GetPositionY(Double_t qmax, Int_t padmax, Double_t qdown, Double_t qup);
    /** Private method to obtain the position Y **/
    [[nodiscard]] Double_t GetPositionYCoG(Double_t qmax, Int_t padmax, Double_t qdown, Double_t qup) const;

  public:
    // Class definition
    ClassDefOverride(R3BMwpc1Cal2Hit, 1); // NOLINT
};
