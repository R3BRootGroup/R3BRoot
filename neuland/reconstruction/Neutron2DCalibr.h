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
#include "TH2D.h"
#include "TString.h"
#include <iostream>
#include <map>
#include <memory>

class TCutG;
class R3BNeulandCluster;

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
        Neutron2DCalibr(unsigned int nmax);
        virtual ~Neutron2DCalibr();

        void AddClusterFile(const TString& file);
        void Optimize(std::vector<double> slope = { 0.04, 0.001, 0.001, 10 },
                      std::vector<double> distance = { 10, 0.5, 1, 20 },
                      std::vector<double> dist_off = { 3, 0.5, 3, 6 });
        void Print(std::ostream& out = std::cout) const;
        void Draw(const TString& img = "") const;
        void WriteParameterFile(const TString& parFile) const;
        void AddFilter(const Filterable<R3BNeulandCluster*>::Filter& f) { fClusterFilters.Add(f); }

      private:
        TCutG* GetCut(unsigned int nNeutrons, double k, double k0, double m);
        double WastedEfficiency(const double* d);

        unsigned int fNMax;
        std::map<unsigned int, std::unique_ptr<TH2D>> fHistsNreac;
        std::map<unsigned int, std::unique_ptr<TH2D>> fHistsNin;
        std::map<unsigned int, TCutG*> fCuts;
        Filterable<R3BNeulandCluster*> fClusterFilters;

        ClassDef(Neutron2DCalibr, 0);
    };

}; // namespace Neuland

#endif // NEULANDNEUTRON2DCALIBR
