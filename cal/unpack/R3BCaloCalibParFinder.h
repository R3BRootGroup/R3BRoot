// -------------------------------------------------------------------------
// -----                R3BCaloCalibParFinder source file              -----
// -----                  Created 22/07/14  by H.Alvarez               -----
// -------------------------------------------------------------------------

#ifndef R3BCALOCALIBPARFINDER_H
#define R3BCALOCALIBPARFINDER_H

#include "FairTask.h"
#include "R3BCaloHit.h"
#include "R3BCaloHitSim.h"
#include "R3BCaloCalibPar.h"

class TClonesArray;

class R3BCaloCalibParFinder : public FairTask
{
    
public:
    
    /** Default constructor **/
    R3BCaloCalibParFinder();
    
    /** Destructor **/
    ~R3BCaloCalibParFinder();
    
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
    
    TClonesArray* fCaloRawHitCA;
    
    // Parameter class
    R3BCaloCalibPar* fCaloCalibPar;
    
    
    ClassDef(R3BCaloCalibParFinder,1);
};


#endif
