// -------------------------------------------------------------------------
// -----                R3BCaloCalParFinder source file              -----
// -----                  Created 22/07/14  by H.Alvarez               -----
// -------------------------------------------------------------------------

#ifndef R3BCALOCALPARFINDER_H
#define R3BCALOCALPARFINDER_H

#include "FairTask.h"
//#include "R3BCaloHit.h"
//#include "R3BCaloHitSim.h"
#include "R3BCaloCalPar.h"

class TClonesArray;
class R3BCaloHitSim;
class R3BCaloHit;
class R3BCaloRawHit;

class R3BCaloCalParFinder : public FairTask
{
    
public:
    
    /** Default constructor **/
    R3BCaloCalParFinder();
    
    /** Destructor **/
    ~R3BCaloCalParFinder();
    
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
    R3BCaloCalPar* fCaloCalibPar;
    
    
    ClassDef(R3BCaloCalParFinder,1);
};


#endif
