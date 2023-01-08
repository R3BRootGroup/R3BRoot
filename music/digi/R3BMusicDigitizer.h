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

// ----------------------------------------------------------------
// -----            R3BMusicDigitizer source file             -----
// -----         Created 18/10/21  by JL Rodriguez            -----
// ----------------------------------------------------------------

#ifndef R3BMusicDigitizer_H
#define R3BMusicDigitizer_H 1

#include "FairTask.h"
#include "R3BMusicCalData.h"
#include <Rtypes.h>
#include <string>

class TClonesArray;

class R3BMusicDigitizer : public FairTask
{

  public:
    /** Default constructor **/
    R3BMusicDigitizer();

    /** Standard constructor **/
    R3BMusicDigitizer(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BMusicDigitizer();

    /** Virtual method Init **/
    virtual InitStatus Init() override;

    /** Virtual method ReInit **/
    virtual InitStatus ReInit() override;

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt) override;

    virtual void Reset();

  private:
    TClonesArray* fMCTrack;
    TClonesArray* fMusicPoints;
    TClonesArray* fMusicCal;
    Float_t fsigma_x;
    TString fName;

    // Add a MusicCalData to the HitCollection
    R3BMusicCalData* AddHitData(Int_t anodeId, Double_t pos, Double_t e);

  public:
    // Class definition
    ClassDefOverride(R3BMusicDigitizer, 1);
};

#endif /* R3BMusicDigitizer_H */
