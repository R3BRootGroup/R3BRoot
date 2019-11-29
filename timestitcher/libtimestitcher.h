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

#ifndef LIBTIMESTITCHER_H_
#define LIBTIMESTITCHER_H_

#include <map>
#include <string>

#include <TTree.h>

namespace R3BCalifaTimestitcher
{
    typedef std::map<std::string, void*> branchptrmap_t;

    void prefixString(std::string& s0, const std::string& prefix);
    int fillClonesArray();
    void addTreeBranches(TTree* merged, TTree* input, std::string prefix, branchptrmap_t& branchmap);
    TTree* findTree(std::string name);
    //   int cloneTreeBranches();
} // namespace R3BCalifaTimestitcher

#endif
