#ifndef LIBTIMESTITCHER_H_
#define LIBTIMESTITCHER_H_

#include <string>
#include <map>

#include <TTree.h>

namespace R3BCalifaTimestitcher
{
   typedef std::map<std::string, void*> branchptrmap_t;

   void prefixString(std::string& s0, const std::string& prefix);
   int fillClonesArray();
   void addTreeBranches(TTree* merged, TTree* input, std::string prefix, branchptrmap_t &branchmap);
   TTree* findTree(std::string name);
//   int cloneTreeBranches();
}

#endif

