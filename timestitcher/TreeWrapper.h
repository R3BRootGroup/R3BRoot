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

#ifndef TREEWRAPPER_H_
#define TREEWRAPPER_H_

#include <stdint.h>

#include <TTree.h>

using namespace std;

namespace R3BCalifaTimestitcher
{

    class TreeWrapper
    {
      protected:
        TTree* tree;
        uint64_t idx;
        uint64_t nEntries;
        uint64_t* ptrTS;

        uint32_t id;

        TreeWrapper(TTree* tree, uint32_t id);

        virtual bool isGood() = 0;

      public:
        virtual uint64_t getTS();
        virtual bool next();

        virtual uint32_t getId();

        virtual ~TreeWrapper(){};
    };

} // namespace R3BCalifaTimestitcher

#endif
