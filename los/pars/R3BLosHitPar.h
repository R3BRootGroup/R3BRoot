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

#pragma once

#include <FairParGenericSet.h>
#include <TArrayF.h>
#include <TObjArray.h>
#include <TString.h>

class FairParamList;

class R3BLosHitPar : public FairParGenericSet
{
  public:
    /** Standard constructor **/
    R3BLosHitPar(const TString& name = "LosHitPar",
                 const TString& title = "Los Hit Parameters",
                 const TString& context = "LosHitParContext");

    /** Destructor **/
    virtual ~R3BLosHitPar();

    /** Reset all parameters **/
    void clear() override;

    /** Store all parameters using FairRuntimeDB **/
    void putParams(FairParamList* list) override;

    /** Retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list) override;

    /** Print values of parameters to the standard output **/
    void print() override;
    void printParams() override;

    /** Accessor functions **/
    const Float_t Getp0() { return fp0; }
    const Float_t Getp1() { return fp1; }
    const Float_t Getp2() { return fp2; }
    const Float_t Getxoffset_MCFD() { return fxoffset_MCFD; }
    const Float_t Getyoffset_MCFD() { return fyoffset_MCFD; }
    const Float_t Getxveff_MCFD() { return fxveff_MCFD; }
    const Float_t Getyveff_MCFD() { return fyveff_MCFD; }
    const Int_t GetNumParamsTamexLE() { return fNumParamsTamexLE; }
    const Int_t GetNumParamsTamexTE() { return fNumParamsTamexTE; }
    TArrayF* GetLEMatchParams() { return fLEMatchParams; }
    TArrayF* GetTEMatchParams() { return fTEMatchParams; }

    inline void Setp0(Float_t value) { fp0 = value; }
    inline void Setp1(Float_t value) { fp1 = value; }
    inline void Setp2(Float_t value) { fp2 = value; }

    inline void SetLEMatchParams(Float_t val, Int_t pmt, Int_t indexpar)
    {
        // 0-base
        int index = (pmt - 1) * fNumParamsTamexLE + indexpar - 1;
        fLEMatchParams->AddAt(val, index);
    }
    inline void SetTEMatchParams(Float_t val, Int_t pmt, Int_t indexpar)
    {
        // 0-base
        int index = (pmt - 1) * fNumParamsTamexTE + indexpar - 1;
        fTEMatchParams->AddAt(val, index);
    }

  private:
    Float_t fp0 = 0., fp1 = 1., fp2 = 0.;
    Float_t fxoffset_MCFD = 0., fyoffset_MCFD = 0., fxveff_MCFD = 1., fyveff_MCFD = 1.;
    TArrayF* fLEMatchParams; // Calibration parameters for Matching Tamex LE
    TArrayF* fTEMatchParams; // Calibration parameters for Matching Tamex TE

    Int_t fNumParamsTamexLE = 2; // number of parameters for matching Tamex LE
    Int_t fNumParamsTamexTE = 2; // number of parameters for matching Tamex TE

    R3BLosHitPar(const R3BLosHitPar&) = default;
    R3BLosHitPar& operator=(const R3BLosHitPar&) = default;

  public:
    ClassDefOverride(R3BLosHitPar, 2);
};
