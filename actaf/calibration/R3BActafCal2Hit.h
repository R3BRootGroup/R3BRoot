/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
// -----               R3BActafCal2Hit                     -----
// -----     Created 14/09/25 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

#pragma once

#include <FairTask.h>

#include "R3BActafHitData.h"

#include <Rtypes.h>
#include <TVector3.h>

class TClonesArray;
class R3BActafGeometry;

class R3BActafCal2Hit : public FairTask
{
  public:
    /** Default constructor **/
    R3BActafCal2Hit();

    /** Standard constructor **/
    R3BActafCal2Hit(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BActafCal2Hit() override;

    /** Method Exec **/
    void Exec(Option_t*) override;

    /** Virtual method Reset **/
    virtual void Reset();

    /** Method Init **/
    InitStatus Init() override;

    /** Method ReInit **/
    InitStatus ReInit() override;

    // Method to setup online mode
    inline void SetOnline(bool option = true) { fOnline = option; }

  private:
    bool fOnline = false; // Don't store data for online
    int fGeoversion = 2025;

    R3BActafGeometry* fActafGeo = nullptr;
    TClonesArray* fActafCalData = nullptr; // Array with Actaf Cal input data
    TClonesArray* fActafHitData = nullptr; // Array with Actaf Hit output data

    // Private method AddHitData
    R3BActafHitData* AddHitData(UInt_t padId,
                                UInt_t side,
                                UInt_t ring,
                                double xpos,
                                double ypos,
                                double zpos,
                                double energy,
                                TVector3 track);

  public:
    // Class definition
    ClassDefOverride(R3BActafCal2Hit, 1);
};
