#ifndef NEULANDNEUTRON2DCALIBR
#define NEULANDNEUTRON2DCALIBR

#include "Rtypes.h"
#include "TString.h"
#include <iostream>
#include <map>

class TCutG;
class TH2D;

/*

    |
    |
  ^ 3
  | X X
k | X   X            y = m*x + a
  | X     X      =>  0 = m*x + a
  | X       X    =>  x = -a/m
  v 0         X
    | X         X
    |___1 X X X X 2______________

*/

namespace Neuland
{
    class Neutron2DCalibr
    {
      public:
        Neutron2DCalibr(UInt_t nmax);

        void AddClusterFile(const TString& file);
        void Optimize(std::vector<Double_t> slope = { 0.04, 0.001, 0.001, 10 },
                      std::vector<Double_t> distance = { 10, 0.5, 1, 20 },
                      std::vector<Double_t> dist_off = { 3, 0.5, 3, 6 });
        void Print(std::ostream& out = std::cout) const;
        void Draw(const TString& img = "") const;
        void WriteParameterFile(const TString& parFile) const;

      private:
        TCutG* GetCut(const UInt_t nNeutrons, const Double_t k, const Double_t k0, const Double_t m);
        Double_t WastedEfficiency(const Double_t* d);

        UInt_t fNMax;
        std::map<UInt_t, TH2D*> fHists;
        std::map<UInt_t, TCutG*> fCuts;
    };

}; // namespace

#endif // NEULANDNEUTRON2DCALIBR
