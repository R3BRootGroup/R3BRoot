#include <stdexcept>
#include <stdint.h>
#include <iostream>

#include <TTree.h>
#include <TLeaf.h>

#include "../r3bdata/califaData/R3BCalifaMappedData.h"
#include "../r3bdata/califaData/R3BCalifaCrystalCalData.h"
#include "../r3bdata/califaData/R3BCalifaHitData.h"

#include "R3BTreeWrapper.h"
#include "libtimestitcher.h"

using namespace std;

namespace R3BCalifaTimestitcher
{
   R3BTreeWrapper::R3BTreeWrapper(TTree *_tree, branchptrmap_t &branchmap, uint32_t _id) : TreeWrapper(_tree, _id)
   {
      // Try...
      // ...R3BCalifaMappedData
      if(branchmap.count("CalifaMappedData") > 0)
      {
         ptrObjArr = static_cast<TObjArray*>(branchmap["CalifaMappedData"]);
         type = CalifaMappedData;
      }
      // ...R3BCalifaCrystalCalData
      else if(branchmap.count("CalifaCrystalCalData") > 0)
      {
         ptrObjArr = static_cast<TObjArray*>(branchmap["CalifaCrystalCalData"]);
         type = CalifaCrystalCalData;
      }
      // ...R3BCalifaHitData
      else if(branchmap.count("CalifaHitData") > 0)
      {
         ptrObjArr = static_cast<TObjArray*>(branchmap["CalifaHitData"]);
         type = CalifaHitData;
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
      case CalifaMappedData:
         {
          R3BCalifaMappedData *rawHit = dynamic_cast<R3BCalifaMappedData*>(ptrObjArr->At(0));
          if(!rawHit)
             throw runtime_error("R3BTreeWrapper::getTS(): Could not cast to R3BCalifaMappedData!");

          return rawHit->GetTime();
          break;
         }
      
      case CalifaCrystalCalData:
         {
            R3BCalifaCrystalCalData *crystalHit = dynamic_cast<R3BCalifaCrystalCalData*>(ptrObjArr->At(0));
            if(!crystalHit)
               throw runtime_error("R3BTreeWrapper::getTS(): Could not cast to R3BCalifaCrystalCalData!");

            return crystalHit->GetTime();
            break;
         }

      case CalifaHitData:
         {
            R3BCalifaHitData *califaHit = dynamic_cast<R3BCalifaHitData*>(ptrObjArr->At(0));
            if(!califaHit)
               throw runtime_error("R3BTreeWrapper::getTS(): Could not cast to R3BCalifaHitData!");

            return califaHit->GetTime();
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
