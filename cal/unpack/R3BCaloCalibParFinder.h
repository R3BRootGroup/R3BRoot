// -------------------------------------------------------------------------
// -----                R3BCaloCalibParFinder source file              -----
// -----                  Created 22/07/14  by H.Alvarez               -----
// -------------------------------------------------------------------------

#ifndef R3BCALOCALIBPARFINDER_H
#define R3BCALOCALIBPARFINDER_H

#include "FairTask.h"

class TClonesArray;

class R3BCaloCalibParFinder : public FairTask
{
    
public:
    
    /** Default constructor **/
    R3BCaloCalibParFinder();
    
    /** Destructor **/
    ~R3BCaloCalibParFinder();
   
    void SetOutputFile(const char *outFile);

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
    
    // (Nf + Ns) / Energy ratio
    Double_t *fRatioPidEnergy;
    UInt_t *fNumEvents;
    UInt_t fNumChannels;
    
    UInt_t nEvents;

    char *fOutputFile;

    ClassDef(R3BCaloCalibParFinder,1);
};


#endif
