// -----------------------------------------------------------------------------
// -----                              R3BLandTSync                         -----
// -----                     Created 07-05-2014 by D.Kresan                -----
// -----------------------------------------------------------------------------

#ifndef R3BLANDTSYNC_H
#define R3BLANDTSYNC_H

#include <fstream>
#include <map>

#include "FairTask.h"

class TClonesArray;
class R3BLandTSyncPar;

class R3BLandTSync : public FairTask
{
  public:
    R3BLandTSync();
    R3BLandTSync(const char* name, Int_t iVerbose);
    virtual ~R3BLandTSync();

    virtual InitStatus Init();
    
    virtual void SetParContainers();
    
    virtual InitStatus ReInit();
    
    virtual void Exec(Option_t* option);

    virtual void FinishEvent();
    
    inline void SetFirstPlaneHorisontal() { fFirstPlaneHorisontal = kTRUE; }

  private:
    void SetParameter();
    
    TClonesArray* fLandPmt;
    TClonesArray* fLandDigi;
    R3BLandTSyncPar* fTSyncPar;
    Int_t fNDigi;
    
    std::ifstream* fInFile;
    Bool_t fFirstPlaneHorisontal;
    
    std::map<Int_t, Bool_t> fMapIsSet;
    std::map<Int_t, Double_t> fMapVeff;
    std::map<Int_t, Double_t> fMapTSync;
    
    void ReadParameters();

  public:
    ClassDef(R3BLandTSync, 0)
};

#endif
