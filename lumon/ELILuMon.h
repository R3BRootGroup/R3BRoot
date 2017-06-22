// -------------------------------------------------------------------------
// -----                        ELILuMon header file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------

/**  ELILuMon.h
 **/


#ifndef ELITOF_H
#define ELITOF_H

#include "R3BDetector.h"

#include "TLorentzVector.h"

class TClonesArray;
class ELILuMonPoint;
class FairVolume;



class ELILuMon : public R3BDetector
{

 public:

  /** Default constructor **/
  ELILuMon();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  ELILuMon(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~ELILuMon();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates a ELILuMonPoint and adds it to the
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
    TLorentzVector fPosIn, fPosOut;    //!  position
    TLorentzVector fMomIn, fMomOut;    //!  momentum
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss

    Int_t          fPosIndex;          //!
    TClonesArray*  fLuMonCollection;     //!  The hit collection
    Bool_t         kGeoSaved;          //!
    TList *flGeoPar; //!

    /** Private method AddHit
     **
     ** Adds a LuMonPoint to the HitCollection
     **/
    ELILuMonPoint* AddHit(Int_t trackID, Int_t detID, Int_t copy, TVector3 posIn,
			TVector3 pos_out, TVector3 momIn, 
			TVector3 momOut, Double_t time, 
			Double_t length, Double_t eLoss);


    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();


    ClassDef(ELILuMon,2);

};


inline void ELILuMon::ResetParameters() {
  fTrackID = fVolumeID = 0;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif 
