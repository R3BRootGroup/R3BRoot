#include <vector>

#include "TreeIterator.h"

using namespace std;

namespace R3BCalifaTimestitcher
{
   TreeIterator::TreeIterator(vector<TreeWrapper*> &trees) : trees(trees)
   {
   }

   TreeWrapper* TreeIterator::first()
   {
      TreeWrapper *t_min = NULL;
      uint64_t ts_min = 0, cts;

      for(unsigned int i = 0; i < trees.size(); i++)
      {
         if(!trees[i]->next())
            return NULL;

         cts = trees[i]->getTS();
         if(ts_min == 0 || cts < ts_min)
         {
            ts_min = cts;
            t_min = trees[i];
         }
      }

      return t_min;
   }

   TreeWrapper* TreeIterator::next()
   {
      TreeWrapper *t_min = NULL;
      uint64_t ts_min = 0, cts;

      for(unsigned int i = 0; i < trees.size(); i++)
      {
         cts = trees[i]->getTS();
         if(ts_min == 0 || cts < ts_min)
         {
            ts_min = cts;
            t_min = trees[i];
         }
      }

      if(!t_min || !t_min->next())
         return NULL;

      return t_min;
   }
}
