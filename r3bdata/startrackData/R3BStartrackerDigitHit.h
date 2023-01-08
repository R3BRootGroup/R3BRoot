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
// -----                  R3BStartrackerDigitHit header file           -----
// -----               Created 28/04/14  by M.Labiche                  -----
// -------------------------------------------------------------------------

#ifndef R3BSTARTRACKERDIGITHIT_H
#define R3BSTARTRACKERDIGITHIT_H

#include "TObject.h"

#include "FairMultiLinkedData.h"

class R3BStartrackerDigitHit : public FairMultiLinkedData
{

  public:
    /** Default constructor **/
    R3BStartrackerDigitHit();

    // R3BStartrackerDigitHit(Double_t ene, Int_t det, Int_t Stripfrt, Int_t Stripbck, Double_t time);
    R3BStartrackerDigitHit(Int_t det, Int_t chip, Int_t side, Int_t strip, Double_t ene, Double_t time);

    /** Copy constructor **/
    R3BStartrackerDigitHit(const R3BStartrackerDigitHit& hit) { *this = hit; };

    /** Destructor **/
    virtual ~R3BStartrackerDigitHit();

    /** Accessors **/
    // Int_t GetNbOfTrackerHits()   const { return fNbOfTrackerHits; }
    Double_t GetEnergy() const { return fEnergy; }
    Double_t GetTime() const { return fTime; }
    Int_t GetDetector() const { return fDetector; }
    Int_t GetChip() const { return fChip; }
    Int_t GetSide() const { return fSide; }
    Int_t GetStrip() const { return fStrip; }
    /*
    Double_t GetStripfrt()   const { return fStripfrt; }
    Double_t GetStripbck()   const { return fStripbck; }
    */
    /** Modifiers **/
    // void SetNbOfTrackerHits(Int_t number){fNbOfTrackerHits = number;}
    void SetEnergy(Double_t ene) { fEnergy = ene; }
    void SetDetector(Int_t det) { fDetector = det; }
    void SetChip(Int_t chip) { fChip = chip; }
    void SetSide(Int_t side) { fSide = side; }
    void SetStrip(Int_t strip) { fStrip = strip; }
    /*
    void SetStripfrt(Int_t Stripfrt){fStripfrt = Stripfrt;}
    void SetStripbck(Int_t Stripbck){fStripbck = Stripbck;}
    */

    /** Output to screen **/
    // virtual void Print(const Option_t* opt) const;

  protected:
    // Basic Hit information
    // Int_t fNbOfTrackerHits; //number of Si hits contributing to the R3BStartrackerHit
    Double_t fEnergy; // total energy deposited
    Double_t fTime;   // global time when hit occur
    Int_t fDetector;  // Detector hit by event
    Int_t fChip;      // ASIC chip hit by event
    Int_t fSide;      // Front or back of Det hit by event
    Int_t fStrip;     // Front or back of Det hit by event
    /*
    Double_t fStripfrt; // front strip hit in detector fdetector
    Double_t fStripbck; // back strip hit in detector fdetector
    */
    ClassDef(R3BStartrackerDigitHit, 1)
};

#endif
