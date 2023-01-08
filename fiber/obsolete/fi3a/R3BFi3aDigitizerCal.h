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

// R3BFi3aDigitizerCal.h

#ifndef R3BFi3aDIGITIZERCAL_H
#define R3BFi3aDIGITIZERCAL_H 1

#include "FairTask.h"
#include "R3BBunchedFiberCalData.h"
#include <TRandom3.h>
#include <map>
#include <string>

class TClonesArray;
class TH1F;
class TH2F;

class R3BFi3aDigitizerCal : public FairTask
{

  public:
    /** Default constructor **/
    R3BFi3aDigitizerCal();

    R3BFi3aDigitizerCal(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BFi3aDigitizerCal();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    virtual void Finish();

    virtual void Reset();

    inline void SetEnergyResolution(Double_t e) { esigma = e; }
    inline void SetTimeResolution(Double_t t) { tsigma = t; }
    inline void SetYPositionResolution(Double_t y) { ysigma = y; }

  protected:
    TClonesArray* fFi3aPoints;

    TClonesArray* fMCTrack;

    TClonesArray* fFi3aCals;
    TClonesArray* fFi3aTriggerCals;

  private:
    TRandom3* prnd;
    Double_t esigma;
    Double_t tsigma;
    Double_t ysigma;

    Float_t detector_height = 10.000000;
    Float_t fiber_thickness = 0.021000;
    Int_t fiber_nbr = 512;
    Float_t dead_layer = 0.9;
    Float_t air_layer = 0.01; // relative to fiber_thickness
    Float_t detector_width = fiber_nbr * fiber_thickness * (1 + air_layer);

    ClassDef(R3BFi3aDigitizerCal, 1);
};

#endif
