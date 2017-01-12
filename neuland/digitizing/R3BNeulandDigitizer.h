#ifndef R3B_NEULAND_DIGITIZER_H
#define R3B_NEULAND_DIGITIZER_H

#include <functional>
#include <map>
#include <vector>

#include "TClonesArray.h"

#include "FairTask.h"

#include "R3BNeulandDigi.h"
#include "R3BNeulandGeoPar.h"
#include "R3BNeulandPixel.h"
#include "R3BNeulandPoint.h"

#include "DigitizingEngine.h"

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
 */

class R3BNeulandDigitizer : public FairTask
{
  public:
    R3BNeulandDigitizer(const TString input = "NeulandPoints",
                        const TString output = "NeulandDigis",
                        const TString outputpx = "NeulandPixels");
    R3BNeulandDigitizer(Neuland::DigitizingEngine* engine,
                        const TString input = "NeulandPoints",
                        const TString output = "NeulandDigis",
                        const TString outputpx = "NeulandPixels");

    ~R3BNeulandDigitizer() = default;

  private:
    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandDigitizer(const R3BNeulandDigitizer&);            // copy constructor
    R3BNeulandDigitizer(R3BNeulandDigitizer&&);                 // move constructor
    R3BNeulandDigitizer& operator=(const R3BNeulandDigitizer&); // copy assignment
    R3BNeulandDigitizer& operator=(R3BNeulandDigitizer&&);      // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;
    void SetParContainers() override;

  public:
    void Exec(Option_t*) override;
    void AddFilter(const std::function<bool(const R3BNeulandDigi*)> f) { fDigiFilters.push_back(f); }

  private:
    std::unique_ptr<Neuland::DigitizingEngine> fDigitizingEngine; // owning (sink)

    TString fInput;
    TString fOutput;
    TString fOutputPx;

    TClonesArray* fNeulandPoints;                 // non-owning
    std::unique_ptr<TClonesArray> fNeulandDigis;  // owning
    std::unique_ptr<TClonesArray> fNeulandPixels; // owning

    R3BNeulandGeoPar* fNeulandGeoPar; // non-owning

    Bool_t IsValid(const R3BNeulandDigi*) const;
    std::vector<std::function<bool(const R3BNeulandDigi*)>> fDigiFilters;

    TH1F* hMultOne;
    TH1F* hMultTwo;
    TH1F* hRLTimeToTrig;
    TH2F* hElossVSQDC;

  public:
    ClassDefOverride(R3BNeulandDigitizer, 0)
};

#endif // R3B_NEULAND_DIGITIZER_H
