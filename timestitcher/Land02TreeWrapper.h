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

      uint32_t *TSLow;
      uint32_t *TSHigh;

      uint32_t *pspx04n;
      uint32_t *pspx05n;

   public:
     Land02TreeWrapper(TTree *t, branchptrmap_t &branchmap, uint32_t id);
   };
}

#endif

