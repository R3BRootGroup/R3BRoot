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
      TTree *tree;
      uint64_t idx;
      uint64_t nEntries;
      uint64_t *ptrTS;

      uint32_t id;

      TreeWrapper(TTree *tree, uint32_t id);

      virtual bool isGood() = 0;

   public:
      virtual uint64_t getTS();
      virtual bool next();

      virtual uint32_t getId();

      virtual ~TreeWrapper() {};
   };

}

#endif

