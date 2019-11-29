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
            CalifaMappedData,
            CalifaCrystalCalData,
            CalifaHitData
        };

        TObjArray* ptrObjArr;
        ArrayType type;

        virtual bool isGood();
        Int_t* ptrN;

      public:
        R3BTreeWrapper(TTree* tree, branchptrmap_t& branchmap, uint32_t id);

        virtual uint64_t getTS();
    };
} // namespace R3BCalifaTimestitcher

#endif
