// -------------------------------------------------------------------------
// -----           R3BStartrackCalibParFinder source file              -----
// -----           from R3BCaloCalibParFinder source file              -----
// -----              Created 22/07/14  by H.Alvarez                   -----
// -------------------------------------------------------------------------

#ifndef R3BSTARTRACKCALIBPARFINDER_H
#define R3BSTARTRACKCALIBPARFINDER_H

#include "FairTask.h"
#include "R3BStartrackHit.h"
#include "R3BStartrackerHit.h"
#include "R3BStartrackCalibPar.h"

class TClonesArray;

class R3BStartrackCalibParFinder : public FairTask
{
    
public:
    
    /** Default constructor **/
    R3BStartrackCalibParFinder();
    
    /** Destructor **/
    ~R3BStartrackCalibParFinder();
    
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
    R3BStartrackCalibPar* fStartrackCalibPar;
    
    
    ClassDef(R3BStartrackCalibParFinder,1);
};


#endif
