// -------------------------------------------------------------------------
// -----                        R3BdTof header file                    -----
// -----                  Created 18/05/15  by D. Kresan               -----
// -------------------------------------------------------------------------

/**  R3BdTof.h
 **/

#ifndef R3BDTOF_H
#define R3BDTOF_H

#include "R3BDetector.h"

#include "TLorentzVector.h"

class TClonesArray;
class R3BdTofPoint;
class FairVolume;

class R3BdTof : public R3BDetector
{

  public:
    /** Default constructor **/
    R3BdTof();

    /** Standard constructor.
     *@param name    detetcor name
     *@param active  sensitivity flag
     **/
    R3BdTof(const char* name, Bool_t active);

    /** Destructor **/
    virtual ~R3BdTof();

    /** Virtual method ProcessHits
     **
     ** Defines the action to be taken when a step is inside the
     ** active volume. Creates a R3BdTofPoint and adds it to the
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

    /** Virtaul method Construct geometry
     **
     ** Constructs the STS geometry
     **/
    virtual void ConstructGeometry();
    virtual Bool_t CheckIfSensitive(std::string name);
    virtual void Initialize();
    virtual void SetSpecialPhysicsCuts();

    //  void SaveGeoParams();

  private:
    /** Track information to be stored until the track leaves the
    active volume. **/
    Int_t fTrackID;                 //!  track index
    Int_t fVolumeID;                //!  volume id
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
    TClonesArray* fdTofCollection; //!  The hit collection
    Bool_t kGeoSaved;              //!
    TList* flGeoPar;               //!

    /** Private method AddHit
     **
     ** Adds a mTofPoint to the HitCollection
     **/
    R3BdTofPoint*
        AddHit(Int_t trackID, Int_t detID, TVector3 posIn, TVector3 pos_out, TVector3 momIn, TVector3 momOut, Double_t time, Double_t length, Double_t eLoss);

    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();

    ClassDef(R3BdTof, 1);
};

inline void R3BdTof::ResetParameters()
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