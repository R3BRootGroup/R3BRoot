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

// --------------------------------------------------------------
// -----             R3BFootMapped2StripCal                 -----
// -----     Created 08/09/21 by J.L. Rodriguez-Sanchez     -----
// --------------------------------------------------------------

#ifndef R3BFootMapped2StripCal_H
#define R3BFootMapped2StripCal_H 1

#include "FairTask.h"

#include "R3BFootCalData.h"

#include <Rtypes.h>

class TClonesArray;
class R3BFootCalPar;

class R3BFootMapped2StripCal : public FairTask
{
  public:
    /** Default constructor **/
    R3BFootMapped2StripCal();

    /** Standard constructor **/
    R3BFootMapped2StripCal(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BFootMapped2StripCal();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option) override;

    /** Virtual method Reset **/
    virtual void Reset();

    virtual void SetParContainers() override;

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init() override;

    /** Virtual method ReInit **/
    virtual InitStatus ReInit() override;

    // Method to setup online mode
    void SetOnline(Bool_t option) { fOnline = option; }

    /**
     * Method for setting the thresholds: Signal>sigma_strip*fTimesSigma
     */
    void SetThresholdSigma(Double_t th) { fTimesSigma = th; }

  private:
    void SetParameter();

    Int_t NumDets;
    Int_t NumStrips;
    Int_t NumParams;
    Int_t MaxSigma;
    Double_t fTimesSigma;
    TArrayF* CalParams;

    Bool_t fOnline; // Don't store data for online

    R3BFootCalPar* fCal_Par;       // Parameter container
    TClonesArray* fFootMappedData; // Array with FOOT Mapped input data
    TClonesArray* fFootCalData;    // Array with FOOT Cal output data

    // Private method AddCalData
    R3BFootCalData* AddCalData(Int_t detid, Int_t stripid, Double_t energy);

  public:
    // Class definition
    ClassDefOverride(R3BFootMapped2StripCal, 1)
};

#endif /*  R3BFootMapped2StripCal_H */
