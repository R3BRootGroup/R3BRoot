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

// ---------------------------------------------------------------
// -----                        R3BTofD                      -----
// -----          Created 18/03/2022 by J.L. Rodriguez       -----
// ---------------------------------------------------------------

#ifndef R3BTofD_H
#define R3BTofD_H 1

#include "R3BDetector.h"
#include "R3BDetectorList.h"
#include "TLorentzVector.h"

class TClonesArray;
class R3BTofdPoint;
class FairVolume;

class R3BTofD : public R3BDetector
{
  public:
    /** Default constructor **/
    R3BTofD();

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param trans   position
     *@param rot     rotation
     */
    R3BTofD(const TString& geoFile,
            DetectorId type = kTOFD1,
            const TGeoTranslation& trans = TGeoTranslation(),
            const TGeoRotation& rot = TGeoRotation(),
            const TString& namedetId = "NULL");

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param combi   position + rotation
     */
    R3BTofD(const TString& geoFile,
            DetectorId type = kTOFD1,
            const TGeoCombiTrans& combi = TGeoCombiTrans(),
            const TString& namedetId = "NULL");

    /** Destructor **/
    virtual ~R3BTofD();

    /** Virtual method ProcessHits
     **
     ** Defines the action to be taken when a step is inside the
     ** active volume. Creates a R3BTofdPoint and adds it to the
     ** collection.
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
    virtual void SetSpecialPhysicsCuts();

  private:
    /** Track information to be stored until the track leaves the
    active volume. **/
    Int_t fTrackID;  //!  track index
    Int_t fPlaneID;  //!  Plane id
    Int_t fPaddleID; //!  Paddle id
    DetectorId fDetId;
    TString fNameDetId;
    TLorentzVector fPosIn, fPosOut; //!  position
    TLorentzVector fMomIn, fMomOut; //!  momentum
    Double32_t fTime_in;            //!  time when entering active volume
    Double32_t fTime_out;           //!  time when exiting active volume
    Double32_t fTime;               //!  time average in detector
    Double32_t fLength_in;          //!  length when entering active volume
    Double32_t fLength_out;         //!  length when exiting active volume
    Double32_t fLength;             //!  length average in detector
    Double32_t fELoss;              //!  energy loss

    Int_t fPosIndex;               //!
    TClonesArray* fTofdCollection; //!  The hit collection

    /** Private method AddHit
     **
     ** Adds a TofdPoint to the HitCollection
     **/
    R3BTofdPoint* AddHit(Int_t trackID,
                         Int_t detID,
                         Int_t planeID,
                         Int_t paddleID,
                         TVector3 posIn,
                         TVector3 pos_out,
                         TVector3 momIn,
                         TVector3 momOut,
                         Double_t time,
                         Double_t length,
                         Double_t eLoss,
                         Double_t Z,
                         Double_t A);

    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();

    ClassDef(R3BTofD, 1);
};

inline void R3BTofD::ResetParameters()
{
    fTrackID = fPlaneID = fPaddleID = 0;
    fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fTime = fLength = fELoss = 0;
    fPosIndex = 0;
};

#endif /* R3BTofD_H */
