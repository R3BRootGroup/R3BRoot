// -------------------------------------------------------------------------
// -----           R3BStarTrackCalibParFinder source file              -----
// -----           from R3BCaloCalibParFinder source file              -----
// -----              Created 22/07/14  by H.Alvarez                   -----
// -------------------------------------------------------------------------

#ifndef R3BSTARTRACKCALIBPARFINDER_H
#define R3BSTARTRACKCALIBPARFINDER_H

#include "FairTask.h"
#include "R3BSTaRTraHit.h"
#include "R3BSTaRTrackerHit.h"
#include "R3BStarTrackCalibPar.h"

class TClonesArray;

class R3BStarTrackCalibParFinder : public FairTask
{
    
public:
    
    /** Default constructor **/
    R3BStarTrackCalibParFinder();
    
    /** Destructor **/
    ~R3BStarTrackCalibParFinder();
    
    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    
    /** Virtual method Reset **/
    virtual void Reset();
    
protected:
    
    /** Virtual method Init **/
    virtual InitStatus Init();
    
    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Register **/
    virtual void Register();
    
    /** Virtual method SetParContainers **/
    virtual void SetParContainers();
    
    /** Virtual method Finish **/
    virtual void Finish();
    
    TClonesArray* fSiDetRawHitCA;
    
    // Parameter class
    R3BStarTrackCalibPar* fStarTrackCalibPar;
    
    
    ClassDef(R3BStarTrackCalibParFinder,1);
};


#endif
