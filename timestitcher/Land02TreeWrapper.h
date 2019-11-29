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

#ifndef LAND02TreeWrapper_H_
#define LAND02TreeWrapper_H_

#include "TreeWrapper.h"
#include "libtimestitcher.h"

namespace R3BCalifaTimestitcher
{
    class Land02TreeWrapper : public TreeWrapper
    {
      protected:
        virtual uint64_t getTS();
        virtual bool isGood();

        uint32_t* TSLow;
        uint32_t* TSHigh;

        uint32_t* pspx04n;
        uint32_t* pspx05n;

      public:
        Land02TreeWrapper(TTree* t, branchptrmap_t& branchmap, uint32_t id);
    };
} // namespace R3BCalifaTimestitcher

#endif
