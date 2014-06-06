// -----------------------------------------------------------------------------
// -----                            R3BLandTdiffFill                       -----
// -----                     Created 02-05-2014 by D.Kresan                -----
// -----------------------------------------------------------------------------

#ifndef R3BLANDTDIFFFILL_H
#define R3BLANDTDIFFFILL_H

#include <fstream>

#include "Math/IFunction.h"

#include "FairTask.h"

class TClonesArray;
class TH1F;
class TH2F;

class R3BLandTdiffFill : public FairTask
{
  public:
    R3BLandTdiffFill();
    R3BLandTdiffFill(const char* name, Int_t iVerbose);
    virtual ~R3BLandTdiffFill();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

    inline void SetNofBars(Int_t nBars)
    {
        fNofBars = nBars;
    }
    
    inline void SetTdiffParName(char* name)
    {
        fParName = name;
    }

  private:
    Int_t fnEvents;

    Int_t fNofBars;

    TClonesArray* fLandPmt;

    TH2F* fh_time;
    TH1F** fh_tdiff;
    TH1F* fh_tdiff_res;
    TH1F* fh_toff_res;
    TH1F* fh_veff_res;
    
    char* fParName;
    std::ofstream* fOutFile;

    void CreateHistos();

    void WriteHistos();

  public:
    ClassDef(R3BLandTdiffFill, 0)
};

#endif
