#ifndef R3BNEULANDDIGITIZER_H
#define R3BNEULANDDIGITIZER_H

#include <map>

#include "FairTask.h"

#include "R3BNeulandPoint.h"
#include "R3BNeulandDigi.h"
#include "R3BNeulandGeoPar.h"

#include "DigitizingEngine.h"

class TClonesArray;
class TGeoNode;
class TH1F;
class TH2F;

/**
 * NeuLAND digitizing finder task
 * @author Jan Mayer
 *
 * For each event, get the R3BNeulandPoints and apply the detector response to create R3BNeulandDigis.
 *   Input:  Branch NeulandPoints = TClonesArray("R3BNeulandPoint")
 *           Stored Neuland Geometry Parameter NeulandGeoPar
 *   Output: Branch NeulandDigis  = TClonesArray("R3BNeulandDigi")
 *   Additional output: Some control histograms
 *
 * The idea is that once ROOT6 is used by default and ROOT5 is abandned, the Digitizing Engine Component can be passed
 * to this task, e.g. R3BNeulandDigitizer(myDigitizingEngine), where all necessary configuration can be done on the
 * engine, such that the task can keep a small footprint. Currently hardcoded.
 */

class R3BNeulandDigitizer : public FairTask
{
  public:
    R3BNeulandDigitizer();
    ~R3BNeulandDigitizer();

  private:
    // Rule of three/five: If a class requires a user-defined destructor, a user-defined copy
    // constructor, or a user-defined copy assignment operator, it almost certainly requires all three
    // Here no copy and no move is allowed
    R3BNeulandDigitizer(const R3BNeulandDigitizer&);            // copy constructor
    R3BNeulandDigitizer(R3BNeulandDigitizer&&);                 // move constructor
    R3BNeulandDigitizer& operator=(const R3BNeulandDigitizer&); // copy assignment
    R3BNeulandDigitizer& operator=(R3BNeulandDigitizer&&);      // move assignment

  protected:
    // TODO: #ROOT6: Declare functions overrriding virtual functions overrride
    InitStatus Init();       // override
    void Finish();           // override
    void SetParContainers(); // override
  public:
    void Exec(Option_t*); // override

  private:
    void Reset();

    // TODO: #ROOT6 Replace raw pointers with std::unique_ptr?
    TClonesArray* fNeulandPoints;
    TClonesArray* fNeulandDigis;

    R3BNeulandGeoPar* fNeulandGeoPar;
    Neuland::DigitizingEngine* fDigitizingEngine;

    TH1F* hPMl;
    TH1F* hPMr;
    TH1F* hMultOne;
    TH1F* hMultTwo;
    TH1F* hRLTimeToTrig;
    TH2F* hElossVSQDC;

  public:
    ClassDef(R3BNeulandDigitizer, 1)
};

#endif // R3BNEULANDDIGITIZER_H
