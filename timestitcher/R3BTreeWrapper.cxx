#include <stdexcept>
#include <stdint.h>
#include <iostream>

#include <TTree.h>
#include <TLeaf.h>

#include "../r3bdata/caloData/R3BCaloRawHit.h"
#include "../r3bdata/caloData/R3BCaloCrystalHit.h"
#include "../r3bdata/caloData/R3BCaloHit.h"

#include "R3BTreeWrapper.h"
#include "libtimestitcher.h"

using namespace std;

namespace R3BCalifaTimestitcher
{
   R3BTreeWrapper::R3BTreeWrapper(TTree *_tree, branchptrmap_t &branchmap, uint32_t _id) : TreeWrapper(_tree, _id)
   {
      // Try...
      // ...R3BCaloRawHit
      if(branchmap.count("CaloRawHit") > 0)
      {
         ptrObjArr = static_cast<TObjArray*>(branchmap["CaloRawHit"]);
         type = CaloRawHit;
      }
      // ...R3BCaloCrystalHit
      else if(branchmap.count("CaloCrystalHit") > 0)
      {
         ptrObjArr = static_cast<TObjArray*>(branchmap["CaloCrystalHit"]);
         type = CaloCrystalHit;
      }
      // ...R3BCaloHit
      else if(branchmap.count("CaloHit") > 0)
      {
         ptrObjArr = static_cast<TObjArray*>(branchmap["CaloHit"]);
         type = CaloHit;
      }
      else
         throw runtime_error("R3BTreeWrapper: Unknown tree structure!");
   }

   uint64_t R3BTreeWrapper::getTS()
   {
//      cerr << "R3BTreeWrapper::getTS()" << endl;
      if(ptrObjArr->GetEntries() == 0)
         return 0;

      switch(type)
      {
      case CaloRawHit:
         {
          R3BCaloRawHit *rawHit = dynamic_cast<R3BCaloRawHit*>(ptrObjArr->At(0));
          if(!rawHit)
             throw runtime_error("R3BTreeWrapper::getTS(): Could not cast to R3BCaloRawHit!");

          return rawHit->GetTime();
          break;
         }
      
      case CaloCrystalHit:
         {
            R3BCaloCrystalHit *crystalHit = dynamic_cast<R3BCaloCrystalHit*>(ptrObjArr->At(0));
            if(!crystalHit)
               throw runtime_error("R3BTreeWrapper::getTS(): Could not cast to R3BCaloCrystalHit!");

            return crystalHit->GetTime();
            break;
         }

      case CaloHit:
         {
            R3BCaloHit *caloHit = dynamic_cast<R3BCaloHit*>(ptrObjArr->At(0));
            if(!caloHit)
               throw runtime_error("R3BTreeWrapper::getTS(): Could not cast to R3BCaloHit!");

            return caloHit->GetTime();
            break;
         }

      default:
         throw runtime_error("R3BTreeWrapper::getTS(): Invalid branch type!");
      }
   }

   bool R3BTreeWrapper::isGood()
   {
//      cerr << "R3BTreeWrapper::isGood()" << endl;
      bool good = ptrObjArr->GetEntries() > 0;
//      cerr << "-> " << (good ? "true" : "false") << endl;
      return good;
   }
}
