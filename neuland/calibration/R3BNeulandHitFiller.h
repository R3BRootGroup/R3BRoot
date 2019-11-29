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

#ifndef R3BNEULANDHITFILLER_H
#define R3BNEULANDHITFILLER_H

#include "FairTask.h"

#define _nPlanes 12

class TClonesArray;
class TRandom3;
class TVector3;

class R3BNeulandHitFiller : public FairTask
{
  public:
    R3BNeulandHitFiller();
    ~R3BNeulandHitFiller();

    virtual InitStatus Init();
    virtual void Exec(Option_t* option);

    const Int_t Planes = _nPlanes;
    const Double_t QDCsplitting = 0.45;
    const Double_t minIonizing = 1.15 * 5;
    const Double_t lAtt = 0.01;
    const Double_t cLight = 29.9792458 * 5;

    Double_t tOffset[_nPlanes][50];  // ns
    Double_t vEff[_nPlanes][50];     // cm/ns
    Double_t tSync[_nPlanes][50];    // ns
    Double_t eGain[_nPlanes][50][2]; // QDC/MEV

  private:
    TVector3 getDirection();

    TClonesArray* data;
    TRandom3* r;
    Int_t nData;

  public:
    ClassDef(R3BNeulandHitFiller, 1);
};

#endif
