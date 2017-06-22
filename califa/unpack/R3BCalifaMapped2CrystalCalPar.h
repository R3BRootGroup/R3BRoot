// -------------------------------------------------------------------------
// -----         3BCalifaMapped2CrystalCalPar source file              -----
// -----             Created 22/07/14  by H.Alvarez                    -----
// -----            Modified 20/03/17  by P.Cabanelas                  -----
// -------------------------------------------------------------------------

#ifndef R3BCALIFAMAPPED2CRYSTALCALPAR_H
#define R3BCALIFAMAPPED2CRYSTALCALPAR_H

#include "FairTask.h"

class TClonesArray;

class R3BCalifaMapped2CrystalCalPar : public FairTask
{
    
public:
    
    /** Default constructor **/
    R3BCalifaMapped2CrystalCalPar();
    
    /** Destructor **/
    ~R3BCalifaMapped2CrystalCalPar();
   
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
    
    TClonesArray* fCalifaMappedDataCA;
    
    // (Nf + Ns) / Energy ratio
    Double_t *fRatioPidEnergy;
    UInt_t *fNumEvents;
    UInt_t fNumChannels;
    
    UInt_t nEvents;

    char *fOutputFile;

    ClassDef(R3BCalifaMapped2CrystalCalPar,2);
};


#endif
