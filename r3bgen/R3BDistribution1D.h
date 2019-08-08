#ifndef R3BDISTRIBUTION1D_H
#define R3BDISTRIBUTION1D_H

#include "TF1.h"
#include "TH1.h"
#include "TGraph.h"

#include "R3BDistribution.h"

class R3BDistribution1D : public R3BDistribution<1> {
public:
        static R3BDistribution<1> Delta(const Double_t value);
        static R3BDistribution<1> Flat(const Double_t lower_value, const Double_t upper_value);
        static R3BDistribution<1> Gaussian(const Double_t mean, const Double_t sigma);
        static R3BDistribution<1> Function(const std::function<Double_t(const Double_t)> func, const Double_t lower_bound, const Double_t upper_bound);
        static R3BDistribution<1> Data(const TF1& data);
        static R3BDistribution<1> Data(const TF1& data, const Double_t lower_bound, const Double_t upper_bound);
        static R3BDistribution<1> Data(const TH1& data);
        static R3BDistribution<1> Data(const TH1& data, const Double_t lower_bound, const Double_t upper_bound);
        static R3BDistribution<1> Data(const TGraph& data);
        static R3BDistribution<1> Data(const TGraph& data, const Double_t lower_bound, const Double_t upper_bound);
        static R3BDistribution<1> DataLogLog(const TGraph& data);
        static R3BDistribution<1> DataLogLog(const TGraph& data, const Double_t lower_bound, const Double_t upper_bound);
};
#endif
