#ifndef R3BNEULANDVISUALIZER_H
#define R3BNEULANDVISUALIZER_H 1

#include "TFile.h"
#include "TTree.h"
#include "TH3D.h"
#include "TCanvas.h"

//#include <vector>

class R3BNeulandVisualizer {
public:
   R3BNeulandVisualizer(const TString &input_file, const TString &what);
   //R3BNeulandVisualizer(const std::vector<TString> files);
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
   TFile *fFile;
   TTree *fTree;
   TH3D *fh3;
   TCanvas *fCanvas;
   UInt_t fIndex;

   ClassDef(R3BNeulandVisualizer, 0);
};

#endif //R3BNEULANDVISUALIZER_H