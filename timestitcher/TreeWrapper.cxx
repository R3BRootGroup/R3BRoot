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

#include "TreeWrapper.h"

using namespace std;

namespace R3BCalifaTimestitcher
{

    TreeWrapper::TreeWrapper(TTree* _tree, uint32_t _id)
        : tree(_tree)
        , idx(0)
        , id(_id)
    {
        this->nEntries = tree->GetEntries();
    }

    uint64_t TreeWrapper::getTS() { return *ptrTS; }

    bool TreeWrapper::next()
    {
        //      cerr << "TreeWrapper::next()" << endl;
        //      Long64_t treeIdx;
        while (idx < nEntries)
        {
            //         cerr << " idx = " << idx << endl;
            //         treeIdx = tree->LoadTree(idx++);
            //         cerr << " treeIdx = " << treeIdx << endl;
            tree->GetEntry(idx++);
            if (isGood())
                return true;
        }
        return false;
    }

    uint32_t TreeWrapper::getId() { return id; }

} // namespace R3BCalifaTimestitcher
