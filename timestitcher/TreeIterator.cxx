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

#include <vector>

#include "TreeIterator.h"

using namespace std;

namespace R3BCalifaTimestitcher
{
    TreeIterator::TreeIterator(vector<TreeWrapper*>& _trees)
        : trees(_trees)
    {
    }

    TreeWrapper* TreeIterator::first()
    {
        TreeWrapper* t_min = NULL;
        uint64_t ts_min = 0, cts;

        for (unsigned int i = 0; i < trees.size(); i++)
        {
            if (!trees[i]->next())
                return NULL;

            cts = trees[i]->getTS();
            if (ts_min == 0 || cts < ts_min)
            {
                ts_min = cts;
                t_min = trees[i];
            }
        }

        return t_min;
    }

    TreeWrapper* TreeIterator::next()
    {
        TreeWrapper* t_min = NULL;
        uint64_t ts_min = 0, cts;

        for (unsigned int i = 0; i < trees.size(); i++)
        {
            cts = trees[i]->getTS();
            if (ts_min == 0 || cts < ts_min)
            {
                ts_min = cts;
                t_min = trees[i];
            }
        }

        if (!t_min || !t_min->next())
            return NULL;

        return t_min;
    }
} // namespace R3BCalifaTimestitcher
