/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef NEULANDNEUTRON2DCALIBR
#define NEULANDNEUTRON2DCALIBR

#include "Filterable.h"
#include "R3BNeulandCluster.h"
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
        void AddFilter(const Filterable<R3BNeulandCluster*>::Filter f) { fClusterFilters.Add(f); }

      private:
        TCutG* GetCut(const UInt_t nNeutrons, const Double_t k, const Double_t k0, const Double_t m);
        Double_t WastedEfficiency(const Double_t* d);

        UInt_t fNMax;
        std::map<UInt_t, TH2D*> fHistsNreac;
        std::map<UInt_t, TH2D*> fHistsNin;
        std::map<UInt_t, TCutG*> fCuts;
        Filterable<R3BNeulandCluster*> fClusterFilters;
    };

}; // namespace Neuland

#endif // NEULANDNEUTRON2DCALIBR
