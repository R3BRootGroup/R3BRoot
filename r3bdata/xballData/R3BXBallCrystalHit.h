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
// -----                  R3BXBallCrystalHit header file                -----
// -----               Created 13/06/12  by Pablo Cabanelas            -----
// -------------------------------------------------------------------------

/**  R3BXBallCrystalHit.h
 **  A cal crystal hit is the representation of the
 **  information obtained from each crystal in the DH-CrystalBall.
 **/

#ifndef R3BXBALLCRYSTALHIT_H
#define R3BXBALLCRYSTALHIT_H

#include "FairMultiLinkedData.h"
#include "TObject.h"

class R3BXBallCrystalHit : public FairMultiLinkedData
{
  public:
    /** Default constructor **/
    R3BXBallCrystalHit();

    /** Constructor with arguments
     *@param fCrystalNb      Crystal number
     *@param fEnergy	   total energy deposited on the crystal [GeV]
     *@param fTime           Time since event start [ns]
     **/
    R3BXBallCrystalHit(Int_t crysnb, Double_t energy, Double_t time);

    /** Copy constructor **/
    R3BXBallCrystalHit(const R3BXBallCrystalHit& hit) { *this = hit; };

    /** Destructor **/
    virtual ~R3BXBallCrystalHit();

    /** Accessors **/
    Int_t GetCrystalNumber() const { return fCrystalNb; }
    Double_t GetEnergy() const { return fEnergy; }
    Double_t GetTime() const { return fTime; }

    /** Modifiers **/
    void SetCrystalNumber(Int_t crysnb) { fCrystalNb = crysnb; }
    void SetEnergy(Double32_t energy) { fEnergy = energy; }
    void SetTime(Double32_t time) { fTime = time; }

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

  protected:
    Double32_t fEnergy; // total energy in the crystal
    Int_t fCrystalNb;   // crystal number (1-162, number of crystals in the CB)
    Double32_t fTime;   // time of the interaction

    ClassDef(R3BXBallCrystalHit, 1)
};

#endif
