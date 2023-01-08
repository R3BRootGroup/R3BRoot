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
// -----                        R3BXBall header file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------

/**  R3BXBallv0.h
 **/

#ifndef R3BXBALLV0_H
#define R3BXBALLV0_H

#include "R3BDetector.h"

#include "TLorentzVector.h"

class TClonesArray;
class R3BXBallPoint;
class FairVolume;
class TGeoRotation;

class R3BXBallv0 : public R3BDetector
{

  public:
    /** Default constructor **/
    R3BXBallv0();

    /** Standard constructor.
     *@param name    detetcor name
     *@param active  sensitivity flag
     **/
    R3BXBallv0(const char* name, Bool_t active);

    /** Destructor **/
    virtual ~R3BXBallv0();

    /** Virtual method ProcessHits
     **
     ** Defines the action to be taken when a step is inside the
     ** active volume. Creates a R3BXBallPoint and adds it to the
     ** collection.
     *@param vol  Pointer to the active volume
     **/
    virtual Bool_t ProcessHits(FairVolume* vol = 0);

    /** Virtual method BeginEvent
     **
     ** If verbosity level is set, print hit collection at the
     ** end of the event and resets it afterwards.
     **/

    virtual void BeginEvent();

    /** Virtual method EndOfEvent
     **
     ** If verbosity level is set, print hit collection at the
     ** end of the event and resets it afterwards.
     **/

    virtual void EndOfEvent();

    /** Virtual method Register
     **
     ** Registers the hit collection in the ROOT manager.
     **/
    virtual void Register();

    /** Accessor to the hit collection **/
    virtual TClonesArray* GetCollection(Int_t iColl) const;

    /** Virtual method Print
     **
     ** Screen output of hit collection.
     **/
    virtual void Print() const;

    /** Virtual method Reset
     **
     ** Clears the hit collection
     **/
    virtual void Reset();

    /** Virtual method CopyClones
     **
     ** Copies the hit collection with a given track index offset
     *@param cl1     Origin
     *@param cl2     Target
     *@param offset  Index offset
     **/
    virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset);

    /** Virtaul method Construct geometry
     **
     ** Constructs the CAL CRYSTAL geometry
     **/
    virtual void ConstructGeometry();
    virtual void ConstructGeometry1();
    virtual void ConstructGeometry2();
    void SetGeometryVersion(Int_t vers) { fGeoVersion = vers; }

    virtual void Initialize();
    virtual void SetSpecialPhysicsCuts() {}

  private:
    /** Track information to be stored until the track leaves the
    active volume. **/
    Int_t fTrackID;                 //!  track index
    Int_t fVolumeID;                //!  volume id
    TLorentzVector fPosIn, fPosOut; //!  position
    TLorentzVector fMomIn, fMomOut; //!  momentum
    Double32_t fTime;               //!  time
    Double32_t fLength;             //!  length
    Double32_t fELoss;              //!  energy loss

    Int_t fPosIndex;                //!
    TClonesArray* fXBallCollection; //!  The hit collection
    Bool_t kGeoSaved;               //!
    TList* flGeoPar;                //!
    Int_t fGeoVersion;              //!

    Int_t fTypeA; //!
    Int_t fTypeB; //!
    Int_t fTypeC; //!
    Int_t fTypeD; //!

    /** Private method AddHit
     **
     ** Adds a XBallPoint to the HitCollection
     **/
    R3BXBallPoint* AddHit(Int_t trackID,
                          Int_t detID,
                          Int_t type,
                          Int_t cp,
                          TVector3 posIn,
                          TVector3 pos_out,
                          TVector3 momIn,
                          TVector3 momOut,
                          Double_t time,
                          Double_t length,
                          Double_t eLoss);

    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();
    TGeoRotation* createMatrix(Double_t phi, Double_t theta, Double_t psi);

    Int_t GetCrystalType(Int_t volID);

    ClassDef(R3BXBallv0, 1);
};

inline Int_t R3BXBallv0::GetCrystalType(Int_t volID)
{
    Int_t type = -1;

    if (volID == fTypeA)
    {
        type = 1;
        return (type);
    }
    if (volID == fTypeB)
    {
        type = 2;
        return (type);
    }
    if (volID == fTypeC)
    {
        type = 3;
        return (type);
    }
    if (volID == fTypeD)
    {
        type = 4;
        return (type);
    }

    return type;
}

inline void R3BXBallv0::ResetParameters()
{
    fTrackID = fVolumeID = 0;
    fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fTime = fLength = fELoss = 0;
    fPosIndex = 0;
};

#endif
