// ---------------------------------------------------------------------
// -----          R3BCalifaCrystalCalDataSim header file           -----
// -----             Created 04/07/12 by P.Cabanelas               -----
// -----            Modified 23/12/16 by P.Cabanelas               -----
// ---------------------------------------------------------------------

/**  R3BCalifaCrystalCalDataSim.h
 **  A simulated crystalCal is the representation of the 
 **  information obtained from each crystal in the calorimeter.
 **/

#ifndef R3BCALIFACRYSTALCALDATASIM_H
#define R3BCALIFACRYSTALCALDATASIM_H

#include "TObject.h"
#include "FairMultiLinkedData.h"
#include "R3BCalifaCrystalCalData.h"

class R3BCalifaCrystalCalDataSim : public R3BCalifaCrystalCalData 
{
public:

  /** Default constructor **/
  R3BCalifaCrystalCalDataSim();

  /** Constructor with arguments
   *@param fCrystalType    Crystal type
   *@param fCrystalCopy    Crystal copy
   *@param fNSteps         Steps inside an active volume
   *@param fEinc           total energy entering the crystal [GeV]
   *@param fTrackID        Track index
   *@param fVolumeID       Volume index
   *@param fParentTrackID  Parent Track index
   *@param fTrackPID       Particle identification
   *@param fTrackUniqueID  Particle unique id (e.g. if Delta electron, fTrackUniqueID=9)
   **/
  R3BCalifaCrystalCalDataSim(Int_t type, Int_t copy, Int_t ident,
		       Double_t energy, Double_t Nf, Double_t Ns, Double_t time,
			Int_t steps, Double_t einc,
			Int_t trackid, Int_t volid, Int_t partrackid,
			Int_t pdgid, Int_t uniqueid);

  /** Copy constructor **/
  R3BCalifaCrystalCalDataSim(const R3BCalifaCrystalCalDataSim&);

  R3BCalifaCrystalCalDataSim& operator=(const R3BCalifaCrystalCalDataSim&) { return *this; }

  /** Destructor **/
  virtual ~R3BCalifaCrystalCalDataSim();

  /** Accessors **/
  Int_t    GetCrystalType()   const { return fCrystalType; }
  Int_t    GetCrystalCopy()   const { return fCrystalCopy; }
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
  void AddMoreEnergy(Double32_t moreEnergy) { fEnergy += moreEnergy; }
  void AddMoreNf(Double32_t moreNf)         { fNf += moreNf; }
  void AddMoreNs(Double32_t moreNs)         { fNs += moreNs; }
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

  Int_t fCrystalType;    //crystal type
  Int_t fCrystalCopy;    //crystal copy
  Int_t fNSteps;         //steps inside an active volume
  Double32_t fEinc;      //total energy entering the crystal
  Int_t fTrackID;        //track index
  Int_t fVolumeID;       //volume index
  Int_t fParentTrackID;  //parent track index
  Int_t fTrackPID;       //particle identification
  Int_t fTrackUniqueID;       //particle unique id

  ClassDef(R3BCalifaCrystalCalDataSim,1)

};

#endif
