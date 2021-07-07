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

#ifndef R3BCALIFAMAPPEDDATA_H
#define R3BCALIFAMAPPEDDATA_H
#include "TObject.h"
#include <stdint.h>

class R3BCalifaFebexReader;

class R3BCalifaMappedData : public TObject
{
#define MEMBER_PLUS_GETTER(type, name)                                  \
    protected: type f##name {};                                         \
    public: inline const type& Get##name() const { return f##name ; }
    
    MEMBER_PLUS_GETTER(uint16_t, CrystalId);
    
    MEMBER_PLUS_GETTER( int16_t, Energy);
    MEMBER_PLUS_GETTER( int16_t, Ns);
    MEMBER_PLUS_GETTER( int16_t, Nf);
    MEMBER_PLUS_GETTER(uint16_t, Tot);

    MEMBER_PLUS_GETTER(uint64_t, FebexTS);
    MEMBER_PLUS_GETTER(uint64_t, WRTS);

    MEMBER_PLUS_GETTER(uint32_t, Overflow);
    MEMBER_PLUS_GETTER(uint16_t, Pileup);
    MEMBER_PLUS_GETTER(uint16_t, Discard);

#undef MEMBER_PLUS_GETTER
    friend class R3BCalifaFebexReader;
  public:
    ClassDef(R3BCalifaMappedData, 5);
};

#endif
