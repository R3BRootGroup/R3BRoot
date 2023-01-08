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

#ifndef R3BMFIDIGITIZER_H
#define R3BMFIDDIGITISER_H 1

#include "FairTask.h"
#include "R3BMfiDigi.h"
#include "R3BMfiDigiPar.h"
#include <map>
#include <string>

#include "TRandom.h"

#include "R3BKeepEventProbabilityMfi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

class R3BMfiDigitizer : public FairTask
{

  public:
    /** Default constructor **/
    R3BMfiDigitizer();

    /** Special constructor with random number generator **/
    R3BMfiDigitizer(TRandom* rndm, std::string file);
    //   TRandom* fRndm; //todo: make this guy private!!!

    /** Destructor **/
    ~R3BMfiDigitizer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();
    virtual void Reset();

    R3BMfiDigi* AddHit(Int_t mf1mul, Double_t mf1x, Int_t fiber);

    void SetFilenameMfiProb(std::string _file);
    std::string GetFilenameMfiProb();

    void SetRandomizer(TRandom* _rndm);
    TRandom* GetRandomizer();

    void SetDoMfiProb(Bool_t _do);
    Bool_t GetDoMfiProb();

    void SetDataPointerMfiProb(R3BKeepEventProbabilityMfi* _fiberProb);
    R3BKeepEventProbabilityMfi* GetDataPointerMfiProb();

    //   void InitMfiProb();

  protected:
    TClonesArray* fMfiPoints;
    TClonesArray* fMfiMCTrack;
    TClonesArray* fMfiDigi;

    std::string fFilenameMfiProb;
    TRandom* fRndm;
    Bool_t fDoMfiProb;
    R3BKeepEventProbabilityMfi* fFiberProb;

    // Parameter class
    R3BMfiDigiPar* fMfiDigiPar;

    //- Control Hitograms

    Int_t eventNoMfi;

  private:
    virtual void SetParContainers();

    ClassDef(R3BMfiDigitizer, 1);
};

#endif
