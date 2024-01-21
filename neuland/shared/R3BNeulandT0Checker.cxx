#include "R3BNeulandT0Checker.h"
#include "R3BException.h"

namespace R3B::Neuland
{
    void T0Checker::TestAndSet(int coarse_time) {
        if(not correction_.has_value())
        {
            throw R3B::runtime_error(fmt::format("Correction of T0 has not been checked before testing the next event!"));
        }


    }
} // namespace R3B::Neuland
