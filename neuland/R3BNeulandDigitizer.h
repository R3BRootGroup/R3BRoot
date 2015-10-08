#ifndef _R3B_NEULAND_DIGITIZER_H_
#define _R3B_NEULAND_DIGITIZER_H_ 1

#include "FairTask.h"

#include "R3BLandPoint.h"
#include "R3BLandDigiPar.h"
#include "R3BLandDigi.h"

#include "DigitizingEngine.h"

class TClonesArray;
class TH1F;


class R3BNeulandDigitizer : public FairTask {

public:
    R3BNeulandDigitizer();
    ~R3BNeulandDigitizer();

    virtual InitStatus Init();
    virtual void Exec(Option_t *);
    virtual void Finish();
    virtual void Reset();

protected:
    TClonesArray *fLandPoints;
    TClonesArray *fLandDigi;

    R3BLandDigiPar *fLandDigiPar;
    Double_t fPlength;

    Neuland::DigitizingEngine *fDigitizingEngine;

    TH1F *hPMl;
    TH1F *hPMr;
    TH1F *hMultOne;
    TH1F *hMultTwo;
    TH1F *hRLTimeToTrig;

    Int_t npaddles;
    Int_t nplanes;
    Int_t paddle_per_plane;

private:
    virtual void SetParContainers();

    ClassDef(R3BNeulandDigitizer, 1)
};

#endif //_R3B_NEULAND_DIGITIZER_H_