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

// ------------------------------------------------------------------
// -----                R3BFootCalPar source file               -----
// -----         Created 07/09/21 by J.L. Rodriguez-Sanchez     -----
// ------------------------------------------------------------------

#ifndef R3BFootCalPar_H
#define R3BFootCalPar_H

#include "FairParGenericSet.h"

#include "TArrayF.h"
#include "TObject.h"
#include <Rtypes.h>

class FairParamList;

class R3BFootCalPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BFootCalPar(const char* name = "footStripCalPar",
                  const char* title = "FOOT Strip Parameters",
                  const char* context = "FootCalParContext");

    /** Destructor **/
    virtual ~R3BFootCalPar();

    /** Method to reset all parameters **/
    virtual void clear() override;

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list) override;

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list) override;

    /** Method to print values of parameters to the standard output **/
    virtual void print() override;

    /** Accessor functions **/
    const Double_t GetNumDets() { return fNumDets; }
    const Double_t GetNumStrips() { return fNumStrips; }
    const Double_t GetNumParsFit() { return fNumParsFit; }
    TArrayF* GetStripCalParams() { return fStripCalParams; }

    void SetNumDets(Int_t n) { fNumDets = n; }
    void SetNumStrips(Int_t n) { fNumStrips = n; }
    void SetNumParsFit(Int_t n) { fNumParsFit = n; }
    void SetStripCalParams(Double_t c, Int_t i) { fStripCalParams->AddAt(c, i); }

    /** Create more Methods if you need them! **/

  private:
    TArrayF* fStripCalParams; // Calibration Parameters of strip
    Int_t fNumDets;           // Number of detectors
    Int_t fNumStrips;         // Number of strips
    Int_t fNumParsFit;        // Number of parameters for the fit

    const R3BFootCalPar& operator=(const R3BFootCalPar&);
    R3BFootCalPar(const R3BFootCalPar&);

    ClassDefOverride(R3BFootCalPar, 1);
};

#endif /* R3BFootCalPar_H */
