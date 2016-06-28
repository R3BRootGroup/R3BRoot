#ifndef NEULANDNEUTRON2DCALIBR
#define NEULANDNEUTRON2DCALIBR

#include <map>
#include <iostream>
#include "Rtypes.h"
#include "TString.h"

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
        Neutron2DCalibr();

        void SetClusterFile(const UInt_t nNeutrons, const TString& file);
        // void SetK(const Double_t )
        void Optimize();
        void Print(std::ostream &out = std::cout) const;
        void Draw(const TString &img = "") const;
        void WriteParameterFile(const TString &parFile) const;

      private:
        TCutG* GetCut(const UInt_t nNeutrons, const Double_t k, const Double_t k0, const Double_t m);
        Double_t WastedEfficiency(const Double_t* d);

        std::map<UInt_t, TH2D*> fHists;
        std::map<UInt_t, TCutG*> fCuts;
    };

}; // namespace

#endif // NEULANDNEUTRON2DCALIBR
