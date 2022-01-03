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

// ---------------------------------------------------------------
// -----                        R3BAlpide                    -----
// -----          Created 11/10/2021 by J.L. Rodriguez       -----
// ---------------------------------------------------------------

#ifndef R3BAlpide_H
#define R3BAlpide_H

#include "R3BDetector.h"
#include "Rtypes.h"
#include "TLorentzVector.h"

class TClonesArray;
class R3BAlpidePoint;
class FairVolume;

class R3BAlpide : public R3BDetector
{
  public:
    /** Default constructor **/
    R3BAlpide();

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param trans   position
     *@param rot     rotation
     */
    R3BAlpide(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot = TGeoRotation());

    /** Standard constructor.
     *@param geoFile name of the ROOT geometry file
     *@param combi   position + rotation
     */
    R3BAlpide(const TString& geoFile, const TGeoCombiTrans& combi = TGeoCombiTrans());

    /** Destructor **/
    virtual ~R3BAlpide();

    /** Virtual method ProcessHits
     **
     ** Defines the action to be taken when a step is inside the
     ** active volume. Creates a R3BAlpidePoint and adds it to the
     ** collection.
     *@param vol  Pointer to the active volume
     **/
    virtual Bool_t ProcessHits(FairVolume* vol = 0) override;

    virtual Bool_t CheckIfSensitive(std::string name) override;

    /** Virtual method EndOfEvent
     **
     ** If verbosity level is set, print hit collection at the
     ** end of the event and resets it afterwards.
     **/

    virtual void EndOfEvent() override;

    /** Virtual method Register
     **
     ** Registers the hit collection in the ROOT manager.
     **/
    virtual void Register() override;

    /** Accessor to the hit collection **/
    virtual TClonesArray* GetCollection(Int_t iColl) const override;

    /** Virtual method Print
     **
     ** Screen output of hit collection.
     **/
    virtual void Print(Option_t* option = "") const override;

    /** Virtual method Reset
     **
     ** Clears the hit collection
     **/
    virtual void Reset() override;

    /** Virtual method CopyClones
     **
     ** Copies the hit collection with a given track index offset
     *@param cl1     Origin
     *@param cl2     Target
     *@param offset  Index offset
     **/
    virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) override;

    virtual void Initialize() override;

    virtual void SetSpecialPhysicsCuts() override;

  private:
    /** Track information to be stored until the track leaves the
    active volume. **/
    Int_t fTrackID;                 //  track index
    Int_t fVolumeID;                //  volume id
    Int_t fDetCopyID;               //  Det volume id
    Int_t fsector;                  //  volume id
    TLorentzVector fPosIn, fPosOut; //  position
    TLorentzVector fMomIn, fMomOut; //  momentum
    Double32_t fTime_in;            //  time when entering active volume
    Double32_t fTime_out;           //  time when exiting active volume
    Double32_t fTime;               //  time
    Double32_t fLength_in;          //  length when entering active volume
    Double32_t fLength_out;         //  length when exiting active volume
    Double32_t fLength;             //  length
    Double32_t fELoss;              //  energy loss
    Int_t fPosIndex;                //
    TClonesArray* fAlpidePoint;     //  The hit collection

    /** Private method AddHit
     **
     ** Adds a TraPoint to the HitCollection
     **/
    R3BAlpidePoint* AddHit(Int_t trackID,
                           Int_t detID,
                           Int_t detCopyID,
                           TVector3 posIn,
                           TVector3 pos_out,
                           TVector3 momIn,
                           TVector3 momOut,
                           Double_t time,
                           Double_t length,
                           Double_t eLoss,
                           Int_t pdgcode);

    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();

  public:
    ClassDefOverride(R3BAlpide, 0);
};

inline void R3BAlpide::ResetParameters()
{
    fTrackID = fVolumeID = 0;
    fDetCopyID = 0; // fDetCopyID added by Marc
    fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
    fTime = fLength = fELoss = 0;
    fPosIndex = 0;
};

#endif /* R3BAlpide_H */
