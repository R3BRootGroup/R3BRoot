// -------------------------------------------------------------------------
// -----                        R3BSTaRTra header file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------

/**  R3BSTaRTra.h
 **/


#ifndef R3BSTARTRA_H
#define R3BSTARTRA_H

#include "R3BDetector.h"

#include "TLorentzVector.h"

class TClonesArray;
class R3BSTaRTraPoint;
class FairVolume;



class R3BSTaRTra : public R3BDetector
{

 public:

  /** Default constructor **/
  R3BSTaRTra();

  /** Standard constructor.
     *@param name    detetcor name
     *@param active  sensitivity flag
  **/
  R3BSTaRTra(const char* name, Bool_t active);

  /** Standard constructor.
   *@param name    detetcor name
   *@param geoFile name of the ROOT geometry file
   *@param active  sensitivity flag
   *@param x       position in cave
   *@param y       position in cave
   *@param z       position in cave
   *@param rot_x   rotation in cave
   *@param rot_y   rotation in cave
   *@param rot_z   rotation in cave
   **/
  R3BSTaRTra(const char* name,
             TString geoFile,
             Bool_t active,
             Float_t x = 0.,
             Float_t y = 0.,
             Float_t z = 0.,
             Float_t rot_x = 0.,
             Float_t rot_y = 0.,
             Float_t rot_z = 0.);

  /** Destructor **/
  virtual ~R3BSTaRTra();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates a R3BTraPoint and adds it to the
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
  virtual void Print(Option_t *option="") const;


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
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2,
			  Int_t offset);


  /** Virtaul method Construct geometry
   **
   ** Constructs the STS geometry
   **/
  virtual void ConstructGeometry();
  virtual Bool_t CheckIfSensitive(std::string name);
  virtual void Initialize();
  virtual void SetSpecialPhysicsCuts() {}

//  void SaveGeoParams();

  private:


    /** Track information to be stored until the track leaves the
	active volume. **/
    Int_t          fTrackID;           //!  track index
    Int_t          fVolumeID;          //!  volume id
    Int_t          fDetCopyID;       //!  Det volume id  // added by Marc
    Int_t          fsector;            //!  volume id
    TLorentzVector fPosIn, fPosOut;    //!  position
    TLorentzVector fMomIn, fMomOut;    //!  momentum
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss
    Int_t          fPosIndex;          //!
    TClonesArray*  fTraCollection;     //!  The hit collection
    Bool_t         kGeoSaved;          //!
    TList *flGeoPar; //!

    /** Private method AddHit
     **
     ** Adds a TraPoint to the HitCollection
     **/
    R3BSTaRTraPoint* AddHit(Int_t trackID, Int_t detID, Int_t detCopyID,    // Int_t detCopyID added by Marc
			TVector3 posIn,
			TVector3 pos_out, TVector3 momIn, 
			TVector3 momOut, Double_t time, 
			Double_t length, Double_t eLoss);


    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();


    ClassDef(R3BSTaRTra,2);

};


inline void R3BSTaRTra::ResetParameters() {
  fTrackID = fVolumeID = 0; fDetCopyID = 0;   // fDetCopyID added by Marc
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif 
