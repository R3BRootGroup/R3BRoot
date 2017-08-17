#ifndef R3BTOFDHITDATA_H
#define R3BTOFDHITDATA_H

#include "TObject.h"

// for the data analysis of the Tofd detectors. 
// Introduced by M.Heil, May 2016

class R3BTofdHitData : public TObject
{
  public:
    // Default Constructor
    R3BTofdHitData();

    // Standard Constructor
    R3BTofdHitData(Double_t t,Double_t x, Double_t y, Double_t Z, Double_t tdiff, Double_t ELoss =0, Double_t ID=0); 
    
    R3BTofdHitData(const R3BTofdHitData& right);

    // Destructor
    virtual ~R3BTofdHitData() { }

    // Getters  
	Double_t GetTime() const {return fTime_ns;}
	Double_t GetX() const {return fXpos_cm;}
	Double_t GetY() const {return fYpos_cm;}
	Double_t GetZ() const {return fZ;}
	Double_t GetTimeDifference() const {return fTimeDiff2Ref_ns;}
	Double_t GetEnergyLoss() const {return fEnergyLoss;}
	Double_t GetDetectorID() const {return fDetectorNumber;}
	
	// Setters
	void SetTime( Double_t time) {fTime_ns = time;}
	void SetX( Double_t x) {fXpos_cm = x;}
	void SetY( Double_t y) {fYpos_cm = y;}
	void SetZ( Double_t Z) {fZ = Z;}
	void SetTimeDifference( Double_t time_diff) {fTimeDiff2Ref_ns = time_diff;}
	void SetEnergyLoss( Double_t ELoss) {fEnergyLoss = ELoss;}
	void SetDetectorNumber( Double_t ID) {fDetectorNumber = ID;}

  private:
    Double_t fTime_ns;         // average time of the scintillators and planes
    Double_t fXpos_cm;         // x-position of the scintillators
    Double_t fYpos_cm;         // y-position of the scintillators 
    Double_t fZ;               // Z of the scintillators 
    Double_t fTimeDiff2Ref_ns; // difference to master trigger time
    Double_t fEnergyLoss;		   // energy loss in the scintillators
    Double_t fDetectorNumber;	   // detector ID of the scintillators

  public:

    ClassDef(R3BTofdHitData, 1)
};

#endif


