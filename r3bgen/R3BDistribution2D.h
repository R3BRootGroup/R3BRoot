#ifndef R3BDISTRIBUTION2D_H
#define R3BDISTRIBUTION2D_H

#include "TGraph2D.h"
#include "TF2.h"
#include "TH2.h"

#include "R3BDistribution.h"

class R3BDistribution2D : public R3BDistribution<2> {
  using Arr = std::array<Double_t, 2>;
  public:
        static R3BDistribution<2> Delta(const Double_t value1, const Double_t value2);
        static R3BDistribution<2> Flat(const Arr lower_values, const Arr upper_values);
        static R3BDistribution<2> Gaussian(const Double_t mean, const Double_t sigma);
        static R3BDistribution<2> Gaussian(const Arr means, const Arr sigmas);
        static R3BDistribution<2> Square(const Arr center, const Double_t edgeLength);
        static R3BDistribution<2> Rectangle(const Arr center, const Arr edgeLengths);
        static R3BDistribution<2> Circle(const Arr center, const Double_t radius);
        //static R3BDistribution<2> Data(const TH2& data); todo
        //static R3BDistribution<2> Data(const TGraph2D& data); todo
        //static R3BDistribution<2> Data(const TF2& data); todo
};
#endif
