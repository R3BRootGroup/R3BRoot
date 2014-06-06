// -----------------------------------------------------------------------------
// -----                              R3BLandTdiff                         -----
// -----                     Created 07-05-2014 by D.Kresan                -----
// -----------------------------------------------------------------------------

#ifndef R3BLANDTDIFF_H
#define R3BLANDTDIFF_H

#include <fstream>
#include <map>

#include "FairTask.h"

class TClonesArray;

class R3BLandTdiff : public FairTask
{
  public:
    R3BLandTdiff();
    R3BLandTdiff(const char* name, Int_t iVerbose);
    virtual ~R3BLandTdiff();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();
    
    inline void SetTdiffParName(char* name) { fParName = name; }

  private:
    TClonesArray* fLandPmt;
    TClonesArray* fLandDigi;
    Int_t fNDigi;
    
    char* fParName;
    std::ifstream* fInFile;
    
    std::map<Int_t, Bool_t> fMapIsSet;
    std::map<Int_t, Double_t> fMapTdiff;
    std::map<Int_t, Double_t> fMapVeff;
    std::map<Int_t, Double_t> fMapTsync;
    
    void ReadParameters();

  public:
    ClassDef(R3BLandTdiff, 0)
};

#endif
