#pragma once

#include "TObject.h"
namespace R3B::Neuland
{
    struct SimCalData : public TObject
    {
        int bar_module{}; // ns
        double let_l{};   // ns
        double let_r{};   // ns
        double tot_l{};   // ns
        double tot_r{};   // ns
        SimCalData() = default;
        SimCalData(Int_t paddleid, double totL, double totR, double tleL, double tleR)
            : bar_module{ paddleid }
            , let_l{ tleL }
            , let_r{ tleR }
            , tot_l{ totL }
            , tot_r{ totR }
        {
        }
        ClassDefNV(SimCalData, 2);
    };
} // namespace R3B::Neuland
