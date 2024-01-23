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

#ifndef R3BFRSSCICALPAR_H
#define R3BFRSSCICALPAR_H

#include "FairParGenericSet.h"
#include "TArrayD.h"
#include "TArrayF.h"
#include "TObjArray.h"
#include "TObject.h"

#include <TObjString.h>

class FairParamList;

class R3BFrsSciCalPar : public FairParGenericSet
{

  public:
    /** Standard constructor **/
    R3BFrsSciCalPar(const char* name = "FrsSciCalPar",
                    const char* title = "FrsSciCal Parameters",
                    const char* context = "FrsSciCalParContext");

    /** Destructor **/
    virtual ~R3BFrsSciCalPar();

    /** Method to reset all parameters **/
    virtual void clear();

    /** Method to store all parameters using FairRuntimeDB **/
    virtual void putParams(FairParamList* list);

    /** Method to retrieve all parameters using FairRuntimeDB**/
    Bool_t getParams(FairParamList* list);

    /** Method to print values of parameters to the standard output **/
    void printParams();

    /** Accessor functions **/

    void SetNumDets(Int_t nDets) { fNumDets = nDets; }
    void SetNumPmts(Int_t nPmts) { fNumPmts = nPmts; }
    void SetNumTofs(Int_t nDets)
    {
        switch (nDets)
        {
            case 1:
                fNumTofs = 0;
                break;
            case 2:
                fNumTofs = 1;
                break;
            default:
                UShort_t fact1 = 1;
                UShort_t fact2 = 1;
                for (Int_t i = 2; i <= nDets; i++)
                    fact1 *= i;
                for (Int_t i = 2; i <= (nDets - 2); i++)
                    fact2 *= i;
                fNumTofs = fact1 / (2 * fact2);
        }
    }

    void SetDetIdS2(Int_t id) { fDetIdS2 = id; }
    void SetDetIdS8(Int_t id) { fDetIdS8 = id; }
    void SetDetIdCaveC(Int_t id) { fDetIdCaveC = id; }

    void SetMinPos(Float_t min, UInt_t rank) { fMinPos->AddAt(min, rank); }
    void SetMaxPos(Float_t max, UInt_t rank) { fMaxPos->AddAt(max, rank); }
    void SetMinTof(Double_t min, UInt_t rank) { fMinTofs->AddAt(min, rank); }
    void SetMaxTof(Double_t max, UInt_t rank) { fMaxTofs->AddAt(max, rank); }

    void SetTof2InvVGain(Double_t gain, UInt_t rank) { fTof2InvVGains->AddAt(gain, rank); }
    void SetTof2InvVOffset(Double_t offset, UInt_t rank) { fTof2InvVOffsets->AddAt(offset, rank); }
    void SetFlightLength(Double_t L, UInt_t rank) { fFlightLengths->AddAt(L, rank); }
    void SetPosCalGain(Float_t gain, UInt_t rank) { fPosCalGains->AddAt(gain, rank); }
    void SetPosCalOffset(Float_t offset, UInt_t rank) { fPosCalOffsets->AddAt(offset, rank); }

    const Int_t GetNumDets() { return fNumDets; }
    const Int_t GetNumPmts() { return fNumPmts; }
    const Int_t GetNumTofs() { return fNumTofs; }

    const Int_t GetDetIdS2() { return fDetIdS2; }
    const Int_t GetDetIdS8() { return fDetIdS8; }
    const Int_t GetDetIdCaveC() { return fDetIdCaveC; }

    const Float_t GetMinPosAtRank(UInt_t rank) { return fMinPos->At(rank); }
    const Float_t GetMaxPosAtRank(UInt_t rank) { return fMaxPos->At(rank); }
    const Double_t GetMinTofAtRank(UInt_t rank) { return fMinTofs->At(rank); }
    const Double_t GetMaxTofAtRank(UInt_t rank) { return fMaxTofs->At(rank); }
    TArrayF* GetAllParams_fMinPos() { return fMinPos; }
    TArrayF* GetAllParams_fMaxPos() { return fMaxPos; }
    TArrayD* GetAllParams_fMinTofs() { return fMinTofs; }
    TArrayD* GetAllParams_fMaxTofs() { return fMaxTofs; }

    const Double_t GetTof2InvVGainAtRank(UInt_t rank) { return fTof2InvVGains->At(rank); }
    const Double_t GetTof2InvVOffsetAtRank(UInt_t rank) { return fTof2InvVOffsets->At(rank); }
    const Double_t GetFlightLengthAtRank(UInt_t rank) { return fFlightLengths->At(rank); }
    const Float_t GetPosCalGainAtRank(UInt_t rank) { return fPosCalGains->At(rank); }
    const Float_t GetPosCalOffsetAtRank(UInt_t rank) { return fPosCalOffsets->At(rank); }
    TArrayD* GetAllParams_fTof2InvVGains() { return fTof2InvVGains; }
    TArrayD* GetAllParams_fTof2InvVOffsets() { return fTof2InvVOffsets; }
    TArrayD* GetAllParams_fFlightLengths() { return fFlightLengths; }
    TArrayF* GetAllParams_fPosCalGains() { return fPosCalGains; }
    TArrayF* GetAllParams_fPosCalOffsets() { return fPosCalOffsets; }

  private:
    // parameters for dimension
    Int_t fNumDets; // number of FrsSci detectors
    Int_t fNumPmts; // number of "Pmts" (by default = 3: 2 Pmts and one Tref)
    Int_t fNumTofs; // number of ToFs (fNumDets! / (2! * (fNumDets-2)!))

    // parameters to identify the different FrsSci
    Int_t fDetIdS2;
    Int_t fDetIdS8;
    Int_t fDetIdCaveC;

    // parameters to select the proper hit per signal if fNumDets=1
    TArrayF* fMinPos;
    TArrayF* fMaxPos;

    // parameters to select the proper hit per signal if fNumDets>1
    TArrayD* fMinTofs; // size = fNumDets-1 (Cave C versus all the others)
    TArrayD* fMaxTofs; // size = fNumDets-1 (Cave C versus all the others)

    // parameters to calibrate all Tofs
    TArrayD* fTof2InvVGains;   // number of Tofs
    TArrayD* fTof2InvVOffsets; // number of Tofs
    TArrayD* fFlightLengths;   // number of Tof

    // parameters to calibrate the position in Mm
    TArrayF* fPosCalGains;   // 1 gain per per detector
    TArrayF* fPosCalOffsets; // 1 offset per per detector

    const R3BFrsSciCalPar& operator=(const R3BFrsSciCalPar&);
    R3BFrsSciCalPar(const R3BFrsSciCalPar&);

    ClassDef(R3BFrsSciCalPar, 1);
};

#endif // R3BFRSSCICALPAR_H
