#ifndef TREEITERATOR_H_
#define TREEITERATOR_H_

#include <vector>

#include "TreeWrapper.h"

namespace R3BCalifaTimestitcher
{

   class TreeIterator
   {
   protected:
      std::vector<TreeWrapper*> &trees;

   public:
      TreeIterator(std::vector<TreeWrapper*> &trees);
      TreeWrapper *first();
      TreeWrapper* next();

   };

}

#endif

