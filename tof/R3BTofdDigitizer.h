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

// dTofDigitzer 07 aug 2017 created by Alicia Wongel

#ifndef R3BTOFDDIGITIZER_H
#define R3BTOFDDIGITIZER_H 1

#include "FairTask.h"
#include "R3BTofdHitData.h"
#include <map>
#include <string>

class TClonesArray;
class TH1F;
class TH2F;
class TRandom3;

class R3BTofdDigitizer : public FairTask
{

  public:
    /** Default constructor **/
    R3BTofdDigitizer();

    /** Destructor **/
    ~R3BTofdDigitizer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();
    virtual void Reset();

    /** Setters for sigmas **/
    void SetSigma_y(Float_t sigma_y) { fsigma_y = sigma_y; }
    void SetSigma_t(Float_t sigma_t) { fsigma_t = sigma_t; }
    void SetSigma_ELoss(Float_t sigma_ELoss) { fsigma_ELoss = sigma_ELoss; }

  protected:
    TClonesArray* fTofdPoints;
    TClonesArray* fMCTrack;
    TClonesArray* fTofdHits;
    TRandom3* fRnd;
    TH1F* fHist1;
    TH1F* fHist2;
    TH1F* fHist3;

    Float_t fsigma_y;
    Float_t fsigma_t;
    Float_t fsigma_ELoss;

    ClassDef(R3BTofdDigitizer, 1);
};

#endif
