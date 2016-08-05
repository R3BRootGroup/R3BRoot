// -----------------------------------------------------------------------------
// -----                         R3BNeulandCal2Hit                         -----
// -----                   Created 07-05-2014 by D.Kresan                  -----
// -----------------------------------------------------------------------------

#ifndef R3BNEULANDCAL2HIT_H
#define R3BNEULANDCAL2HIT_H

#include <fstream>
#include <map>

#include "FairTask.h"

class TClonesArray;
class R3BNeulandHitPar;

class R3BNeulandCal2Hit : public FairTask
{
  public:
    R3BNeulandCal2Hit();
    R3BNeulandCal2Hit(const char* name, Int_t iVerbose);
    virtual ~R3BNeulandCal2Hit();

    virtual InitStatus Init();
    
    virtual void SetParContainers();
    
    virtual InitStatus ReInit();
    
    virtual void Exec(Option_t* option);

    virtual void FinishEvent();
    
    inline void SetFirstPlaneHorisontal() { fFirstPlaneHorisontal = kTRUE; }
        
    //Distance to target in cm
    inline void SetDistanceToTarget(Double_t d) { fDistanceToTarget = d; }

  private:
    void SetParameter();
    
    TClonesArray* fLandPmt;
    TClonesArray* fLandDigi;
    R3BNeulandHitPar* fPar;
    Int_t fNDigi;
    
    std::ifstream* fInFile;
    Bool_t fFirstPlaneHorisontal;
    Double_t fDistanceToTarget;
    
    std::map<Int_t, Bool_t> fMapIsSet;
    std::map<Int_t, Double_t> fMapVeff;
    std::map<Int_t, Double_t> fMapTSync;
    
    void ReadParameters();

  public:
    ClassDef(R3BNeulandCal2Hit, 0)
};

#endif
