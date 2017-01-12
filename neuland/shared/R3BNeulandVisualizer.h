#ifndef R3BNEULANDVISUALIZER_H
#define R3BNEULANDVISUALIZER_H 1

#include "TCanvas.h"
#include "TFile.h"
#include "TH3D.h"
#include "TTree.h"

class R3BNeulandVisualizer
{
  public:
    R3BNeulandVisualizer(const TString& input_file, const TString& what);
    virtual ~R3BNeulandVisualizer();

    inline void ShowEvent(const UInt_t i)
    {
        fIndex = i;
        Visualize();
    }

    inline void ShowNext()
    {
        fIndex++;
        Visualize();
    }

  protected:
    void Visualize();

  private:
    TFile* fFile;
    TTree* fTree;
    TH3D* fh3;
    TCanvas* fCanvas;
    UInt_t fIndex;

    ClassDef(R3BNeulandVisualizer, 0);
};

#endif // R3BNEULANDVISUALIZER_H
