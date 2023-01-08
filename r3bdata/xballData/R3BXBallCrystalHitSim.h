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
// -----                R3BXBallCrystalHitSim header file                -----
// -----               Created 02/07/12  by Pablo Cabanelas            -----
// -------------------------------------------------------------------------

/**  R3BXBallCrystalHitSim.h
 **  A cal crystal hit is the representation of the
 **  information obtained from each crystal in the DH-CrystalBall.
 **/

#ifndef R3BXBALLCRYSTALHITSIM_H
#define R3BXBALLCRYSTALHITSIM_H

#include "FairMultiLinkedData.h"
#include "R3BXBallCrystalHit.h"
#include "TObject.h"

class R3BXBallCrystalHitSim : public R3BXBallCrystalHit
{
  public:
    /** Default constructor **/
    R3BXBallCrystalHitSim();

    /** Constructor with arguments
     *@param fCrystalType    Crystal type
     *@param fNSteps         Steps inside an active volume
     *@param fEinc           total energy entering the crystal [GeV]
     *@param fTrackID        Track index
     *@param fVolumeID       Volume index
     *@param fParentTrackID  Parent Track index
     *@param fTrackPID       Particle identification
     *@param fTrackUniqueID  Particle unique id (e.g. if Delta electron, fTrackUniqueID=9)
     **/
    R3BXBallCrystalHitSim(Int_t type,
                          Int_t crysnb,
                          Double_t energy,
                          Double_t time,
                          Int_t steps,
                          Double_t einc,
                          Int_t trackid,
                          Int_t volid,
                          Int_t partrackid,
                          Int_t pdgid,
                          Int_t uniqueid);

    /** Copy constructor **/
    R3BXBallCrystalHitSim(const R3BXBallCrystalHitSim& hit) { *this = hit; };

    /** Destructor **/
    virtual ~R3BXBallCrystalHitSim();

    /** Accessors **/
    Int_t GetCrystalType() const { return fCrystalType; }
    Int_t GetNSteps() const { return fNSteps; }
    Double_t GetEinc() const { return fEinc; }
    Int_t GetTrackId() const { return fTrackID; }
    Int_t GetVolumeId() const { return fVolumeID; }
    Int_t GetParentTrackId() const { return fParentTrackID; }
    Int_t GetTrackPID() const { return fTrackPID; }
    Int_t GetTrackUniqueID() const { return fTrackUniqueID; }

    /** Modifiers **/
    void SetCrystalType(Int_t type) { fCrystalType = type; }
    void AddMoreEnergy(Double32_t moreEnergy) { fEnergy += moreEnergy; }
    void SetNSteps(Int_t steps) { fNSteps = steps; }
    void SetEinc(Double32_t einc) { fEinc = einc; }
    void SetTrackId(Int_t trackid) { fTrackID = trackid; }
    void SetVolumeId(Int_t volid) { fVolumeID = volid; }
    void SetParentTrackId(Int_t partrackid) { fParentTrackID = partrackid; }
    void SetTrackPID(Int_t pdgid) { fTrackPID = pdgid; }
    void SetTrackUniqueID(Int_t uniqueid) { fTrackUniqueID = uniqueid; }

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

  protected:
    Int_t fCrystalType;   // crystal type (1-5, the four kind of crystal shapes in the CB
    Int_t fNSteps;        // steps inside an active volume
    Double32_t fEinc;     // total energy entering the crystal
    Int_t fTrackID;       // track index
    Int_t fVolumeID;      // volume index
    Int_t fParentTrackID; // parent track index
    Int_t fTrackPID;      // particle identification
    Int_t fTrackUniqueID; // particle unique id

    ClassDef(R3BXBallCrystalHitSim, 1)
};

#endif
