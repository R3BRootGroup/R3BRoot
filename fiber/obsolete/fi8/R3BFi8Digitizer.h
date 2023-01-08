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

#ifndef R3BFI8DIGITIZER_H
#define R3BFI8DDIGITISER_H 1

#include "FairTask.h"
#include "R3BFi8Digi.h"
#include "R3BFi8DigiPar.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

class R3BFi8Digitizer : public FairTask
{

  public:
    /** Default constructor **/
    R3BFi8Digitizer();

    /** Destructor **/
    ~R3BFi8Digitizer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();
    virtual void Reset();

    R3BFi8Digi* AddHit(Int_t gf1mul, Double_t gf1_01x, Int_t gf2mul, Double_t gf2_01x);

  protected:
    TClonesArray* fFi8Points;
    TClonesArray* fFi8MCTrack;
    TClonesArray* fFi8Digi;

    // Parameter class
    R3BFi8DigiPar* fFi8DigiPar;

    //- Control Hitograms

    Int_t eventNoFi8;

  private:
    virtual void SetParContainers();

    ClassDef(R3BFi8Digitizer, 1);
};

#endif
