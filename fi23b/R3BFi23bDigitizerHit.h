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

// R3BFi23bDigitizer.h

#ifndef R3BFi23BDIGITIZERHIT_H
#define R3BFi23BDIGITIZERHIT_H 1

#include "FairTask.h"
#include "R3BFiberMAPMTHitData.h"
#include <TRandom3.h>
#include <map>
#include <string>

class TClonesArray;
class TH1F;
class TH2F;

class R3BFi23bDigitizerHit : public FairTask
{

  public:
    /** Default constructor **/
    R3BFi23bDigitizerHit();

    R3BFi23bDigitizerHit(Double_t esigma, Double_t tsigma, Double_t xsigma, Double_t ysigma);

    /** Destructor **/
    ~R3BFi23bDigitizerHit();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();
    virtual void Reset();

    void SetEnergyResolution(Double_t e);
    void SetTimeResolution(Double_t t);
    void SetXPositionResolution(Double_t x);
    void SetYPositionResolution(Double_t y);

  protected:
    TClonesArray* fFi23bPoints;

    TClonesArray* fMCTrack;

    TClonesArray* fFi23bHits;

  private:
    TRandom3* prnd;
    Double_t esigma;
    Double_t tsigma;
    Double_t ysigma;
    Double_t xsigma;
    Int_t fiber_nbr = 384;

    ClassDef(R3BFi23bDigitizerHit, 1);
};

#endif
