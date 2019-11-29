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

#include <iostream>
#include <stdexcept>
#include <stdint.h>

#include "Land02TreeWrapper.h"

using namespace std;

namespace R3BCalifaTimestitcher
{
    Land02TreeWrapper::Land02TreeWrapper(TTree* t, branchptrmap_t& branchmap, uint32_t _id)
        : TreeWrapper(t, _id)
    {
        if (branchmap.count("Wr_time_l") == 0)
            throw runtime_error("Land02TreeWrapper: Could not find branch Wr_time_l!");
        TSLow = static_cast<uint32_t*>(branchmap["Wr_time_l"]);

        if (branchmap.count("Wr_time_h") == 0)
            throw runtime_error("Land02TreeWrapper: Could not find branch Wr_time_h!");
        TSHigh = static_cast<uint32_t*>(branchmap["Wr_time_h"]);

        if (branchmap.count("Pspx04n") == 0)
            throw runtime_error("Land02TreeWrapper: Could not find branch Pspx04n!");
        pspx04n = static_cast<uint32_t*>(branchmap["Pspx04n"]);

        if (branchmap.count("Pspx05n") == 0)
            throw runtime_error("Land02TreeWrapper: Could not find branch Pspx05n!");
        pspx05n = static_cast<uint32_t*>(branchmap["Pspx05n"]);
    }

    uint64_t Land02TreeWrapper::getTS()
    {
        //      cerr << "Land02TreeWrapper::getTS()" << endl;
        uint64_t ts = (uint64_t)*TSLow | ((uint64_t)*TSHigh << 32);
        //      cerr << "-> " << ts << endl;
        return ts;
    }

    bool Land02TreeWrapper::isGood()
    {
        //      cerr << "Land02TreeWrapper::isGood()" << endl;
        bool good = *pspx04n > 0 && *pspx05n > 0;
        //      cerr << "pspx04n: " << *pspx04n << endl << "pspx05n: " << *pspx05n << endl;
        //      cerr << "-> " << (good ? "true" : "false") << endl;
        return good;
    }
} // namespace R3BCalifaTimestitcher
