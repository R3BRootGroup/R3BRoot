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

#ifndef R3BMusic_H
#define R3BMusic_H 1

#include "R3BDetector.h"
#include "TLorentzVector.h"
#include <map>

class TClonesArray;
class R3BMusicPoint;
class FairVolume;
class TGeoRotation;

class R3BMusic : public R3BDetector
{
  public:
    /** Default constructor */
    R3BMusic();

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param trans   position
     *@param rot     rotation
     */
    R3BMusic(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param combi   position + rotation
     */
    R3BMusic(const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    /** Destructor **/
    ~R3BMusic();

    /** Virtual method ProcessHits
     **
     ** Defines the action to be taken when a step is inside the
     ** active volume. Creates a R3BMusicPoint and adds it
     ** to the collection.
     *@param vol  Pointer to the active volume
     **/
    virtual Bool_t ProcessHits(FairVolume* vol = 0);

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
    virtual void Print(Option_t* option = "") const;

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

    virtual Bool_t CheckIfSensitive(std::string name);

    virtual void Initialize();
    virtual void SetSpecialPhysicsCuts() {}

  private:
    /** Track information to be stored until the track leaves the
        active volume. **/
    Int_t fTrackID;                 //!  track index
    Int_t fTrackPID;                //!  particle identification
    Int_t fVolumeID;                //!  volume id
    Int_t fDetCopyID;               //!  Det volume id
    Double_t fZ;                    //!  atomic number fragment
    Double_t fA;                    //!  mass number fragment
    Int_t fParentTrackID;           //!  parent track index
    Int_t fUniqueID;                //!  particle unique id (e.g. if Delta electron, fUniqueID=9)
    TLorentzVector fPosIn, fPosOut; //!  position
    TLorentzVector fMomIn, fMomOut; //!  momentum
    Double32_t fTime;               //!  time
    Double32_t fLength;             //!  length
    Double32_t fELoss;              //!  energy loss
    Int_t fPosIndex;                //!
    Int_t fNSteps;                  //!  Number of steps in the active volume
    Double32_t fEinc;               //!  Total incident energy

    TClonesArray* fMusicCollection; //!  The point collection

    /** Private method AddHit
     **
     ** Adds a MusicPoint to the HitCollection
     **/
    R3BMusicPoint* AddPoint(Int_t trackID,
                            Int_t detID,
                            Int_t volid,
                            Double_t Z,
                            Double_t A,
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

    ClassDef(R3BMusic, 1);
};

inline void R3BMusic::ResetParameters()
{
    fTrackID = fVolumeID = fParentTrackID = fTrackPID = fUniqueID = 0;
    fDetCopyID = 0;
    fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fTime = fLength = fELoss = fEinc = fZ = fA = 0;
    fPosIndex = 0;
    fNSteps = 0;
};

#endif
