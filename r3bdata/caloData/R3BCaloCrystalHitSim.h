// -------------------------------------------------------------------------
// -----                R3BCaloCrystalHitSim header file               -----
// -----               Created 04/07/12  by P. Cabanelas               -----
// -------------------------------------------------------------------------

/**  R3BCaloCrystalHitSim.h
 **  A simulation calorimeter crystal hit is the representation of the 
 **  information obtained from each crystal in the calorimeter.
 **/

#ifndef R3BCALOCRYSTALHITSIM_H
#define R3BCALOCRYSTALHITSIM_H

#include "TObject.h"
#include "FairMultiLinkedData.h"
#include "R3BCaloCrystalHit.h"

class R3BCaloCrystalHitSim : public R3BCaloCrystalHit 
{
public:

  /** Default constructor **/
  R3BCaloCrystalHitSim();

  /** Constructor with arguments
   // ---- *@param fCrystalId      Crystal unique identifier
   *@param fCrystalType    Crystal type
   *@param fCrystalCopy    Crystal copy
   // ---- *@param fEnergy	   total energy deposited on the crystal [GeV]
   // ---- *@param fTime           Time since event start [ns]
   *@param fNSteps         Steps inside an active volume
   *@param fEinc           total energy entering the crystal [GeV]
   *@param fTrackID        Track index
   *@param fVolumeID       Volume index
   *@param fParentTrackID  Parent Track index
   *@param fTrackPID       Particle identification
   *@param fTrackUniqueID  Particle unique id (e.g. if Delta electron, fTrackUniqueID=9)
   **/
  R3BCaloCrystalHitSim(Int_t type, Int_t copy, Int_t ident,
			Double_t energy, Double_t time,
			Int_t steps, Double_t einc,
			Int_t trackid, Int_t volid, Int_t partrackid,
			Int_t pdgid, Int_t uniqueid);

  /** Copy constructor **/
  R3BCaloCrystalHitSim(const R3BCaloCrystalHitSim& hit) { *this = hit; };

  /** Destructor **/
  virtual ~R3BCaloCrystalHitSim();

  /** Accessors **/
  Int_t    GetCrystalType()   const { return fCrystalType; }
  Int_t    GetCrystalCopy()   const { return fCrystalCopy; }
  // ---- Int_t    GetCrystalId()     const { return fCrystalId; }
  // ---- Double_t GetEnergy()        const { return fEnergy; }
  // ---- Double_t GetTime()          const { return fTime; }
  Int_t    GetNSteps()        const { return fNSteps; }
  Double_t GetEinc()          const { return fEinc; }
  Int_t    GetTrackId()       const { return fTrackID; }
  Int_t    GetVolumeId()      const { return fVolumeID; }
  Int_t    GetParentTrackId() const { return fParentTrackID; }
  Int_t    GetTrackPID()      const { return fTrackPID; }
  Int_t    GetTrackUniqueID() const { return fTrackUniqueID; }
	
  /** Modifiers **/
  void SetCrystalType(Int_t type)           { fCrystalType = type; }
  void SetCrystalCopy(Int_t copy)           { fCrystalCopy = copy; }
  // ---- void SetCrystalId(Int_t ident)            { fCrystalId = ident; }
  // ---- void SetEnergy(Double32_t energy)         { fEnergy = energy; }
  void AddMoreEnergy(Double32_t moreEnergy) { fEnergy += moreEnergy; }
  // ---- void SetTime(Double32_t time)             { fTime = time; }
  void SetNSteps(Int_t steps)               { fNSteps = steps; }
  void SetEinc(Double32_t einc)             { fEinc = einc; }
  void SetTrackId(Int_t trackid)            { fTrackID = trackid; }
  void SetVolumeId(Int_t volid)             { fVolumeID = volid; }
  void SetParentTrackId(Int_t partrackid)   { fParentTrackID = partrackid; }
  void SetTrackPID(Int_t pdgid)             { fTrackPID = pdgid; }
  void SetTrackUniqueID(Int_t uniqueid)     { fTrackUniqueID = uniqueid; }

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

protected:

  // ---- Double32_t fEnergy;    //total energy in the crystal
  // ---- Double32_t fTime;      //time of the interaction
  Int_t fCrystalType;    //crystal type
  Int_t fCrystalCopy;    //crystal copy
  // ---- Int_t fCrystalId;      //crystal unique identifier
  Int_t fNSteps;         //steps inside an active volume
  Double32_t fEinc;      //total energy entering the crystal
  Int_t fTrackID;        //track index
  Int_t fVolumeID;       //volume index
  Int_t fParentTrackID;  //parent track index
  Int_t fTrackPID;       //particle identification
  Int_t fTrackUniqueID;       //particle unique id

  ClassDef(R3BCaloCrystalHitSim,1)

};

#endif
