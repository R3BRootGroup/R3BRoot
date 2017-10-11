#ifndef R3BTREEWRAPPER_H_
#define R3BTREEWRAPPER_H_

#include <TObjArray.h>

#include "TreeWrapper.h"
#include "libtimestitcher.h"

namespace R3BCalifaTimestitcher
{
   class R3BTreeWrapper : public TreeWrapper
   {
   protected:
      enum ArrayType
      {
         CalifaMappedData, CalifaCrystalCalData, CalifaHitData
      };

      TObjArray *ptrObjArr;
      ArrayType type;

      virtual bool isGood();
      Int_t *ptrN;

   public:
      R3BTreeWrapper(TTree *tree, branchptrmap_t &branchmap, uint32_t id);

      virtual uint64_t getTS();
   };
}

#endif

