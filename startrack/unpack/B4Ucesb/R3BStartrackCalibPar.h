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

// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                      from R3BCaloCalibPar                         -----
// -----                Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BSTARTRACKCALIBPAR_H
#define R3BSTARTRACKCALIBPAR_H

#include <TObjString.h>
#include <TVector3.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BStartrackCalibPar : public FairParGenericSet
{
  public:
    R3BStartrackCalibPar(const char* name = "R3BStartrackCalibPar",
                         const char* title = "Si Strip det Calibration Parameters",
                         const char* context = "TestDefaultContext");
    ~R3BStartrackCalibPar(void){};
    void clear(void){};
    void putParams(FairParamList* list);
    Bool_t getParams(FairParamList* list);

    void Print(Option_t* option = "") const;
    /** Accessor functions **/
    const Double_t GetEneCalibGain(Int_t strip) { return eneCalibGain[strip]; }
    const Double_t GetEneCalibOffset(Int_t strip) { return eneCalibOffset[strip]; }
    const Double_t GetEneCalibGainUncert(Int_t strip) { return eneCalibGainUncert[strip]; }
    const Double_t GetEneCalibOffsetUncert(Int_t strip) { return eneCalibOffsetUncert[strip]; }

    const Double_t GetTimeCalib_p0(Int_t strip) { return timeCalib_p0[strip]; }
    const Double_t GetTimeCalib_p1(Int_t strip) { return timeCalib_p1[strip]; }
    const Double_t GetTimeCalib_p2(Int_t strip) { return timeCalib_p2[strip]; }
    const Double_t GetTimeCalibUncert_p0(Int_t strip) { return timeCalibUncert_p0[strip]; }
    const Double_t GetTimeCalibUncert_p1(Int_t strip) { return timeCalibUncert_p1[strip]; }
    const Double_t GetTimeCalibUncert_p2(Int_t strip) { return timeCalibUncert_p2[strip]; }

    void SetEneCalibGain(Int_t strip, Double_t value) { eneCalibGain[strip] = value; }
    void SetEneCalibOffset(Int_t strip, Double_t value) { eneCalibOffset[strip] = value; }
    void SetEneCalibGainUncert(Int_t strip, Double_t value) { eneCalibGainUncert[strip] = value; }
    void SetEneCalibOffsetUncert(Int_t strip, Double_t value) { eneCalibOffsetUncert[strip] = value; }

    void SettimeCalib_p0(Int_t strip, Double_t value) { timeCalib_p0[strip] = value; }
    void SettimeCalib_p1(Int_t strip, Double_t value) { timeCalib_p1[strip] = value; }
    void SettimeCalib_p2(Int_t strip, Double_t value) { timeCalib_p2[strip] = value; }
    void SettimeCalibUncert_p0(Int_t strip, Double_t value) { timeCalibUncert_p0[strip] = value; }
    void SettimeCalibUncert_p1(Int_t strip, Double_t value) { timeCalibUncert_p1[strip] = value; }
    void SettimeCalibUncert_p2(Int_t strip, Double_t value) { timeCalibUncert_p2[strip] = value; }

  private:
    // Polynomial (second order) parameters for high gain (gammas) calibration
    //+ uncertainty. [2×3×double×number_of_crystals].
    Double_t* eneCalibGain;
    Double_t* eneCalibOffset;
    Double_t* eneCalibGainUncert;
    Double_t* eneCalibOffsetUncert;

    // Polynomial (second order) parameters for time calibration
    //+ uncertainty.
    Double_t* timeCalib_p0;
    Double_t* timeCalib_p1;
    Double_t* timeCalib_p2;
    Double_t* timeCalibUncert_p0;
    Double_t* timeCalibUncert_p1;
    Double_t* timeCalibUncert_p2;

    TString fGeometryFileName;

    ClassDef(R3BStartrackCalibPar, 1); //
};

#endif /* !R3BStartrackCALIBPAR_H*/
