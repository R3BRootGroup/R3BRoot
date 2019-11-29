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

#ifndef TREEITERATOR_H_
#define TREEITERATOR_H_

#include <vector>

#include "TreeWrapper.h"

namespace R3BCalifaTimestitcher
{

    class TreeIterator
    {
      protected:
        std::vector<TreeWrapper*>& trees;

      public:
        TreeIterator(std::vector<TreeWrapper*>& trees);
        TreeWrapper* first();
        TreeWrapper* next();
    };

} // namespace R3BCalifaTimestitcher

#endif
