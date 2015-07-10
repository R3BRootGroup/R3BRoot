#ifndef R3BNEULANDVISUALIZER_H
#define R3BNEULANDVISUALIZER_H 1

#include "TFile.h"
#include "TTree.h"
#include "TH3D.h"
#include "TCanvas.h"

//#include <vector>

class R3BNeulandVisualizer
{
public:
  R3BNeulandVisualizer(const TString &input_file, const TString &what);
  //R3BNeulandVisualizer(const std::vector<TString> files);
  ~R3BNeulandVisualizer();

  inline void ShowEvent(const UInt_t i)
  {
    i_ = i;
    Visualize();
  }

  inline void ShowNext()
  {
    i_++;
    Visualize();
  }

protected:
  void Visualize();

private:
  TFile* f_;
  TTree* t_;
  TH3D* h3_;
  TCanvas* canvas_;
  UInt_t i_;

  ClassDef(R3BNeulandVisualizer, 0);
};

#endif //R3BNEULANDVISUALIZER_H