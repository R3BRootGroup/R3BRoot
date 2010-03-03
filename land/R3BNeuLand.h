// -------------------------------------------------------------------------
// -----                        R3BNeuLand header file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------

/**  R3BNeuLand.h
 **/


#ifndef R3BNEULAND_H
#define R3BNEULAND_H

#include "R3BDetector.h"
#include "TLorentzVector.h"
#include <iostream>
#include <map>
#include <TGeoMedium.h>

using namespace std;

class TClonesArray;
class R3BNeuLandPoint;
class FairVolume;



class R3BNeuLand : public R3BDetector
{

 public:

  /** Default constructor **/
  R3BNeuLand();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  R3BNeuLand(const char* name, Bool_t active);


  /** Destructor **/
  virtual ~R3BNeuLand();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates a R3BNeuNeuLandPoint and adds it to the
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
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2,
			  Int_t offset);


  /** Virtaul method Construct geometry
   **
   ** Constructs the STS geometry
   **/
  virtual void ConstructGeometry();
  virtual void ConstructGeometry1();
  virtual void ConstructGeometry2();
  virtual void ConstructGeometry3();
  virtual void Initialize();
  virtual void SetSpecialPhysicsCuts(){;}
  void SetGeomVersion(Int_t vers ) { fVersion = vers; }
 

  //  void SaveGeoParams();

  private:

    /** Track information to be stored until the track leaves the
	active volume. **/
    Int_t          fTrackID;           //!  track index
    Int_t          fMot0TrackID;       //!  0th mother track index
    Int_t          fMot1TrackID;       //!  1st mother track index
    Int_t          fMot2TrackID;       //!  2nd mother track index
    Int_t          fMot3TrackID;       //!  3rd mother track index
    Int_t          fDetID;             //!  detector id
    Int_t          fSegID;             //!  detector segment id
    Int_t          fCellID;            //!  detector cell id
    TLorentzVector fPosIn, fPosOut;    //!  position
    TLorentzVector fMomIn, fMomOut;    //!  momentum
    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss

    Int_t          fPosIndex;          //!
    TClonesArray*  fLandCollection;    //!  The hit collection
    Bool_t         kGeoSaved;          //!
    TList *flGeoPar; //!
    Int_t fVersion;                    //! geometry version
  
    
    /** Private method AddHit
     **
     ** Adds a NeuLandPoint to the HitCollection
     **/
    R3BNeuLandPoint* AddHit(Int_t trackID,
			    Int_t mot0trackID, Int_t mot1trackID, Int_t mot2trackID, Int_t mot3trackID,
			    Int_t detID, Int_t segID, Int_t cellID,
			    TVector3 posIn,
			    TVector3 pos_out, TVector3 momIn,
			    TVector3 momOut, Double_t time,
			    Double_t length, Double_t eLoss);


    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();

    /** Map of MCis with fixed VolID */
    map <Int_t,Int_t> fMapMcId;


    Int_t fIdMedFe;
    Int_t fIdMedGlass;
    Int_t fIdMedGas;
 
   TGeoMedium * pMedFe;
   TGeoMedium* pMed_glas;
   TGeoMedium* pMed_gas;

   // veriosn 4
   Int_t fIDMedSteel;
   Int_t fIDMedGlass;
   Int_t fIDMedGas;

   TGeoMedium* pMedAir;
   TGeoMedium* pMedSteel;
   TGeoMedium* pMedGlass;
   TGeoMedium* pMedBak;
   TGeoMedium* pMedGas;

   


    ClassDef(R3BNeuLand,1);

};


inline void R3BNeuLand::ResetParameters() {
  fTrackID = fDetID = fSegID = fCellID = -1;
  fMot0TrackID = fMot1TrackID = fMot2TrackID = fMot3TrackID = -1;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = 0;
  fPosIndex = 0;
};


#endif 
