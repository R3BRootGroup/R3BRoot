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

// -------------------------------------------------------------------------
// -----                   R3BStartrackHitFinder source file                 -----
// -----                  Created 01/06/11  by N.Ashwood               -----
// -------------------------------------------------------------------------

/**  R3BStartrackHitFinder.h
 **/

#ifndef R3BSTARTRACKCAL2HIT_H
#define R3BSTARTRACKCAL2HIT_H

#include "FairTask.h"
#include "R3BStartrackerDigitHit.h"

#include "R3BStartrackHitPar.h"

class TClonesArray;

class R3BStartrackCal2Hit : public FairTask
{

  public:
    /** Default constructor **/
    R3BStartrackCal2Hit();

    /** Destructor **/
    ~R3BStartrackCal2Hit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

    /** Public method SetExperimentalResolution
     **
     ** Defines the experimental resolution of the tracker.
     **
     **/
    void SetExperimentalResolution(Double_t trackerRes);

    /** Public method SetDetectionThreshold
     **
     ** Defines the minimum energy requested in a strip to be considered in a tracker Hit
     **
     **/
    void SetDetectionThreshold(Double_t thresholdEne);

  protected:
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Finish **/
    virtual void Finish();

    TClonesArray* fStartrackerHitCA;
    TClonesArray* fStartrackHitCA;

    // Minimum energy requested in a crystal to be considered in a calorimeter Hit
    Double_t fThreshold;
    // Experimental resolution @ 1 MeV
    Double_t fTrackerResolution;

    // Parameter class
    R3BStartrackHitPar* fStartrackHitPar;

  private:
    /** Private method ExpResSmearing **/
    Double_t ExpResSmearing(Double_t inputEnergy);

    /** Private method Addhit **/
    // R3BStartrackerDigitHit* AddHit(Double_t ene,Int_t det,Int_t Stripfrt,Int_t Stripbck,Double_t time);
    R3BStartrackerDigitHit* AddHit(Int_t det, Int_t chip, Int_t Side, Int_t Strip, Double_t energy, Double_t time);

    virtual void SetParContainers();

    ClassDef(R3BStartrackCal2Hit, 1);
};

#endif
