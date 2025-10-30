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

// ----------------------------------------------------------------
// -----             R3BActafCalPar source file               -----
// -----    Created 17/09/25  by Pablo González Rusell        -----
// ----------------------------------------------------------------

#pragma once

#include <FairParGenericSet.h>
#include <FairParamList.h>
#include <TArrayD.h>
#include <TArrayF.h>
#include <iostream>
#include <vector>

class R3BActafCalPar : public FairParGenericSet
{

  public:
    R3BActafCalPar(const TString& name = "actafCalPar",
                   const TString& title = "ACTAF Cal parameters",
                   const TString& context = "actafCalContext");

    virtual ~R3BActafCalPar() override;

    void clear() override;
    void putParams(FairParamList* list) override;
    bool getParams(FairParamList* list) override;
    void print() override;

    // Methods to retrieve or set values (assumed from 0 to 127)
    [[nodiscard]] double GetGainVal(int index) { return fGainPars->GetAt(index); }
    [[nodiscard]] double GetThresholdVal(int index) { return fThresholdsPars->GetAt(index); }
    [[nodiscard]] Double_t GetNbSGCoeffs() const { return fNbSGcoefs; }
    [[nodiscard]] Double_t GetSGCoeff(Int_t index) const
    {
        if (index < 0 || index >= static_cast<Int_t>(fSGCoeffs.size()))
        {
            throw std::out_of_range("GetPad: index out of range");
        }
        return fSGCoeffs[index];
    }

    void SetGainVal(int index, double val) { fGainPars->SetAt(val, index); }
    void SetThresholdVal(int index, double val) { fThresholdsPars->SetAt(val, index); }
    void SetNbSGCoeffs(Int_t num);
    void SetSGCoeff(Int_t index, double val)
    {
        if (index < 0 || index >= static_cast<Int_t>(fSGCoeffs.size()))
        {
            throw std::out_of_range("SetSGCoeffs: index out of range");
        }
        fSGCoeffs[index] = val;
    }

  private:
    static constexpr int fPads = 128;
    int fNbSGcoefs = 49;
    std::vector<double> fSGCoeffs;
    TArrayF* fGainPars = nullptr;
    TArrayF* fThresholdsPars = nullptr;

    // Do not allow copy or assignment
    const R3BActafCalPar& operator=(const R3BActafCalPar&); /*< an assignment operator>*/
    R3BActafCalPar(const R3BActafCalPar&);                  /*< a copy constructor >*/

  public:
    ClassDefOverride(R3BActafCalPar, 1);
};