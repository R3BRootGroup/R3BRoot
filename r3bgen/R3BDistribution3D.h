#ifndef R3BDISTRIBUTION3D_H
#define R3BDISTRIBUTION3D_H

#include "TF3.h"
#include "TH3.h"

#include "R3BDistribution.h"

class R3BDistribution3D : public R3BDistribution<3> {
  using Arr = std::array<Double_t, 3>;
  public:
        static R3BDistribution<3> Delta(const Double_t value1, const Double_t value2, const Double_t value3);
        static R3BDistribution<3> Constant(const Arr lower_values, Arr upper_values);
        static R3BDistribution<3> Gaussian(const Arr means, Arr sigmas);
        static R3BDistribution<3> Cube(const Arr center, const Double_t edgeLength);
        static R3BDistribution<3> Prism(R3BDistribution<2> xybase, Double_t zfrom, Double_t zto);
        static R3BDistribution<3> Sphere(const Arr center, const Double_t radius);
        static R3BDistribution<3> Cylindric(R3BDistribution<2> xydist, R3BDistribution<1> zdist);
};
#endif
