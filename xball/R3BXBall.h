// -------------------------------------------------------------------------
// -----                        R3BXBall header file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -----        new Version: 08/04/10 <wranne@student.chalmers.se>     -----
// -----        new Version: 08/06/12 <pablo.cabanelas@usc.es>         -----
// -------------------------------------------------------------------------

/**  R3BXBall.h
 **/

#ifndef R3BXBALL_H
#define R3BXBALL_H

#include <string>

#include "R3BDetector.h"
#include "TGeoSphere.h"

#include "TLorentzVector.h"

class TClonesArray;
class R3BXBallPoint;
class R3BXBallCrystalHit;
class R3BXBallCrystalHitSim;
class FairVolume;
class TGeoRotation;

struct xb_wrapping
{
  int no;
  int type;
  double theta, phi, psi;
  bool active;
};

struct xb_wrapping_mod
{
  int no;
  int mod;
  double delta;
};

struct xb_crystal
{
  int no;
  int type;
  double theta, phi, psi;
  bool active;
};


class R3BXBall : public R3BDetector
{

 public:

  /** Default constructor **/
  R3BXBall();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  R3BXBall(const char* name, Bool_t active);


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
  R3BXBall(const char* name,
           TString geoFile,
           Bool_t active,
           Float_t x = 0.,
           Float_t y = 0.,
           Float_t z = 0.,
           Float_t rot_x = 0.,
           Float_t rot_y = 0.,
           Float_t rot_z = 0.);


  /** Destructor **/
  virtual ~R3BXBall();


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
  virtual void Print(Option_t* option="") const;


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


  /** Virtual method Construct geometry
   **
   ** Constructs the CAL CRYSTAL geometry
   **/
  virtual void ConstructGeometry();

  virtual Bool_t CheckIfSensitive(std::string name);

  virtual void Initialize();
  virtual void SetSpecialPhysicsCuts();


  /** Public method DrawCrystals
   **
   ** Selects whether to draw crystals or wrappings in the R3BRoot evtviewer
   **/
  void DrawCrystals( Bool_t drawCrystals ) {kDrawCrystals = drawCrystals;}


  /** Public method SelectCollectionOption
   **
   ** Selects the TObjectArray collection to be stored
   ** Syntaxis:
   **     fCollectionOption = 0  -- CrystalPoint only
   **     fCollectionOption = 1  -- CrystalHit only
   **     fCollectionOption = 2  -- Both
   ** By default, only CrystalPoint collection is stored
   **/
  void SelectCollectionOption(Int_t option) {fCollectionOption = option;}

  /** Public method SetNonUniformity
  **
  ** Defines the fNonUniformity parameter in % deviation from the central value 
  *@param nonU  Double parameter setting the maximum non-uniformity allowed 
  **/	
  void SetNonUniformity(Double_t nonU);


  private:

    /** Track information to be stored until the track leaves the
  active volume. **/

    Int_t          fTrackID;           //!  track index
    Int_t          fTrackPID;          //!  particle identification
    Int_t          fVolumeID;          //!  volume id
    Int_t          fParentTrackID;     //!  parent track index
    Int_t          fUniqueID;          //!  particle unique id (e.g. if Delta electron, fUniqueID=9)

    TLorentzVector fPosIn, fPosOut;    //!  position
    TLorentzVector fMomIn, fMomOut;    //!  momentum

    Double32_t     fTime;              //!  time
    Double32_t     fLength;            //!  length
    Double32_t     fELoss;             //!  energy loss
    Double32_t     fEinc;              //!  Total incident energy
    Int_t          fNSteps;            //!  Number of steps in the active volume

    Int_t          fPosIndex;          //!
    Bool_t         kGeoSaved;          //!
    TList*         flGeoPar;           //!
    Int_t          fGeoVersion;        //!
    Bool_t         kDrawCrystals;      //! flag indicating whether draw either crystals or wrappings
    Int_t          fCollectionOption;  //! object collection option
    Double_t       fNonUniformity;     //! Adding some non-uniformity preliminary description


    Int_t fTypeA;                      //!
    Int_t fTypeB;                      //!
    Int_t fTypeC;                      //!
    Int_t fTypeD;                      //!
    
    TClonesArray*  fXBallCollection;               //!  The hit collection
    TClonesArray*  fXBallCrystalHitCollection;     //!  The CB crystal hit collection

    /** Private method AddHit
     **
     ** Adds a XBallPoint to the HitCollection
     **/
    R3BXBallPoint* AddHit(Int_t trackID, Int_t detID, Int_t type, Int_t cp, TVector3 posIn,
      TVector3 pos_out, TVector3 momIn, 
      TVector3 momOut, Double_t time, 
      Double_t length, Double_t eLoss);

    /** Private method AddCrystalHit
     **
     ** Adds a XBallCrystalhit to the HitCollection
     **/
    R3BXBallCrystalHitSim* AddCrystalHit(Int_t type, Int_t copy,
	Double_t energy, Double_t tof,
	Int_t steps, Double_t einc,
	Int_t trackid, Int_t volid, Int_t partrackid,
	Int_t pdgid, Int_t uniqueid);
	
    /** Private method NUSmearing
     **
     ** Smears the energy according to some non-uniformity distribution	
     ** Very simple preliminary scheme where the NU is introduced as a flat random
     ** distribution with limits fNonUniformity (%) of the energy value.
     **/
    Double_t NUSmearing(Double_t inputEnergy);
	

    /** Private method ResetParameters
     **
     ** Resets the private members for the track parameters
     **/
    void ResetParameters();

    Int_t  GetCrystalType(Int_t volID);


    ClassDef(R3BXBall,2);

};

inline Int_t R3BXBall::GetCrystalType(Int_t volID) {
Int_t type=-1;

if (volID==fTypeA) { type=1;return (type);}
if (volID==fTypeB) { type=2;return (type);}
if (volID==fTypeC) { type=3;return (type);}
if (volID==fTypeD) { type=4;return (type);}

return type;
}


inline void R3BXBall::ResetParameters() {
  fTrackID = fVolumeID = fParentTrackID = fTrackPID = fUniqueID = 0;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = fEinc = 0;
  fPosIndex = 0;
  fNSteps = 0;
};


#endif 
