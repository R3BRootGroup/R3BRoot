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

// -------------------------------------------------------------
// -----               R3BAlpideNoisyPixels                -----
// -----     Created 22/03/22 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

#ifndef R3BAlpideNoisyPixels_H
#define R3BAlpideNoisyPixels_H 1

#include "FairTask.h"

#include "R3BAlpideMappingPar.h"

#include <Rtypes.h>
#include <stdint.h>
#include <vector>

class TClonesArray;

class R3BAlpideNoisyPixels : public FairTask
{
  public:
    /** Default constructor **/
    R3BAlpideNoisyPixels();

    /** Standard constructor **/
    R3BAlpideNoisyPixels(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BAlpideNoisyPixels();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option) override;

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init() override;

    /** Virtual method FinishTask **/
    virtual void FinishTask() override;

    void SetNbSensors(UInt_t n);

    void SetThreshold(UInt_t th) { fThr = th; }

  private:
    Int_t fNbSensors;
    Int_t fThr;
    std::vector<Int_t> fMap[DAlpideCols][DAlpideRows];
    TClonesArray* fAlpideMappedData; // Array with Alpide Mapped input data
    R3BAlpideMappingPar* fMap_Par;   /**< Parameter container. >*/

  public:
    // Class definition
    ClassDefOverride(R3BAlpideNoisyPixels, 1)
};

#endif /*  R3BAlpideNoisyPixels_H */
