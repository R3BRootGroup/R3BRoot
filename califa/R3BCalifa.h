// -------------------------------------------------------------------------
// -----                      R3BCalifa header file                    -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -----	    Last modification 15/12/16 by P.Cabanelas          -----
// -------------------------------------------------------------------------

/**  R3BCalifa.h
 **/


#ifndef R3BCALIFA_H
#define R3BCALIFA_H

#include <map>

#include "R3BDetector.h"
#include "TF1.h"
#include "TLorentzVector.h"

class TClonesArray;
class R3BCalifaPoint;
class R3BCalifaCrystalCalData;
class R3BCalifaCrystalCalDataSim;
class FairVolume;
class TGeoRotation;


class R3BCalifa : public R3BDetector
{

 public:

  struct sCrystalInfo
  {
    Int_t   crystalType;
    Int_t   crystalCopy;
    Int_t   crystalId;
    Int_t   fEndcapIdentifier;
    Int_t   fPhoswichIdentifier;

    Int_t   volIdAlv;
    Int_t   cpAlv;
    Int_t   cpCry;

    Double_t density;
  };

  /** Default constructor **/
  R3BCalifa();


  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  R3BCalifa(const char* name, Bool_t active);

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
  R3BCalifa(const char* name,
          TString geoFile,
          Bool_t active,
          Float_t x = 0.,
          Float_t y = 0.,
          Float_t z = 0.,
          Float_t rot_x = 0.,
          Float_t rot_y = 0.,
          Float_t rot_z = 0.);

  /** Destructor **/
  virtual ~R3BCalifa();


  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates a R3BCalifaPoint and adds it 
   ** to the collection.
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
   ** Added support for R3BCalifaCrystalCalData
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
   ** Constructs the STS geometry
   **/
  virtual void ConstructGeometry();
	

  virtual Bool_t CheckIfSensitive(std::string name);
  
  /** Select the version of the CALIFA geometry
   **
   *@param version Geometry version (see documentation /r3broot/cal/perlScripts/README)
   **/	
  void SelectGeometryVersion(Int_t version);
  
  
  /** Public method SetNonUniformity
   **
   ** Defines the fNonUniformity parameter in % deviation from the central value 
   *@param nonU  Double parameter setting the maximum non-uniformity allowed 
   **/	
  void SetNonUniformity( Double_t nonU );
  
	
  virtual void Initialize();
  virtual void SetSpecialPhysicsCuts() {}


//  void SaveGeoParams();

  private:

  // Mapping of volume ID to crystal information
  std::map<Int_t, sCrystalInfo> fCrystalMap;

  // Current active crystal
  sCrystalInfo    *fCrystal;

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
    Double32_t     fNf;                //!  fast CsI(Tl) amplitude
    Double32_t     fNs;                //!  slow CsI(Tl) amplitude
    Int_t          fPosIndex;          //!
    Int_t          fNSteps;            //!  Number of steps in the active volume
    Double32_t     fEinc;              //!  Total incident energy
    Bool_t         kGeoSaved;          //!
    TList*         flGeoPar;	       //!

    TClonesArray*  fCaloCollection;              //!  The point collection
    TClonesArray*  fCaloCrystalHitCollection;    //!  The crystal hit collection

    //! Defining functions for energy to light output calculation
//    TF1 *tf_p_dNs;    //!
//    TF1 *tf_p_dNf;    //!
//    TF1 *tf_g_dNs;    //!
//    TF1 *tf_g_dNf;    //!

    TF1 *tf_dNf_dE; //!
    TF1 *tf_dNs_dE; //!

    // Selecting the geometry of the CALIFA calorimeter
    Int_t fGeometryVersion;
    // Adding some non-uniformity preliminary description
    Double_t  fNonUniformity;
	
    /** Private method AddHit
     **
     ** Adds a CalifaPoint to the HitCollection
     **/
    R3BCalifaPoint* AddPoint(Int_t trackID, Int_t detID, Int_t volid, Int_t copy, Int_t ident,
			 TVector3 posIn,
			 TVector3 pos_out, TVector3 momIn, 
			 TVector3 momOut, Double_t time, 
			 Double_t length, Double_t eLoss,
			 Double_t Nf, Double_t Ns);

	
    /** Private method AddCrystalHit
     **
     ** Adds a CalifaCrystalCalDataSim to the HitCollection
     **/
    R3BCalifaCrystalCalDataSim* AddCrystalHit(Int_t type, Int_t copy, Int_t ident,
					Double_t energy, Double_t Nf, Double_t Ns, Double_t tof,
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
	
    TGeoRotation* createMatrix( Double_t phi, Double_t theta, Double_t psi);

    Bool_t GetCrystalInfo(sCrystalInfo &info);

    ClassDef(R3BCalifa,5);
};

inline void R3BCalifa::ResetParameters() {
  fTrackID = fVolumeID = fParentTrackID = fTrackPID = fUniqueID = 0;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = fNf = fNs = fEinc = 0;
  fPosIndex = 0;
  fNSteps = 0;
};

#endif 
