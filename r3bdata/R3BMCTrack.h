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

// -------------------------------------------------------------------------
// -----                      R3BMCTrack header file                   -----
// -----                  Created 03/08/04                             -----
// -------------------------------------------------------------------------

/** R3BMCTrack.h
 **/

#ifndef R3BMCTRACK_H
#define R3BMCTRACK_H 1

#include "R3BDetectorList.h"

#include "TLorentzVector.h"
#include "TObject.h"
#include "TParticle.h"
#include "TVector3.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

#include <array>

class R3BMCTrack : public TObject
{

  public:
    /**  Default constructor  **/
    R3BMCTrack();

    /**  Standard constructor  **/
    R3BMCTrack(Int_t pdgCode,
               Int_t motherID,
               Double_t px,
               Double_t py,
               Double_t pz,
               Double_t x,
               Double_t y,
               Double_t z,
               Double_t t,
               std::array<int, kLAST + 1> nPoints);

    /**  Copy constructor  **/
    R3BMCTrack(const R3BMCTrack&);

    /**  Constructor from TParticle  **/
    R3BMCTrack(TParticle* particle, Int_t fMC);

    /**  Destructor  **/
    virtual ~R3BMCTrack();

    R3BMCTrack& operator=(const R3BMCTrack&) { return *this; }

    /**  Output to screen  **/
    virtual void Print(Option_t* option = "") const;

    /**  Accessors  **/
    Int_t GetPdgCode() const { return fPdgCode; }
    Int_t GetMotherId() const { return fMotherId; }
    Double_t GetPx() const { return fPx; }
    Double_t GetPy() const { return fPy; }
    Double_t GetPz() const { return fPz; }
    Double_t GetStartX() const { return fStartX; }
    Double_t GetStartY() const { return fStartY; }
    Double_t GetStartZ() const { return fStartZ; }
    Double_t GetStartT() const { return fStartT; }
    Double_t GetMass() const;
    Double_t GetEnergy() const;
    Double_t GetPt() const { return TMath::Sqrt(fPx * fPx + fPy * fPy); }
    Double_t GetP() const { return TMath::Sqrt(fPx * fPx + fPy * fPy + fPz * fPz); }
    Double_t GetRapidity() const;
    void GetMomentum(TVector3& momentum);
    void Get4Momentum(TLorentzVector& momentum);
    void GetStartVertex(TVector3& vertex);

    /** Accessors to the number of MCPoints in the detectors **/
    Int_t GetNPoints(DetectorId detId) const;

    /**  Modifiers  **/
    void SetMotherId(Int_t id) { fMotherId = id; }
    void SetNPoints(Int_t iDet, Int_t np);
    void SetNPoints(std::array<int, kLAST + 1> nPoints) { fNPoints = nPoints; }

  private:
    /**  PDG particle code  **/
    Int_t fPdgCode;

    /**  Index of mother track. -1 for primary particles.  **/
    Int_t fMotherId;

    /** Momentum components at start vertex [GeV]  **/
    Double32_t fPx, fPy, fPz;

    /** Coordinates of start vertex [cm, ns]  **/
    Double32_t fStartX, fStartY, fStartZ, fStartT;

    /**  Bitvector representing the number of MCPoints for this track in
     **  each subdetector. The detectors are represented by
     **  REF:         Bit  0      (1 bit,  max. value  1)
     **  The respective point numbers can be accessed and modified
     **  with the inline functions.
     **  Bits 26-31 are spare for potential additional detectors.
     **/
    std::array<int, kLAST + 1> fNPoints;

    // case of HIons
    Double32_t fMass;

    ClassDef(R3BMCTrack, 1);
};

// ==========   Inline functions   ========================================

inline Double_t R3BMCTrack::GetEnergy() const
{
    Double_t mass = GetMass();
    return TMath::Sqrt(mass * mass + fPx * fPx + fPy * fPy + fPz * fPz);
}

inline void R3BMCTrack::GetMomentum(TVector3& momentum) { momentum.SetXYZ(fPx, fPy, fPz); }

inline void R3BMCTrack::Get4Momentum(TLorentzVector& momentum) { momentum.SetXYZT(fPx, fPy, fPz, GetEnergy()); }

inline void R3BMCTrack::GetStartVertex(TVector3& vertex) { vertex.SetXYZ(fStartX, fStartY, fStartZ); }

#endif
